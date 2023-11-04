/*
 * RGB2R-C128-Kassenprogramm
 * © 2007-2009 phil_fry, sECuRE, sur5r
 * See LICENSE for license information
 *
 */
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <cbm.h>
#include <c128.h>
#include <6502.h>

#include "general.h"
#include "config.h"
#include "kasse.h"
#include "credit_manager.h"
#include "itemz.h"
#include "c128time.h"
#include "print.h"
#include "version.h"
#include "vdc_patch_charset.h"
#include "globals.h"
// drucker 4 oder 5
// graphic 4,0,10

void print_item(BYTE i, BYTE current_selection) {
  char profit[EUR_FORMAT_MINLEN + 1];
  if (format_euro(profit, sizeof(profit), status.status[i].price) == NULL) {
    cprintf("Preis %ld konnte nicht umgerechnet werden\r\n",
            status.status[i].price);
    exit(1);
  }
  if (current_selection == i) {
    textcolor(TC_LIGHT_RED);
  } else {
    textcolor(TC_YELLOW);
  }
  cprintf("%2d", i);
  textcolor(TC_LIGHT_GRAY);
  cprintf(" %-" xstr(MAX_ITEM_NAME_LENGTH) "s \xDD%3dx %s",
          status.status[i].item_name, status.status[i].times_sold, profit);
}

void print_kasse_screen_items(BYTE current_selection) {
  BYTE i = 0;

  // clang-format off
  cprintf("\xB0"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xB2"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xB2"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xB2"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xAE"
          "\r\n");
  // clang-format on
  for (; i < min(status.num_items, 15); ++i) {

    cprintf("\xDD");
    print_item(i, current_selection);
    cprintf("\xDD");

    /* if we have more than 15 items, use the second column */
    if ((i + 15) < status.num_items) {
      print_item(i + 15, current_selection);
      cprintf("\xDD");
    } else {
      cprintf("   %-" xstr(MAX_ITEM_NAME_LENGTH) "s \xDD            \xDD", "");
    }

    cprintf("\r\n");
  }
  // clang-format off
  cprintf("\xAD"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xB1"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xB1"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xB1"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0"
          "\xBD"
          "\r\n");
  // clang-format on
}

/* Hauptbildschirm ausgeben */
static void print_kasse_screen(void) {
  char profit[EUR_FORMAT_MINLEN + 1];
  clrscr();
  if (format_euro(profit, sizeof(profit), money) == NULL) {
    cprintf("Einnahme %ld konnte nicht umgerechnet werden\r\n", money);
    profit[0] = '\0';
  }
  textcolor(TC_CYAN);
  /* fill whole line with cyan, so color bits are set up for the clock */
  cprintf("%-80s", "C128-Kasse (" KASSE_AUTHORS ") " GV);
  textcolor(TC_LIGHT_GRAY);
  cprintf("\r\n\r\n"
          "Ertrag: %s (%ld Artikel); Drucken: %s\r\n",
          profit, items_sold, (printing == 1 ? "ein" : "aus"));
  textcolor(TC_LIGHT_GRAY);

  print_kasse_screen_items(0xFF);

  MENU_KEY("   s", "Daten sichern       ");
  MENU_KEY("i", "Itemzverwaltung         ");
  MENU_KEY("g", "Guthabenverwaltung\r\n");
  MENU_KEY("   z", "Zeit setzen         ");
  MENU_KEY("f", "Freitext verkaufen      ");
  MENU_KEY("q", "Beenden\r\n");
}

/*
 * Prints a line and logs it to file. Every line can be at max 80 characters.
 *
 */
