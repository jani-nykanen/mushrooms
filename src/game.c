#include "game.h"

#include "bitmap.h"
#include "system.h"
#include "err.h"
#include "stage.h"
#include "player.h"
#include "keyb.h"
#include "keycodes.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>



typedef struct {

    Bitmap* bmpSprites;
    Bitmap* bmpTileset;
    Bitmap* bmpFont;
    Bitmap* bmpIcons;

    Stage* stage;
    Player* player;

    bool backgroundDrawn;

    i16 turnTimer;

} GameScene;

static GameScene* game;


static void play_turn() {

    game->turnTimer = GAME_TURN_TIME;
}


static i16 get_turn_time() {

    return game->turnTimer;
}


static i16 game_init() {

    game = (GameScene*) calloc(sizeof(GameScene), 1);
    if (game == NULL) {

        ERROR_MALLOC();
        return 1;
    }

    if ((game->bmpTileset = load_bitmap("TILESET.SPR")) == NULL ||
        (game->bmpSprites = load_bitmap("SPRITES.SPR")) == NULL ||
        (game->bmpIcons = load_bitmap("ICONS.SPR")) == NULL ||
        (game->bmpFont = load_bitmap("FONT.SPR")) == NULL) {

        return 1;
    }

    game->stage = new_stage("LEVELS1.DAT", 0);
    if (game->stage == NULL) {

        return 1;
    }

    game->player = new_player(0, 0, play_turn, get_turn_time);
    if (game->player == NULL) {

        return 1;
    }
    stage_parse_objects(game->stage, (void*)game->player);

    game->backgroundDrawn = false;
    game->turnTimer = 0;

    return 0;
}


static void reset_game() {

    stage_reset(game->stage);
    stage_parse_objects(game->stage, (void*)game->player);

    game->turnTimer = 0;
}


static i16 game_update(i16 step) {

    if (keyb_get_normal_key(KEY_R) == STATE_PRESSED) {

        reset_game();
        return 0;
    }

    if (game->turnTimer > 0) {

        game->turnTimer -= step;
    }

    stage_update(game->stage, step);
    player_update(game->player, game->stage, step);

    return 0;
}


static void draw_stage_borders() {

    i16 i;
    i16 w = game->stage->width; 
    i16 h = game->stage->height;

	i16 tx = game->stage->topCorner.x / 4;
	i16 ty = game->stage->topCorner.y;

    for (i = 0; i < w; ++ i) {

        draw_sprite_fast(game->bmpIcons, 0, tx + i*4, ty-16);

        draw_sprite_fast(game->bmpIcons, 2, tx + i*4, ty + h*16);   
    }

    for (i = 0; i < h; ++ i) {

        draw_sprite_fast(game->bmpIcons, 1, tx - 4, ty + i*16);

        draw_sprite_fast(game->bmpIcons, 3, tx + w*4, ty + i*16);
    }

    // Corners
    draw_sprite_fast(game->bmpIcons, 4, tx - 4, ty-16);
    draw_sprite_fast(game->bmpIcons, 5, tx + w*4, ty-16);
    draw_sprite_fast(game->bmpIcons, 6, tx - 4, ty + h*16);
    draw_sprite_fast(game->bmpIcons, 7, tx + w*4, ty + h*16);
}


// In the case that the player is "looping"
static void draw_overlaying_frame() {

    i16 w = game->stage->width * 4; 
    i16 h = game->stage->height * 16;

	i16 tx = game->stage->topCorner.x / 4;
	i16 ty = game->stage->topCorner.y;

    if (game->player->loopx != 0) {

        ty += game->player->target.y*16;

        draw_sprite_fast(game->bmpIcons, 1, tx - 4, ty);
        draw_sprite_fast(game->bmpIcons, 3, tx + w, ty);
    }

    if (game->player->loopy != 0) {

        tx += game->player->target.x*4;

        draw_sprite_fast(game->bmpIcons, 0, tx, ty - 16);
        draw_sprite_fast(game->bmpIcons, 2, tx, ty + h);
    }
}


static void game_redraw() {

    if (!game->backgroundDrawn) {
        
        clear_screen(0);
        draw_stage_borders();

        draw_text_fast(game->bmpFont, "CGA DEMO 2", 2, 8, -1, false);

        game->backgroundDrawn = true;
    }

    player_pre_draw(game->player, game->stage);
    stage_draw(game->stage, game->bmpTileset);
    player_draw(game->player, game->stage, game->bmpSprites);

    if (game->player->loopx != 0 || game->player->loopy != 0) {

        draw_overlaying_frame();
    }

    stage_clear_redraw_buffer(game->stage);
}


i16 init_game_scene() {

    return game_init();
}


void dispose_game_scene() {

    if (game == NULL) return;

    dispose_stage(game->stage);

    dispose_bitmap(game->bmpTileset);
    dispose_bitmap(game->bmpSprites);
    dispose_bitmap(game->bmpFont);
    dispose_bitmap(game->bmpIcons);

    free(game);
}


void game_register_event_callbacks() {

    system_register_callbacks(
        game_update, game_redraw);
}
