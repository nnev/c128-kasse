#ifndef GENERAL_H
#define GENERAL_H

#include <peekpoke.h>
#include <stdint.h>
#include "vdc_patch_charset.h"

typedef unsigned char BYTE;
typedef enum {
  INPUT_TERMINATOR_RETURN = (1 << 0),
  INPUT_TERMINATOR_SPACE = (1 << 1),
} input_terminator_t;
typedef input_terminator_t input_terminator_mask_t;
input_terminator_t get_input_terminated_by(input_terminator_mask_t terminators,
                                           char *out, BYTE outlen);
char *get_input(void);
BYTE cgetn_input(char *s, BYTE len);
int16_t cget_number(int16_t default_val);
void cget_return(void);
uint8_t cget_nickname(char *buf, uint8_t len);
char retry_or_quit(void);
char *format_euro(char *s, int maxlen, int32_t cent);
void c128_perror(BYTE, char *);
extern BYTE _oserror;

#define KASSE_AUTHORS "phil_fry, sECuRE, sur5r, mxf, ada"

#define MENU_KEY(key, label)                                                   \
  textcolor(TC_YELLOW);                                                        \
  cprintf(key);                                                                \
  textcolor(TC_LIGHT_GRAY);                                                    \
  cprintf(") " label);

#define ERROR(message)                                                         \
  do {                                                                         \
    textcolor(TC_RED);                                                         \
    cprintf(message);                                                          \
    textcolor(TC_LIGHT_GRAY);                                                  \
  } while (0)

#define GOOD(message)                                                          \
  do {                                                                         \
    textcolor(TC_GREEN);                                                       \
    cprintf(message);                                                          \
    textcolor(TC_LIGHT_GRAY);                                                  \
  } while (0)

/* C128 color codes, see PDF page 127 of
 * http://www.pagetable.com/docs/Commodore%20128%20Programmer%27s%20Reference%20Guide.pdf
 */
#define TC_BLACK 0
#define TC_WHITE 1
#define TC_RED 2
#define TC_CYAN 3
#define TC_PURPLE 4
#define TC_GREEN 5
#define TC_BLUE 6
#define TC_YELLOW 7
#define TC_ORANGE 8
#define TC_BROWN 9
/* This is the good red */
#define TC_LIGHT_RED 10
#define TC_DARK_GRAY 11
#define TC_MEDIUM_GRAY 12
/* This is the good green */
#define TC_LIGHT_GREEN 13
#define TC_LIGHT_BLUE 14
#define TC_LIGHT_GRAY 15

/* Carriage return */
#define PETSCII_CR 13
/* Delete */
#define PETSCII_DEL 20
/* Escape */
#define PETSCII_ESC 27
/* Space */
#define PETSCII_SP 32
#define PETSCII_0 48
#define PETSCII_1 49
#define PETSCII_2 50
#define PETSCII_3 51
#define PETSCII_4 52
#define PETSCII_5 53
#define PETSCII_6 54
#define PETSCII_7 55
#define PETSCII_8 56
#define PETSCII_9 57

#define VIDEOMODE (((*(BYTE *)0xD7) == 0x80) ? 80 : 40)

/* "-999,99€" */
#define EUR_FORMAT "%3ld,%02lu" EURSYM
#define EUR_FORMAT_MINLEN 8
#define EUR_FORMAT_NEG " -%ld,%02lu" EURSYM

/* because there is no macro expansion when stringifying, we need to use two
 * levels of macros to stringify the value of a macro (for example
 * MAX_ITEM_NAME_LENGTH) */
#define xstr(s) str(s)
#define str(s) #s

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) > (b) ? (b) : (a))

#endif /* GENERAL_H */