static void print_log(char *name, int32_t item_price, int16_t einheiten,
                      char *nickname, char *rest) {
  char *time = get_time();
  char *ptr = NULL;
  uint8_t n;
  char price[EUR_FORMAT_MINLEN + 1];
  if (format_euro(price, sizeof(price), item_price) == NULL) {
    cprintf("Preis %d konnte nicht umgerechnet werden\r\n", item_price);
    exit(1);
  }

  /* clang-format off */
  n = snprintf(print_buffer, sizeof(print_buffer),
        /* enable lower case letters -- 1 */
        "%c"
        /*  Transaction-ID (Anzahl verkaufter Einträge, inklusive des zu druckenden!)
            -- 6-stellig */
        "[%3u] "
        /* Uhrzeit -- 8-stellig + 3 */
        "%8s - "
        /*  Eintragname (= Getränk) -- 9-stellig + 3 */
        "%-" xstr(MAX_ITEM_NAME_LENGTH) "s - "
        /*  Preis (in Cents) -- 8-stellig  + 3 */
        "%" xstr(EUR_FORMAT_MINLEN) "s - "
        /*  restguthaben (8-stellig) + 3 */
        "%" xstr(EUR_FORMAT_MINLEN) "s - "
        /*  Anzahl -- 2-stellig + 3 */
        "%2d - "
        /*  Nickname (falls es vom Guthaben abgezogen wird) -- 10-stellig + 4 */
        "an %" xstr(MAX_CREDIT_NAME_LENGTH)"s\r",
        17, status.transaction_id, time, name, price, rest, einheiten,
        (*nickname != '\0' ? nickname : "Unbekannt"));
  /* clang-format on */

  /* TODO: teach the EUR sign to the printer.
   * Until then, we just overwrite it with "E" */
  ptr = print_buffer;
  while (ptr = strchr(ptr, EURSYM[0])) {
    *ptr = 'E';
  }

  if (n > sizeof(print_buffer)) {
    cprintf("\r\nprint_log(): print_buffer overflowed!\r\n"
            "Wanted to write %d bytes\r\n%s\r\n",
            n, print_buffer);
    exit(1);
  }

  status.transaction_id++;
  print_the_buffer();
}

/* dialog which is called for each bought item */
static signed int buy(char *name, int32_t price) {
  BYTE matches = 0;
  BYTE c, nickname_len;
  int16_t einheiten;
  int8_t i;
  char nickname[MAX_CREDIT_NAME_LENGTH + 1];
  char rest[EUR_FORMAT_MINLEN + 1];
  struct credits_t *credit;

  clrscr();
  cprintf("Wieviel Einheiten \"%s\"? [1] \r\n", name);

  einheiten = cget_number(1);

  if (einheiten > 100 || einheiten < -100 || einheiten == 0) {
    cprintf("\r\nEinheit nicht in [-100, 100] oder 0, Abbruch, dr" uUML "cke "
            "RETURN...\r\n");
    cget_return();
    return 1;
  }

  cprintf("\r\nAuf ein Guthaben kaufen? Wenn ja, Nickname eingeben (press "
          "space to complete):\r\n");
  nickname_len = cget_nickname(nickname, sizeof(nickname));

  if (nickname_len && *nickname != '\0' && *nickname != PETSCII_SP) {
    /* go through credits and remove the amount of money or set nickname
     * to NULL if no such credit could be found */
    i = find_credit_idx(nickname);
    if (i != -1) {
      credit = &credits.credits[i];
      while ((int32_t)credit->credit < (price * einheiten)) {
        if (format_euro(rest, sizeof(rest), credit->credit) == NULL) {
          cprintf("Preis %d konnte nicht umgerechnet werden\r\n",
                  credit->credit);
          exit(1);
        }
        cprintf("\r\n%s hat nicht genug Geld (%s)", nickname, rest);
        MENU_KEY(" e", "einzahlen");
        MENU_KEY(" a", "abbruch");
        MENU_KEY(" t", "trotzdem");
        c = cgetc();
        if (c == 'e') {
          deposit_credit(nickname);
        } else if (c == 't') {
          break;
        } else {
          return 0;
        }
      }
      /* substract money */
      credit->credit -= (price * einheiten);

      if (format_euro(rest, sizeof(rest), credit->credit) == NULL) {
        cprintf("Preis %d konnte nicht umgerechnet werden\r\n", credit->credit);
        exit(1);
      }

      textcolor(TC_LIGHT_GREEN);
      cprintf("\r\nVerbleibendes Guthaben f" uUML "r %s: %s. Dr" uUML
              "cke RETURN...\r\n",
              nickname, rest);
      textcolor(TC_LIGHT_GRAY);
      cget_return();
      matches++;
    } else {
      textcolor(TC_LIGHT_RED);
      cprintf("\r\nNickname nicht gefunden in der Guthabenverwaltung! Abbruch, "
              "dr" uUML "cke RETURN...\r\n");
      textcolor(TC_LIGHT_GRAY);
      cget_return();
      return 0;
    }
  }

  money += price * einheiten;
  items_sold += einheiten;
  if (printing == 1)
    print_log(name, price, einheiten, nickname, rest);

  return einheiten;
}

