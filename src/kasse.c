/* 
 * RGB2R-C128-Kassenprogramm
 * (c) 2007 phil_fry, sECuRE, sur5r
 * See LICENSE for license information
 *
 */
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <cbm.h>

#include "general.h"
#include "config.h"
#include "kasse.h"
#include "credit_manager.h"
#include "c128time.h"
// drucker 4 oder 5
// graphic 4,0,10

char print_buffer[81];

/* Hauptbildschirm ausgeben */
static void print_screen() {
	BYTE i = 0;
	char *time = get_time();
	char profit[10];
	clrscr();
	if (format_euro(profit, 10, money) == NULL) {
		cprintf("Einname %ld konnte nicht umgerechnet werden\r\n", money);
		exit(1);
	}
	cprintf("C128-Kassenprogramm (phil_fry, sECuRE, sur5r)\r\
\r\nUhrzeit: %s (wird nicht aktualisiert)\r\
Eingenommen: %s, Verkauft: %ld Flaschen, Drucken: %s\r\n\r\n", 
	time, profit, items_sold, (printing == 1 ? "ein" : "aus"));
	for (; i < status.num_items; ++i)
		cprintf("Eintrag %x: %s (%d Cents, %d mal verkauft)\r\n",
			i, status.status[i].item_name, status.status[i].price, status.status[i].times_sold);
	cprintf("\r\nBefehle: s) Daten sichern d) Drucken umschalten\r\
g) Guthabenverwaltung     z) Zeit setzen\r\
n) Neues Getraenk\r\n");
}

static void log_file(const char *s) {
	BYTE c = cbm_open((BYTE)8, (BYTE)8, (BYTE)1, "log");
	if (c != 0) {
		c128_perror(c, "cbm_open(log)");
		save_items();
		save_credits();
		exit(1);
	}
	c = cbm_write((BYTE)8, s, strlen(s));
	if (c != strlen(s)) {
		cprintf("Could not save logfile, please make sure the floppy is not full!\n");
		save_items();
		save_credits();
		exit(1);
	}
	cbm_close((BYTE)8);
}

/* Druckt eine entsprechende Zeile aus */
static void print_log(BYTE n, int einheiten, char *nickname) {
	BYTE c;
	char *time = get_time();
	char price[10];
	/* Format: 
	   Transaction-ID (Anzahl verkaufter Einträge, inklusive des zu druckenden!)
	   Uhrzeit
	   Eintragname (= Getränk)
	   Preis (in Cents)
	   Anzahl
	   Nickname (falls es vom Guthaben abgezogen wird)
	   */
	if (format_euro(price, 10, status.status[n].price) == NULL) {
		cprintf("Preis %d konnte nicht umgerechnet werden\r\n", status.status[n].price);
		exit(1);
	}
		
	sprintf(print_buffer, "[%lu] %s - %s - %s - %d - an %s\r\n",
		items_sold, time, status.status[n].item_name, price, 
		einheiten, (nickname != NULL ? nickname : "Unbekannt"));
	c = cbm_open((BYTE)4, (BYTE)4, (BYTE)0, NULL);
	if (c != 0) {
		c128_perror(c, "cbm_open(printer)");
		save_items();
		save_credits();
		exit(1);
	}
	c = cbm_write((BYTE)4, print_buffer, strlen(print_buffer));
	if (c != strlen(print_buffer)) {
		c128_perror(c, "write(printer)");
		save_items();
		save_credits();
		exit(1);
	}
	cbm_close((BYTE)4);
	log_file(print_buffer);
}

