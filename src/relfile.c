#include <conio.h>
#include <cbm.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "relfile.h"

#define RELFILENAME_MAX (FILENAME_MAX + 4) // FOO,L,\xFF\0

uint8_t relfile_open(struct relfile *rf, char *name) {
  char namebuf[RELFILENAME_MAX];

  if (strlen(name) > FILENAME_MAX)
    return ENOENT;
  snprintf(namebuf, RELFILENAME_MAX, "%s,l,%c", name, rf->recsize);
  cbm_open(rf->fd, rf->devnr, rf->sa, namebuf);
  if (_oserror)
    return _oserror;

  cbm_open(rf->cmdfd, rf->devnr, 15, NULL);
  if (_oserror)
    return _oserror;
}

uint8_t relfile_is_split_record(struct relfile *rf, uint16_t recnum) {
  return ((recnum * rf->recsize) % 254) < rf->recsize;
}

uint8_t relfile_pos(struct relfile *rf, uint16_t recnum, uint8_t offset) {
  char cmdbuf[5]; // 'P' (SA) (RECLO) (RECHI) (OFFSET)

  cmdbuf[0] = 80; // P
  cmdbuf[1] = rf->sa + 0x60;
  cmdbuf[2] = (uint8_t)(recnum & 0x00FF);
  cmdbuf[3] = (uint8_t)(recnum >> 8);
  cmdbuf[4] = offset;

  // cprintf("CMD: %2X %2X %2X %2X %2X\r\n", cmdbuf[0], cmdbuf[1], cmdbuf[2],
  // cmdbuf[3], cmdbuf[4]);

  cbm_write(rf->cmdfd, cmdbuf, 5);
  return _oserror;
}

uint8_t relfile_read(struct relfile *rf, uint8_t *buf) {
  cbm_read(rf->fd, buf, rf->recsize);
  return _oserror;
}

uint8_t relfile_write(struct relfile *rf, uint8_t *buf) {
  cbm_write(rf->fd, buf, rf->recsize);
  return _oserror;
}

uint8_t relfile_clear(struct relfile *rf) {
  cbm_write(rf->fd, "\xFF", 1);
  return _oserror;
}

void relfile_close(struct relfile *rf) {
  cbm_close(rf->cmdfd);
  cbm_close(rf->fd);
}
