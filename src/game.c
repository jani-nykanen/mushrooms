#include "game.h"

#include "bitmap.h"
#include "system.h"
#include "err.h"

#include <stdlib.h>
#include <string.h>


typedef struct {

    Bitmap* bmpTest;

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

    return 0;
}


static i16 game_update(i16 step) {

    // ...

    return 0;
}


static void game_redraw() {

    clear_screen(1);

    draw_sprite_fast(game->bmpTest, 1, 4, 16);

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
