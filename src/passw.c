#include "passw.h"


static const u16 PASSWORDS[] = {

    12321,
    43209,
    32191,
    45328,
    54999,
    21001,
    43129,
    10023,
    50001,
    61149,

    10232,
    32322,
    29997,
    49311,
    35005,
    45003,
    10002,
    60043,
    53010,
    60666,

    11112,
    40022,
    63011,
    33332,
    54541,
    60002,
    40401,
    19992,
    29807,
    59939,
};


u16 gen_password(u16 levelPackIndex, u16 levelIndex) {

    // "Generate" indeed...

    u16 i = levelPackIndex*10 + levelIndex;
    if (i >= 30) return 0;

    return PASSWORDS[i];
}


bool get_level_from_password(u16 password, u16* levelPackIndex, u16* levelIndex) {

    u16 i;

    for (i = 0; i < 30; ++ i) {

        if (PASSWORDS[i] == password) {

            *levelIndex = i % 10;
            *levelPackIndex = i / 10;

            return true;
        }
    }

    return false;
}
