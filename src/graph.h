#ifndef __GRAPHICS__
#define __GRAPHICS__


#include "types.h"


typedef enum {

    PALETTE_WARM = 0,
    PALETTE_COOL = 1,
    

} CGAPalette;


void init_graphics(CGAPalette palette);
void dispose_graphics();

void vblank();

void set_palette(CGAPalette index);

void clear_screen(u8 color);

#endif // __GRAPHICS__
