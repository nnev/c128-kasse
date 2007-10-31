#ifndef _KASSE_H
#define _KASSE_H

#ifdef _IS_KASSE
BYTE printing = 1;
char print_buffer[81];
int log_num = 0;
#else
extern int log_num;
#endif
#endif
