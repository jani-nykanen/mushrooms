#include "passw.h"


static const u16 MAX_LEVEL_INDEX = 9;
static const u16 MAX_LEVEL_PACK_INDEX = 2;


static u16 SUITABLE_NUMBER_1 = 3;
static u16 SUITABLE_NUMBER_2 = 97;


u16 gen_password(u16 levelPackIndex, u16 levelIndex) {

    u16 out = (levelPackIndex+1) * SUITABLE_NUMBER_1 * 10000;

    out += levelIndex * SUITABLE_NUMBER_2;
    out += (levelIndex + levelPackIndex) % 10;

    return ~out;
}


bool get_level_from_password(u16 password, u16* levelPackIndex, u16* levelIndex) {

    u16 pack, level;

    password = ~password;

    pack = password / 10000;
    pack /= SUITABLE_NUMBER_1;

    level = password - pack * SUITABLE_NUMBER_1 * 10000;
    level /= 10;
    level /= SUITABLE_NUMBER_2;

    if ((pack + level) % 10 != password % 10 ||
        level > MAX_LEVEL_INDEX ||
        pack > MAX_LEVEL_PACK_INDEX) {

        return false;
    }

    *levelPackIndex = (i16)pack;
    *levelIndex = (i16)level;

    return true;
}
