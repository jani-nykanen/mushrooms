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


static const i16 INTRO_TIME = 200;
static const i16 FLICKER_TIME = 60;


typedef struct {

    Bitmap* bmpFont;
    Bitmap* bmpLogo;

    u16 cursorPos;
    bool optionsDrawn;
    bool backgroundDrawn;
    
    bool inputPassword;
    i16 passwordCursor;
    char passwBuffer [PASSWORD_BUFFER_SIZE];

    bool enterPressed;
    i16 enterTimer;

    bool introPlayed;
    i16 introTimer;

    i16 flickerTimer;

} TitleScreen;


static TitleScreen* title = NULL;


i16 init_title_scene(bool playIntro) {

    static const i16 INTRO_WAIT_TIME = 30;

    title = (TitleScreen*) calloc(1, sizeof(TitleScreen));
    if (title == NULL) {

        ERROR_MALLOC();
        return 1;
    }

    if ((title->bmpFont = load_bitmap("FONT.SPR")) == NULL ||
        (title->bmpLogo = load_bitmap("LOGO.SPR")) == NULL) {

        dispose_title_scene();
        return 1;
    }

    title->cursorPos = 0;
    title->optionsDrawn = false;
    title->backgroundDrawn = false;

    title->passwordCursor = 0;
    title->inputPassword = 0;
    title->passwBuffer[0] = '\0';

    title->enterTimer = 59;
    title->enterPressed = !playIntro;

    title->introPlayed = !playIntro;
    title->introTimer = -INTRO_WAIT_TIME;

    title->flickerTimer = 0;

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

    if (title->flickerTimer > 0) {

        if ((title->flickerTimer -= step) <= 0) {

            if (go_to_game_scene() != 0) {

            return 1;
            }
            dispose_title_scene();
        }
    }

    if (!title->introPlayed) {
        
        if ((title->introTimer += step) >= INTRO_TIME) {

            title->introPlayed = true;
            title->backgroundDrawn = false;
        }
        
        return 0;
    }

    if (!title->enterPressed) {

        title->enterTimer = (title->enterTimer + step) % 60;
        if (keyb_get_normal_key(KEY_RETURN) == STATE_PRESSED) {

            title->enterPressed = true;
            mixer_beep_2_step(40000, 6, 52000, 16);
        }

        return 0;
    }

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

                mixer_beep_2_step(36000, 10, 42000, 20);

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

        mixer_beep_3_step(40000, 6, 42500, 12, 45000, 30);

        title->flickerTimer = FLICKER_TIME;
        title->optionsDrawn = false;

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

    static const i16 TOP_Y = 200-72;
    static const i16 LEFT_X = 160 - 10*4;
    static const i16 OFFSET = 12;

    i16 i;
    str text;

    if (!title->optionsDrawn) {

        fill_rect_fast(LEFT_X/4-2, TOP_Y, 24, 5*12, 0);

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

    static const i16 TOP_Y = 128;

    if (!title->optionsDrawn) {

        fill_rect_fast(0, TOP_Y, 80, 200-TOP_Y, 0);
        title->optionsDrawn = true;

        draw_text_fast(title->bmpFont, "INPUT PASSWORD:",
            40, TOP_Y, -1, true);
        draw_text_fast(title->bmpFont, "(NUMBERS ONLY!)",
            40, TOP_Y+12, -1, true);
    }

    fill_rect_fast(140/4, TOP_Y+32, 6*2, 8, 0);
    draw_colored_text(title->bmpFont, title->passwBuffer,
            140, TOP_Y+32, false, 1);

    if (title->passwordCursor < 5) {

        draw_colored_text(title->bmpFont, "_",
                140 + title->passwordCursor*8, TOP_Y+32, false, 1);
    }
}


static void draw_intro() {

    static u8 COLORS[] = {0, 2, 1, 3};

    i16 color = 3;

    if (title->introTimer < 0) return;

    if (title->introTimer < 40) {

        color = title->introTimer / 10;
    }
    else if (title->introTimer >= INTRO_TIME - 40) {

        color = 3 - min_i16((title->introTimer - (INTRO_TIME-40)) / 10, 3);
    }

    color = COLORS[color];

    draw_colored_text(title->bmpFont, "A game by", 160, 100-10, true, color);
    draw_colored_text(title->bmpFont, "Jani Nyk~nen", 160, 100+1, true, color);
}


static void draw_flickering_new_game() {

    static const i16 TOP_Y = 128;
    static const i16 LEFT_X = 160 - 10*4;

    i16 t = title->flickerTimer % 10;

    if (!title->optionsDrawn) {

        fill_rect_fast(0, TOP_Y, 80, 172-TOP_Y, 0);
        title->optionsDrawn = true;
    }

    draw_colored_text(title->bmpFont, (const str)"NEW GAME",
                LEFT_X, TOP_Y, false, t < 5 ? 3 : 0);
}


static void title_redraw() {

    static const i16 ENTER_Y = 144;

    if (!title->backgroundDrawn) {

        clear_screen(0);
        title->backgroundDrawn = true;

        if (title->introPlayed) {

            draw_colored_text(title->bmpFont, "@2021 Jani Nyk~nen", 160, 190, true, 1);
            draw_sprite_fast(title->bmpLogo, 0, 40-32, 32);
        }
    }

    if (!title->introPlayed) {

        draw_intro();
        return;
    }

    if (title->flickerTimer > 0) {

        draw_flickering_new_game();
        return;
    }

    if (!title->enterPressed) {

        fill_rect_fast(40 - 11, ENTER_Y, 22, 8, 0);
        if (title->enterTimer < 30) {

            draw_text_fast(title->bmpFont, "PRESS ENTER",
                40, ENTER_Y, -1, true);
        }
        return;
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
    dispose_bitmap(title->bmpLogo);

    free(title);

    title = NULL;
}


void title_register_event_callbacks() {

    system_register_callbacks(title_update, title_redraw);
}
