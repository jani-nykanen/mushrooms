#include "bitmap.h"

#include "err.h"

#include <stdio.h>
#include <stdlib.h>


Bitmap* load_bitmap(const str path) {

    Bitmap* bmp;
    FILE* f;
    byte hasMask;

    f = fopen(path, "rb");
    if (f == NULL) {

        error_throw_str("Failed to open a file in ", (str)path);
        return NULL;
    }

    bmp = (Bitmap*) calloc(1, sizeof(Bitmap));
    if (bmp == NULL) {

        ERROR_MALLOC();
        fclose(f);
        return NULL;
    }

    fread(&bmp->frameWidth, sizeof(u16), 1, f);
    fread(&bmp->frameHeight, sizeof(u16), 1, f);
    fread(&bmp->frameCount, sizeof(u16), 1, f);
    fread(&hasMask, 1, 1, f);

    bmp->width = bmp->frameWidth;
    bmp->height = bmp->frameHeight * bmp->frameCount;

    bmp->pixels = (byte*) calloc(bmp->width * bmp->height / 4, sizeof(byte));
    if (bmp->pixels == NULL) {

        free(bmp);

        ERROR_MALLOC();
        fclose(f);
        return NULL;
    }

    fread(bmp->pixels, 1, bmp->width * bmp->height / 4, f);

    if (hasMask) {

        bmp->mask = (byte*) calloc(bmp->width * bmp->height / 4, sizeof(byte));
        if (bmp->mask == NULL) {

            free(bmp->pixels);
            free(bmp);

            ERROR_MALLOC();
            fclose(f);
            return NULL;
        }
        fread(bmp->mask, 1, bmp->width * bmp->height / 4, f);
    }
    else {

        bmp->mask = NULL;
    }

    fclose(f);
    return bmp;
}   


void dispose_bitmap(Bitmap* bmp) {

    if (bmp == NULL) return;

    if (bmp->mask != NULL) {

        free(bmp->mask);
    }

    free(bmp->pixels);
    free(bmp);
}
