#include "mixer.h"

#include "err.h"
#include "sound.h"

#include <stdlib.h>
#include <stdio.h>


static u16* buffer = NULL;
static u16 bufferPointer = 0;
static u16 bufferStop = 0;

static u16 activeFreq = 0; 
static bool playNext;

static bool enabled = true;


static void clear() {

    bufferStop = 0;
    bufferPointer = 0;
}


static void push_sound(u16 freq, u16 len) {

    u16 i;

    // nosound();

    for (i = bufferStop; i < bufferStop + len; i ++) {

        buffer[i] = freq;
    }

    bufferPointer = 0;
    bufferStop += len;

    playNext = true;
    activeFreq = buffer[0];
}


i16 init_mixer(u16 bufferSize, bool initialState) {

    buffer = (u16*) calloc(bufferSize, sizeof(u16));
    if (buffer == NULL) {

        return 1;
    }

    bufferPointer = 0;
    bufferStop = 0;
    activeFreq = 0;
    playNext = false;

    enabled = initialState;

    return 0;
}


void dispose_mixer() {

    free(buffer);
    nosound();
}


void mixer_update(i16 step) {

    u16 f;

    if (bufferStop == 0 || !enabled) return;

    if (playNext) {

        sound(activeFreq);
    }

    f = buffer[bufferPointer];
    if (f != activeFreq) {

        activeFreq = f;
        playNext = true;

        nosound();
    }

    if ( (bufferPointer += step) >= bufferStop) {

        nosound();
        bufferStop = 0;
        activeFreq = 0;
    }
}


void mixer_beep(u16 frequency, u16 length) {
    
    if (!enabled) return;

    clear();
    push_sound(frequency, length);
}


void mixer_beep_2_step(u16 freq1, u16 len1, u16 freq2, u16 len2) {

    if (!enabled) return;

    clear();
    push_sound(freq1, len1);
    push_sound(freq2, len2);
}


void mixer_beep_3_step(u16 freq1, u16 len1, u16 freq2, u16 len2, u16 freq3, u16 len3) {

    if (!enabled) return;

    clear();
    push_sound(freq1, len1);
    push_sound(freq2, len2);
    push_sound(freq3, len3);
}


void mixer_play_buffered_sound(u16* freqBuf, u16* lenBuf, u16 size) {

    u16 i;

    if (!enabled) return;

    clear();

    for (i = 0; i < size; ++ i) {

        push_sound(freqBuf[i], lenBuf[i]);
    }
}


void mixer_quiet() {

    if (bufferStop == 0) return;

    nosound();
    bufferStop = 0;
    bufferPointer = 0;
}


void mixer_toggle(bool state) {

    enabled = state;

    if (!state) {

        nosound();
        clear();
    }
}


bool mixer_is_audio_enabled() {

    return enabled;
}
