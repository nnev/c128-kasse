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
#include <stdint.h>

#include "general.h"
#include "vdc_patch_charset.h"

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

BYTE cgetn_input(char *s, BYTE len) {
  memset(s, '\0', len);
  get_input_terminated_by(INPUT_TERMINATOR_RETURN, s, len);
  return strlen(s);
}

int16_t cget_number(int16_t default_val) {
  char c;
  int x, y;
  uint8_t num_chars = 0;
  char buf[6] = {0, 0, 0, 0, 0, 0};
  int i = 0;
  x = wherex();
  y = wherey();
  while (1) {
    c = cgetc();

    /* Enter */
    if (c == PETSCII_CR)
      break;

    /* Backspace */
    if (c == PETSCII_DEL) {
      if (i == 0)
        continue;
      buf[--i] = '\0';
      cputcxy(x + i, y, ' ');
      gotoxy(x + i, y);
      continue;
    }

    /* Abort */
    if (c == PETSCII_ESC) {
      return default_val;
    }

    /* end of buffer? wait for user to press RETURN */
    if (i == (sizeof(buf) - 1))
      continue;

    /* match either numbers or iff it's the first entered char a minus sign */
    if ((c >= PETSCII_0 && c <= PETSCII_9) || (c == '-' && i == 0)) {
      buf[i] = c;
      ++i;
      ++num_chars;
      cputc(c);
    }
  }

  if (num_chars == 0) {
    return default_val;
  } else if ((num_chars == 1) && (c == '-')) {
    return default_val;
  }

  return atoi(buf);
}

void cget_return() {
  while (cgetc() != PETSCII_CR) {
  }
  return;
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
  if (snprintf(s, maxlen, "%3d,%02d" EURSYM, cent / 100, cent % 100) > maxlen)
    return NULL;
  return s;
}

void c128_perror(BYTE c, char *msg) {
  cprintf("\r\nError (Code %d) while: %s\r\nOS Error = %d\r\n", c, msg,
          _oserror);
}
