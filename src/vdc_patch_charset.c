#include <string.h>

#include "charset_umlauts.h"
#include "vdc_util.h"

void vdc_patch_charset(void) {
  int i = 0;
  /* start of the shifted charset */
  const unsigned int base_addr = 0x3000;
  /*                      EUR   Ä     Ö     Ü     ä     ö     ü     ß */
  unsigned char map[8] = {0x62, 0x5C, 0x5E, 0x5F, 0x66, 0x68, 0x69, 0x7F};
  unsigned char *p = map;

  for (; i < sizeof(chars_umlauts); i += 8) {
    vdc_write_mem(base_addr + *p * 16, chars_umlauts + i, 8);
    ++p;
  }
}
