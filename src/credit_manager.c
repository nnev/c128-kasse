/*
 * RGB2R-C128-Kassenprogramm
 * © 2007-2009 phil_fry, sECuRE, sur5r
 * See LICENSE for license information
 *
 */
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "general.h"
#include "kasse.h"
#include "c128time.h"
#include "print.h"
#include "version.h"
#include "vdc_patch_charset.h"

static char *filter = NULL;
static BYTE filter_len;

static BYTE current_credits_page = 0;
static BYTE current_selection = 0xFF;

static void print_item(BYTE i) {
  char buffer[EUR_FORMAT_MINLEN + 1];
  const unsigned int credit = credits.credits[i].credit;

  if (format_euro(buffer, sizeof(buffer), credit) != buffer) {
    cprintf("Error: Could not format credit %d\r\n", credit);
    exit(1);
  }

  if (current_selection == i) {
    textcolor(TC_LIGHT_RED);
  } else {
    textcolor(TC_YELLOW);
  }
  cprintf("%2d", i);
  textcolor(TC_LIGHT_GRAY);
  // TODO: switch to MAX_CREDIT_NAME_LENGTH once that is increased
  cprintf(" %-" xstr(MAX_ITEM_NAME_LENGTH) "s \xDD%3s ",
          credits.credits[i].nickname, buffer);
}

static void print_line(BYTE i) {
  cprintf("\xDD");
  print_item(i);
  cprintf("\xDD");

  /* if we have more than 15 items, use the second column */
  if ((i + 15) < credits.num_items) {
    print_item(i + 15);
    cprintf("\xDD");
  } else {
    // TODO: switch to MAX_CREDIT_NAME_LENGTH once that is increased
    cprintf("   %-" xstr(MAX_ITEM_NAME_LENGTH) "s \xDD        \xDD", "");
  }

  cprintf("\r\n");
}

static void credit_print_screen(void) {
  BYTE i, pages;

  clrscr();
  textcolor(TC_CYAN);
  cprintf("credit_manager (" KASSE_AUTHORS ") v:" GV "\r\n\r\n");
  textcolor(TC_LIGHT_GRAY);
  pages = (credits.num_items / CREDITS_PER_PAGE);
  if (current_credits_page > pages)
    current_credits_page = pages;
  cprintf("Datei: CREDITS (Seite %d von %d), ", current_credits_page + 1,
          pages + 1);
  if (filter != NULL) {
    cprintf("Filter: %s", filter);
  } else {
    cprintf("Kein Filter");
  }
  cprintf("\r\n\r\n");
  // clang-format off
  cprintf("\xB0"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xB2"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xB2"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xB2"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xAE"
          "\r\n");
  // clang-format on

  if (filter != NULL) {
    for (i = 0; i < credits.num_items; i++) {
      if (strncasecmp(credits.credits[i].nickname, filter, filter_len) != 0) {
        continue;
      }
      cprintf("\xDD");
      print_item(i);
      cprintf("\xDD\r\n");
    }
  } else {
    const BYTE offset = (current_credits_page * CREDITS_PER_PAGE);
    BYTE last = ((current_credits_page + 1) * CREDITS_PER_PAGE);
    if (last > credits.num_items) {
      last = credits.num_items;
    }
    if (last > (offset + 15)) {
      last = (offset + 15);
    }
    for (i = offset; i < last; i++) {
      print_line(i);
    }
  }

  // clang-format off
  cprintf("\xAD"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xB1"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xB1"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xB1"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xBD"
          "\r\n");
  // clang-format on

  cprintf("\r\n");
  MENU_KEY("  n", "Neu");
  MENU_KEY("      d", "L" oUML "schen");
  MENU_KEY("    p", "Einzahlen");
  MENU_KEY("  b", "Seite hoch");
  MENU_KEY("  f", "Seite runter");
  cprintf("\r\n");
  MENU_KEY("  g", "Filtern");
  MENU_KEY("  s", "Speichern");
  MENU_KEY("  z", "Zur" uUML "ck");
  cprintf("\r\n");
}

int8_t find_credit_idx(char *name) {
  int8_t i;
  for (i = 0; i < credits.num_items; ++i) {
    if (strncasecmp(name, credits.credits[i].nickname,
                    MAX_CREDIT_NAME_LENGTH + 1) == 0) {
      return i;
    }
  }
  return -1;
}

