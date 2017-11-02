/*
 * RGB2R-C128-Kassenprogramm
 * © 2007-2009 phil_fry, sECuRE, sur5r
 * See LICENSE for license information
 *
 */
#include <peekpoke.h>
#include <stdlib.h>
#include <stdio.h>
#include <c128.h>
#include <stdint.h>

#include "bcd2dec.h"
#include "general.h"
#include "globals.h"

void update_time(void) {
  uint8_t bcd_hour, hour, min, sec, tenth;

  /* Read the hour register first to stop the clock from updating the external
   * registers from the internal (still ticking!) CIA registers. */

  bcd_hour = CIA1.tod_hour;

  /* if high bit is set, it is pm */
  if (bcd_hour & 0x80) {
    hour = bcd2dec(bcd_hour ^ 0x80);
    /* adjust for 24h clock, 12:??pm is still 12:?? */
    if (hour != 12) {
      hour += 12;
    }
  } else {
    hour = bcd2dec(bcd_hour);
  }

  sec = bcd2dec(CIA1.tod_sec);
  min = bcd2dec(CIA1.tod_min);

  /* MUST read tod_10 to enable the clock latch again */
  tenth = CIA1.tod_10;

  if (daytime.hour > hour) {
    daytime.day++;
  }

  daytime.hour = hour;
  daytime.min = min;
  daytime.sec = sec;
}

char *get_time(void) {
  static char buffer[9];
  update_time();
  sprintf(buffer, "%02d:%02d:%02d", daytime.hour, daytime.min, daytime.sec);
  return buffer;
}

/* divide by 10; put quotient in high nibble, reminder in low nibble */
uint8_t dec2bcd(uint8_t dec) { return (((dec / 10) << 4) | (dec % 10)); }

void set_time(uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
  uint8_t bcd_hour;

  /* CIA TOD will always flip the pm bit
   * when either 0 or 12 is written to the hour register */
  if (hour == 0) {
    /* bcd 12 with high bit (pm) set */
    bcd_hour = 0x92;
  } else if (hour > 12) {
    /* convert 24h clock to 12h with pm bit set */
    bcd_hour = dec2bcd(hour - 12);
    bcd_hour = bcd_hour ^ 0x80;
  } else {
    /* includes 12pm since the bit gets automatically flipped */
    bcd_hour = dec2bcd(hour);
  }

  daytime.day = day;
  daytime.hour = hour;
  daytime.min = min;
  daytime.sec = sec;

  CIA1.tod_hour = bcd_hour;
  CIA1.tod_min = dec2bcd(min);
  CIA1.tod_sec = dec2bcd(sec);

  /* set CIA1.tod_10 and program "Control Timer A" */
  __asm__("jsr initsystime");
}
