/*
 * RGB2R-C128-Kassenprogramm
 * © 2007-2009 phil_fry, sECuRE, sur5r
 * See LICENSE for license information
 *
 */
#define IS_CONFIG_C

#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "kasse.h"
#include "general.h"
#include "config.h"
#include "print.h"

/* NOTE: undocumented function which scratches files
   We need to use this function because linking unistd.h
   makes our program break at runtime.
 */
unsigned char __fastcall__ _sysremove(const char *name);

int32_t money = 0;
int32_t items_sold = 0;
BYTE printer_port = 4;
static bool items_exists = false;
static bool credits_exists = false;
struct status_array_t status;
struct credits_array_t credits;

/*
 * Checks if items/credits-files are existing to avoid having to recover
 * the error state of the drive (we'd have to if we would just access the
 * files directly)
 *
 */
static void lookup_needed_files(void) {
  BYTE lfn = 8, c;
  struct cbm_dirent dirent;
  char filename[8];

  if (cbm_opendir(lfn, (BYTE)8) != 0) {
    cprintf("could not open directory\r\n");
    return;
  }
  while (cbm_readdir(lfn, &dirent) == 0) {
    /* NOTE: You MUST NOT delete any logfiles. This does only work
     * under the assumption that logfiles are named continuously */
    if (strncmp(dirent.name, "log", 3) == 0)
      log_num++;
    if (strcasecmp(dirent.name, "items") == 0)
      items_exists = true;
    if (strcasecmp(dirent.name, "credits") == 0)
      credits_exists = true;
  }
  cbm_closedir(lfn);

  /* Try to find out how many lines the last logfile got to seamlessly
   * append to it, if we got more than one logfile. */
  if (log_num > 0) {
    log_num--;

    sprintf(filename, "log-%u", log_num);
    if ((c = cbm_open(lfn, (BYTE)8, (BYTE)CBM_READ, filename)) != 0) {
      c128_perror(c, "cbm_open(log) for reading");
      exit(1);
    }
    log_heap_offset = cbm_read(lfn, log_heap_buf, LOG_SIZE);
    if (log_heap_offset < 0) {
      cprintf("error while cbm_read()ing the logfile\r\n");
      exit(1);
    }
    log_heap_flushed = log_heap_offset;
    log_heap_buf[log_heap_offset] = '\0';
    cbm_close(lfn);
  }
}

void load_items(void) {
  BYTE c;

  if (items_exists) {
    items_sold = 0;
    money = 0;
    cbm_load("items", (BYTE)8, &status);
    for (c = 0; c < status.num_items; c++) {
      items_sold += status.status[c].times_sold;
      money += (((int32_t)status.status[c].price) *
                ((int32_t)status.status[c].times_sold));
    }
  } else
    memset(&status, 0, sizeof(struct status_array_t));
}

void load_credits(void) {
  if (credits_exists)
    cbm_load("credits", (BYTE)8, &credits);
  else
    memset(&credits, 0, sizeof(struct credits_array_t));
}

void save_items(void) {
  cprintf("\r\nspeichere items.. ");
  if (items_exists)
    _sysremove("items");
  cbm_save("items", (BYTE)8, &status, sizeof(struct status_array_t));
  items_exists = true;
  cprintf("ok");
}

void save_credits(void) {
  cprintf("\r\nspeichere credits.. ");
  if (credits_exists)
    _sysremove("credits");
  cbm_save("credits", (BYTE)8, &credits, sizeof(struct credits_array_t));
  credits_exists = true;
  cprintf("ok");
}

void load_config(void) { lookup_needed_files(); }
