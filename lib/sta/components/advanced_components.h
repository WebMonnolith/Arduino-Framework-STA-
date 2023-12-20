#ifndef _STA_ADVANCED_COMPONENTS_
#define _STA_ADVANCED_COMPONENTS_

#include "sta.h"
#include <Arduino.h>

BEGIN_NP_BLOCK

#define COUNT_FALLING 0
#define COUNT_RISING  1
#define COUNT_BOTH    2

/// <summary>
/// More advanced button api, can be used to control joysticks, controllers, etc.
/// </summary>
class _STAXXEXPORT ez_button {
public:
    explicit ez_button(int32 pin) : ez_button(pin, INPUT_PULLUP) {}
    explicit ez_button(int32 pin, int32 mode) 
        : btnPin(pin), 
        debounceTime(0),
        count(0),
        countMode(COUNT_FALLING)
    {

        pinMode(this->btnPin, mode);

        this->previousSteadyState = digitalRead(this->btnPin);
        this->lastSteadyState = this->previousSteadyState;
        this->lastFlickerableState = this->previousSteadyState;

        this->lastDebounceTime = 0;
    }
public:
    bool isPressed(void) {
        if (this->previousSteadyState == HIGH && this->lastSteadyState == LOW)
            return true;
        return false;
    }

    bool isReleased(void) {
        if (this->previousSteadyState == LOW && this->lastSteadyState == HIGH)
            return true;
        return false;
    }

    void resetCount(void) {
        this->count = 0;
    }

    void loop(void) {
        // read the state of the switch/button:
        int32 currentState = digitalRead(this->btnPin);
        uint64 currentTime = millis();

        // check to see if you just pressed the button
        // (i.e. the input went from LOW to HIGH), and you've waited long enough
        // since the last press to ignore any noise:

        // If the switch/button changed, due to noise or pressing:
        if (currentState != this->lastFlickerableState) {
            // reset the debouncing timer
            this->lastDebounceTime = currentTime;
            // save the the last flickerable state
            this->lastFlickerableState = currentState;
        }

        if ((currentTime - this->lastDebounceTime) >= this->debounceTime) {
            // whatever the reading is at, it's been there for longer than the debounce
            // delay, so take it as the actual current state:

            // save the the steady state
            this->previousSteadyState = this->lastSteadyState;
            this->lastSteadyState = currentState;
        }

        if (this->previousSteadyState != this->lastSteadyState) {
            if (this->countMode == COUNT_BOTH)
                this->count++;
            else if (this->countMode == COUNT_FALLING) {
                if (this->previousSteadyState == HIGH && this->lastSteadyState == LOW)
                    this->count++;
            }
            else if (this->countMode == COUNT_RISING) {
                if (this->previousSteadyState == LOW && this->lastSteadyState == HIGH)
                    this->count++;
            }
        }
    }

public:
    inline void setDebounceTime(uint64 time) 
    _STAXX_NOEXCEPT {
        this->debounceTime = time;
    }

    inline int32 getLastSteadyState(void) const
    _STAXX_NOEXCEPT {
        return this->lastSteadyState;
    }

    inline int32 getStateRaw(void) const 
    _STAXX_NOEXCEPT {
        return digitalRead(btnPin);
    }

    inline void setCountMode(int32 mode) 
    _STAXX_NOEXCEPT {
        this->countMode = mode;
    }

    inline uint64 getCount(void) const 
    _STAXX_NOEXCEPT {
        return this->count;
    }

private:
    int32 btnPin;
    uint64 debounceTime;
    uint64 count;
    int32 countMode;

    int32 previousSteadyState;  // the previous steady state from the input pin, used to detect pressed and released event
    int32 lastSteadyState;      // the last steady state from the input pin
    int32 lastFlickerableState; // the last flickerable state from the input pin

    uint64 lastDebounceTime; // the last time the output pin was toggled
};

#define OPEN        LOW
#define CLOSED      HIGH

const int8 NO_KEY = '\0';

