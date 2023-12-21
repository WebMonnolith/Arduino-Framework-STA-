#ifndef _STA_ARDUINO_FRAME_
#define _STA_ARDUINO_FRAME_

#include "sta.h"

BEGIN_NP_BLOCK

class _STAXXEXPORT micro_controller {
public:
    micro_controller() = default;
    virtual ~micro_controller() = default;
    micro_controller(const micro_controller& other) = default;

    micro_controller& operator=(const micro_controller& other) {
        if (this == &other) {
            return *this;
        }
        return *this;
    }

public:
    virtual inline bool onInit() = 0;
    virtual inline bool onLoop() = 0;
    virtual inline bool onUpdate() { return false; };
    virtual inline void onEnd();
};


END_NP_BLOCK

#endif