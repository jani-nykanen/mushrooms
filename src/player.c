#include "player.h"

#include "err.h"
#include "graph.h"
#include "keyb.h"
#include "keycodes.h"
#include "game.h"
#include "mathext.h"

#include <stdlib.h>


Player* new_player(i16 x, i16 y, 
    void (*playTurn) (void), 
    i16 (*getTurnTime) (void)) {
    
    Player* player = (Player*) calloc(1, sizeof(Player));
    if (player == NULL) {

        ERROR_MALLOC();
        return NULL;
    }

    player->pos = vec2(x, y);
    player->target = player->pos;
    player->renderPos = vec2(x*16, y*16);

    player->redraw = true;
    player->moving = false;

    player->playTurn = playTurn;
    player->getTurnTime = getTurnTime;

    player->animationFrame = 0;
    player->faceDir = 0;

    return player;
}


void dispose_player(Player* player) {
    
    if (player == NULL) return;

    free(player);
}


void player_set_starting_position(Player* player, i16 x, i16 y) {

    player->pos = vec2(x, y);
    player->target = player->pos;
    player->renderPos = vec2(x*16, y*16);
}


static void player_stop_moving(Player* player, Stage* stage) {

    stage_mark_for_redraw(stage, player->pos.x, player->pos.y);
    stage_mark_for_redraw(stage, player->target.x, player->target.y);   

    player->target.x = neg_mod(player->target.x, stage->width);
    player->target.y = neg_mod(player->target.y, stage->height);

    player->moving = false;
    player->pos = player->target;
    player->renderPos = vec2(player->pos.x*16, player->pos.y*16); 
}


static void player_control(Player* player, Stage* stage, i16 step) {

    i16 dirx = 0;
    i16 diry = 0;
    Vector2 target;
    i16 faceDir;

    if (player->moving) {

        player->redraw = true;
        if (player->getTurnTime() <= 0) {

            player_stop_moving(player, stage);
        }
        else {

            return;
        }
    }
    else if (player->getTurnTime() > 0)
        return;

    if (keyb_get_ext_key(KEY_UP) & STATE_DOWN_OR_PRESSED) {

        diry = -1;
        faceDir = 1;
    }
    else if (keyb_get_ext_key(KEY_DOWN) & STATE_DOWN_OR_PRESSED) {

        diry = 1;
        faceDir = 0;
    }
    else if (keyb_get_ext_key(KEY_LEFT) & STATE_DOWN_OR_PRESSED) {

        dirx = -1;
        faceDir = 3;
    }
    else if (keyb_get_ext_key(KEY_RIGHT) & STATE_DOWN_OR_PRESSED) {

        dirx = 1;
        faceDir = 2;
    }

    target = vec2(player->pos.x + dirx, player->pos.y + diry);

    if ((dirx != 0 || diry != 0) && 
        stage_can_be_moved_to(stage, 
            target.x, target.y, dirx, diry)) {

        player->target = target;
        player->moving = true;
        player->redraw = true;
        player->faceDir = faceDir;

        player->playTurn();
    }

}


static void player_animate(Player* player, Stage* stage, i16 step) {

    i16 turnTime = GAME_TURN_TIME - player->getTurnTime();

    if (!player->moving) return;

    player->animationFrame = player->faceDir*2 + (turnTime / 8);    

    player->renderPos.x = player->pos.x*16 + 
        turnTime * (player->target.x - player->pos.x);
    player->renderPos.y = player->pos.y*16 + 
        turnTime * (player->target.y - player->pos.y);

    stage_mark_for_redraw(stage, player->pos.x, player->pos.y);
    stage_mark_for_redraw(stage, player->target.x, player->target.y);
    
}


void player_update(Player* player, Stage* stage, i16 step) {
    
    player_control(player, stage, step);
    player_animate(player, stage, step);
}


void player_draw(Player* player, Stage* stage, Bitmap* bmpSprites) {

    if (!player->redraw) return;

    player->redraw = false;

    draw_sprite(bmpSprites, player->animationFrame,
        stage->topCorner.x + player->renderPos.x,
        stage->topCorner.y + player->renderPos.y);
}
