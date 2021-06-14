#ifndef __MIXER__
#define __MIXER__


#include "types.h"


i16 init_mixer(u16 bufferSize, bool initialState);
void dispose_mixer();

void mixer_update(i16 step);

void mixer_beep(u16 frequency, u16 length);
void mixer_beep_2_step(u16 freq1, u16 len1, u16 freq2, u16 len2);
void mixer_beep_3_step(u16 freq1, u16 len1, u16 freq2, u16 len2, u16 freq3, u16 len3);
void mixer_play_buffered_sound(u16* freqBuf, u16* lenBuf, u16 size);

void mixer_quiet();

void mixer_toggle(bool state);
bool mixer_is_audio_enabled();


#endif // __MIXER__
