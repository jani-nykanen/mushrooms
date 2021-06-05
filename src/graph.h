#ifndef __GRAPHICS__
#define __GRAPHICS__


#include "types.h"
#include "bitmap.h"


typedef enum {

    PALETTE_WARM = 0,
    PALETTE_COOL = 1,
    

} CGAPalette;


void init_graphics(CGAPalette palette);
void dispose_graphics();

void vblank();

void set_palette(CGAPalette index);

void clear_screen(u8 color);

void draw_sprite_fast(Bitmap* bmp, i16 frame, i16 x, i16 y);

#endif // __GRAPHICS__
