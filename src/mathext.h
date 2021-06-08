#ifndef __MATHEXT__
#define __MATHEXT__


#include "types.h"


i16 fixed_round(i16 x, i16 d);

i16 min_i16(i16 a, i16 b);
i16 max_i16(i16 a, i16 b);

i16 neg_mod(i16 m, i16 n);

i16 clamp_i16(i16 x, i16 min, i16 max);


#endif // __MATHEXT__
