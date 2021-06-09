#include "graph.h"

#include "dosutil.h"

#include <stdlib.h>
#include <string.h>


#define COLOR(p) (p | p << 2 | p << 4 | p << 6)


static const u32 CGA_EVEN = 0xB8000000L;
static const u32 CGA_ODD = 0xB8002000L;
static const u32 ADDR[] = {0xB8000000L, 0xB8002000L};


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


static void draw_sprite_no_mask(Bitmap* bmp, 
    i16 frame, i16 dx, i16 dy) {

    i16 x, y, w;
    u8 leftShift, rightShift;
    u8 leftMask, rightMask;
    u32 djump;
    u32 sjump;
    u8 color;

    i16 sy = frame * bmp->frameHeight;
    i16 sw = bmp->frameWidth;
    i16 sh = bmp->frameHeight;

    u8* out;

    w = bmp->width >> 2;
    sw >>= 2;

    rightShift = (u8)((dx % 4) * 2);
    leftShift = (u8)(8 - rightShift);

    leftMask = 255 << leftShift;
    rightMask = 255 >> rightShift;

    dx >>= 2;
    djump = (u32)((dy/2)*80 + dx);
    sjump = (u32)(sy*w);

    for (y = dy; y < dy + sh; ++ y) {   

        out = (u8*)ADDR[y & 1];

        // Left-most pixel
        color = (bmp->pixels[sjump] >> rightShift) & rightMask;
        out[djump] = (color & rightMask) | (out[djump] & leftMask);

        ++ sjump;
        ++ djump;

        // Middle pixels
        for (x = 1; x < sw; ++ x) {
            
            out[djump] = (bmp->pixels[sjump-1] << leftShift) |  
                (bmp->pixels[sjump] >> rightShift);

            ++ sjump;
            ++ djump;
        }

        // Right-most pixel
        color = (bmp->pixels[sjump-1] << leftShift) & leftMask;
        out[djump] = (out[djump] & rightMask) | (color & leftMask);

        djump += 80 * (y & 1) - sw;
    }   
}


static void draw_text_base(
    void (*draw_func) (Bitmap*, i16, i16, i16),
    Bitmap* font, const str text, 
    i16 x, i16 y, i16 endIndex, bool center, i16 div) {

    i16 dx, dy;
    i16 i = 0;
    i16 d = font->frameWidth;
    u8 c;

    if (endIndex < 0)
        endIndex = strlen(text);

    if (center) {

        x -= strlen(text) * (d / div) / 2;
    }
    dx = x;
    dy = y;

    while (i < endIndex && (c = text[i ++]) != '\0') {

        if (c == '\n') {

            dx = x;
            dy += d;
            continue;
        }

        draw_func(font, (i16)c, dx, dy);
        dx += d / div;
    }
}



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


void draw_sprite_fast(Bitmap* bmp, i16 frame, i16 dx, i16 dy) {

    i16 i;
    u32 djump;
    u32 sjump;
    u16 w = bmp->frameWidth / 4;

    i16 sy = bmp->frameHeight * frame;

    if (frame < 0 || frame >= bmp->frameCount) return;

    djump = (u32)((dy/2)*80 + dx);
    sjump = (u32)(sy*w);

    for (i = dy; i < dy + bmp->frameHeight; ++ i) {

        memcpy((void*)(ADDR[i & 1] + djump), 
               (void*)((u32)bmp->pixels + sjump), w);

        djump += 80 * (i & 1);
        sjump += w;
    }  
}


