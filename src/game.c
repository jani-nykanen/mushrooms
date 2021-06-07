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


static void game_redraw() {

    if (!game->backgroundDrawn) {
        
        clear_screen(0);
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
    free(game);
}


void game_register_event_callbacks() {

    system_register_callbacks(
        game_update, game_redraw);
}