#ifndef INPUT_PULLUP
#warning "Using  pinMode() INPUT_PULLUP AVR emulation"
#define INPUT_PULLUP 0x2
#define pinMode(_pin, _mode) _mypinMode(_pin, _mode)
#define _mypinMode(_pin, _mode)  \
do {							 \
	if(_mode == INPUT_PULLUP)	 \
		pinMode(_pin, INPUT);	 \
		digitalWrite(_pin, 1);	 \
	if(_mode != INPUT_PULLUP)	 \
		pinMode(_pin, _mode);	 \
}while(0)
#endif

typedef int8 KeypadEvent;

#define LIST_MAX 10		            // Max number of keys on the active list.
#define MAPSIZE 10		            // MAPSIZE is the number of rows (times 16 columns)
#define _STA_MAKEKEYMAP(x) ((int8*)x)

template <typename T>
_STAXXEXPORT int8* make_keymap(T* map) 
_STAXX_NOEXCEPT {
    return _STA_MAKEKEYMAP(map);
}

/// <summary>
/// Simple keypad api that is able to translate the hardware to a software component
/// </summary>
class _STAXXEXPORT keypad {
public: // INTERNAL TYPES
    enum class key_state : uint32 {
        IDLE,
        PRESSED,
        HOLD,
        RELEASED
    };

    typedef struct {
        uint32 rows;
        uint32 columns;
    } KeypadSize;

    struct key {
    public:
        int8 kchar;
        int32 kcode;
        key_state kstate;
        bool stateChanged;
    public:
        key() _STAXX_NOEXCEPT {
            this->kchar = NO_KEY;
            this->kstate = key_state::IDLE;
            this->stateChanged = false;
        }

        key(int8 userKeyChar) _STAXX_NOEXCEPT {
            this->kchar = userKeyChar;
            this->kcode = -1;
            this->kstate = key_state::IDLE;
            this->stateChanged = false;
        }

        void key_update(int8 userKeyChar, key_state userState, bool userStatus) {
            this->kchar = userKeyChar;
            this->kstate = userState;
            this->stateChanged = userStatus;
        }

    };
public:
    uint32 bitMap[MAPSIZE];	// 10 row x 16 column array of bits. Except Due which has 32 columns.
    key key_list[LIST_MAX];
    uint64 holdTimer;
public: // CON-/DESTRUCTORS
    keypad(int8* userKeymap, byte* row, byte* col, byte numRows, byte numCols) {
        this->rowPins = row;
        this->columnPins = col;
        this->sizeKpd.rows = numRows;
        this->sizeKpd.columns = numCols;

        this->begin(userKeymap);

        this->setDebounceTime(10);
        this->setHoldTime(500);
        this->keypadEventListener = 0;

        this->startTime = 0;
        this->single_key = false;
    }
    virtual ~keypad() {}
public: // VIRTUAL
    virtual void pin_mode(byte pinNum, byte mode) _STAXX_NOEXCEPT { sta::pin_mode(pinNum, mode); }
    virtual void pin_write(byte pinNum, bool level) _STAXX_NOEXCEPT  { sta::pin_write(pinNum, level); }
    virtual int32  pin_read(byte pinNum) _STAXX_NOEXCEPT { return sta::pin_read(pinNum); }
public:
    inline void begin(int8* userKeymap) {
        this->keymap = userKeymap;
    }

    bool isPressed(int8 keyChar) {
        for (byte i = 0; i < LIST_MAX; i++) {
            if (key_list[i].kchar == keyChar) {
                if ((key_list[i].kstate == key_state::PRESSED) && key_list[i].stateChanged)
                    return true;
            }
        }
        return false;	// Not pressed.
    }


    int32 findInList(int8 keyChar) {
        for (byte i = 0; i < LIST_MAX; i++) {
            if (key_list[i].kchar == keyChar) {
                return i;
            }
        }
        return -1;
    }

    int32 findInList(int32 keyCode) {
        for (byte i = 0; i < LIST_MAX; i++) {
            if (key_list[i].kcode == keyCode) {
                return i;
            }
        }
        return -1;
    }

