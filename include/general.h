#ifndef GENERAL_H_
#define GENERAL_H_
typedef unsigned char BYTE;
char *get_input();
char * format_euro(char * s, int maxlen, int cent);
void c128_perror(BYTE, char*);
extern BYTE _oserror;
#define POKE(addr,val)     (*(unsigned char*) (addr) = (val))
#define POKEW(addr,val)    (*(unsigned*) (addr) = (val))
#define PEEK(addr)         (*(unsigned char*) (addr))
#define PEEKW(addr)        (*(unsigned*) (addr))
#endif /*GENERAL_H_*/
