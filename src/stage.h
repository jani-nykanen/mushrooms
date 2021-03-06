#ifndef __STAGE__
#define __STAGE__


#include "tilemap.h"
#include "bitmap.h"


typedef struct {

    Tilemap* activeMap;

    u8* staticLayer;
    u8* redrawBuffer;
    u8* solidMap;

    u16 width;
    u16 height;
    u16 index;

    Vector2 topCorner;

    Vector2 startPos;
    i16 startPosAnimTimer;

    i16 foodLeft;

} Stage;


Stage* new_stage(TilemapPack* mapPack, i16 initialMapIndex);
void dispose_stage(Stage* stage);

void stage_reset(Stage* stage);

void stage_update(Stage* stage, i16 step);

void stage_draw(Stage* stage, Bitmap* bmpTileset);
void stage_clear_redraw_buffer(Stage* stage);

void stage_mark_for_redraw(Stage* stage, i16 x, i16 y);
void stage_redraw_all(Stage* stage);
bool stage_does_redraw(Stage* stage, i16 x, i16 y);

i16 stage_parse_objects(Stage* stage, void* pplayer, 
    void** penemies, i16* enemyCount,
    i16 (*getTurnTime)(void));

bool stage_can_be_moved_to(Stage* stage, i16 x, i16 y, i16 dirx, i16 diry);
bool stage_check_underlying_tile(Stage* stage, i16 x, i16 y, i16* dirx, i16* diry);

void stage_mark_solid(Stage* stage, i16 x, i16 y, u8 state);

void stage_set_static_tile(Stage* stage, i16 x, i16 y, u8 id);

#endif // __STAGE__