static void deposit_credit_idx(int8_t i) {
  char *time = get_time();
  struct credits_t *credit = &credits.credits[i];
  unsigned int deposit;

  cprintf("\r\nEinzahlung in Cent:\r\n");
  if ((deposit = cget_number(0)) == 0)
    return;

  credit->credit += deposit;

  print_the_buffer();
  cprintf("\r\nEinzahlung durchgef" uUML "hrt, dr" uUML "cke RETURN...\r\n");
  cget_return();
}

void deposit_credit(char *nickname) {
  int8_t i;

  if ((i = find_credit_idx(nickname)) == -1) {
    return; // cannot find named credit
  }

  deposit_credit_idx(i);
}

static void new_credit(void) {
  char name[MAX_CREDIT_NAME_LENGTH + 1];
  char *time;
  int credit;

  if (credits.num_items == MAX_CREDIT_ITEMS) {
    cprintf("\rEs ist bereits die maximale Anzahl an Eintr" aUML
            "gen erreicht, dr" uUML "cke RETURN...\r\n");
    cget_return();
    return;
  }

  clrscr();
  cprintf("\rNickname (max. 10 Zeichen):\r\n");
  if (cgetn_input(name, sizeof(name)) == 0)
    return;

  if (find_credit_idx(name) >= 0) {
    cprintf("\rNickname existiert bereits, dr" uUML "cke RETURN...\r\n");
    cget_return();
    return;
  }

  cprintf("\r\nGuthaben in Cents:\r\n");
  if ((credit = cget_number(0)) == 0)
    return;
  strncpy(credits.credits[credits.num_items].nickname, name,
          MAX_CREDIT_NAME_LENGTH);
  credits.credits[credits.num_items].credit = credit;

  time = get_time();
  sprintf(print_buffer, "%c%s - Guthaben mit %d Cent f" uUML "r %s angelegt\r",
          17, time, credit, name);
  print_the_buffer();

  credits.num_items++;
}

static void delete_credit_idx(int8_t i) {
  --credits.num_items;
  if (i != credits.num_items) {
    credits.credits[i] = credits.credits[credits.num_items];
  }
  memset(credits.credits[credits.num_items].nickname, '\0',
         MAX_CREDIT_NAME_LENGTH + 1);
  credits.credits[credits.num_items].credit = 0;
}

static void delete_credit(char *nickname) {
  int8_t i;
  if ((i = find_credit_idx(nickname)) < 0) {
    cprintf("\r Nick existiert nicht\r\n");
    return;
  }
  delete_credit_idx(i);
}

void credit_manager(void) {
  char nickname[MAX_CREDIT_NAME_LENGTH + 1];
  char *c;
  current_selection = 0xFF;
  while (1) {
    credit_print_screen();
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
        new_credit();
        break;
      case 'd':
        if (current_selection != 0xFF) {
          delete_credit_idx(current_selection);
          current_selection = 0xFF;
        } else {
          cputs("\rName? (press space to complete)\r\n");
          if (cget_nickname(nickname, sizeof(nickname))) {
            delete_credit(nickname);
          }
        }
        break;
      case 's':
        save_credits();
        break;
      case 'f':
        if (current_credits_page < (credits.num_items / CREDITS_PER_PAGE))
          current_credits_page++;
        break;
      case 'b':
        if (current_credits_page > 0)
          current_credits_page--;
        break;
      case 'p':
        if (current_selection != 0xFF) {
          deposit_credit_idx(current_selection);
          current_selection = 0xFF;
        } else {
          cputs("\rName? (press space to complete)\r\n");
          if (cget_nickname(nickname, sizeof(nickname))) {
            deposit_credit(nickname);
          }
        }
        break;
      case 'g':
        cprintf("Filter eingeben:\r\n");
        filter = get_input();
        if (filter == NULL || *filter == PETSCII_SP ||
            (filter_len = strlen(filter)) == 0)
          filter = NULL;
        break;
      case 'z':
        save_credits();
        return;
      default:
        cprintf("Unbekannter Befehl, dr" uUML "cke RETURN...\r\n");
        cget_return();
      }
    }
  }
}
