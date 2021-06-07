#ifndef __STAGE__
#define __STAGE__


#include "tilemap.h"
#include "bitmap.h"


typedef struct {


    TilemapPack* mapPack;
    Tilemap* activeMap;

    u8* staticLayer;
    u8* redrawBuffer;

    u16 width;
    u16 height;
    u16 index;

    Vector2 topCorner;

} Stage;


Stage* new_stage(const str mapPackPath, i16 initialMapIndex);
void dispose_stage(Stage* stage);

void stage_draw(Stage* stage, Bitmap* bmpTileset);


#endif // __STAGE__
