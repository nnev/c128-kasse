#ifndef GENERAL_H_
#define GENERAL_H_
typedef unsigned char BYTE;
char *get_input();
char * format_euro(char * s, int cent);
void c128_perror(BYTE, char*);
extern BYTE _oserror;
#endif /*GENERAL_H_*/
