#include "stage.h"

#include "err.h"
#include "graph.h"
#include "player.h"
#include "mathext.h"
#include "enemy.h"
#include "mixer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define SOLID_TILES_LENGTH 2

static const u8 SOLID_TILES[] = {1, 5};


static const u8 MIN_ARROW_INDEX = 8;
static const i16 X_DIR[] = {0, 0, 1, -1};
static const i16 Y_DIR[] = {-1, 1, 0, 0};

static const i16 START_POS_ANIM_FRAME = 10;


static i16 compute_index(Tilemap* tmap, i16 index) {

    i16 i;
    i16 count = 0;

    for (i = 0; i < tmap->width*tmap->height; ++ i) {

        if (tmap->data[i] == index) {

            ++ count;
        }
    }
    return count;   
}


static i16 count_food(Tilemap* tmap) {

    return compute_index(tmap, 3) + compute_index(tmap, 14) + compute_index(tmap, 15);
}


static Vector2 find_start_pos(Tilemap* tmap) {

    i16 x, y;

    for (x = 0; x < tmap->width; ++ x) {

        for (y = 0; y < tmap->height; ++ y) {

            if (tmap->data[y * tmap->width + x] == 2) {

                return vec2(x, y);
            }
        }
    }
    return vec2(-1, -1);
}


static void set_static_layer(u8* dest, Tilemap* tmap) {

    i16 i;

    for (i = 0; i < tmap->width*tmap->height; ++ i) {

        if ((tmap->data[i] >= 12 && tmap->data[i] <= 13))
            dest[i] = 0;
        else
            dest[i] = tmap->data[i];
    }
}


static void compute_initial_solid_map(Stage* stage) {

    i16 i, j;

    for (i = 0; i < stage->width*stage->height; ++ i) {

        for (j = 0; j < SOLID_TILES_LENGTH; ++ j) {

            if (SOLID_TILES[j] == stage->activeMap->data[i]) {

                stage->solidMap[i] = 1;
                break;
            }
        }
    }
}


static u8* copy_static_layer(Tilemap* tmap) {

    u8* data = (u8*) malloc(tmap->width*tmap->height);
    if (data == NULL) {

        ERROR_MALLOC();
        return NULL;
    }
    set_static_layer(data, tmap);

    return data;
}



Stage* new_stage(TilemapPack* mapPack, i16 initialMapIndex) {

    Stage* stage;

    stage = (Stage*) calloc(1, sizeof(Stage));
    if (stage == NULL) {

        ERROR_MALLOC();
        return NULL;
    }

    stage->index = initialMapIndex;
    stage->activeMap = mapPack->maps[initialMapIndex];

    stage->width = stage->activeMap->width;
    stage->height = stage->activeMap->height;

    stage->staticLayer = copy_static_layer(stage->activeMap);
    if (stage->staticLayer == NULL) {

        free(stage);
        return NULL;
    }

    stage->redrawBuffer = (u8*) calloc(stage->width * stage->height, 1);
    if (stage->redrawBuffer == NULL) {

        ERROR_MALLOC();
        dispose_stage(stage);

        return NULL;
    }

    stage->solidMap = (u8*) calloc(stage->width*stage->height, 1);
    if (stage->solidMap == NULL) {

        ERROR_MALLOC();
        dispose_stage(stage);

        return NULL;
    }
    compute_initial_solid_map(stage);

    memset(stage->redrawBuffer, 1, stage->width*stage->height);

    stage->topCorner = vec2(
        160 - stage->width*8,
        100 - stage->height*8);

    stage->startPos = find_start_pos(stage->activeMap);
    stage->startPosAnimTimer = 0;

    stage->foodLeft = count_food(stage->activeMap);

    return stage;
}


void dispose_stage(Stage* stage) {

    if (stage == NULL) return;

    if (stage->redrawBuffer != NULL)
        free(stage->redrawBuffer);
    if (stage->solidMap != NULL)
        free(stage->solidMap);
    if (stage->staticLayer != NULL)
        free(stage->staticLayer);

    free(stage);
}


void stage_reset(Stage* stage) {

    set_static_layer(stage->staticLayer, stage->activeMap);

    memset(stage->redrawBuffer, 1, stage->width*stage->height);

    stage->foodLeft = count_food(stage->activeMap);

    memset(stage->solidMap, 0, stage->width*stage->height);
    compute_initial_solid_map(stage);
}


void stage_update(Stage* stage, i16 step) {

    if ((stage->startPosAnimTimer += step) >= START_POS_ANIM_FRAME*2) {

        stage->startPosAnimTimer -= START_POS_ANIM_FRAME*2;
    } 
}