    int8 waitForKey() {
        int8 waitKey = NO_KEY;
        while ((waitKey = getKey()) == NO_KEY);	// Block everything while waiting for a keypress.
        return waitKey;
    }

public: // GETTERS/SETTERS
    inline void addEventListener(void (*listener)(int8)) {
        this->keypadEventListener = listener;
    }

    inline byte numKeys() const {
        return sizeof(this->key_list) / sizeof(key);
    }

    inline bool keyStateChanged() const {
        return key_list[0].stateChanged;
    }

    int8 getKey() {
        this->single_key = true;
        if (getKeys() && key_list[0].stateChanged && (key_list[0].kstate == key_state::PRESSED))
            return key_list[0].kchar;
        this->single_key = false;
        return NO_KEY;
    }

    bool getKeys() {
        bool keyActivity = false;
        if ((millis() - startTime) > debounceTime) {
            scanKeys();
            keyActivity = updateList();
            startTime = millis();
        }
        return keyActivity;
    }

    inline key_state getState() const {
        return key_list[0].kstate;
    }

    inline void setDebounceTime(uint32 debounce) {
        debounce < 1 ? this->debounceTime = 1 : this->debounceTime = debounce;
    }

    inline void setHoldTime(uint32 hold) {
        holdTime = hold;
    }
private:
    uint64 startTime;
    int8* keymap;
    byte* rowPins;
    byte* columnPins;
    KeypadSize sizeKpd;
    uint32 debounceTime;
    uint32 holdTime;
    bool single_key;
private:
    void scanKeys() {
        for (byte r = 0; r < this->sizeKpd.rows; r++) 
            this->pin_mode(this->rowPins[r], INPUT_PULLUP);
        
        // bitMap stores ALL the keys that are being pressed.
        for (byte c = 0; c < this->sizeKpd.columns; c++) {
            this->pin_mode(this->columnPins[c], OUTPUT);
            this->pin_write(this->columnPins[c], LOW);	// Begin column pulse output.
            for (byte r = 0; r < this->sizeKpd.rows; r++) 
                bitWrite(this->bitMap[r], c, !pin_read(this->rowPins[r]));  // keypress is active low so invert to high.
            // Set pin to high impedance input. Effectively ends column pulse.
            this->pin_write(this->columnPins[c], HIGH);
            this->pin_mode(this->columnPins[c], INPUT);
        }
    }

    bool updateList() {
        bool anyActivity = false;

        // Delete any IDLE keys
        for (byte i = 0; i < LIST_MAX; i++) {
            if (this->key_list[i].kstate == key_state::IDLE) {
                this->key_list[i].kchar = NO_KEY;
                this->key_list[i].kcode = -1;
                this->key_list[i].stateChanged = false;
            }
        }

        // Add new keys to empty slots in the key_list list.
        for (byte r = 0; r < this->sizeKpd.rows; r++) {
            for (byte c = 0; c < this->sizeKpd.columns; c++) {
                bool button = bitRead(this->bitMap[r], c);
                int8 keyChar = this->keymap[r * this->sizeKpd.columns + c];
                int32 keyCode = r * this->sizeKpd.columns + c;
                int32 idx = findInList(keyCode);
                // key_list is already on the list so set its next state.
                if (idx > -1) {
                    nextKeyState(idx, button);
                }
                // key_list is NOT on the list so add it.
                if ((idx == -1) && button) {
                    for (byte i = 0; i < LIST_MAX; i++) {
                        if (this->key_list[i].kchar == NO_KEY) {		// Find an empty slot or don't add key_list to list.
                            this->key_list[i].kchar = keyChar;
                            this->key_list[i].kcode = keyCode;
                            this->key_list[i].kstate = key_state::IDLE;		// Keys NOT on the list have an initial state of IDLE.
                            nextKeyState(i, button);
                            break;	// Don't fill all the empty slots with the same key_list.
                        }
                    }
                }
            }
        }

        // Report if the user changed the state of any key_list.
        for (byte i = 0; i < LIST_MAX; i++) {
            if (this->key_list[i].stateChanged) anyActivity = true;
        }

        return anyActivity;
    }

