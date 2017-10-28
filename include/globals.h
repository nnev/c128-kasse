#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdint.h>

#ifdef _IS_GLOBALS_C
#define GLOBAL
#else
#define GLOBAL extern
#endif

void init_globals(void);

GLOBAL uint8_t printing;

#endif // _GLOBALS_H_
