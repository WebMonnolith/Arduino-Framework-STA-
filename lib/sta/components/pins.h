#ifndef _STA_PINS_
#define _STA_PINS_

#include "sta.h"
#include <Arduino.h>

BEGIN_NP_BLOCK

typedef int pin_num;

/*
Simple arduino pin repesentation as a struct type.
*/
struct _STAXXEXPORT iopin {
public:
    iopin(pin_num pin) : _pin(pin) {}
    virtual ~iopin() {}
    pin_num pinNumber() { return _pin; }
protected:
    const pin_num _pin;
public:
    iopin operator = (const iopin& io) { return *this; }
};

enum class 
_STAXXEXPORT state : bool {
    High = true,
    Low = false
};

enum class 
_STAXXEXPORT trigger_on {
    High,
    Low
};

enum class 
_STAXXEXPORT input_pull {
    None,
    Up,
    Down,
};

/*
Simple Digital output representation class.
*/
class _STAXXEXPORT dout : public iopin {
public:
    dout(pin_num pin) 
        : iopin(pin)
    {
        pinMode(pinNumber(), OUTPUT);
    }

    virtual ~dout() {}
public:
    virtual void high() {
        digitalWrite(pinNumber(), true);
    }

    virtual void low() {
        digitalWrite(pinNumber(), false);
    }

public:
    void write(const state& s) {
        if(s == state::Low) this->low();
		else if (s == state::High) this->high();
    }
public:
    dout operator = (const dout& out) { return *this; }
};

/*
Simple Digital input representation class.
*/
class _STAXXEXPORT din : public iopin {
public:
    din(pin_num pin) 
        : din(pin, input_pull::None)
    {}
    din(pin_num pin, const input_pull& pull)
        : iopin(pin) 
    {
        if(pull == input_pull::None) pinMode(pin, INPUT);
		else if(pull == input_pull::Up) pinMode(pin, INPUT_PULLUP);
    }
    virtual ~din() {}
public:
    inline bool isHigh() {
        return digitalRead(pinNumber());
    }

    inline bool isLow() {
        return !isHigh();
    }
public:
    din operator = (const din& in) { return *this; }
};

/**
 Simple analog output representation class.
*/
class _STAXXEXPORT aout : public dout {
public:
    aout(pin_num pin) 
        : dout(pin)
    {
    #ifdef ARDUINO_ARCH_ESP32
        static unsigned char pwmChannelCounter = 0;

		this->pwmChannel = pwmChannelCounter++;
		ledcAttachPin(pin, this->pwmChannel);
		ledcSetup(this->pwmChannel, 12000, 8);
    #endif
    }
    ~aout() {}
public:
    inline void write(float value) {
    #ifdef ARDUINO_ARCH_ESP32
        ledcWrite(this->pwmChannel, 255 * value);
    #else
        analogWrite(pinNumber(), 255 * value);
    #endif
    }
public:
    aout operator = (const aout& in) { return *this; }
private:
    #ifdef ARDUINO_ARCH_ESP32
        unsigned char pwmChannel;
    #endif
};

class _STAXXEXPORT ain : public din {
public:
    ain(pin_num pin): ain(pin, input_pull::None) {}
    ain(pin_num pin, const input_pull& pull) : din(pin, pull) {}
public:
    inline float read() {
        return analogRead(pinNumber()) / 1024.0;
    }
public:
    ain operator = (const ain& in) { return *this; }
};
END_NP_BLOCK

#endif