#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include <c128.h>
#include <6502.h>

#include "vdc_util.h"
#include "vdc_patch_charset.h"

int main(void) {
  int i = 80;
  unsigned char c[2] = {0x00, 0x20};
  unsigned char pos[5];

  /* char attribute, alternate char set, white, full intensity.
   * set to 0x0f for normal char set
   */
  unsigned char blank = 0x8f;

  videomode(0x80);
  fast();
  clrscr();

  cputsxy(4, 0, "0 1 2 3 4 5 6 7 8 9 A B C D E F");

  /* Manipulate the VDC with IRQs turned off.
   * KERNALs default IRQ handler will also try to read the VDC status
   * register, which could interfere with our code trying to read it.
   */
  SEI();

  // vdc_load_thinfont();
  vdc_patch_charset();

  /* write 16 chars per line */
  do {
    if ((*c % 16) == 0) {
      sprintf(pos, "%02x  ", *c);
      vdc_write_mem(i, pos, 4);
      i = i + 4;
    }

    vdc_write_mem(i, c, 2);
    ++(*c);
    i = i + 2;

    if ((*c % 16) == 0) {
      i = i + 44;
    }
  } while (*c);

  /* clear attribute mem */
  i = 0;
  while (++i <= 2000)
    vdc_write_mem(i + 0x800, &blank, 1);

  CLI();

  /* set cursor, so basic's prompt won't overwrite our output */
  gotoxy(0, 18);
  cputs(EURSYM aUML oUML uUML AUML OUML UUML szLIG);
  return 0;
}
