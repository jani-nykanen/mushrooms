#ifndef __DOS_UTIL__
#define __DOS_UTIL__


#include "types.h"


typedef void (interrupt far *InterruptHandler) (void);


u16 port_in(u16 address);
void port_out(u16 address, u16 value);


void set_vect(u16 num, InterruptHandler handler);
InterruptHandler get_vect(u16 num);


#endif // __DOS_UTIL__
