
#ifndef _STA_ENTRY_POINT_
#define _STA_ENTRY_POINT_

#include "sta.h"
#include "microcontroller.h"
#include "./core/memory.h"

using namespace sta::literals;

BEGIN_NP_BLOCK
extern sta::micro_controller* create_app();
extern sta::int32 set_interval(); 
END_NP_BLOCK

static sta::int32 loop_interval = 1000;
static sta::uint64 previous = 0_sdp;
static sta::unique_ptr<sta::micro_controller> app;
static bool breakLoop = false;

void setup() {
    app.reset(sta::create_app());
    while(!app->onInit());
    sta::begin(9600);
}

void loop() {
    if (breakLoop) return;
    if (!app->onLoop()) 
        breakLoop = true;
    
    if (breakLoop)
        app->onEnd();

    loop_interval = sta::set_interval();
    sta::safe_delay_point current;
    if (sta::delta(current, previous) > loop_interval) {
        app->onUpdate();
        previous = current;
    }
}

#endif