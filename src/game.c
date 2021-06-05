#include "game.h"

#include "bitmap.h"
#include "system.h"
#include "err.h"

#include <stdlib.h>
#include <string.h>


typedef struct {

    Bitmap* bmpTest;
    i16 spriteFrame;
    i16 spriteCounter;

} GameScene;

static GameScene* game;


static i16 game_init() {

    game = (GameScene*) calloc(sizeof(GameScene), 1);
    if (game == NULL) {

        ERROR_MALLOC();
        return 1;
    }

    if ((game->bmpTest = load_bitmap("TEST1.SPR")) == NULL) {

        return 1;
    }

    game->spriteFrame = 0;
    game->spriteCounter = 0;

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

    clear_screen(1);

    draw_sprite(game->bmpTest, 1 + frame, 4, 16);

}


i16 init_game_scene() {

    return game_init();
}


void dispose_game_scene() {

    if (game == NULL) return;

    dispose_bitmap(game->bmpTest);
    free(game);
}


void game_register_event_callbacks() {

    system_register_callbacks(
        game_update, game_redraw);
}
