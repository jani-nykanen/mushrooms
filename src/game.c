#include "game.h"

#include "bitmap.h"
#include "system.h"
#include "err.h"
#include "stage.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


typedef struct {

    Bitmap* bmpSprites;
    Bitmap* bmpTileset;
    Bitmap* bmpFont;
    Bitmap* bmpIcons;

    Stage* stage;

    bool backgroundDrawn;

} GameScene;

static GameScene* game;


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

    game->backgroundDrawn = false;

    return 0;
}


static i16 game_update(i16 step) {

    return 0;
}


static void draw_stage_borders() {

    i16 i;
    i16 w = game->stage->width * 2; 
    i16 h = game->stage->height * 2;

	i16 tx = game->stage->topCorner.x / 4;
	i16 ty = game->stage->topCorner.y;

    for (i = 0; i < w; ++ i) {

        draw_sprite_fast(game->bmpIcons, 0, tx + i*2, ty-8);

        draw_sprite_fast(game->bmpIcons, 2, tx + i*2, ty + h*8);   
    }

    for (i = 0; i < h; ++ i) {

        draw_sprite_fast(game->bmpIcons, 1, tx - 2, ty + i*8);

        draw_sprite_fast(game->bmpIcons, 3, tx + w*2, ty + i*8);
    }

    // Corners
    draw_sprite_fast(game->bmpIcons, 4, tx - 2, ty-8);
    draw_sprite_fast(game->bmpIcons, 5, tx + w*2, ty-8);
    draw_sprite_fast(game->bmpIcons, 6, tx - 2, ty + h*8);
    draw_sprite_fast(game->bmpIcons, 7, tx + w*2, ty + h*8);
}


static void game_redraw() {

    if (!game->backgroundDrawn) {
        
        clear_screen(0);
        draw_stage_borders();

        game->backgroundDrawn = true;
    }

    stage_draw(game->stage, game->bmpTileset);

    draw_text_fast(game->bmpFont, "CGA DEMO 2", 2, 8, -1, false);
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
