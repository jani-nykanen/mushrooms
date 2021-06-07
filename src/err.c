#include "err.h"

#include <stdio.h>

#define MAX_error_COUNT 8

static char errorMessages [MAX_error_COUNT] [ERROR_MAX_LENGTH];
static u8 errCount = 0;


void init_error_system() {

    errCount = 0;
}


void error_throw(const str msg) {

    if (errCount == MAX_error_COUNT)
        return;

    snprintf(errorMessages[errCount ++], ERROR_MAX_LENGTH, "%s", msg);
}


void error_throw_str(const str msg, str param) {

    if (errCount == MAX_error_COUNT)
        return;

    snprintf(errorMessages[errCount ++], ERROR_MAX_LENGTH, "%s%s", msg, param);
}


bool error_has_any() {

    return errCount > 0;
}


bool error_pop(str buffer) {

    if (errCount == 0) 
        return false;

    snprintf(buffer, ERROR_MAX_LENGTH, "%s", errorMessages[-- errCount]);
    return true;
}
