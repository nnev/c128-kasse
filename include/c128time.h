#ifndef TIME_H
#define TIME_H
#include <stdint.h>

void set_time(uint8_t hrs, uint8_t min, uint8_t sec);
char *get_time(void);

#endif /* TIME_H */
