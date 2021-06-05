#ifndef __SYSTEM__
#define __SYSTEM__


#include "types.h"
#include "graph.h"


typedef i16 (*UpdateCallback)(i16);
typedef void (*RedrawCallback)(void);


i16 init_system(CGAPalette initialPalette);
void dispose_system();

void system_register_callbacks(
    UpdateCallback updateCb,
    RedrawCallback redrawCb);
void system_run_application(i16 frameSkip);



#endif // __SYSTEM__
