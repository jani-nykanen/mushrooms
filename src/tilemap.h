#ifndef __TILEMAP__
#define __TILEMAP__


#include "types.h"


typedef struct {

    u8* data;
    u16 width;
    u16 height;

} Tilemap;


Tilemap* load_tilemap_from_map_pack(const str path, i16 index);
void dispose_tilemap(Tilemap* tmap);


#endif // __TILEMAP__
