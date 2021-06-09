#ifndef __GAME__
#define __GAME__


#include "types.h"


#define GAME_TURN_TIME 16


i16 init_game_scene();
void dispose_game_scene();

void game_register_event_callbacks();


#endif // __GAME__
