#include "title.h"

#include "err.h"
#include "system.h"
#include "graph.h"
#include "keycodes.h"
#include "keyb.h"

#include <stdlib.h>


typedef struct {

    Bitmap* bmpFont;

    u16 cursorPos;
    bool optionsDrawn;

} TitleScreen;


static TitleScreen* title = NULL;



i16 init_title_scene() {

    title = (TitleScreen*) calloc(1, sizeof(TitleScreen));
    if (title == NULL) {

        ERROR_MALLOC();
        return NULL;
    }

    if ((title->bmpFont = load_bitmap("FONT.SPR")) == NULL) {

        dispose_title_scene();
        return 1;
    }

    title->cursorPos = 0;
    title->optionsDrawn = false;

    return 0;
}


void dispose_title_scene() {

    if (title == NULL) return;

    dispose_bitmap(title->bmpFont);

    free(title);

    title = NULL;
}


void title_register_event_callbacks() {

    // ...
}
