#include "stage.h"

#include "err.h"
#include "graph.h"
#include "player.h"
#include "mathext.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static const u8 MIN_ARROW_INDEX = 8;
static const i16 X_DIR[] = {0, 0, 1, -1};
static const i16 Y_DIR[] = {-1, 1, 0, 0};


static u8* copy_static_layer(Tilemap* tmap) {

    i16 i;
    u8* data = (u8*) malloc(tmap->width*tmap->height);
    if (data == NULL) {

        ERROR_MALLOC();
        return NULL;
    }
    for (i = 0; i < tmap->width*tmap->height; ++ i) {

        if (data[i] == 2)
            data[i] = 0;
        else
            data[i] = tmap->data[i];
    }

    return data;
}


Stage* new_stage(const str mapPackPath, i16 initialMapIndex) {

    Stage* stage;

    stage = (Stage*) calloc(1, sizeof(Stage));
    if (stage == NULL) {

        ERROR_MALLOC();
        return NULL;
    }

    stage->mapPack = load_tilemap_pack(mapPackPath);
    if (stage->mapPack == NULL) {

        free(stage);
        return NULL;
    }

    stage->activeMap = stage->mapPack->maps[initialMapIndex];

    stage->width = stage->activeMap->width;
    stage->height = stage->activeMap->height;

    stage->staticLayer = copy_static_layer(stage->activeMap);
    if (stage->staticLayer == NULL) {

        dispose_tilemap_pack(stage->mapPack);

        free(stage);
        return NULL;
    }

    stage->redrawBuffer = (u8*) malloc(stage->width * stage->height);
    if (stage->redrawBuffer == NULL) {

        ERROR_MALLOC();

        dispose_tilemap_pack(stage->mapPack);

        free(stage->staticLayer);
        free(stage);
        return NULL;
    }

    memset(stage->redrawBuffer, 1, stage->width*stage->height);

    stage->topCorner = vec2(
        160 - stage->width*8,
        100 - stage->height*8);

    return stage;
}


void dispose_stage(Stage* stage) {

    if (stage == NULL) return;

    dispose_tilemap_pack(stage->mapPack);
    free(stage->activeMap);
    free(stage->redrawBuffer);
    free(stage);
}


void stage_mark_for_redraw(Stage* stage, i16 x, i16 y) {

    x = neg_mod(x, stage->width);
    y = neg_mod(y, stage->height);

    stage->redrawBuffer[y * stage->width + x] = true;
}


bool stage_does_redraw(Stage* stage, i16 x, i16 y) {

    x = neg_mod(x, stage->width);
    y = neg_mod(y, stage->height);

    return stage->redrawBuffer[y * stage->width + x];
}


void stage_draw(Stage* stage, Bitmap* bmpTileset) {

    static const i16 TILE_INDEX[] = {
        1, 13, 4, 2, 
        3, 9, 11, 5, 
        6, 7, 8,  0,
        0, 0, 0,  0,
        10, 12};

    i16 x, y;
    i16 dx, dy;
    i16 index;
    i16 tile;

    for (y = 0; y < stage->height; ++ y) {

        for (x = 0; x < stage->width; ++ x) {

            index = y * stage->width + x;
            if (stage->redrawBuffer[index]) {

                dx = stage->topCorner.x + x*16;
                dy = stage->topCorner.y + y*16;

                tile = stage->staticLayer[index];
                if (tile == 0) {

                    fill_rect_fast(dx/4, dy, 4, 16, 1);
                }
                else {

                    tile = TILE_INDEX[tile-1];
                    
                    draw_sprite_fast(bmpTileset, tile-1, dx/4, dy);
                }
            }
        }
    }
}


void stage_clear_redraw_buffer(Stage* stage) {

    memset(stage->redrawBuffer, 0, stage->width*stage->height);
}


void stage_parse_objects(Stage* stage, void* pplayer) {

    i16 x, y;
    u8 tile;

    for (y = 0; y < stage->height; ++ y) {

        for (x = 0; x < stage->width; ++ x) {

            tile = stage->activeMap->data[y * stage->width + x];
            if (tile == 2) {

                player_set_starting_position((Player*)pplayer, x, y);
            }
        }
    }
}


bool stage_can_be_moved_to(Stage* stage, i16 x, i16 y, i16 dirx, i16 diry) {

    #define SOLID_TILES_LENGTH 2

    static const u8 SOLID_TILES[] = {1, 5};

    u8 tile;
    i16 i;

    x = neg_mod(x, stage->width);
    y = neg_mod(y, stage->height);

    tile = stage->staticLayer[y * stage->width + x];

    for (i = 0; i < SOLID_TILES_LENGTH; ++ i) {

        if (SOLID_TILES[i] == tile) {

            return false;
        }

        if (tile >= MIN_ARROW_INDEX && tile < MIN_ARROW_INDEX + 4) {

            if ((dirx * X_DIR[tile - MIN_ARROW_INDEX] < 0) ||
                (diry * Y_DIR[tile - MIN_ARROW_INDEX] < 0)) {

                return false;
            }
        }
    }

    return true;

    #undef SOLID_TILES_LENGTH
}


static void toggle_special_walls(Stage* stage, i16 j) {

    i16 i;

    for (i = 0; i < stage->width * stage->height; ++ i) {

        if (stage->staticLayer[i] == 4) {

            stage->staticLayer[i] = 5;
            stage->redrawBuffer[i] = true;
        }
        else if (stage->staticLayer[i] == 5) {

            stage->staticLayer[i] = 4;
            stage->redrawBuffer[i] = true;
        }
        else if (stage->staticLayer[i] == 17) {

            stage->staticLayer[i] = 6;
            stage->redrawBuffer[i] = true;
        }
    }

    stage->staticLayer[j] = 17;
}


static void reverse_arrow_tiles(Stage* stage, i16 j) {

    static const u8 NEW_TILE_INDEX[] = {9, 8, 11, 10};

    i16 i;
    u8 tile;

    for (i = 0; i < stage->width * stage->height; ++ i) {

        tile = stage->staticLayer[i];
        if (tile >= MIN_ARROW_INDEX && tile < MIN_ARROW_INDEX+4) {

            stage->staticLayer[i] = NEW_TILE_INDEX[tile - MIN_ARROW_INDEX];
            stage->redrawBuffer[i] = true;
        }
        else if (stage->staticLayer[i] == 18) {

            stage->staticLayer[i] = 7;
            stage->redrawBuffer[i] = true;
        }
    }

    stage->staticLayer[j] = 18;
}


bool stage_check_underlying_tile(Stage* stage, i16 x, i16 y, 
    i16* dirx, i16* diry) {

    u8 tile;
    i16 index;

    x = neg_mod(x, stage->width);
    y = neg_mod(y, stage->height);

    index = y * stage->width + x;
    tile = stage->staticLayer[index];

    // Arrows
    if (tile >= MIN_ARROW_INDEX && tile < MIN_ARROW_INDEX + 4) {

        *dirx = X_DIR[tile - MIN_ARROW_INDEX];
        *diry = Y_DIR[tile - MIN_ARROW_INDEX];

        return (*dirx) != 0 || (*diry) != 0;
    }
    // Button to toggle walls on/off
    else if (tile == 6) {

        toggle_special_walls(stage, index);
    }
    // Button to swap arrows
    else if (tile == 7) {

        reverse_arrow_tiles(stage, index);
    }
    // Apple, eat it!
    else if (tile == 3) {

        stage->staticLayer[index] = 0;
        return 2;
    }

    return 0;
}
