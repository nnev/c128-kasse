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

#define VIDEOMODE (((* (BYTE *)0xD7) == 0x80) ? 80 : 40)

/* because there is no macro expansion when stringifying, we need to use two
 * levels of macros to stringify the value of a macro (for example
 * MAX_ITEM_NAME_LENGTH) */
#define xstr(s) str(s)
#define str(s) #s

#endif /*GENERAL_H_*/
