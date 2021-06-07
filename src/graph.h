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
void draw_sprite(Bitmap* bmp, i16 frame, i16 x, i16 y);

void fill_rect_fast(i16 x, i16 y, i16 w, i16 h, u8 color);
void fill_rect(i16 x, i16 y, i16 w, i16 h, u8 color);

void draw_text_fast(Bitmap* font, const str text, 
    i16 x, i16 y, i16 endIndex, bool center);
void draw_text(Bitmap* font, const str text, 
    i16 x, i16 y, i16 endIndex, bool center);

#endif // __GRAPHICS__
