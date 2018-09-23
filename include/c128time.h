#ifndef TIME_H
#define TIME_H
#include <stdint.h>

void set_time(uint8_t day, uint8_t hrs, uint8_t min, uint8_t sec);
char *get_time(void);
void update_time(void);
void install_daytime_irq(void);

struct daytime_t {
  uint8_t day;
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
};

#endif /* TIME_H */
