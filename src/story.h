#ifndef __STORY__
#define __STORY__


#include "types.h"


i16 init_story_scene(bool isEnding);
void dispose_story_scene();

void story_register_event_callbacks();


#endif // __STORY__