/* Dialog, der einen durch's Abrechnen der Einträge führt */
void buy(BYTE n) {
	int negative = 1;
	char entered[5] = {'1', 0, 0, 0, 0};
	BYTE i = 0, matches = 0;
	BYTE c, nickname_len;
	int einheiten;
	char *nickname;

	if (status.status[n].item_name == NULL) {
		cprintf("FEHLER: Diese Einheit existiert nicht.\r\n");
		return;
	}
	cprintf("Wieviel Einheiten \"%s\"? [1] \r\n", status.status[n].item_name);
	while (1) {
		c = getchar();
		if (c == 13)
			break;
		else if (c == 27) {
			cprintf("Kauf abgebrochen, druecke RETURN...\r\n");
			get_input();
			return;
		} else if (c == '-' && i == 0)
			negative = -1;
		else if (c > 47 && c < 58)
			entered[i++] = c;
	}
	einheiten = atoi(entered) * negative;
	cprintf("\r\nAuf ein Guthaben kaufen? Wenn ja, Nickname eingeben:\r\n");
	nickname = get_input();
	if (nickname != NULL && *nickname != '\0' && *nickname != 32) {
		nickname_len = strlen(nickname);
		/* go through credits and remove the amount of money or set nickname
		 * to NULL if no such credit could be found */
		for (c = 0; c < credits.num_items; ++c)
			if (strncmp(nickname, credits.credits[c].nickname, nickname_len) == 0) {
				if (credits.credits[c].credit < (status.status[n].price * einheiten)) {
					cprintf("Sorry, %s hat nicht genug Geld :-(\r\n", nickname);
					return;
				}
				/* Geld abziehen */
				credits.credits[c].credit -= (status.status[n].price * einheiten);
				cprintf("\r\nVerbleibendes Guthaben fuer %s: %d Cents. Druecke RETURN...\r\n",
					nickname, credits.credits[c].credit);
				get_input();
				matches++;
				break;
			}
		if (matches == 0) {
			cprintf("\r\nNickname nicht gefunden in der Guthabenverwaltung! Abbruch, druecke RETURN...\r\n");
			get_input();
			return;
		}
	} else {
		/* Ensure that nickname is NULL if it's empty because it's used in print_log */
		nickname = NULL;
	}
	status.status[n].times_sold += einheiten;
	money += status.status[n].price * einheiten;
	items_sold += einheiten;
	if (printing == 1)
		print_log(n, einheiten, nickname);
}

void set_time_interactive() {
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
	cprintf("Zeit gesetzt: %s\r\n", time);
}

int main() {
	char *c;
	toggle_videomode();
	/* Zeit erstmalig setzen */
	set_time_interactive();
	POKE(216, 255);
	/* Variablen zurechtbiegen */
	credits.num_items = 0;
	status.num_items = 0;
	/* Konfigurationsdatei laden */
	load_config();
	/* Einträge (=Getränke) und Zustand laden */
	load_items();
	/* Guthaben laden */
	load_credits();
	while (1) {
		/* Bildschirm anzeigen */
		print_screen();
		/* Tastatureingaben abfragen */
		c = get_input();
		/* und eventuell weitere Dialoge anzeigen */
		if (*c > 47 && *c < 58)
			buy((*c) - 48);
		else if (*c == 's') {
			/* Zustandsdatei schreiben */
			save_items();
			save_credits();
			cprintf("Statefile/Creditfile gesichert, druecke RETURN...\r\n");
			get_input();
		} else if (*c == 'd') {
			/* Drucken an- oder ausschalten */
			printing = (printing == 1 ? 0 : 1);
			cprintf("Drucken ist nun %s, druecke RETURN...\r\n", 
				(printing == 1 ? "eingeschaltet" : "ausgeschaltet"));
			get_input();
		} else if (*c == 'g') {
			/* Guthabenverwalter aufrufen */
			credit_manager();
		} else if (*c == 'z') {
			/* Zeit setzen */
			set_time_interactive();
		} else if (*c == 'n') {
			strcpy(status.status[status.num_items].item_name, "mate");
			status.status[status.num_items].price = 23;
			status.status[status.num_items].times_sold = 5;
			status.num_items++;
		} else if (*c == 'q')
			break;
	}
	cprintf("BYEBYE\r\n");
}
