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

static void credit_print_screen(void) {
  BYTE i, pages;
  char buffer[EUR_FORMAT_MINLEN + 1];

  clrscr();
  cprintf("credit_manager (phil_fry, sECuRE, sur5r) v:" GV "\r\n\r\n");
  pages = (credits.num_items / CREDITS_PER_PAGE);
  if (current_credits_page > pages)
    current_credits_page = pages;
  cprintf("Datei: CREDITS (Seite %d von %d)\r\n\r\n", current_credits_page,
          pages);
  for (i = (current_credits_page * CREDITS_PER_PAGE);
       i < credits.num_items &&
       i < ((current_credits_page + 1) * CREDITS_PER_PAGE);
       i++) {
    if (filter == NULL ||
        strncmp(credits.credits[i].nickname, filter, filter_len) == 0) {
      if (format_euro(buffer, sizeof(buffer), credits.credits[i].credit) !=
          buffer) {
        cprintf("Error: Could not format credit %d\r\n",
                credits.credits[i].credit);
        exit(1);
      }

      cprintf("%d: %s: %s\r\n", i, credits.credits[i].nickname, buffer);
    }
  }
  cprintf("\r\nn) Neu d) L" oUML "schen p) Einzahlen b) Seite hoch "
          "f) Seite runter\r\ng) Filtern e) " AUML "ndern s) Speichern "
          "z) Zur" uUML "ck\r\n");
}

struct credits_t *find_credit(char *name) {
  int i;
  for (i = 0; i < credits.num_items; i++)
    if (strncmp(name, credits.credits[i].nickname, NICKNAME_MAX_LEN + 1) == 0)
      return &credits.credits[i];
  return NULL;
}

void deposit_credit(char *nickname) {
  char *time = get_time();
  struct credits_t *credit;
  unsigned int deposit;

  if ((credit = find_credit(nickname)) == NULL)
    return; // cannot find named credit

  cprintf("\r\nEinzahlung in Cent:\r\n");
  if (cget_number(0) == 0)
    return;

  credit->credit += deposit;

  print_the_buffer();
  cprintf("\r\nEinzahlung durchgef" uUML "hrt, dr" uUML "cke RETURN...\r\n");
  cget_return();
}

static void new_credit(void) {
  char name[NICKNAME_MAX_LEN + 1];
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

  cprintf("\r\nGuthaben in Cents:\r\n");
  if ((credit = cget_number(0)) == 0)
    return;
  strncpy(credits.credits[credits.num_items].nickname, name, NICKNAME_MAX_LEN);
  credits.credits[credits.num_items].credit = credit;

  time = get_time();
  sprintf(print_buffer, "%c%s - Guthaben mit %d Cent f" uUML "r %s angelegt\r",
          17, time, credit, name);
  print_the_buffer();

  credits.num_items++;
}

static void _delete_credit(BYTE num) {
  memset(credits.credits[num].nickname, '\0', NICKNAME_MAX_LEN + 1);
  credits.credits[num].credit = 0;
}

static void delete_credit(void) {
  char *input;
  BYTE num, last;

  cprintf("\r Welcher Eintrag soll gel" oUML "scht werden?\r\n");
  if ((input = get_input()) == NULL || *input == '\0')
    return;
  num = atoi(input);
  if (credits.num_items > 1) {
    /* Swap last item with this one and delete the last one to avoid holes */
    last = (credits.num_items - 1);
    strncpy(credits.credits[num].nickname, credits.credits[last].nickname,
            NICKNAME_MAX_LEN);
    credits.credits[num].credit = credits.credits[last].credit;
    _delete_credit(last);
  } else {
    /* Just delete it */
    _delete_credit(num);
  }
  credits.num_items--;
}

void credit_manager() {
  char nickname[NICKNAME_MAX_LEN + 1];
  char *c;
  while (1) {
    credit_print_screen();
    c = get_input();
    switch (*c) {
    case 'n':
      new_credit();
      break;
    case 'd':
      delete_credit();
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
      cputs("\rName?\r\n");
      if (cgetn_input(nickname, sizeof(nickname)))
        deposit_credit(nickname);
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
