#include "player.h"

#include "err.h"
#include "graph.h"
#include "keyb.h"
#include "keycodes.h"
#include "game.h"
#include "mathext.h"
#include "mixer.h"

#include <stdlib.h>


static const i16 MAX_MUSHROOMS = 32;


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

    player->animationFrame = 1;
    player->animationRow = 0;
    player->animate = true;

    player->loopx = 0;
    player->loopy = 0;

    player->mushrooms = (Vector2*) calloc(MAX_MUSHROOMS, sizeof(Vector2));
    if (player->mushrooms == NULL) {

        free(player);

        ERROR_MALLOC();
        return NULL;
    }
    player->mushroomCount = 1;
    player->mushrooms[0] = player->pos;
    player->redrawMushrooms = false;
    player->animateLastMushroom = true;
    player->autoSpawnMushrooms = false;

    return player;
}


void dispose_player(Player* player) {
    
    if (player == NULL) return;

    free(player->mushrooms);
    free(player);
}


void player_set_starting_position(Player* player, i16 x, i16 y) {

    i16 i;

    player->pos = vec2(x, y);
    player->target = player->pos;
    player->renderPos = vec2(x*16, y*16);

    // This should prevent infinite "STUCK!" loop...
    for (i = 1; i < player->mushroomCount; ++ i) {

        player->mushrooms[i] = vec2(0, 0);
    }

    player->mushroomCount = 1;
    player->mushrooms[0] = player->pos;
    player->redrawMushrooms = false;
    player->animateLastMushroom = true;
    player->autoSpawnMushrooms = false;

    player->animationFrame = 1;
    player->animationRow = 0;
    player->animate = true;

    player->redraw = true;
    player->moving = false;

    player->loopx = 0;
    player->loopy = 0;
}


static void shift_mushrooms(Player* player, Stage* stage) {

    Vector2 previous[2];
    i16 pointer = 0;
    i16 i;

    if (player->mushroomCount == 0) return;

    stage_mark_solid(stage, 
        player->mushrooms[player->mushroomCount-1].x,
        player->mushrooms[player->mushroomCount-1].y, 0);

    i = player->mushroomCount-1;
    stage_mark_for_redraw(stage, 
        player->mushrooms[i].x,
        player->mushrooms[i].y);

    previous[0] = player->mushrooms[0];
    player->mushrooms[0] = player->pos;

    for (i = 1; i < player->mushroomCount; ++ i) {

        previous[!pointer] = player->mushrooms[i];
        player->mushrooms[i] = previous[pointer];

        pointer = !pointer;
    }

    if (player->mushroomCount > 1)
        player->redrawMushrooms = true;

    for (i = 0; i < player->mushroomCount; ++ i) {

        stage_mark_solid(stage, 
            player->mushrooms[i].x,
            player->mushrooms[i].y, 1);
    }
}


static void stop_moving(Player* player, Stage* stage) {

    stage_mark_for_redraw(stage, player->pos.x, player->pos.y);
    stage_mark_for_redraw(stage, player->target.x, player->target.y);   

    player->target.x = neg_mod(player->target.x, stage->width);
    player->target.y = neg_mod(player->target.y, stage->height);

    player->animateLastMushroom = true;

    player->moving = false;
    player->pos = player->target;
    player->renderPos = vec2(player->pos.x*16, player->pos.y*16); 

    player->loopx = 0;
    player->loopy = 0;

    shift_mushrooms(player, stage);
}


static void add_mushroom(Player* player, bool soundEffect) {

    if (player->mushroomCount == MAX_MUSHROOMS) return;

    ++ player->mushroomCount;
    player->mushrooms[player->mushroomCount-1] = 
        player->mushrooms[player->mushroomCount-2];

    player->animateLastMushroom = false;

    if (soundEffect)
        mixer_beep_2_step(40000, 6, 45000, 10);
}


