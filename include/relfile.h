#include <stdint.h>

struct relfile {
  uint8_t fd;
  uint8_t cmdfd;
  uint8_t devnr;
  uint8_t sa;
  uint8_t recsize;
};

uint8_t relfile_open(struct relfile *rf, char *name);
uint8_t relfile_is_split_record(struct relfile *rf, uint16_t recnum);
uint8_t relfile_pos(struct relfile *rf, uint16_t recnum, uint8_t offset);
uint8_t relfile_read(struct relfile *rf, uint8_t *buf);
uint8_t relfile_write(struct relfile *rf, uint8_t *buf);
uint8_t relfile_clear(struct relfile *rf);
void relfile_close(struct relfile *rf);
