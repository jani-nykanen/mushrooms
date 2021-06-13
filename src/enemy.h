#ifndef __ENEMY__
#define __ENEMY__


#include "types.h"
#include "bitmap.h"
#include "stage.h"


typedef struct {

    Vector2 pos;
    Vector2 renderPos;

    bool moving;
    bool redraw;

    i16 dirx;
    i16 diry;

    i16 type;

    i16 (*getTurnTime) (void);

} Enemy;


Enemy* new_enemy(i16 x, i16 y, i16 type, 
    i16 (*getTurnTime)(void), Stage* stage);
void dispose_enemy(Enemy* enemy);

void enemy_update(Enemy* enemy, Stage* stage);
void enemy_draw(Enemy* enemy, Stage* stage, Bitmap* bmpTileset);


#endif // __ENEMY__
