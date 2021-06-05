#include "system.h"

#include "keyb.h"
#include "keycodes.h"
#include "err.h"

#include <stdlib.h>


static u16 frameCounter = 0;

static UpdateCallback updateCallback;
static RedrawCallback redrawCallback;


static bool check_default_key_shortcuts() {

    if (keyb_get_normal_key(KEY_Q) == STATE_PRESSED &&
        (keyb_get_normal_key(KEY_LCTRL) & STATE_DOWN_OR_PRESSED)) {

        return true;
    }

    return false;
}


static bool main_loop(i16 frameSkip) {

     if ( (frameCounter ++) == frameSkip) {

        frameCounter = 0;
    
        if (updateCallback != NULL) {

            if (updateCallback(frameSkip+1) != 0)
                return true;
        }

        if (check_default_key_shortcuts())
            return true;
            
        keyb_update();

        vblank();

        if (redrawCallback != NULL) {

            redrawCallback();
        }
    }
    else {

        vblank();
    }

    return false;
}



i16 init_system(CGAPalette initialPalette) {

    init_error_system();
    init_graphics(initialPalette);
    init_keyboard_listener();

    return 0;
}


void dispose_system() {

    dispose_graphics();
    dispose_keyboard_listener();
}


void system_register_callbacks(
    UpdateCallback updateCb,
    RedrawCallback redrawCb) {
        
    updateCallback = updateCb;
    redrawCallback = redrawCb;
}


void system_run_application(i16 frameSkip) {

    while(!main_loop(frameSkip));
}
