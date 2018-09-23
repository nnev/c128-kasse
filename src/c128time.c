/*
 * RGB2R-C128-Kassenprogramm
 * © 2007-2017 phil_fry, sECuRE, sur5r, mxf
 * See LICENSE for license information
 *
 */
#include <peekpoke.h>
#include <stdlib.h>
#include <stdio.h>
#include <c128.h>
#include <stdint.h>
#include <6502.h>
#include <conio.h>

#include "bcd2dec.h"
#include "general.h"
#include "globals.h"
#include "vdc_util.h"

/* This file uses the CIA TOD (Complex Interface Adapter, Time of Day)
 * for timekeeping, see https://www.c64-wiki.com/wiki/CIA and its Links section
 * for a bit more information */

/* the Time of Day PM bit is set for hours >= 12 */
#define TOD_PM 0x80

/* VDC charmap starts at 0x0000; 80 chars per line.
 * We want to draw at 72 chars on the 1st line.
 */
#define CLOCK_ADDR 72

/* arbitrarly chosen stack size, should be large enough */
#define DAYTIME_IRQ_STACK_SIZE 32
uint8_t daytime_irq_stack[DAYTIME_IRQ_STACK_SIZE];

void update_time(void) {
  uint8_t bcd_hour, hour, min, sec, dummy;

  /* Read the hour register first to stop the clock from updating the external
   * registers from the internal (still ticking!) CIA registers. */

  bcd_hour = CIA1.tod_hour;

  /* hour is >= 12 if TOD_PM is set */
  if (bcd_hour & TOD_PM) {
    hour = bcd2dec(bcd_hour ^ TOD_PM);
    /* adjust for 24h clock, 12:??pm should still be 12:?? */
    if (hour != 12) {
      hour += 12;
    }
  } else {
    hour = bcd2dec(bcd_hour);
  }

  sec = bcd2dec(CIA1.tod_sec);
  min = bcd2dec(CIA1.tod_min);

  /* MUST read tod_10 to enable the clock latch again */
  dummy = CIA1.tod_10;

  /* it's a new day when hour wraps */
  if (daytime.hour > hour) {
    daytime.day++;
  }

  daytime.hour = hour;
  daytime.min = min;
  daytime.sec = sec;
}

char *get_time(void) {
  static char buffer[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  update_time();
  sprintf(buffer, "%02d:%02d:%02d", daytime.hour, daytime.min, daytime.sec);
  return buffer;
}

/* divide by 10; put quotient in high nibble, remainder in low nibble */
uint8_t dec2bcd(const uint8_t dec) { return (((dec / 10) << 4) | (dec % 10)); }

void set_time(uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
  uint8_t bcd_hour;

  /* CIA TOD will always flip the PM bit
   * when we either want to write the 0th or 12th hour.
   * So we need to write the hour with the PM bit inverted,
   * for the CIA to flip it again */
  if (hour == 0) {
    /* the 0th hour is 12am in 12h clock format, 0x12 is BCD for 12 */
    bcd_hour = 0x12 ^ TOD_PM;
  } else if (hour > 12) {
    /* convert 24h clock to 12h with PM bit set */
    bcd_hour = dec2bcd(hour - 12);
    bcd_hour = bcd_hour ^ TOD_PM;
  } else {
    /* includes 12pm since the PM bit gets automatically flipped */
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

uint8_t _daytime_irq(void) {
  char *t;
  /* We are called 60 times a second. We only want to draw a clock
   * when we are a) on the mainscreen and b) the seconds changed */
  if (kasse_menu == MENU_MAIN && CIA1.tod_sec != daytime.sec) {
    t = get_time();
    vdc_write_mem(CLOCK_ADDR, t, 8);
  }
  /* always call additional handlers */
  return (IRQ_NOT_HANDLED);
}

void install_daytime_irq(void) {
  SEI();
  set_irq(&_daytime_irq, daytime_irq_stack, DAYTIME_IRQ_STACK_SIZE);
  CLI();
}
