#include "graph.h"

#include "dosutil.h"

#include <stdlib.h>
#include <string.h>


#define COLOR(p) (p | p << 2 | p << 4 | p << 6)


static const u32 CGA_EVEN = 0xB8000000L;
static const u32 CGA_ODD = 0xB8002000L;
static u32 ADDR[2];


static void set_video_mode(u8 mode);
#pragma aux set_video_mode = \
    "mov ah, 0" \
    "int 10h"\
    parm [al];

static void asm__set_palette(u8 index);
#pragma aux asm__set_palette = \
    "mov ah, 11" \
    "mov bh, 1" \
    "int 10h"\
    parm [bl];



void init_graphics(CGAPalette palette) {

    set_video_mode(5);
    set_palette(palette);
}


void dispose_graphics() {

    set_video_mode(2);
}


void vblank() {

    while (port_in(0x3DA) & 8);
    while (!(port_in(0x3DA) & 8));
}


void set_palette(CGAPalette index) {

    asm__set_palette((u8)index);
}


void clear_screen(u8 color) {

    u8 p = COLOR(color);

    memset((void*)CGA_EVEN, p, 8000);
    memset((void*)CGA_ODD, p, 8000);
}
