#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#include "c128time.h"
#include "kasse.h"

#ifdef IS_GLOBALS_C
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL uint8_t printing;
GLOBAL enum kasse_menu kasse_menu;
GLOBAL struct daytime_t daytime;

#endif /*  GLOBALS_H */
