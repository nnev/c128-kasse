#ifndef PRINT_H
#define PRINT_H

void init_log(void);
void print_the_buffer(void);
void print_header(void);
void log_file(const char *s);
void log_flush(void);

#ifdef IS_PRINT_C
char print_buffer[80 + 2 + 1];
unsigned char log_num = 0;
#else
extern char print_buffer[80 + 2 + 1];
extern unsigned char log_num;
extern char *log_heap_buf;
extern int log_heap_offset;
extern int log_heap_flushed;
extern const int LOG_SIZE;
#endif

#endif