    void nextKeyState(byte idx, bool button) {
        this->key_list[idx].stateChanged = false;

        switch (key_list[idx].kstate) {
        case key_state::IDLE:
            if (button == CLOSED) {
                transitionTo(idx, key_state::PRESSED);
                holdTimer = millis();
            }		// Get ready for next HOLD state.
            break;
        case key_state::PRESSED:
            if ((millis() - holdTimer) > holdTime)	// Waiting for a key_list HOLD...
                transitionTo(idx, key_state::HOLD);
            else if (button == OPEN)				// or for a key_list to be RELEASED.
                transitionTo(idx, key_state::RELEASED);
            break;
        case key_state::HOLD:
            if (button == OPEN)
                transitionTo(idx, key_state::RELEASED);
            break;
        case key_state::RELEASED:
            transitionTo(idx, key_state::IDLE);
            break;
        }
    }

    void transitionTo(byte idx, key_state nextState) {
        this->key_list[idx].kstate = nextState;
        this->key_list[idx].stateChanged = true;

        // Sketch used the getKey() function.
        // Calls keypadEventListener only when the first key in slot 0 changes state.
        if (this->single_key) {
            if ((this->keypadEventListener != NULL) && (idx == 0)) {
                this->keypadEventListener(this->key_list[0].kchar);
            }
        }
        // Sketch used the getKeys() function.
        // Calls keypadEventListener on any key that changes state.
        else {
            if (this->keypadEventListener != NULL) {
                this->keypadEventListener(this->key_list[idx].kchar);
            }
        }
    }
private:
    void (*keypadEventListener)(int8);
public: // OPERATOR OVERLOADS
    keypad& operator = (const keypad& kp) = default;
};

const static uint8 charTable[] PROGMEM = {
    B01111110,B00110000,B01101101,B01111001,B00110011,B01011011,B01011111,B01110000,
    B01111111,B01111011,B01110111,B00011111,B00001101,B00111101,B01001111,B01000111,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B10000000,B00000001,B10000000,B00000000,
    B01111110,B00110000,B01101101,B01111001,B00110011,B01011011,B01011111,B01110000,
    B01111111,B01111011,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B01110111,B00011111,B00001101,B00111101,B01001111,B01000111,B00000000,
    B00110111,B00000000,B00000000,B00000000,B00001110,B00000000,B00000000,B00000000,
    B01100111,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00001000,
    B00000000,B01110111,B00011111,B00001101,B00111101,B01001111,B01000111,B00000000,
    B00110111,B00000000,B00000000,B00000000,B00001110,B00000000,B00010101,B00011101,
    B01100111,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000
};

//the opcodes for the MAX7221 and MAX7219
#define OP_NOOP   0
#define OP_DIGIT0 1
#define OP_DIGIT1 2
#define OP_DIGIT2 3
#define OP_DIGIT3 4
#define OP_DIGIT4 5
#define OP_DIGIT5 6
#define OP_DIGIT6 7
#define OP_DIGIT7 8
#define OP_DECODEMODE  9
#define OP_INTENSITY   10
#define OP_SCANLIMIT   11
#define OP_SHUTDOWN    12
#define OP_DISPLAYTEST 15

/// <summary>
/// Simple led-matrix class to work with the corresponding hardware.
/// </summary>
class _STAXXEXPORT led_matrix {
public:
    led_matrix() = default;

    /// <summary>
    /// Create a new controler
    /// </summary>
    /// <param name="dataPin"> - pin on the Arduino where data gets shifted out</param>
    /// <param name="clkPin"> - pin for the clock</param>
    /// <param name="csPin"> - pin for selecting the device</param>
    /// <param name="numDevices"> - maximum number of devices that can be controled</param>
    led_matrix(int32 dataPin, int32 clkPin, int32 csPin, int32 numDevices = 1)
        : SPI_MOSI(dataPin), SPI_CLK(clkPin), SPI_CS(csPin)
    {
        if (numDevices <= 0 || numDevices > 8)
            numDevices = 8;
        this->maxDevices = numDevices;
        pinMode(this->SPI_MOSI, OUTPUT);
        pinMode(this->SPI_CLK, OUTPUT);
        pinMode(this->SPI_CS, OUTPUT);
        digitalWrite(this->SPI_CS, HIGH);
        this->SPI_MOSI = dataPin;
        for (int32 i = 0; i < 64; i++)
            this->status[i] = 0x00;
        for (int32 i = 0; i < this->maxDevices; i++) {
            this->spiTransfer(i, OP_DISPLAYTEST, 0);
            this->setScanLimit(i, 7);
            this->spiTransfer(i, OP_DECODEMODE, 0);
            this->clearDisplay(i);
            this->shutdown(i, true);
        }
    }

public:

