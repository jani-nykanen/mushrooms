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
    i16 len;
    char* buffer;

    i16 charX;
    i16 charY;
    bool charChanged;

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

    story->buffer = isEnding ? STORY_ENDING : STORY_INTRO;
    story->len = (i16) strlen(story->buffer);

    story->charX = 0;
    story->charY = 0;
    story->charChanged = true;

    return 0;
}


void dispose_story_scene() {

    if (story == NULL) return;

    dispose_bitmap(story->bmpFont);

    free(story);
}


static i16 story_update(i16 step) {

    static const i16 CHAR_TIME = 4;

    if (keyb_get_normal_key(KEY_RETURN) == STATE_PRESSED) {

        if (story->charPos < story->len-1) {

            story->charPos = story->len-1;
            story->charTimer = 0;
        }
        else {

            if (init_game_scene(0, 0) != 0) {

                return 1;
            }
            game_register_event_callbacks();
            
            dispose_story_scene();

            return 0;
        }
    }

    if (story->charPos < story->len-1) {

        if ((story->charTimer += step) >= CHAR_TIME) {

            story->charTimer -= CHAR_TIME;
            ++ story->charPos;

            story->charChanged = true;

            if (story->buffer[story->charPos] == '\n') {

                ++ story->charY;
                story->charX = -1;
            }
            else {

                ++ story->charX;
            }
        }
    }

    return 0;
}


static void story_redraw() {

    static const i16 LEFT = 20;
    static const i16 TOP = 32;
    static const i16 OFFSET = 10;

    if (!story->backgroundCleared) {

        clear_screen(0);

        story->backgroundCleared = true;
    }

    if (story->charChanged && story->buffer[story->charPos] != '\n') {

        draw_sprite_fast(story->bmpFont,
            story->buffer[story->charPos],
            (LEFT + story->charX*8) / 4,
            TOP + story->charY*OFFSET);

        story->charChanged = false;
    }
}


void story_register_event_callbacks() {

    system_register_callbacks(story_update, story_redraw);
}
