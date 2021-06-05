#include "system.h"


i16 test_update(i16 frameSkip) {

    return 0;
}


void test_redraw() {

    clear_screen(1);
}



i16 main(i16 argc, str* argv) {

    init_system(PALETTE_WARM);

    system_register_callbacks(test_update, test_redraw);
    system_run_application(1);

    dispose_system();

    return 0;
}
