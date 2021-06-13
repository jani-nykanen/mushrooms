#include "enemy.h"

#include "player.h"
#include "graph.h"
#include "err.h"

#include <stdio.h>
#include <stdlib.h>


Enemy* new_enemy(i16 x, i16 y, i16 type, 
    i16 (*getTurnTime)(void), Stage* stage) {
    
    Enemy* enemy = (Enemy*) calloc(1, sizeof(Enemy));
    if (enemy == NULL) {

        ERROR_MALLOC();
        return NULL;
    }

    enemy->pos = vec2(x, y);
    enemy->renderPos = vec2(x*16, y*16);

    enemy->type = type;

    enemy->redraw = true;
    enemy->moving = false;

    enemy->getTurnTime = getTurnTime;

    enemy->dirx = 0;
    enemy->diry = 0;

    if (enemy->type == 0) {

        enemy->dirx = -1 + 2 * (x % 2); 
    }
    else if (enemy->type == 1) {

        enemy->diry = -1 + 2 * (y % 2); 
    }

    stage_mark_solid(stage, x, y, 1);

    return enemy;
}


void dispose_enemy(Enemy* enemy) {

    if (enemy == NULL) return;

    free(enemy);
}


void enemy_update(Enemy* enemy, Stage* stage) {

    i16 t = enemy->getTurnTime();
    i16 i;

    if (enemy->moving && t <= 0) {

        stage_mark_for_redraw(stage, enemy->pos.x, enemy->pos.y);

        enemy->pos.x += enemy->dirx;
        enemy->pos.y += enemy->diry;

        stage_mark_solid(stage, enemy->pos.x, enemy->pos.y, 1);

        enemy->renderPos.x = enemy->pos.x * 16;
        enemy->renderPos.y = enemy->pos.y * 16;

        enemy->moving = false;
        enemy->redraw = true;

        return;
    }

    if (!enemy->moving && t > 0) {

        i = 0;

        // Repeat twice: if then fails, stuck and do not move
        for (i = 0; i < 3; ++ i) {

            if (i == 2) return;

            if (!stage_can_be_moved_to(stage, 
                enemy->pos.x + enemy->dirx, 
                enemy->pos.y + enemy->diry, 0, 0)) {

                enemy->dirx *= -1;
                enemy->diry *= -1;
                continue;
            }
            else {

                break;
            }
        }

        stage_mark_solid(stage, enemy->pos.x, enemy->pos.y, 0);
        
        enemy->moving = true;
    }
    if (!enemy->moving) {

        stage_mark_solid(stage, enemy->pos.x, enemy->pos.y, 1);

        if (stage_does_redraw(stage, enemy->pos.x, enemy->pos.y)) {

            enemy->redraw = true;
        }

        return;
    }

    enemy->renderPos.x = enemy->pos.x * 16 + (16 - t) * enemy->dirx;
    enemy->renderPos.y = enemy->pos.y * 16 + (16 - t) * enemy->diry;

    enemy->redraw = true;
    stage_mark_for_redraw(stage, enemy->pos.x, enemy->pos.y);
}


void enemy_draw(Enemy* enemy, Stage* stage, Bitmap* bmpTileset) {

    i16 frame;
    i16 dx, dy;

    if (!enemy->redraw) return;

    if (enemy->type == 0) {

        frame = enemy->dirx > 0 ? 16 : 17;
    }
    else if (enemy->type == 1) {

        frame = enemy->diry > 0 ? 18 : 19;
    }

    dx = stage->topCorner.x + enemy->renderPos.x;
    dy = stage->topCorner.y + enemy->renderPos.y;

    draw_sprite(bmpTileset, frame, dx, dy);
    
    enemy->redraw = false;
}
