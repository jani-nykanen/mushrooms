#include "game.h"

#include "bitmap.h"
#include "system.h"
#include "err.h"
#include "stage.h"
#include "player.h"
#include "keyb.h"
#include "keycodes.h"
#include "mathext.h"
#include "enemy.h"
#include "passw.h"
#include "mixer.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


static const u16 VICTORY_JINGLE_FREQ[] = {
    40000, 32000, 35000, 38000, 41000
};

static const u16 VICTORY_JINGLE_LEN[] = {
    16, 10, 10, 10, 30
};



static const str STAGE_TITLES[] = {
    "\"FIRST STEPS\"",
    "\"AUTOMATION\"",
    "\"MAGIC WALL\"",
    "\"PRESS THE BUTTON\"",
    "\"ON AND OFF\"",
    "\"OUTSIDE\"",
    "\"LOOP\"",
    "\"DIRECTIONS\"",
    "\"BACK AND FORTH\"",
    "\"ALL TOGETHER\"",
};



typedef struct {

    Bitmap* bmpSprites;
    Bitmap* bmpTileset;
    Bitmap* bmpFont;
    Bitmap* bmpIcons;
    TilemapPack* maps;

    Stage* stage;
    Player* player;
    Enemy** enemies;
    i16 enemyCount;

    bool backgroundDrawn;
    bool bordersDrawn;

    i16 turnTimer;

    u8 messageIndex;
    bool messageDrawn;

    bool pauseMenuActive;
    i16 cursorPos;

} GameScene;

static GameScene* game;


static void play_turn() {

    game->turnTimer = GAME_TURN_TIME;
}


static i16 get_turn_time() {

    return game->turnTimer;
}


static i16 game_init() {

    static const i16 INITIAL_STAGE = 3;
    static const i16 INITIAL_LEVEL_PACK = 2;

    char buffer [16];

    game = (GameScene*) calloc(1, sizeof(GameScene));
    if (game == NULL) {

        ERROR_MALLOC();
        return 1;
    }

    snprintf(buffer, 16, "LEVELS%d.DAT", INITIAL_LEVEL_PACK);

    if ((game->maps = load_tilemap_pack(buffer)) == NULL) {

        return 1;
    }

    if ((game->bmpTileset = load_bitmap("TILESET.SPR")) == NULL ||
        (game->bmpSprites = load_bitmap("SPRITES.SPR")) == NULL ||
        (game->bmpIcons = load_bitmap("ICONS.SPR")) == NULL ||
        (game->bmpFont = load_bitmap("FONT.SPR")) == NULL) {

        return 1;
    }

    game->stage = new_stage(game->maps, INITIAL_STAGE-1);
    if (game->stage == NULL) {

        return 1;
    }

    game->player = new_player(0, 0, play_turn, get_turn_time);
    if (game->player == NULL) {

        return 1;
    }

    game->enemies = NULL;
    game->enemyCount = 0;

    if (stage_parse_objects(game->stage, (void*)game->player, 
        (void**)&game->enemies, &game->enemyCount, get_turn_time) == 1) {

        return 1;
    }

    game->backgroundDrawn = false;
    game->bordersDrawn = false;
    game->turnTimer = 0;

    game->messageIndex = 0;
    game->messageDrawn = false;
    game->pauseMenuActive = false;

    return 0;
}


static void clear_enemy_array() {

    i16 i;

    for (i = 0; i < game->enemyCount; ++ i) {

        dispose_enemy(game->enemies[i]);
    }
    free(game->enemies);

    game->enemies = NULL;
}


static i16 reset_game() {

    clear_enemy_array();

    stage_reset(game->stage);
    if (stage_parse_objects(game->stage, (void*)game->player, 
        (void**)&game->enemies, &game->enemyCount, get_turn_time)) {

        return 1;
    }

    game->turnTimer = 0;
    game->messageIndex = 0;
    game->bordersDrawn = false;

    return 0;
}


static i16 next_stage() {

    i16 stageIndex = game->stage->index;

    clear_enemy_array();

    dispose_stage(game->stage);

    game->stage = new_stage(game->maps, (stageIndex+1) % (game->maps->count));
    if (game->stage == NULL) {

        return 1;
    }

    if (stage_parse_objects(game->stage, (void*)game->player, 
        (void**)&game->enemies, &game->enemyCount, get_turn_time)) {

        return 1;
    }

    game->turnTimer = 0;

    game->messageIndex = 0;
    game->messageDrawn = false;
    game->pauseMenuActive = false;

    game->backgroundDrawn = false;
    game->bordersDrawn = false;

    return 0;
}


