#ifndef __TYPES__
#define __TYPES__


#define INTEGER(type, bits) typedef type i##bits; \
typedef unsigned type u##bits;

typedef unsigned char byte;

INTEGER(char, 8)
INTEGER(short, 16)
INTEGER(int, 32)
INTEGER(long, 64)

typedef float f32;
typedef double f64;

typedef char* str;


#endif // __TYPES__
