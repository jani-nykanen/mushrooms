#ifndef __PLAYER__
#define __PLAYER__


#include "types.h"
#include "stage.h"



typedef struct {

    Vector2 pos;
    Vector2 target;
    Vector2 renderPos;

    bool moving;

    void (*playTurn) (void);
    i16 (*getTurnTime) (void);

    i16 animationFrame;
    i16 animationTimer;
    i16 animationRow;

    bool animate;
    bool redraw;

    i8 loopx;
    i8 loopy;

    Vector2* mushrooms;
    i16 mushroomCount;
    bool redrawMushrooms;
    bool animateLastMushroom;
    bool autoSpawnMushrooms;

} Player;


Player* new_player(i16 x, i16 y, 
    void (*playTurn) (void), 
    i16 (*getTurnTime) (void));
void dispose_player(Player* player);

void player_set_starting_position(Player* player, i16 x, i16 y);

u8 player_update(Player* player, Stage* stage, i16 step);
void player_pre_draw(Player* player, Stage* stage);
void player_draw(Player* player, Stage* stage, Bitmap* bmpSprites);

void player_force_redraw(Player* player);

#endif // __PLAYER__
