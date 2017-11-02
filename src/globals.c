#define IS_GLOBALS_C
#include "globals.h"

void init_globals(void) {
  printing = 1;
  /* initialize daytime global, start the CIA TOD */
  set_time(0, 0, 0, 0);
  return;
}
