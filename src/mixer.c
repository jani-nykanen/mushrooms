#include "mixer.h"

#include "err.h"
#include "sound.h"

#include <stdlib.h>


static u16* buffer = NULL;
static u16 bufferPointer = 0;
static u16 bufferStop = 0;

static u16 activeFreq = 0; 



static void push_sound(u16 freq, u16 len) {

    u16 i;

    // nosound();

    for (i = 0; i < len; i ++) {

        buffer[i] = freq;
    }

    bufferPointer = 0;
    bufferStop = len;

    activeFreq = 0;
}


i16 init_mixer(u16 bufferSize) {

    buffer = (u16*) calloc(bufferSize, sizeof(u16));
    if (buffer == NULL) {

        return 1;
    }

    bufferPointer = 0;
    bufferStop = 0;
    activeFreq = 0;

    return 0;
}


void dispose_mixer() {

    free(buffer);
    nosound();
}


void mixer_update(i16 step) {

    u16 f;

    if (bufferStop == 0) return;

    f = buffer[bufferPointer];
    if (f != activeFreq) {

        activeFreq = f;
        // nosound();
        sound(f);
    }

    if ( (bufferPointer += step) >= bufferStop) {

        nosound();
        bufferStop = 0;
    }
}


void mixer_beep(u16 frequency, u16 length) {

    push_sound(frequency, length);
}


void mixer_quiet() {

    if (bufferStop == 0) return;

    nosound();
    bufferStop = 0;
    bufferPointer = 0;
}
