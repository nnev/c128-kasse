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
#include "c128time.h"
#include "print.h"
#include "version.h"
#include "vdc_patch_charset.h"
#include "globals.h"
// drucker 4 oder 5
// graphic 4,0,10

void print_item(BYTE i) {
  char profit[EUR_FORMAT_MINLEN];
  if (format_euro(profit, sizeof(profit), status.status[i].price) == NULL) {
    cprintf("Preis %ld konnte nicht umgerechnet werden\r\n",
            status.status[i].price);
    exit(1);
  }
  textcolor(TC_YELLOW);
  cprintf("%2d", i);
  textcolor(TC_LIGHT_GRAY);
  cprintf(": %-" xstr(MAX_ITEM_NAME_LENGTH) "s \xDD%s,   %3dx ",
          status.status[i].item_name, profit, status.status[i].times_sold);
}

/* Hauptbildschirm ausgeben */
static void print_screen(void) {
  BYTE i = 0;
  char *time = get_time();
  char profit[EUR_FORMAT_MINLEN];
  clrscr();
  if (format_euro(profit, sizeof(profit), money) == NULL) {
    cprintf("Einnahme %ld konnte nicht umgerechnet werden\r\n", money);
    exit(1);
  }
  textcolor(TC_CYAN);
  cprintf("C128-Kassenprogramm (phil_fry, sECuRE, sur5r, mxf) " GV "\r\n");
  textcolor(TC_LIGHT_GRAY);
  cprintf("\r\nUhrzeit:     %s (wird nicht aktualisiert)\r\n"
          "Eingenommen: %s, Verkauft: %ld Dinge, Drucken: %s\r\n",
          time, profit, items_sold, (printing == 1 ? "ein" : "aus"));
  textcolor(TC_LIGHT_GRAY);
  cprintf("      \xB0"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xB2"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xB2"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xB2"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xAE"
          "\r\n");
  for (; i < min(status.num_items, 15); ++i) {

    cprintf("      \xDD");
    print_item(i);
    cprintf("\xDD");

    /* if we have more than 15 items, use the second column */
    if ((i + 15) < status.num_items) {
      print_item(i + 15);
      cprintf("\xDD");
    } else {
      cprintf("              \xDD                \xDD");
    }

    cprintf("\r\n");
  }
  cprintf("      \xAD"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xB1"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xB1"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xB1"
          "\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xBD"
          "\r\n");
  textcolor(TC_YELLOW);
  cprintf("   s");
  textcolor(TC_LIGHT_GRAY);
  cprintf(") Daten sichern                                  ");
  textcolor(TC_YELLOW);
  cprintf("g");
  textcolor(TC_LIGHT_GRAY);
  cprintf(") Guthabenverwaltung\r\n");
  textcolor(TC_YELLOW);
  cprintf("   z");
  textcolor(TC_LIGHT_GRAY);
  cprintf(") Zeit setzen         ");
  textcolor(TC_YELLOW);
  cprintf("f");
  textcolor(TC_LIGHT_GRAY);
  cprintf(") Freitext verkaufen      ");
  textcolor(TC_YELLOW);
  cprintf("q");
  textcolor(TC_LIGHT_GRAY);
  cprintf(") Beenden\r\n");
}

/*
 * Prints a line and logs it to file. Every line can be at max 80 characters.
 *
 */
static void print_log(char *name, int item_price, int einheiten, char *nickname,
                      char *rest) {
  char *time = get_time();
  char price[EUR_FORMAT_MINLEN];
  /* Format:
     Transaction-ID (Anzahl verkaufter Einträge, inklusive des zu druckenden!)
     -- 6-stellig
     Uhrzeit -- 8-stellig
     Eintragname (= Getränk) -- 9-stellig
     Preis (in Cents) -- 9-stellig
     Anzahl -- 2-stellig
     Nickname (falls es vom Guthaben abgezogen wird) -- 10-stellig
     restguthaben (9-stellig)

     + 7 leerzeichen
     --> 48 zeichen
     */
  if (format_euro(price, sizeof(price), item_price) == NULL) {
    cprintf("Preis %d konnte nicht umgerechnet werden\r\n", item_price);
    exit(1);
  }

  sprintf(print_buffer, "%c[%3u] %s - %-" xstr(
                            MAX_ITEM_NAME_LENGTH) "s - %s - %s - %d - an %s\r",
          17, status.transaction_id, time, name, price, rest, einheiten,
          (*nickname != '\0' ? nickname : "Unbekannt"));
  status.transaction_id++;
  print_the_buffer();
}

/* dialog which is called for each bought item */
static signed int buy(char *name, unsigned int price) {
  BYTE matches = 0;
  BYTE c, nickname_len;
  int einheiten;
  char nickname[NICKNAME_MAX_LEN + 1];
  char rest[EUR_FORMAT_MINLEN];
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

  cprintf("\r\nAuf ein Guthaben kaufen? Wenn ja, Nickname eingeben:\r\n");
  nickname_len = cget_nickname(nickname, sizeof(nickname));

  if (nickname_len && *nickname != '\0' && *nickname != PETSCII_SP) {
    /* go through credits and remove the amount of money or set nickname
     * to NULL if no such credit could be found */
    credit = find_credit(nickname);
    if (credit != NULL) {
      while ((signed int)credit->credit < ((signed int)price * einheiten)) {
        if (format_euro(rest, sizeof(rest), credit->credit) == NULL) {
          cprintf("Preis %d konnte nicht umgerechnet werden\r\n",
                  credit->credit);
          exit(1);
        }
        cprintf(
            "\r\n%s hat nicht genug Geld (%s). e) einzahlen a) abbruch \r\n",
            nickname, rest);
        c = cgetc();
        if (c == 'e') {
          deposit_credit(nickname);
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
  BYTE part[3] = {'0', '0', '\0'};
  BYTE tp1, tp2, tp3;
  char *time_input, *time;
  cprintf("Gib die aktuelle Uhrzeit ein (Format HHMMSS):\r\n");
  time_input = get_input();
  part[0] = time_input[0];
  part[1] = time_input[1];
  tp1 = atoi(part);
  part[0] = time_input[2];
  part[1] = time_input[3];
  tp2 = atoi(part);
  part[0] = time_input[4];
  part[1] = time_input[5];
  tp3 = atoi(part);
  set_time(tp1, tp2, tp3);

  time = get_time();
  cprintf("\r\nZeit gesetzt: %s\r\n", time);
}

int main(void) {
  char *c;
  char *time;

  init_globals();

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
  sprintf(print_buffer, "%c----------------------------------------------------"
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
    print_screen();
    c = get_input();
    /* ...display dialogs eventually */
    if (*c >= PETSCII_0 && *c <= PETSCII_9) {
      /* if the input starts with a digit, we will interpret it as a number
       * for the item to be sold */
      buy_stock(atoi(c));
    } else if (*c == 'f') {
      buy_custom();
    } else if (*c == 's') {
      cprintf("\r\nsaving items.. ");
      save_items();
      cprintf("ok\r\nsaving credits.. ");
      save_credits();
      cprintf("ok\r\nflushing log.. ");
      log_flush();
      cprintf("ok\r\nStatefile/Creditfile/Log gesichert, dr" uUML
              "cke RETURN...\r\n");
      cget_return();
    } else if (*c == 'g') {
      credit_manager();
    } else if (*c == 'z') {
      set_time_interactive();
    } else if (*c == 'q')
      break;
  }
  clrscr();
  cprintf("\r\nBYEBYE\r\n");

  return 0;
}