    /// <summary>
    /// Gets the number of devices attached to this LedControl
    /// </summary>
    /// <returns>int32</returns>
    inline int32 getDeviceCount() const {
        return this->maxDevices;
    }

    /*
     * Set the shutdown (power saving) mode for the device
     * Params :
     * addr	The address of the display to control
     * status	If true the device goes into power-down mode. Set to false
     *		for normal operation.
     */

     /// <summary>
     /// Set the shutdown (power saving) mode for the device
     /// </summary>
     /// <param name="addr"> - The address of the display to control</param>
     /// <param name="status"> - If true the device goes into power-down mode. Set to false for normal operation.</param>
    void shutdown(int32 addr, bool status) {
        if (addr < 0 || addr >= this->maxDevices)
            return;
        if (status)
            this->spiTransfer(addr, OP_SHUTDOWN, 0);
        else
            this->spiTransfer(addr, OP_SHUTDOWN, 1);
    }

    /// <summary>
    /// Set the number of digits (or rows) to be displayed.
    /// See datasheet for sideeffects of the scanlimit on the brightness
    /// of the display.
    /// </summary>
    /// <param name="addr"> - address of the display to control</param>
    /// <param name="limit"> - number of digits to be displayed (1..8)</param>
    void setScanLimit(int32 addr, int32 limit) {
        if (addr < 0 || addr >= this->maxDevices)
            return;
        if (limit >= 0 && limit < 8)
            this->spiTransfer(addr, OP_SCANLIMIT, limit);
    }

    /// <summary>
    /// Set the brightness of the display.
    /// </summary>
    /// <param name="addr"> - the address of the display to control</param>
    /// <param name="intensity"> - he brightness of the display. (0..15)</param>
    void setIntensity(int32 addr, int32 intensity) {
        if (addr < 0 || addr >= this->maxDevices)
            return;
        if (intensity >= 0 && intensity < 16)
            this->spiTransfer(addr, OP_INTENSITY, intensity);
    }

    /// <summary>
    /// Switch all Leds on the display off.
    /// </summary>
    /// <param name="addr"> - address of the display to control</param>
    void clearDisplay(int32 addr) {
        int32 offset;

        if (addr < 0 || addr >= this->maxDevices)
            return;
        offset = addr * 8;
        for (int32 i = 0; i < 8; i++) {
            this->status[offset + i] = 0;
            this->spiTransfer(addr, i + 1, this->status[offset + i]);
        }
    }

    /// <summary>
    /// Set the status of a single led
    /// </summary>
    /// <param name="addr"> - address of the display</param>
    /// <param name="row"> - the row of the Led (0..7)</param>
    /// <param name="col"> - the column of the Led (0..7)</param>
    /// <param name="state"> - On/off</param>
    void setLed(int32 addr, int32 row, int32 col, bool state) {
        int32 offset;
        uint8 val = 0x00;

        if (addr < 0 || addr >= this->maxDevices)
            return;
        if (row < 0 || row>7 || col < 0 || col >7)
            return;
        offset = addr * 8;
        val = B10000000 >> col;
        if (state)
            this->status[offset + row] = this->status[offset + row] | val;
        else {
            val = ~val;
            this->status[offset + row] = this->status[offset + row] & val;
        }
        this->spiTransfer(addr, row + 1, this->status[offset + row]);
    }

    /// <summary>
    /// Set all 8 Led's in a row to a new state.
    /// </summary>
    /// <param name="addr"> - address of the display</param>
    /// <param name="row"> - row which is to be set (0..7)</param>
    /// <param name="value"></param>
    void setRow(int32 addr, int32 row, uint8 value) {
        int32 offset;
        if (addr < 0 || addr >= this->maxDevices)
            return;
        if (row < 0 || row > 7)
            return;
        offset = addr * 8;
        this->status[offset + row] = value;
        this->spiTransfer(addr, row + 1, this->status[offset + row]);
    }