static i16 update_pause_menu() {

    i16 i;
    i16 oldPos = game->cursorPos;
    if (keyb_get_ext_key(KEY_UP) == STATE_PRESSED) {

        -- game->cursorPos;
    }
    else if (keyb_get_ext_key(KEY_DOWN) == STATE_PRESSED) {

        ++ game->cursorPos;
    }
    game->cursorPos = neg_mod(game->cursorPos, 4);

    if (game->cursorPos != oldPos) {

        mixer_beep(42000, 6);
    }

    if (keyb_get_normal_key(KEY_RETURN) == STATE_PRESSED) {

        switch (game->cursorPos) {

        case 0:

            game->pauseMenuActive = false;

            game->bordersDrawn = false;

            stage_redraw_all(game->stage);
            player_force_redraw(game->player);

            for (i = 0; i < game->enemyCount; ++ i) {

                game->enemies[i]->redraw = true;
            }

            break;

        case 1:

            game->pauseMenuActive = false;
            if (reset_game() != 0) {

                return -1;
            }

            break;

        case 2:

            game->messageDrawn = false;
            mixer_toggle(!mixer_is_audio_enabled());
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


static i16 game_update(i16 step) {

    static const i16 MESSAGE_TIME[] = {60, 120};

    u8 ret;
    i16 i;

    if (game->messageIndex > 0) {

        if ((game->turnTimer += step) >= MESSAGE_TIME[game->messageIndex-1]) {

            if (game->messageIndex == 1) {

                if (reset_game() != 0) {

                    return 1;
                }
            }
            else if(game->messageIndex == 2) {

                if (next_stage() != 0) {

                    return 1;
                }
            }
        }
        return 0;
    }

    if (game->pauseMenuActive) {
        
        if (update_pause_menu()) {

            return 1;
        }
        return 0;
    }

    if (keyb_get_normal_key(KEY_RETURN) == STATE_PRESSED) {

        game->pauseMenuActive = true;
        game->cursorPos = 0;
        game->messageDrawn = false;

        mixer_beep(32000, 14);

        return 0;
    }

    if (keyb_get_normal_key(KEY_R) == STATE_PRESSED) {

        if (reset_game() == 1) {

            return 1;
        }

        mixer_beep_2_step(40000, 10, 30000, 16);

        return 0;
    }

    if (game->turnTimer > 0) {

        game->turnTimer -= step;
    }

    stage_update(game->stage, step);

    for (i = 0; i < game->enemyCount; ++ i) {

        enemy_update(game->enemies[i], game->stage);
    }

    ret = player_update(game->player, game->stage, step);
    if (ret > 0) {

        if (ret == 1) {

            mixer_beep_2_step(32000, 20, 24000, 40);
        }
        else if (ret == 2) {

            mixer_play_buffered_sound((u16*)VICTORY_JINGLE_FREQ, (u16*)VICTORY_JINGLE_LEN, 5);
        }

        game->turnTimer = 0;
        game->messageIndex = ret;
        game->messageDrawn = false;
    }

    return 0;
}


static void draw_stage_borders() {

    i16 i;
    i16 w = game->stage->width; 
    i16 h = game->stage->height;

	i16 tx = game->stage->topCorner.x / 4;
	i16 ty = game->stage->topCorner.y;

    for (i = 0; i < w; ++ i) {

        draw_sprite_fast(game->bmpIcons, 0, tx + i*4, ty-16);

        draw_sprite_fast(game->bmpIcons, 2, tx + i*4, ty + h*16);   
    }

    for (i = 0; i < h; ++ i) {

        draw_sprite_fast(game->bmpIcons, 1, tx - 4, ty + i*16);

        draw_sprite_fast(game->bmpIcons, 3, tx + w*4, ty + i*16);
    }

    // Corners
    draw_sprite_fast(game->bmpIcons, 4, tx - 4, ty-16);
    draw_sprite_fast(game->bmpIcons, 5, tx + w*4, ty-16);
    draw_sprite_fast(game->bmpIcons, 6, tx - 4, ty + h*16);
    draw_sprite_fast(game->bmpIcons, 7, tx + w*4, ty + h*16);
}


// In the case that the player is "looping"
static void draw_overlaying_frame() {

    i16 w = game->stage->width * 4; 
    i16 h = game->stage->height * 16;

	i16 tx = game->stage->topCorner.x / 4;
	i16 ty = game->stage->topCorner.y;

    if (game->player->loopx != 0) {

        ty += game->player->target.y*16;

        draw_sprite_fast(game->bmpIcons, 1, tx - 4, ty);
        draw_sprite_fast(game->bmpIcons, 3, tx + w, ty);
    }

    if (game->player->loopy != 0) {

        tx += game->player->target.x*4;

        draw_sprite_fast(game->bmpIcons, 0, tx, ty - 16);
        draw_sprite_fast(game->bmpIcons, 2, tx, ty + h);
    }
}


static void draw_message() {

    static const str MESSAGES[] = {
        "STUCK!",
        "STAGE CLEAR!"
    };

    i16 len = (i16)strlen(MESSAGES[game->messageIndex-1]);

    i16 x = 160 - len*4;
    i16 y = 100 - 4;

    fill_rect_fast(x/4 - 1, y - 2, len*2 + 2, 12, 0);
    draw_text_fast(game->bmpFont, MESSAGES[game->messageIndex-1],
        x/4, y, -1, false);
}


static void draw_pause_menu() {

    static const str TEXT[] = {
        "RESUME",
        "RESTART",
        "AUDIO: ON",
        "QUIT"
    };
    static const str TEXT_AUDIO_OFF = "AUDIO: OFF";
    static const i16 BOX_WIDTH = 26;
    static const i16 BOX_HEIGHT = 52;

    i16 i;
    i16 x, y;

    str text;

    x = 40 - BOX_WIDTH/2;
    y = 100 - BOX_HEIGHT/2 + 4;

    if (!game->messageDrawn) {

        fill_rect_fast(40 - BOX_WIDTH/2, 100 - BOX_HEIGHT/2, 
            BOX_WIDTH, BOX_HEIGHT, 0);

        for (i = 0; i < 4; ++ i) {

            text = (str)TEXT[i];
            if (i == 2 && !mixer_is_audio_enabled())
                text = (str)TEXT_AUDIO_OFF;

            draw_text_fast(game->bmpFont, (const str)text,
                x + 5, y + i * 12, -1, false);
        }

        game->messageDrawn = true;
    }
    
    fill_rect_fast(40 - BOX_WIDTH/2, 100 - BOX_HEIGHT/2, 
        4, BOX_HEIGHT, 0);

    draw_text_fast(game->bmpFont, "\3\4",
        x, y + game->cursorPos * 12, -1, false);
}


static void draw_stage_info() {

    char buffer [10];

    snprintf(buffer, 10, "LEVEL %d", (i16)game->stage->index+1);

    draw_text_fast(game->bmpFont, buffer, 2, 8, -1, false);
    draw_text_fast(game->bmpFont, "BEGINNER", 40, 8, -1, true);

    draw_colored_text(game->bmpFont, STAGE_TITLES[game->stage->index], 
        160, 200-16, true, 1);

    draw_text_fast(game->bmpFont, "PASSWORD:", 80 - 18, 8, -1, false);

    snprintf(buffer, 10, "%u", gen_password(0, game->stage->index));
    draw_colored_text(game->bmpFont, buffer, 320 - 60, 18, false, 1);
}


static void game_redraw() {

    i16 i;

    if (game->messageIndex > 0 &&
        game->messageDrawn) {

        return;
    }

    if (game->pauseMenuActive) {

        draw_pause_menu();
        return;
    }

    if (!game->backgroundDrawn) {
        
        clear_screen(0);
        draw_stage_info();

        game->backgroundDrawn = true;
        return;
    }

    if (!game->bordersDrawn) {

        draw_stage_borders();
        game->bordersDrawn = true;
    }

    player_pre_draw(game->player, game->stage);
    stage_draw(game->stage, game->bmpTileset);

    for (i = 0; i < game->enemyCount; ++ i) {

        enemy_draw(game->enemies[i], game->stage, game->bmpTileset);
    }

    player_draw(game->player, game->stage, game->bmpSprites);

    if (game->player->loopx != 0 || game->player->loopy != 0) {

        draw_overlaying_frame();
    }

    stage_clear_redraw_buffer(game->stage);

    if (game->messageIndex > 0) {

        if (!game->messageDrawn) {
            
            draw_message(); 
            game->messageDrawn = true;
        }
        return;
    }
}


i16 init_game_scene() {

    return game_init();
}


void dispose_game_scene() {

    i16 i;

    if (game == NULL) return;

    dispose_stage(game->stage);
    dispose_player(game->player);

    dispose_bitmap(game->bmpTileset);
    dispose_bitmap(game->bmpSprites);
    dispose_bitmap(game->bmpFont);
    dispose_bitmap(game->bmpIcons);
    
    dispose_tilemap_pack(game->maps);

    for (i = 0; i < game->enemyCount; ++ i) {

        dispose_enemy(game->enemies[i]);
    }

    free(game);
}


void game_register_event_callbacks() {

    system_register_callbacks(
        game_update, game_redraw);
}
