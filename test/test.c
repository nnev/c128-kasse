#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>

#include "general.h"
#include "config.h"
#include "kasse.h"
#include "credit_manager.h"

#warning "please fix me, load_state() isn't available anymore"
int main() {
  /* clang-format off */
/*
  BYTE i = 0;
  char euro[10];
  load_items();
  load_state();
  for (i = 0; i < status.num_items; ++i) {
    cprintf("%x: %s (%s, %d mal)\n", i, status.status[i].item_name,
            format_euro(euro, 9, status.status[i].price),
            status.status[i].times_sold);
    status.status[i].times_sold += 10;
  }

  save_state();
  load_items();
  load_state();
  for (i = 0; i < status.num_items; ++i) {
    cprintf("%x: %s (%s, %d mal)\n", i, status.status[i].item_name,
            format_euro(euro, 9, status.status[i].price),
            status.status[i].times_sold);
  }

  cprintf("strlen(%s)==%d\n", status.status[1].item_name,
          strlen(status.status[1].item_name));
  cprintf("%d %d %d\n", status.status[1].item_name[0], '\r', '\n');
*/
  return -1;
}
