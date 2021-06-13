#ifndef __MIXER__
#define __MIXER__


#include "types.h"


i16 init_mixer(u16 bufferSize);
void dispose_mixer();

void mixer_update(i16 step);

void mixer_beep(u16 frequency, u16 length);
void mixer_beep_2_step(u16 freq1, u16 len1, u16 freq2, u16 len2);

void mixer_quiet();

#endif // __MIXER__
