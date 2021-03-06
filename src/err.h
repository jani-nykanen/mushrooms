#ifndef __ERROR__
#define __ERROR__


#include "types.h"


#define ERROR_MAX_LENGTH 64
#define ERROR_MALLOC() error_throw("Memory allocation error!");


void init_error_system();

void error_throw(const str msg);
void error_throw_str(const str msg, str param);

bool error_has_any();
bool error_pop(str buffer);

str error_get_next_direct();


#endif // __ERROR__
