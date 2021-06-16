#include "title.h"

#include "err.h"
#include "system.h"
#include "graph.h"
#include "keycodes.h"
#include "keyb.h"
#include "mixer.h"
#include "mathext.h"
#include "game.h"

#include <stdlib.h>


typedef struct {

    Bitmap* bmpFont;

    u16 cursorPos;
    bool optionsDrawn;
    bool backgroundDrawn;

} TitleScreen;


static TitleScreen* title = NULL;


i16 init_title_scene() {

    title = (TitleScreen*) calloc(1, sizeof(TitleScreen));
    if (title == NULL) {

        ERROR_MALLOC();
        return 1;
    }

    if ((title->bmpFont = load_bitmap("FONT.SPR")) == NULL) {

        dispose_title_scene();
        return 1;
    }

    title->cursorPos = 0;
    title->optionsDrawn = false;
    title->backgroundDrawn = false;

    return 0;
}


static i16 update_options() {

    i16 oldPos = title->cursorPos;
    if (keyb_get_ext_key(KEY_UP) == STATE_PRESSED) {

        -- title->cursorPos;
    }
    else if (keyb_get_ext_key(KEY_DOWN) == STATE_PRESSED) {

        ++ title->cursorPos;
    }
    title->cursorPos = neg_mod(title->cursorPos, 4);

    if (title->cursorPos != oldPos) {

        mixer_beep(42000, 6);
    }

    if (keyb_get_normal_key(KEY_RETURN) == STATE_PRESSED) {

        switch (title->cursorPos) {

        case 0:

            mixer_beep(38000, 12);
            return 2;

        case 2:

            mixer_toggle(!mixer_is_audio_enabled());
            title->optionsDrawn = false;
            break;

        case 3:

            return 1;
        
        default:
            break;
        }
    
        mixer_beep(38000, 12);
    }

    return 0;
}


static i16 go_to_game_scene() {

    if (init_game_scene() != 0) {

        return 1;
    }
    game_register_event_callbacks();

    return 0;
}


static i16 title_update(i16 step) {

    i16 ret;

    ret = update_options();
    if (ret == 2) {

        if (go_to_game_scene() != 0) {

            return 1;
        }
        dispose_title_scene();
        
        return 0;
    }
    else if (ret == 1) {

        return 1;
    }

    return 0;
}


static void draw_options() {

    static const str OPTIONS[] = {
        "NEW GAME",
        "PASSWORD",
        "AUDIO: ON ",
        "QUIT GAME"
    };
    static const str AUDIO_OFF = "AUDIO: OFF";

    static const i16 TOP_Y = 200-64;
    static const i16 LEFT_X = 160 - 10*4;
    static const i16 OFFSET = 12;

    i16 i;
    str text;

    if (!title->optionsDrawn) {

        for (i = 0; i < 4; ++ i) {
            
            text = (str)OPTIONS[i];
            if (i == 2 && !mixer_is_audio_enabled())
                text = (str)AUDIO_OFF;

            draw_text_fast(title->bmpFont, (const str)text,
                LEFT_X/4, TOP_Y + i*OFFSET, -1, false);
        }

        title->optionsDrawn = true;
    }

    fill_rect_fast(LEFT_X/4 - 5, TOP_Y, 4, OFFSET*4, 0);

    draw_text_fast(title->bmpFont, "\3\4",
        LEFT_X/4-5, TOP_Y + title->cursorPos * OFFSET, -1, false);
}


static void title_redraw() {

    if (!title->backgroundDrawn) {

        clear_screen(0);
        title->backgroundDrawn = true;
    }

    draw_options();
}


void dispose_title_scene() {

    if (title == NULL) return;

    dispose_bitmap(title->bmpFont);

    free(title);

    title = NULL;
}


void title_register_event_callbacks() {

    system_register_callbacks(title_update, title_redraw);
}
