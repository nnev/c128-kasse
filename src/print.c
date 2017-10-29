/*
 * © 2007-2009 phil_fry, sECuRE, sur5r
 * See LICENSE for license information
 *
 */
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <cbm.h>

#include "general.h"
#define IS_PRINT_C
#include "print.h"
#include "globals.h"

/* NOTE: undocumented function which scratches files
   We need to use this function because linking unistd.h
   makes our program break at runtime.
 */
unsigned char __fastcall__ _sysremove(const char *name);

/* 8192 bytes of log buffer on the heap used for storing one entire logfile in
 * memory before writing (unless flushed). */
char *log_heap_buf;
int log_heap_offset = 0;
int log_heap_flushed = 0;

const int LOG_SIZE = 8192;

void init_log(void) {
  log_heap_buf = malloc(sizeof(char) * LOG_SIZE);
  if (log_heap_buf == NULL) {
    cprintf("malloc(log_heap_buf) failed");
    exit(1);
  }
}

void print_the_buffer(void) {
  BYTE status;
  char *c;

  if (!printing)
    return;

RETRY:
  status = cbm_open((BYTE)4, (BYTE)4, (BYTE)0, NULL);
  if (status != 0) {
    c128_perror(status, "cbm_open(printer)");

    do {
      cprintf("\r\nr)etry, c)ontinue or q)uit?\r\n");
      c = get_input();
    } while ((*c != 'r') && (*c != 'c') && (*c != 'q'));

    if (*c == 'q')
      exit(1);
    else if (*c == 'c') {
      printing = 0;
      return;
    }

    goto RETRY;
  }
  status = cbm_write((BYTE)4, print_buffer, strlen(print_buffer));
  if (status != strlen(print_buffer)) {
    c128_perror(status, "write(printer)");
    if (retry_or_quit() == 'q')
      exit(1);
    goto RETRY;
  }
  cbm_close((BYTE)4);
  log_file(print_buffer);
}

void print_header(void) {
  sprintf(print_buffer, "%c----------------------------------------------------"
                        "----------------------------\r",
          17);
  print_the_buffer();

  sprintf(
      print_buffer,
      "%c#kauf Uhrzeit  - Ding      -     Preis - Restguthbn - # - Nickname\r",
      17);
  print_the_buffer();

  sprintf(print_buffer, "%c----------------------------------------------------"
                        "----------------------------\r",
          17);
  print_the_buffer();
}

/*
 * Flushes the current log buffer to disk. Called either by log_file() when one
 * entire log file is completed or interactively.
 *
 */
void log_flush(void) {
  int c;
  static char filename[8];
  sprintf(filename, "log-%d", log_num);

  /* If we have written to this logfile before, we need to remove it first */
  if (log_heap_flushed > 0)
    _sysremove(filename);

  if ((c = cbm_open((BYTE)8, (BYTE)8, (BYTE)1, filename)) != 0) {
    c128_perror(c, "cbm_open(log)");
    exit(1);
  }
  c = cbm_write((BYTE)8, log_heap_buf, log_heap_offset);
  if (c != log_heap_offset) {
    textcolor(TC_LIGHT_RED);
    cprintf("\r\nCould not save logfile (wrote %d bytes, wanted %d bytes), "
            "please make sure the floppy is not full!\n",
            c, log_heap_offset);
    c128_perror(c, "cbm_write");
    exit(1);
  }
  cbm_close((BYTE)8);

  log_heap_flushed = log_heap_offset;
}

/*
 * Logs to memory and eventually to file (log_flush() is called when one entire
 * logfile was completed in memory).
 *
 */
void log_file(const char *s) {
  strcpy(log_heap_buf + log_heap_offset, s);
  log_heap_offset += strlen(s);

  /* Force a flush when there are only five lines left */
  if (log_heap_offset > (LOG_SIZE - (5 * 80))) {
    log_flush();
    log_num++;
    log_heap_offset = 0;
    log_heap_flushed = 0;
  }
}