void stage_mark_for_redraw(Stage* stage, i16 x, i16 y) {

    x = neg_mod(x, stage->width);
    y = neg_mod(y, stage->height);

    stage->redrawBuffer[y * stage->width + x] = true;
}


void stage_redraw_all(Stage* stage) {

    memset(stage->redrawBuffer, 1, stage->width*stage->height);
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
        0, 16, 22,  0,
        10, 12, 21};

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

    // "Goal"
    if (stage->foodLeft <= 0) {

        index = stage->startPosAnimTimer / START_POS_ANIM_FRAME;

        draw_sprite_fast(bmpTileset, 13 + index, 
            stage->topCorner.x/4 + stage->startPos.x*4,
            stage->topCorner.y + stage->startPos.y*16);
    }
}


void stage_clear_redraw_buffer(Stage* stage) {

    memset(stage->redrawBuffer, 0, stage->width*stage->height);
}


i16 stage_parse_objects(Stage* stage, void* pplayer, 
    void** penemies, i16* enemyCount, 
    i16 (*getTurnTime)(void)) {

    i16 x, y, i;
    u8 tile;
    Enemy** enemies;

    *enemyCount = compute_index(stage->activeMap, 12) + 
                  compute_index(stage->activeMap, 13);

    if (*enemyCount > 0) {

        enemies = (Enemy**) calloc(*enemyCount, sizeof(Enemy*));
        if (enemies == NULL) {

            ERROR_MALLOC();
            return 1;
        }
    }

    i = 0;
    for (y = 0; y < stage->height; ++ y) {

        for (x = 0; x < stage->width; ++ x) {

            tile = stage->activeMap->data[y * stage->width + x];
            if (tile == 2) {

                player_set_starting_position((Player*)pplayer, x, y);
            }
            else if(tile >= 12 && tile <= 13) {

                enemies[i ++] = new_enemy(x, y, tile-12, getTurnTime, stage);
                if (enemies[i-1] == NULL) {

                    *enemyCount = i-1;
                    return 1;
                }
            }
        }
    }

    *penemies = (void*)enemies;

    return 0;
}


bool stage_can_be_moved_to(Stage* stage, i16 x, i16 y, i16 dirx, i16 diry) {

    u8 tile;
    i16 i;

    x = neg_mod(x, stage->width);
    y = neg_mod(y, stage->height);

    i = y * stage->width + x;
    if (stage->solidMap[i] == 1)
        return false;

    tile = stage->staticLayer[i];
    if (tile >= MIN_ARROW_INDEX && tile < MIN_ARROW_INDEX + 4) {

        if ((dirx * X_DIR[tile - MIN_ARROW_INDEX] < 0) ||
                (diry * Y_DIR[tile - MIN_ARROW_INDEX] < 0)) {

            return false;
        }
    }

    return true;
}


static void toggle_special_walls(Stage* stage, i16 j) {

    i16 i;

    mixer_beep_3_step(40000, 6, 35000, 6, 30000, 18);

    for (i = 0; i < stage->width * stage->height; ++ i) {

        if (stage->staticLayer[i] == 4 &&
            stage->solidMap[i] != 1) {

            stage->staticLayer[i] = 5;
            stage->redrawBuffer[i] = true;

            stage->solidMap[i] = 1;
        }
        else if (stage->staticLayer[i] == 5) {

            stage->staticLayer[i] = 4;
            stage->redrawBuffer[i] = true;

            stage->solidMap[i] = 0;
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

    mixer_beep_3_step(30000, 6, 35000, 6, 40000, 18);

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

    if (stage->foodLeft <= 0 &&
        x == stage->startPos.x &&
        y == stage->startPos.y) {

        return 3;
    }

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
        -- stage->foodLeft;

        return 2;
    }
    // Radish, weird things happen
    else if (tile == 14) {

        stage->staticLayer[index] = 0;
        -- stage->foodLeft;

        return 4;
    }
    // Star (at least for now...)
    else if (tile == 15) {

        stage->staticLayer[index] = 0;
        -- stage->foodLeft;

        return 5;
    }
    // Portal
    // (sometimes this does not work, maybe the tile value
    // in the initial position is altered somehow?)
    /*
    else if (tile == 2 && stage->foodLeft <= 0) {

        return 3;
    }
    */

    return 0;
}


void stage_mark_solid(Stage* stage, i16 x, i16 y, u8 state) {

    x = neg_mod(x, stage->width);
    y = neg_mod(y, stage->height);

    stage->solidMap[y * stage->width + x] = state;
}


void stage_set_static_tile(Stage* stage, i16 x, i16 y, u8 id) {

    x = neg_mod(x, stage->width);
    y = neg_mod(y, stage->height);

    stage->staticLayer[y * stage->width + x] = id;
    stage->redrawBuffer[y * stage->width + x] = true;
}
