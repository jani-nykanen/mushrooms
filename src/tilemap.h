#ifndef __TILEMAP__
#define __TILEMAP__


#include "types.h"


typedef struct {

    u8* data;
    u16 width;
    u16 height;

} Tilemap;


typedef struct {

    Tilemap** maps;
    u8 count;

} TilemapPack;


TilemapPack* load_tilemap_pack(const str path);
void dispose_tilemap_pack(TilemapPack* pack);

void dispose_tilemap(Tilemap* tmap);


#endif // __TILEMAP__
