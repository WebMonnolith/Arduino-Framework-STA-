#ifndef _AR_BASIC_COMPONENTS_
#define _AR_BASIC_COMPONENTS_

#pragma GCC visibility push(default)
#pragma GCC diagnostic ignored "-Wconversion-null"

#include "sta.h"
#include "./core/memory.h"
#include "./core/containers.h"
#include "pins.h"
#include "./types/callback.h"

BEGIN_NP_BLOCK

/*
Component represents the base class for all components. 
this is the class to extend if you want to create custom components.
 Custom components should:
 - Override privateLoop
 - Call RegisterChild on children (not mendatory)
*/
class _STAXXEXPORT component {
public:
    component(ref_ptr<component> parent = nullptr) : _parent(parent), _children(5), _isLooping(false) {}
    void loop() {
        _isLooping = true;
        loopChildren();
        if(_isLooping) {
            privateLoop();
        }
    }

    virtual ~component() {
        this->_children.~vector();
    }
protected:
    void SkipLoop() {
        _isLooping = false;
        if(*_parent != nullptr) _parent->SkipLoop();
    }
    void RegisterChild(ref_ptr<component> child) {
        _children.push(child);
    }

private:
    void loopChildren() {
        for(unsigned i = 0; i < _children.size(); i++) {
            _children.get(i)->loop();
            if(!_isLooping) {
                SkipLoop();
                return;
            }
        }
    }
private:
    virtual void privateLoop() = 0;
private:
    ref_ptr<component> _parent;
    vector<ref_ptr<component>> _children;
    bool _isLooping;
};

/*
Simple LED encapsulation class.

Able to turn the LED on/off, toggle it and check wether the led is on or not.
*/
class _STAXXEXPORT led : public component {
public:
    led() : _output(NULL) {}
    led(pin_num pin, const state& startState = state::Low) : led(dout(pin), startState) {}
    led(dout out, const state& startState = state::Low)  
        : _output(out), _state(startState) { this->_output.write(_state); }
public:
    void on() {
        this->_output.high();
		this->_state = state::High;
    }

    void off() {
        this->_output.low();
		this->_state = state::Low; 
    }

    void toggle() {
        if(_state == state::Low) this->on();
		else this->off();
    }

    void set(const state& state) {
        if(state == state::Low) this->off();
		else this->on();
    }

    inline bool isOn() const {return _state == state::High; }

private:
    dout _output;
    state _state;
private:
    void privateLoop() override {}
public:
    inline operator bool() const { return this->isOn(); }

    led operator = (const led& l) {
        this->_output = l._output;
        this->_state = l._state;
        return *this;
    }
};

/*
Class: NonBlocckingBlinkingLed -> led_nbb

*/
class _STAXXEXPORT led_nbb : public component {
public:
    led_nbb() = default;
	led_nbb(led led_lamp) : led_lamp(led_lamp), blinking(false) {}
	led_nbb(led led_lamp, uint64 interval) 
        : led_lamp(led_lamp), blinking(true) { this->resumeBlinking(interval); }
public:
	void resumeBlinking(int64 interval) {
        if(this->blinking && interval == this->blinkInterval) return;
	
        this->blinking = true;
        this->millisLeft = this->blinkInterval = interval;
        this->lastMillis = millis();
    }

	void stopBlinking() {
	    this->blinking = false;
    }

	void on() {
        this->stopBlinking();
	    this->led_lamp.on();
    }

	void off() {
        this->stopBlinking();
	    this->led_lamp.off();
    }

private:
	void privateLoop() override {
        if(!blinking) return;
        
        auto deltaSinceLastUpdate = millis() - this->lastMillis;
        this->lastMillis = millis();
        this->millisLeft -= deltaSinceLastUpdate;
        
        if (this->millisLeft < 0) {
            this->millisLeft = this->blinkInterval;
            this->led_lamp.toggle();
        }
    }
private:
	led led_lamp;
	int64 millisLeft;
	int64 lastMillis;
	int64 blinkInterval;
	bool blinking;
};

