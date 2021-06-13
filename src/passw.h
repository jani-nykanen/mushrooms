#ifndef __PASSWORD__
#define __PASSWORD__


#include "types.h"


u16 gen_password(u16 levelPackIndex, u16 levelIndex);
bool get_level_from_password(u16 password, u16* levelPackIndex, u16* levelIndex);


#endif // __PASSWORD__
