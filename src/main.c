#include "system.h"
#include "game.h"
#include "err.h"

#include <stdio.h>


static void print_errors() {

    char errBuffer [ERROR_MAX_LENGTH];

    while (error_pop(errBuffer)) {

        printf("ERROR: %s\n", errBuffer);
    }
}


i16 main(i16 argc, str* argv) {

    if (init_system(PALETTE_WARM) != 0 ||
        init_game_scene() != 0) {

        dispose_system();
        print_errors();

        return 1;
    }

    game_register_event_callbacks();
    system_run_application(1);

    dispose_game_scene();
    dispose_system();

    print_errors();

    return 0;
}