void buy_stock(BYTE n) {
  if (n >= status.num_items || status.status[n].item_name == NULL) {
    cprintf("FEHLER: Diese Einheit existiert nicht.\r\n");
    cget_return();
    return;
  }

  status.status[n].times_sold +=
      buy(status.status[n].item_name, status.status[n].price);
}

void buy_custom(void) {
  char name[MAX_ITEM_NAME_LENGTH + 1];
  int price;

  clrscr();
  cprintf("\r\nWas soll gekauft werden?\r\n");
  if (cgetn_input(name, sizeof(name)) == 0)
    return;

  cprintf("\r\nWie teuer ist \"%s\" (in cents)?\r\n", name);

  price = cget_number(0);

  if (price == 0) {
    cprintf("Kauf abgebrochen, dr" uUML "cke RETURN...\r\n");
    cget_return();
    return;
  }

  buy(name, price);
}

void set_time_interactive(void) {
  char part[3] = {'\0', '\0', '\0'};
  uint8_t day, tp1, tp2, tp3;
  char *time_input, *time;
  cprintf("Gib den aktuellen Tag des Events und Uhrzeit ein\r\n"
          "Format DHHMMSS, 0-indexiert, z.B. 0174259 f" uUML
          "r \"erster Tag um "
          "17:42:59\":\r\n");
  time_input = get_input();
  part[0] = time_input[0];
  day = atoi(part);
  part[0] = time_input[1];
  part[1] = time_input[2];
  tp1 = atoi(part);
  part[0] = time_input[3];
  part[1] = time_input[4];
  tp2 = atoi(part);
  part[0] = time_input[5];
  part[1] = time_input[6];
  tp3 = atoi(part);
  set_time(day, tp1, tp2, tp3);

  time = get_time();
  cprintf("\r\nZeit gesetzt: %s\r\n", time);
}

int main(void) {
  char *c;
  char *time;

  printing = 1;
  /* initialize daytime global, start the CIA TOD */
  set_time(0, 0, 0, 0);
  kasse_menu = MENU_UNDEFINED;

  videomode(VIDEOMODE_80x25);

  /* clock CPU at double the speed (a whopping 2 Mhz!) */
  fast();

  /* Manipulate the VDC with IRQs turned off.
   * KERNALs default IRQ handler will also try to read the VDC status
   * register, which could interfere with our code trying to read it.
   */
  SEI();
  vdc_patch_charset();
  CLI();

  clrscr();

  install_daytime_irq();

  /* Allocate logging buffer memory */
  init_log();

  /* Set time initially, c128 doesn't know it */
  set_time_interactive();

  /* disable interrupt driven VIC screen editor */
  POKE(0xD8, 255);

  /* Load configuration */
  load_config();

  /* Load items (= drinks) */
  load_items();
  /* Load credits */
  load_credits();

  time = get_time();
  sprintf(print_buffer,
          "%c----------------------------------------------------"
          "----------------------------\r",
          17);
  print_the_buffer();
  sprintf(print_buffer, "%cC128-Kasse Version " GV "\r", 17);
  print_the_buffer();

  sprintf(print_buffer,
          "%cKasse gestartet um %s. Nutze logfile log-%u, offset %d.\r", 17,
          time, log_num, log_heap_offset);
  print_the_buffer();

  print_header();

  while (1) {
    print_kasse_screen();
    kasse_menu = MENU_MAIN;
    c = get_input();
    kasse_menu = MENU_UNDEFINED;
    /* ...display dialogs eventually */
    if (*c >= PETSCII_0 && *c <= PETSCII_9) {
      /* if the input starts with a digit, we will interpret it as a number
       * for the item to be sold */
      buy_stock(atoi(c));
    } else if (*c == 'f') {
      buy_custom();
    } else if (*c == 's') {
      save_items();
      save_credits();
      log_flush();
      cprintf("\r\nDr" uUML "cke RETURN...\r\n");
    } else if (*c == 'g') {
      credit_manager();
    } else if (*c == 'i') {
      itemz_manager();
      load_items();
    } else if (*c == 'z') {
      set_time_interactive();
    } else if (*c == 'q') {
      save_items();
      save_credits();
      log_flush();
      cprintf("\r\nDr" uUML "cke RETURN...\r\n");
      cget_return();
      break;
    }
  }
  clrscr();
  cprintf("\r\nBYEBYE\r\n");

  return 0;
}
