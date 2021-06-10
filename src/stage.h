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

    Vector2 startPos;
    i16 startPosAnimTimer;

    i16 foodLeft;

} Stage;


Stage* new_stage(const str mapPackPath, i16 initialMapIndex);
void dispose_stage(Stage* stage);

void stage_reset(Stage* stage);

void stage_update(Stage* stage, i16 step);

void stage_draw(Stage* stage, Bitmap* bmpTileset);
void stage_clear_redraw_buffer(Stage* stage);

void stage_mark_for_redraw(Stage* stage, i16 x, i16 y);
bool stage_does_redraw(Stage* stage, i16 x, i16 y);

void stage_parse_objects(Stage* stage, void* pplayer);

bool stage_can_be_moved_to(Stage* stage, i16 x, i16 y, i16 dirx, i16 diry);

bool stage_check_underlying_tile(Stage* stage, i16 x, i16 y, i16* dirx, i16* diry);

#endif // __STAGE__
