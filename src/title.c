#include "title.h"

#include "err.h"
#include "system.h"
#include "graph.h"
#include "keycodes.h"
#include "keyb.h"
#include "mixer.h"
#include "mathext.h"
#include "game.h"
#include "passw.h"

#include <stdlib.h>
#include <string.h>


#define PASSWORD_BUFFER_SIZE 12


typedef struct {

    Bitmap* bmpFont;

    u16 cursorPos;
    bool optionsDrawn;
    bool backgroundDrawn;
    
    bool inputPassword;
    i16 passwordCursor;
    char passwBuffer [PASSWORD_BUFFER_SIZE];

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

    title->passwordCursor = 0;
    title->inputPassword = 0;
    title->passwBuffer[0] = '\0';

    return 0;
}


static i16 input_password() {

    i16 i;

    if (title->passwordCursor < 5) {

        for (i = 1; i <= 9; ++ i) {

            if (keyb_get_normal_key(KEY_1 + (i-1)) == STATE_PRESSED) {

                title->passwBuffer[title->passwordCursor ++] = '1' + (i-1);
                mixer_beep(44000, 6);
                break;
            }

            if (keyb_get_normal_key(KEY_0) == STATE_PRESSED) {
                
                title->passwBuffer[title->passwordCursor ++] = '0';
                mixer_beep(44000, 6);
                break;
            }
        }
    }

    if (title->passwordCursor > 0 &&
        keyb_get_normal_key(KEY_BACKSPACE) == STATE_PRESSED) {

        title->passwBuffer[-- title->passwordCursor] = '\0';
        mixer_beep(36000, 6);
    }
    
    if (keyb_get_normal_key(KEY_RETURN) == STATE_PRESSED) {

        return 1;
    }

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

        case 1:

            title->inputPassword = true;
            title->optionsDrawn = false;
            title->passwordCursor = 0;

            memset(title->passwBuffer, 0, 6);

            break;

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

    if (init_game_scene(0, 0) != 0) {

        return 1;
    }
    game_register_event_callbacks();

    return 0;
}


static i16 check_password() {

    u16 levelIndex;
    u16 packIndex;

    u16 num;

    if (title->passwordCursor < 5 ||
        title->passwBuffer[0] > '6') {

        return -1;
    }

    num = (u16)strtoul(title->passwBuffer, NULL, 10);

    if (get_level_from_password(num, &packIndex, &levelIndex)) {

        if (init_game_scene(levelIndex, packIndex) != 0) {

            return 1;
        }
        game_register_event_callbacks();

        return 0;
    }

    return -1;
}


static i16 title_update(i16 step) {

    i16 ret;

    if (title->inputPassword) {

        ret = input_password();
        if (ret == 1) {

            ret = check_password();
            if (ret == -1) {

                title->inputPassword = false;
                title->optionsDrawn = false;
                title->backgroundDrawn = false;

                mixer_beep(32000, 30);
            }
            else if (ret == 0) {

                mixer_beep(42000, 30);

                dispose_title_scene();
                return 0;
            }
            else if (ret == 1) {

                return 1;
            }
        }

        return 0;
    }

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


static void draw_password_input() {

    if (!title->optionsDrawn) {

        fill_rect_fast(0, 100, 80, 100, 0);
        title->optionsDrawn = true;

        draw_text_fast(title->bmpFont, "INPUT PASSWORD:",
            40, 100, -1, true);
        draw_text_fast(title->bmpFont, "(NUMBERS ONLY!)",
            40, 112, -1, true);
    }

    fill_rect_fast(140/4, 128, 6*2, 8, 0);
    draw_colored_text(title->bmpFont, title->passwBuffer,
            140, 128, false, 1);

    if (title->passwordCursor < 5) {

        draw_colored_text(title->bmpFont, "_",
                140 + title->passwordCursor*8, 128, false, 1);
    }
}


static void title_redraw() {

    if (!title->backgroundDrawn) {

        clear_screen(0);
        title->backgroundDrawn = true;
    }

    if (title->inputPassword) {

        draw_password_input();
        return;
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
