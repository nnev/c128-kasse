#ifndef GENERAL_H_
#define GENERAL_H_
typedef unsigned char BYTE;
char *get_input();
char retry_or_quit();
char *format_euro(char * s, int maxlen, int cent);
void c128_perror(BYTE, char*);
extern BYTE _oserror;
#define POKE(addr,val)     (*(unsigned char*) (addr) = (val))
#define POKEW(addr,val)    (*(unsigned*) (addr) = (val))
#define PEEK(addr)         (*(unsigned char*) (addr))
#define PEEKW(addr)        (*(unsigned*) (addr))

/* C128 color codes, see PDF page 127 of
 * http://www.pagetable.com/docs/Commodore%20128%20Programmer%27s%20Reference%20Guide.pdf */
#define TC_BLACK        1
#define TC_WHITE        2
#define TC_RED          3
#define TC_CYAN         4
#define TC_PURPLE       5
#define TC_GREEN        6
#define TC_BLUE         7
#define TC_YELLOW       8
#define TC_ORANGE       9
#define TC_BROWN       10
/* This is the good red */
#define TC_LIGHT_RED   11
#define TC_DARK_GRAY   12
#define TC_MEDIUM_GRAY 13
/* This is the good green */
#define TC_LIGHT_GREEN 14
#define TC_LIGHT_BLUE  15
#define TC_LIGHT_GRAY  16

#define VIDEOMODE (((* (BYTE *)0xD7) == 0x80) ? 80 : 40)

/* because there is no macro expansion when stringifying, we need to use two
 * levels of macros to stringify the value of a macro (for example
 * MAX_ITEM_NAME_LENGTH) */
#define xstr(s) str(s)
#define str(s) #s

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) > (b) ? (b) : (a))

#endif /*GENERAL_H_*/
