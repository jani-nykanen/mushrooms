#include "game.h"

#include "bitmap.h"
#include "system.h"
#include "err.h"

#include <stdlib.h>
#include <string.h>


typedef struct {

    Bitmap* bmpNoMask;
    Bitmap* bmpFont;

    i16 spriteFrame;
    i16 spriteCounter;

    bool backgroundDrawn;

} GameScene;

static GameScene* game;


static i16 game_init() {

    game = (GameScene*) calloc(sizeof(GameScene), 1);
    if (game == NULL) {

        ERROR_MALLOC();
        return 1;
    }

    if ((game->bmpNoMask = load_bitmap("NOMASK.SPR")) == NULL ||
        (game->bmpFont = load_bitmap("FONT.SPR")) == NULL) {

        return 1;
    }

    game->spriteFrame = 0;
    game->spriteCounter = 0;
    game->backgroundDrawn = false;

    return 0;
}


static i16 game_update(i16 step) {

    static const i16 FRAME_LENGTH = 8;

    if ((game->spriteCounter += step) >= FRAME_LENGTH) {

        game->spriteCounter -= FRAME_LENGTH;
        game->spriteFrame = (game->spriteFrame + 1) % 4;
    }

    return 0;
}


static void game_redraw() {

    i16 frame = game->spriteFrame;
    if (frame == 3) frame = 1;

    if (!game->backgroundDrawn) {
        
        clear_screen(1);
        game->backgroundDrawn = true;
    }

    fill_rect_fast(2, 32, 16, 16, 1);
    draw_sprite(game->bmpNoMask, frame, 8, 32);
    draw_text_fast(game->bmpFont, "CGA DEMO 2", 2, 8, -1, false);
}


i16 init_game_scene() {

    return game_init();
}


void dispose_game_scene() {

    if (game == NULL) return;

    dispose_bitmap(game->bmpNoMask);
    free(game);
}


void game_register_event_callbacks() {

    system_register_callbacks(
        game_update, game_redraw);
}
