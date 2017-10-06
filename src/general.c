/*
 * RGB2R-C128-Kassenprogramm
 * © 2007-2009 phil_fry, sECuRE, sur5r
 * See LICENSE for license information
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>

#include "general.h"

/*
 * get_input_terminated_by() reads input (handling backspace correctly) until
 * a terminator of |terminators| is encountered or |out| is full (outlen-1
 * characters were read).
 *
 * get_input_terminated_by() returns the terminator it encountered.
 *
 */
input_terminator_t get_input_terminated_by(input_terminator_mask_t terminators,
                                           char *out, BYTE outlen) {
  BYTE i = strlen(out);
  BYTE c, x, y;
  x = wherex() - i;
  y = wherey();
  while (1) {
    c = cgetc();
    if (((terminators & INPUT_TERMINATOR_RETURN) == INPUT_TERMINATOR_RETURN) &&
        (c == PETSCII_CR)) {
      return INPUT_TERMINATOR_RETURN;
    } else if (((terminators & INPUT_TERMINATOR_SPACE) ==
                INPUT_TERMINATOR_SPACE) &&
               (c == PETSCII_SP)) {
      return INPUT_TERMINATOR_SPACE;
    } else if (c == PETSCII_DEL) {
      /* If you are at the left-most position, do nothing */
      if (i == 0)
        continue;
      out[--i] = '\0';
      cputcxy(x + i, y, ' ');
      gotoxy(x + i, y);
      continue;
    }
    if (i == (outlen - 1)) {
      continue;
    }
    cputc(c);
    out[i++] = c;
  }
}

/*
 * Liest (maximal 31) Zeichen ein, bis Enter gedrückt wird.
 * Vorsicht: Es wird ein statischer Buffer benutzt, sodass man
 * das Ergebnis via strdup() retten muss, bevor man get_input()
 * erneut aufruft
 *
 */
char *get_input(void) {
  static char output[32];
  memset(output, '\0', sizeof(output));
  get_input_terminated_by(INPUT_TERMINATOR_RETURN, output, sizeof(output));
  return output;
}

char retry_or_quit(void) {
  char *c;
  do {
    cprintf("\r\nr)etry or q)uit?\r\n");
    c = get_input();
  } while ((*c != 'r') && (*c != 'q'));
  return *c;
}

char *format_euro(char *s, int maxlen, int cent) {
  if (snprintf(s, maxlen, "%3d,%02dEUR", cent / 100, cent % 100) > maxlen)
    return NULL;
  return s;
}

void c128_perror(BYTE c, char *msg) {
  cprintf("\r\nError (Code %d) while: %s\r\nOS Error = %d\r\n", c, msg,
          _oserror);
}