static void transform_mushrooms(Player* player, Stage* stage) {

    i16 i;
    i16 x, y;

    for (i = 1; i < player->mushroomCount; ++ i) {

        x = player->mushrooms[i].x;
        y = player->mushrooms[i].y;

        stage_set_static_tile(stage, x, y, 19);
        stage_mark_solid(stage, x, y, 1);
    }

    player->mushroomCount = 1;
    player->redrawMushrooms = false;

    mixer_beep_3_step(38000, 6, 32000, 8, 36000, 16);
}


static u8 player_control(Player* player, Stage* stage, i16 step) {

    i16 dirx = 0;
    i16 diry = 0;
    Vector2 target;
    i16 animationRow = player->animationRow;
    bool stopped = false;
    i16 ret;

    if (player->moving) {

        player->redraw = true;
        if (player->getTurnTime() <= 0) {

            stop_moving(player, stage);
            stopped = true;
        }
        else {

            return 0;
        }
    }
    else if (player->getTurnTime() > 0)
        return 0;

    player->animate = true;

    if (stopped) {

        ret = stage_check_underlying_tile(stage, 
            player->pos.x, player->pos.y,
            &dirx, &diry);

        if (ret == 1) {

            if (!stage_can_be_moved_to(stage, 
                player->pos.x + dirx, 
                player->pos.y + diry, 
                dirx, diry)) {

                dirx = 0;
                diry = 0;
            }
            else {

                player->animate = false;
            }
        }
        else if (ret == 2) {

            add_mushroom(player, true);
        }
        else if (ret == 3) {

            return 2;
        }
        else if (ret == 4) {

            transform_mushrooms(player, stage);
            player->autoSpawnMushrooms = false;
        }
        else if (ret == 5) {

            player->autoSpawnMushrooms = true;
            add_mushroom(player, false);

            mixer_beep_3_step(40000, 6, 38000, 8, 42000, 16);
        }
    }

    if (dirx == 0 && diry == 0) {

        // Check if stuck
        if (stopped &&
            !stage_can_be_moved_to(stage, player->pos.x-1, player->pos.y, -1, 0) &&
            !stage_can_be_moved_to(stage, player->pos.x+1, player->pos.y, 1, 0) &&
            !stage_can_be_moved_to(stage, player->pos.x, player->pos.y-1, 0, -1) &&
            !stage_can_be_moved_to(stage, player->pos.x, player->pos.y+1, 0, 1)) {

            return 1;
        }

        // Check keyboard
        if (keyb_get_ext_key(KEY_UP) & STATE_DOWN_OR_PRESSED) {

            diry = -1;
            animationRow = 1;
        }
        else if (keyb_get_ext_key(KEY_DOWN) & STATE_DOWN_OR_PRESSED) {

            diry = 1;
            animationRow = 0;
        }
        else if (keyb_get_ext_key(KEY_LEFT) & STATE_DOWN_OR_PRESSED) {

            dirx = -1;
            animationRow = 3;
        }
        else if (keyb_get_ext_key(KEY_RIGHT) & STATE_DOWN_OR_PRESSED) {

            dirx = 1;
            animationRow = 2;
        }
    }

    target = vec2(player->pos.x + dirx, player->pos.y + diry);

    if ((dirx != 0 || diry != 0) && 
        stage_can_be_moved_to(stage, 
            target.x, target.y, dirx, diry)) {

        if (player->autoSpawnMushrooms) {

            add_mushroom(player, false);
        }

        player->target = target;
        player->moving = true;
        player->redraw = true;

        if (animationRow != player->animationRow) {

            player->animationFrame = 1;
            player->animationTimer = 0;
            player->animationRow = animationRow;
        }

        player->playTurn();

        player->loopx = 0;
        player->loopy = 0;

        if (player->target.x < 0)
            player->loopx = 1;
        else if (player->target.x >= stage->width)
            player->loopx = -1;

        if (player->target.y < 0)
            player->loopy = 1;
        else if (player->target.y >= stage->height)
            player->loopy = -1;
    }

    return 0;
}


