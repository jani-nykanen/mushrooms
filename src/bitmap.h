
#ifndef __BITMAP__
#define __BITMAP__

#include "types.h"


typedef struct {

    byte* pixels;
    byte* mask;

    // "Data size", to be used for faster
    // memory access when rendering only
    u16 width;
    u16 height;

    u16 frameWidth;
    u16 frameHeight;
    u16 frameCount;

} Bitmap;


Bitmap* load_bitmap(const str path);
void dispose_bitmap(Bitmap* bmp);


#endif // __BITMAP__
