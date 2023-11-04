/*
 * RGB2R-C128-Kassenprogramm
 * © 2007-2009 phil_fry, sECuRE, sur5r
 * See LICENSE for license information
 *
 * itemz.c: Verwaltet die ITEMS- und CREDITS-datei
 *
 */
#include <c128.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "general.h"
#include "config.h"
#include "credit_manager.h"
#include "version.h"
#include "vdc_patch_charset.h"
#include "globals.h"

static void itemz_print_screen(BYTE current_selection) {
  clrscr();
  textcolor(TC_CYAN);
  cprintf("itemz_manager (" KASSE_AUTHORS ") v:" GV "\r\n\r\n");
  textcolor(TC_LIGHT_GRAY);
  cprintf("Datei: ITEMS\r\n\r\n");

  print_kasse_screen_items(current_selection);

  cprintf("\r\n");
  MENU_KEY("  n", "Neu");
  MENU_KEY("    d", "L" oUML "schen");
  MENU_KEY("  s", "Speichern");
  cprintf("\r\n");
  MENU_KEY("  z", "Zur" uUML "ck");
  MENU_KEY(" r", "Reset aller Verkauft-Z" aUML "hler");
  cprintf("\r\n");
}

static void new_item(void) {
  char name[MAX_ITEM_NAME_LENGTH + 1];
  int price, times_sold;

  if (status.num_items == MAX_ITEMS) {
    ERROR("\rEs ist bereits die maximale Anzahl an Eintr" aUML
          "gen erreicht, dr" uUML "cke RETURN...\r\n");
    cget_return();
    return;
  }

  cprintf("\rName des Eintrags:\r\n");
  if (cgetn_input(name, sizeof(name)) == 0)
    return;
  cprintf("\r\nPreis in Cents:\r\n");
  if ((price = cget_number(0)) <= 0)
    return;
  cprintf("\r\nWie oft schon verkauft? [0] \r\n");
  if ((times_sold = cget_number(0)) < 0)
    return;
  memset(status.status[status.num_items].item_name, '\0',
         MAX_ITEM_NAME_LENGTH + 1);
  strncpy(status.status[status.num_items].item_name, name,
          MAX_ITEM_NAME_LENGTH);
  status.status[status.num_items].price = price;
  status.status[status.num_items].times_sold = times_sold;
  status.num_items++;
}

static void _delete_item(BYTE num) {
  memset(status.status[num].item_name, '\0', MAX_ITEM_NAME_LENGTH);
  status.status[num].price = 0;
  status.status[num].times_sold = 0;
}

static void delete_item_idx(BYTE num) {
  uint8_t last;
  if (status.num_items > 1) {
    /* Swap last item with this one and delete the last one to avoid holes */
    last = (status.num_items - 1);
    strncpy(status.status[num].item_name, status.status[last].item_name,
            MAX_ITEM_NAME_LENGTH);
    status.status[num].price = status.status[last].price;
    status.status[num].times_sold = status.status[last].times_sold;
    _delete_item(last);
  } else {
    /* Just delete it */
    _delete_item(num);
  }
  status.num_items--;
}

static void delete_item(void) {
  int16_t num;
  cprintf("\r Welcher Eintrag soll gel" oUML "scht werden?\r\n");

  num = cget_number(-1);
  if (num < 0)
    return;
  delete_item_idx(num);
}

static void reset_counters(void) {
  BYTE i;

  for (i = 0; i < status.num_items; i++) {
    status.status[i].times_sold = 0;
  }
}

void itemz_manager() {
  char *c;
  BYTE current_selection = 0xFF;
  while (1) {
    itemz_print_screen(current_selection);
    c = get_input();
    if (*c >= PETSCII_0 && *c <= PETSCII_9) {
      /* if the input starts with a digit, we will interpret it as a number
       * for the item to be selected */
      current_selection = atoi(c);
    } else {
      switch (*c) {
      case '\0':
        current_selection = 0xFF;
        break;
      case 'n':
        new_item();
        break;
      case 'd':
        if (current_selection != 0xFF) {
          delete_item_idx(current_selection);
          current_selection = 0xFF;
        } else {
          delete_item();
        }
        break;
      case 's':
        save_items();
        break;
      case 'r':
        reset_counters();
        break;
      case 'z':
        save_items();
        GOOD("\r\nDr" uUML "cke RETURN...\r\n");
        cget_return();
        return;
      default:
        ERROR("\r\nUnbekannter Befehl, dr" uUML "cke RETURN...\r\n");
        cget_return();
      }
    }
  }
}
