#ifndef __PLAYER__
#define __PLAYER__


#include "types.h"
#include "stage.h"


typedef struct {

    Vector2 pos;
    Vector2 target;
    Vector2 renderPos;

    bool moving;
    bool redraw;

    void (*playTurn) (void);
    i16 (*getTurnTime) (void);

    i16 animationFrame;
    i16 animationTimer;
    i16 animationRow;

    i8 loopx;
    i8 loopy;

} Player;


Player* new_player(i16 x, i16 y, 
    void (*playTurn) (void), 
    i16 (*getTurnTime) (void));
void dispose_player(Player* player);

void player_set_starting_position(Player* player, i16 x, i16 y);

void player_update(Player* player, Stage* stage, i16 step);
void player_draw(Player* player, Stage* stage, Bitmap* bmpSprites);


#endif // __PLAYER__
