#include "sound.h"

#include "dosutil.h"


void nosound() {

    u8 b = port_in(0x61) & 0xFC;
 	port_out(0x61, b);
}


void sound(u16 frequency) {

    u8 b;

    u32 f = 1193180 / (u32) frequency;

    frequency = (u16)f;

    port_out(0x43, 0x6b);
 	port_out(0x42, frequency);
 
 	b = port_in(0x61) | 3;
 	port_out(0x61, b);
}
