#include "dosutil.h"


static u16 asm__port_in(u16 address);
#pragma aux asm__port_in = \
    "in ax, dx" \
    parm [dx] \
    value [ax];


static void asm__port_out(u16 address, u16 value);
#pragma aux asm__port_out = \
    "out dx, ax" \
    parm [dx] [ax];


static void asm__set_vect(u16 num, InterruptHandler handler);
#pragma aux asm__set_vect = \
    "mov ah, 25h" \
    "int 21h" \
    parm [al] [ds dx];

static InterruptHandler asm__get_vect(u16 num);
#pragma aux asm__get_vect = \
    "mov ah, 35h" \
    "int 21h" \
    parm [al] \
    value [es bx];


u16 port_in(u16 address) {

    return asm__port_in(address);
}


void port_out(u16 address, u16 value) {

    asm__port_out(address, value);
}



void set_vect(u16 num, InterruptHandler handler) {

    asm__set_vect(num, handler);
}


InterruptHandler get_vect(u16 num) {

    return asm__get_vect(num);
}
