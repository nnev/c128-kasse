#ifndef _KASSE_H
#define _KASSE_H

#ifdef _IS_KASSE
BYTE printing = 1;
char print_buffer[81];
char log_num = 0;
int log_lines_written = 0;
#else
extern int log_num;
extern int log_lines_written;
#endif
#endif
