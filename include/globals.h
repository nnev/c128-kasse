#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#include "c128time.h"

#ifdef IS_GLOBALS_C
#define GLOBAL
#else
#define GLOBAL extern
#endif

void init_globals(void);

GLOBAL uint8_t printing;
GLOBAL struct daytime_t daytime;

#endif /*  GLOBALS_H */
