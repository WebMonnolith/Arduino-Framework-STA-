
#ifndef _STA_ENTRY_POINT_
#define _STA_ENTRY_POINT_

#include "sta.h"
#include "microcontroller.h"
#include "./core/memory.h"

BEGIN_NP_BLOCK
extern sta::micro_controller* create_app();
END_NP_BLOCK

static sta::unique_ptr<sta::micro_controller> app;
static bool breakLoop = false;

void setup() {
    app.reset(sta::create_app());
    while(!app->onInit());
    sta::begin(9600);
}

void loop() {
    if (breakLoop) return;
    if (!app->onUpdate()) 
        breakLoop = true;
    
    if (breakLoop)
        app->onEnd();
}

#endif