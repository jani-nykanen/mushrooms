#include "tilemap.h"

#include  "err.h"

#include <stdlib.h>
#include <stdio.h>


Tilemap* load_tilemap_from_map_pack(const str path, i16 index) {

    Tilemap* out;
    u16 i;
    u16 width, height;
    FILE* f;
    
    out = (Tilemap*) calloc(1, sizeof(Tilemap));
    if (out == NULL) {

        ERROR_MALLOC();
        return NULL;
    }
    
    f = fopen(path, "rb");
    if (f == NULL) {

        error_throw_str("Failed to open a file in ", path);
        return NULL;
    }

    // Find starting position
    fread(&width, sizeof(u16), 1, f);
    fread(&height, sizeof(u16), 1, f);

    for (i = 0; i < index; ++ i) {

        fseek(f, width*height, SEEK_CUR);
        fread(&width, sizeof(u16), 1, f);
        fread(&height, sizeof(u16), 1, f);
    }

    out->data = (u8*) malloc(width*height);
    if (out->data == NULL) {

        fclose(f);
        free(out);

        ERROR_MALLOC();
        return NULL;
    }

    fread(out->data, 1, width*height, f);

    fclose(f);

    out->width = width;
    out->height = height;

    return out;
}


void dispose_tilemap(Tilemap* tmap) {

    if (tmap == NULL) return;

    if (tmap->data != NULL)
        free(tmap->data);

    free(tmap);
}
