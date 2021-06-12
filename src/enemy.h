#ifndef __ENEMY__
#define __ENEMY__


#include "types.h"
#include "bitmap.h"
#include "stage.h"


typedef struct {

    Vector2 pos;
    Vector2 target;
    Vector2 renderPos;

    bool moving;
    i16 dirx;
    i16 diry;

    i16 (*getTurnTime) (void);

} Enemy;


void enemy_update(Enemy* enemy, Stage* stage);
void enemy_draw(Enemy* enemy, Stage* stage, Bitmap* bmpTileset);


#endif // __ENEMY__