void draw_sprite(Bitmap* bmp, i16 frame, i16 dx, i16 dy) {

    i16 x, y, w;
    u8 leftShift, rightShift;
    u8 leftMask, rightMask;
    u8 alphaMask;
    u32 djump;
    u32 sjump;
    u8 leftBits, rightBits, color;

    i16 sy = frame * bmp->frameHeight;
    i16 sw = bmp->frameWidth;
    i16 sh = bmp->frameHeight;

    u8* out;

    if (frame < 0 || frame >= bmp->frameCount) return;

    if (bmp->mask == NULL) {

        draw_sprite_no_mask(bmp, frame, dx, dy);
        return;
    }

    w = bmp->width >> 2;
    sw >>= 2;

    rightShift = (u8)((dx % 4) * 2);
    leftShift = (u8)(8 - rightShift);

    leftMask = 255 << leftShift;
    rightMask = 255 >> rightShift;

    dx >>= 2;
    djump = (u32)((dy/2)*80 + dx);
    sjump = (u32)(sy*w);

    for (y = dy; y < dy + sh; ++ y) {   

        out = (u8*)ADDR[y & 1];

        // Left-most pixel
        alphaMask = bmp->mask[sjump] >> rightShift;
        color = (bmp->pixels[sjump] >> rightShift) & rightMask;
        out[djump] = (color & alphaMask) | (out[djump] & (~alphaMask));

        ++ sjump;
        ++ djump;

        // Middle pixels
        for (x = 1; x < sw; ++ x) {
            
            alphaMask = (bmp->mask[sjump-1] << leftShift) | 
                (bmp->mask[sjump] >> rightShift);

            leftBits = ((bmp->pixels[sjump-1] << leftShift) & leftMask);
            rightBits = ((bmp->pixels[sjump] >> rightShift) & rightMask);
            color = leftBits | rightBits;

            out[djump] = (color & alphaMask) | (out[djump] & (~alphaMask));

            ++ sjump;
            ++ djump;
        }

        // Right-most pixel
        alphaMask = bmp->mask[sjump-1] << leftShift;
        color = (bmp->pixels[sjump-1] << leftShift) & leftMask;
        out[djump] = (color & alphaMask) | (out[djump] & (~alphaMask));

        djump += 80 * (y & 1) - sw;
    }   
}


void fill_rect_fast(i16 x, i16 y, i16 w, i16 h, u8 color) {

    i16 i;
    u32 jump;
    u8 p = COLOR(color);

    jump = (u32)((y/2)*80 + x);
    for (i = y; i < y+h; ++ i) {

        memset((void*)(ADDR[i & 1] + jump), p, w);
        jump += 80 * (i & 1);
    }
}


void fill_rect(i16 dx, i16 dy, i16 w, i16 h, u8 color) {

    i16 y;
    u8 leftMask, rightMask;
    u32 offset;
    u8 p = COLOR(color);
    u8* out;

    offset = (u32)((dy/2)*80 + (dx >> 2));

    if ((dx % 4) + w <= 4) {

        leftMask = 255 << (8 - w * 2);
        leftMask >>= ((dx % 4)*2);

        for (y = dy; y < dy + h; ++ y) {

            out = (u8*)ADDR[y & 1];
            out[offset] = (out[offset] & (~leftMask)) | (p & leftMask);

            offset += 80 * (y & 1);
        }
        return;
    }

    leftMask = 255 >> ((dx % 4)*2);
    rightMask = 255 >> (((dx + w) % 4)*2);

    w >>= 2;
    dx >>= 2;

    for (y = dy; y < dy + h; ++ y) {

        out = (u8*)ADDR[y & 1];

        // Left-most pixel(s)
        out[offset] = (out[offset] & (~leftMask)) | (p & leftMask);
        // Right-most pixel(s)
        out[offset + w] = (out[offset + w] & rightMask) | (p & (~rightMask));

        // Middle pixels
        if (w > 1)
            memset((void*)(out + offset + 1), p, w - 1);

        offset += 80 * (y & 1);
    }
}


void draw_text_fast(Bitmap* font, const str text, 
    i16 x, i16 y, i16 endIndex, bool center) {

    draw_text_base(draw_sprite_fast,
        font, text, x, y, 
        endIndex, center, 4);
}


void draw_text(Bitmap* font, const str text, 
    i16 x, i16 y, i16 endIndex, bool center) {

    draw_text_base(draw_sprite,
        font, text, x, y, 
        endIndex, center, 4);
}
