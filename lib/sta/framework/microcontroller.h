#ifndef _STA_ARDUINO_FRAME_
#define _STA_ARDUINO_FRAME_

#include "sta.h"

#pragma GCC visibility push(default)

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

    inline int32 interval() const {
        return this->onUpdateInterval;
    }

public:
    virtual inline bool onInit() = 0;
    virtual inline bool onLoop() = 0;
    virtual inline bool onUpdate() { return false; };
    virtual inline void onEnd() {}
protected:
    int32 onUpdateInterval = 1000;
};


END_NP_BLOCK

#endif