static void player_animate(Player* player, Stage* stage, i16 step) {

    static const i16 FRAME_TIME = 8;

    i16 turnTime = GAME_TURN_TIME - player->getTurnTime();

    if (!player->moving) return;
    
    if (player->animate) {

        player->animationTimer += step; 
        if (player->animationTimer >= FRAME_TIME) {

            player->animationTimer -= FRAME_TIME;
            player->animationFrame = (player->animationFrame + 1) % 4;
        }
    }

    player->renderPos.x = player->pos.x*16 + 
        turnTime * (player->target.x - player->pos.x);
    player->renderPos.y = player->pos.y*16 + 
        turnTime * (player->target.y - player->pos.y);

    stage_mark_for_redraw(stage, player->pos.x, player->pos.y);
    stage_mark_for_redraw(stage, player->target.x, player->target.y);
    
}


u8 player_update(Player* player, Stage* stage, i16 step) {
    
    u8 ret = player_control(player, stage, step);
    if (ret > 0) {

        return ret;
    }
    player_animate(player, stage, step);

    return 0;
}


static void player_draw_mushrooms(Player* player, Stage* stage, Bitmap* bmpSprites) {

    i16 i;
    i16 frame;
    i16 end;

    if (player->mushroomCount <= 1) return;

    // Static mushrooms
    if (player->redrawMushrooms) {

        end = player->mushroomCount - (i16)(player->moving);
        for (i = 1; i < end; ++ i) {

            if (stage_does_redraw(stage, player->mushrooms[i].x, player->mushrooms[i].y)) {

                draw_sprite(bmpSprites, 14, 
                    stage->topCorner.x + player->mushrooms[i].x*16,
                    stage->topCorner.y + player->mushrooms[i].y*16);
            }
        }
        
        player->redrawMushrooms = false;
    }

    if (player->moving) {

        // Disappearing mushrom
        frame = min_i16(player->getTurnTime()/4 + 1, 3);
        if (!player->animateLastMushroom) {

            frame = 3; 
        }
    
        i = player->mushroomCount-1;
        draw_sprite(bmpSprites, 11 + frame,
            stage->topCorner.x + player->mushrooms[i].x*16,
            stage->topCorner.y + player->mushrooms[i].y*16);

         // Appearing mushroom
        frame = min_i16(4 - player->getTurnTime()/4, 3);

        draw_sprite(bmpSprites, 11 + frame,
            stage->topCorner.x + player->pos.x*16,
            stage->topCorner.y + player->pos.y*16);
    }
}


void player_pre_draw(Player* player, Stage* stage) {

    i16 i = player->mushroomCount-1;

    if (player->mushroomCount <= 1) return;

    if (player->redrawMushrooms || player->moving) {

        stage_mark_for_redraw(stage, 
            player->mushrooms[i].x, 
            player->mushrooms[i].y);
        
    }
}


void player_draw(Player* player, Stage* stage, Bitmap* bmpSprites) {

    i16 frame;
    i16 dx, dy;

    if (!player->redraw) return;

    player->redraw = false;

    frame = player->animationFrame;
    if (frame == 3)
        frame = 1;
        
    dx = stage->topCorner.x + player->renderPos.x;
    dy = stage->topCorner.y + player->renderPos.y;

    player_draw_mushrooms(player, stage, bmpSprites);

    draw_sprite(bmpSprites, player->animationRow*3 + frame, dx, dy);

    if (player->loopx != 0 || player->loopy != 0) {

        draw_sprite(bmpSprites, 
            player->animationRow*3 + frame,
            dx + player->loopx * stage->width * 16,
            dy + player->loopy * stage->height * 16);
    }
}


void player_force_redraw(Player* player) {

    player->redraw = true;
    player->redrawMushrooms = true;
}
