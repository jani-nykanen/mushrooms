#include "system.h"
#include "game.h"


i16 main(i16 argc, str* argv) {

    if (init_system(PALETTE_WARM) != 0 ||
        init_game_scene() != 0) {

        return 1;
    }

    game_register_event_callbacks();
    system_run_application(1);

    dispose_system();

    return 0;
}