    /// <summary>
    /// Set all 8 Led's in a row to a new state.
    /// </summary>
    /// <param name="addr"> - address of the display</param>
    /// <param name="col"> - col which is to be set (0..7)</param>
    /// <param name="value"></param>
    void setColumn(int32 addr, int32 col, uint8 value) {
        uint8 val;

        if (addr < 0 || addr >= this->maxDevices)
            return;
        if (col < 0 || col > 7)
            return;
        for (int32 row = 0; row < 8; row++) {
            val = value >> (7 - row);
            val = val & 0x01;
            this->setLed(addr, row, col, val);
        }
    }

    /// <summary>
    /// Display a hexadecimal digit on a 7-Segment Display
    /// </summary>
    /// <param name="addr"></param>
    /// <param name="digit"></param>
    /// <param name="value"></param>
    /// <param name="dp"></param>
    void setDigit(int32 addr, int32 digit, uint8 value, bool dp) {
        int32 offset;
        uint8 v;

        if (addr < 0 || addr >= this->maxDevices)
            return;
        if (digit < 0 || digit>7 || value > 15)
            return;
        offset = addr * 8;
        v = pgm_read_byte_near(charTable + value);
        if (dp)
            v |= B10000000;
        this->status[offset + digit] = v;
        this->spiTransfer(addr, digit + 1, v);
    }

    /// <summary>
    ///  Display a character on a 7-Segment display.
    ///  There are only a few characters that make sense here :
    /// 	'0','1','2','3','4','5','6','7','8','9','0',
    ///   'A','b','c','d','E','F','H','L','P',
    ///   '.','-','_',' '
    /// </summary>
    /// <param name="addr"></param>
    /// <param name="digit"></param>
    /// <param name="value"></param>
    /// <param name="dp"></param>
    void setChar(int32 addr, int32 digit, int8 value, bool dp) {
        int32 offset;
        uint8 index, v;

        if (addr < 0 || addr >= this->maxDevices)
            return;
        if (digit < 0 || digit>7)
            return;
        offset = addr * 8;
        index = (uint8)value;
        if (index > 127) {
            //no defined beyond index 127, so we use the space int8
            index = 32;
        }
        v = pgm_read_byte_near(charTable + index);
        if (dp)
            v |= B10000000;
        this->status[offset + digit] = v;
        this->spiTransfer(addr, digit + 1, v);
    }

private:
    /// <summary>
    /// Send out a single command to the device
    /// </summary>
    /// <param name="addr"></param>
    /// <param name="opcode"></param>
    /// <param name="data"></param>
    void spiTransfer(int32 addr, uint8 opcode, uint8 data) {
        //Create an array with the data to shift out
        int32 offset = addr * 2;
        int32 maxbytes = this->maxDevices * 2;

        for (int32 i = 0; i < maxbytes; i++)
            this->spidata[i] = (uint8)0;
        //put our device data into the array
        this->spidata[offset + 1] = opcode;
        this->spidata[offset] = data;
        //enable the line 
        digitalWrite(SPI_CS, LOW);
        //Now shift out the data 
        for (int32 i = maxbytes; i > 0; i--)
            shiftOut(this->SPI_MOSI, this->SPI_CLK, MSBFIRST, this->spidata[i - 1]);
        //latch the data onto the display
        digitalWrite(this->SPI_CS, HIGH);
    }
private:
    // The array for shifting the data to the devices 
    uint8 spidata[16];

    // We keep track of the led-status for all 8 devices in this array
    uint8 status[64];
    // Data is shifted out of this pin
    int32 SPI_MOSI;
    // The clock is signaled on this pin 
    int32 SPI_CLK;
    // This one is driven LOW for chip selectzion
    int32 SPI_CS;
    // The maximum number of devices we use
    int32 maxDevices;
};


END_NP_BLOCK

#endif