class _STAXXEXPORT tactile_button : public component {
public: // CON-/DESTRUCTORS
    tactile_button(din input, trigger_on trigger) 
        : tactile_button(nullptr, input, trigger) 
    {}
    tactile_button(ref_ptr<component> parent, din input, trigger_on trigger) 
        : component(parent), _input(input), _pressed(false), _on_press([](){}), _on_release([](){}), _trigger(trigger)
    {}
    tactile_button(pin_num pin, trigger_on trigger, input_pull pull = input_pull::None) 
        : tactile_button(nullptr, pin, trigger, pull) 
    {}
    tactile_button(ref_ptr<component> parent, pin_num pin, trigger_on trigger, input_pull pull = input_pull::None)
        : tactile_button(parent, din(pin, pull), trigger) 
    {}
public: // HELPERS
    void onPress(void_callback cbk) {
        this->_on_press = cbk;
    }

    void onRelease(void_callback cbk) {
        this->_on_release = cbk;
    }

    bool isPressed() {
        return (_trigger == trigger_on::Low) ? _input.isLow() : _input.isHigh();
    }
    
private: // INTERNAL ATTRIBUTES
    din _input;
    bool _pressed;
    void_callback _on_press;
    void_callback _on_release;
    trigger_on _trigger;
private: // INTERNAL METHODS
    void privateLoop() override {
        bool pressedState = this->isPressed();
		if(pressedState != _pressed) {
			if(pressedState == true) _on_press();
			else if(pressedState == false) _on_release();
			_pressed = pressedState;
		}
    }
public: // OPERATORS
    inline operator bool() { return this->isPressed(); }
};

/*
Name: timer
Sentence: A class for calling a callback function after specified delay.
BasicUsage:
	// For running code after some delay:
	sta::timer t;
	t.setTimeout([](){
		// Do Something after the delay
	}, 1000); // 1000 -> is the delay in milliseconds

	// For runnig code in fixed intervals
	sta::timer t;
	t.onInterval([](){
		// Do Something on intervals
	}, 1500); // 1500 -> is the interval in milliseconds


	// Always needed:
	void loop() {
		// ... Code
		t.loop();
		// ... Code
	}
Notes:
	- Timer has a state so its `loop` function must be called for it to work!
	- You don't have to use lambda function, you could pass a normal function (it must have no parameters).
	- calling setTimeout(..) while timer is already running (when isRunning() returns `true`) will cancel the 
	  already running timer and will restart it for the specified callback and delay.
*/
class _STAXXEXPORT timer : public component {
public:
    timer() : timer(nullptr) {}
    timer(ref_ptr<component> parent) _STAXX_NOEXCEPT
        : 
            component(parent), _timeout_callback([](){}),
            _timer_running(false), _deadline(0) 
        {}
public:
    void setTimeout(void_callback callback, uint64 delay) {
        this->cancel();
		this->_intervaling = false;
		this->_timer_running = true;
		this->_timeout_callback = callback;
		this->_deadline = millis() + delay;
    }

    void onInterval(void_callback callback, uint64 interval) {
        this->cancel();
		this->_interval = interval;
		this->_intervaling = true;
		this->_timer_running = true;
		this->_timeout_callback = callback;
		this->_deadline = millis() + interval;
    }

    void cancel() {
        this->_timer_running = false;
    }
public:
    inline bool isRunning() {
        return this->_timer_running;
    }

    inline uint64 timeLeft() {
        return this->_deadline - millis();
    }
private:
    void privateLoop() override {
        if(!_timer_running) return;
		if(millis() >= _deadline) {
			if(_intervaling) {
				_deadline = millis() + _interval;
				_timeout_callback();
			} else {
				cancel();
				_timeout_callback();
			}
		}
    }
private:
    void_callback _timeout_callback;
    bool _timer_running;
    uint64 _deadline;
    
    uint64 _interval;
    bool _intervaling;
};

/// <summary>
/// Simple push button abstraction class.
/// </summary>
class _STAXXEXPORT push_button 
    : public component {
public:
    push_button() = default;
	push_button(pin_num pin) : pin(pin) 
	{}
public:
	/// <summary>
	/// Checks if the button is pressed or not.
	/// </summary>
	/// <returns>uint8</returns>
	int8 isPressed()
    _STAXX_NOEXCEPT {
		if (this->state)
			return true;
		return false;
	}

	/// <summary>
	/// Listens for changes in the button state (On/Off)
	/// </summary>
	/// <returns>VOID</returns>
	void listen()
    _STAXX_NOEXCEPT {
		this->state = digitalRead(this->pin);
	}
public:
    inline int16 getState() const { return state; }
    inline pin_num getPin() const { return pin; }
private:
    int16 state;
    pin_num pin;
private:
    void privateLoop() override {}
public:
    inline operator bool() { return this->isPressed(); }
};


END_NP_BLOCK

#endif