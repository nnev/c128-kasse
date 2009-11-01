#ifndef _PRINT_H
#define _PRINT_H

void print_the_buffer();
void log_file(const char *s);

#ifdef _IS_PRINT
char print_buffer[81];
char log_num = 0;
int log_lines_written = 0;
#else
extern char print_buffer[81];
extern int log_num;
extern int log_lines_written;

#endif

#endif
