#include "story.h"

#include "err.h"
#include "system.h"
#include "graph.h"
#include "keycodes.h"
#include "keyb.h"
#include "mixer.h"
#include "mathext.h"
#include "game.h"

#include <stdlib.h>
#include <string.h>


static const str STORY_INTRO = 
"Welcome to the world of MUSHROOMS!\n"
"This game does not really have a\n"
"story, but let us pretend that it\n"
"does, because it is the only way to\n"
"give you, the player, enough moti-\n"
"vation to solve 30 crappy puzzles.";


static const str STORY_ENDING = 
"Congratulations! You have beaten\n"
"every single level in the game.\n"
"Your life must be really boring!";


typedef struct {

    Bitmap* bmpFont;

    bool isEnding;
    bool backgroundCleared;

    i16 charPos;
    i16 charTimer;

} StoryScene;

static StoryScene* story = NULL;


i16 init_story_scene(bool isEnding) {

    if ((story->bmpFont = load_bitmap("FONT.SPR")) == NULL) {

        return 1;
    }

    story->isEnding = isEnding;
    story->backgroundCleared = false;

    story->charPos = 0;
    story->charTimer = 0;

    return 0;
}


void dispose_story_scene() {

    if (story == NULL) return;

    dispose_bitmap(story->bmpFont);

    free(story);
}


static i16 story_update(i16 step) {

    return 0;
}


static void story_redraw() {

    if (!story->backgroundCleared) {

        clear_screen(0);

        story->backgroundCleared = true;
    }
}


void story_register_event_callbacks() {

    system_register_callbacks(story_update, story_redraw);
}
