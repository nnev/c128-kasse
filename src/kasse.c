/* 
 * RGB2R-C128-Kassenprogramm
 * (c) 2007-2008 phil_fry, sECuRE, sur5r
 * See LICENSE for license information
 *
 */
#define _IS_KASSE
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

/* NOTE: undocumented function which scratches files
   We need to use this function because linking unistd.h
   makes our program break at runtime.
 */
unsigned char __fastcall__ _sysremove(const char *name);

static void sane_exit() {
	save_items();
	save_credits();
	exit(1);
}

/* Hauptbildschirm ausgeben */
static void print_screen() {
	BYTE i = 0;
	char *time = get_time();
	char profit[10];
	clrscr();
	if (format_euro(profit, 10, money) == NULL) {
		cprintf("Einnahme %ld konnte nicht umgerechnet werden\r\n", money);
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
q) Beenden\r\n");
}

static void log_file(const char *s) {
	/* A log-entry has usually 50 bytes, so we take 64 bytes.
	   Because files are wrapped (log.0, log.1, ...) every 100
	   lines, we don't need more than 100 * 64 bytes. */
	char *buffer = malloc(sizeof(char) * 64 * 100);
	char filename[8];
	char read = 0;
	unsigned int c;
	if (buffer == NULL) {
		cprintf("No memory available\n");
		}
	buffer[0] = '\0';
	if (((++log_lines_written) % 100) == 0)
		log_num++;
	sprintf(filename, "log-%d", log_num);
	/* Don't read log if there were no lines written before */
	if (log_lines_written != 1) {
		if ((c = cbm_open((BYTE)8, (BYTE)8, (BYTE)0, filename)) != 0) {
			c128_perror(c, "cbm_open(log)");
			sane_exit();
		}
		read = cbm_read((BYTE)8, buffer, sizeof(char) * 64 * 100);
		cbm_close((BYTE)8);
		_sysremove(filename);
	}
	if ((c = cbm_open((BYTE)8, (BYTE)8, (BYTE)1, filename)) != 0) {
		c128_perror(c, "cbm_open(log)");
		sane_exit();
	}
	/* TODO: read  < 0? */
	strcpy(buffer+read, s);
	c = cbm_write((BYTE)8, buffer, read+strlen(s));
	if (c != (read+strlen(s))) {
		cprintf("Could not save logfile (wrote %d bytes, wanted %d bytes), please make sure the floppy is not full!\n", c, (read+strlen(s)));
		sane_exit();
	}
	cbm_close((BYTE)8);
	free(buffer);
}

static char retry_or_quit() {
	char *c;
	do {
		cprintf("\r\nr)etry or q)uit?\r\n");
		c = get_input();
	} while ((*c != 'r') && (*c != 'q'));
	return *c;
}

/* Prints a line and logs it to file */
static void print_log(BYTE n, int einheiten, char *nickname) {
	BYTE c;
	char *time = get_time();
	char price[10];
	/* Format: 
	   Transaction-ID (Anzahl verkaufter Eintr�ge, inklusive des zu druckenden!)
	   Uhrzeit
	   Eintragname (= Getr�nk)
	   Preis (in Cents)
	   Anzahl
	   Nickname (falls es vom Guthaben abgezogen wird)
	   */
	if (format_euro(price, 10, status.status[n].price) == NULL) {
		cprintf("Preis %d konnte nicht umgerechnet werden\r\n", status.status[n].price);
		exit(1);
	}
		
	sprintf(print_buffer, "%c[%lu] %s - %s - %s - %d - an %s\r\n",  17,
			items_sold, time, status.status[n].item_name, price, 
			einheiten, (*nickname != '\0' ? nickname : "Unbekannt"));
RETRY:
	c = cbm_open((BYTE)4, (BYTE)4, (BYTE)0, NULL);
	if (c != 0) {
		c128_perror(c, "cbm_open(printer)");
		if (retry_or_quit() == 'q')
			sane_exit();

		goto RETRY;
	}
	c = cbm_write((BYTE)4, print_buffer, strlen(print_buffer));
	if (c != strlen(print_buffer)) {
		c128_perror(c, "write(printer)");
		if (retry_or_quit() == 'q') {
			save_items();
			save_credits();
			exit(1);
		}
		goto RETRY;
	}
	cbm_close((BYTE)4);
	log_file(print_buffer);
}

/* dialog which is called for each bought item */
void buy(BYTE n) {
	int negative = 1;
	char entered[5] = {'1', 0, 0, 0, 0};
	BYTE i = 0, matches = 0;
	BYTE c, nickname_len;
	int einheiten;
	char *input;
	char nickname[11];
	struct credits_t *credit;

	if (n >= status.num_items || status.status[n].item_name == NULL) {
		cprintf("FEHLER: Diese Einheit existiert nicht.\r\n");
		get_input();
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
	
	toggle_videomode();
	cprintf("%dx %s fuer ", einheiten, status.status[n].item_name);
	toggle_videomode();
	
	cprintf("\r\nAuf ein Guthaben kaufen? Wenn ja, Nickname eingeben:\r\n");
	input = get_input();
	strncpy(nickname, input, 11);
	if (*nickname != '\0') {
		toggle_videomode();
		cprintf("%s\r\n", nickname);
		toggle_videomode();
	}

	if (nickname != NULL && *nickname != '\0' && *nickname != 32) {
		nickname_len = strlen(nickname);
		/* go through credits and remove the amount of money or set nickname
		 * to NULL if no such credit could be found */
		credit = find_credit(nickname);
		if (credit != NULL) {
			if ((signed int)credit->credit < ((signed int)status.status[n].price * einheiten)) {
				cprintf("Sorry, %s hat nicht genug Geld :-(\r\n", nickname);
				get_input();
				return;
			}
			/* substract money */
			credit->credit -= (status.status[n].price * einheiten);
			cprintf("\r\nVerbleibendes Guthaben fuer %s: %d Cents. Druecke RETURN...\r\n",
				nickname, credit->credit);
			toggle_videomode();
			cprintf("\r\nDein verbleibendes Guthaben betraegt %d Cents.\r\n", credit->credit);
			toggle_videomode();
			get_input();
			matches++;
		} else {
			cprintf("\r\nNickname nicht gefunden in der Guthabenverwaltung! Abbruch, druecke RETURN...\r\n");
			get_input();
			return;
		}
	} else {
		/* Ensure that nickname is NULL if it's empty because it's used in print_log */
		*nickname = '\0';
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

	if (VIDEOMODE == 40)
		toggle_videomode();
	/* Set time initially, c128 doesn't know it */
	set_time_interactive();

	POKE(216, 255);

	/* Load configuration */
	load_config();
	cprintf("got %d logfiles\r\n", log_num);

	/* Load items (= drinks) */
	load_items();
	/* Load credits */
	load_credits();
	while (1) {
		print_screen();
		c = get_input();
		/* ...display dialogs eventually */
		if (*c > 47 && *c < 58) {
			buy((*c) - 48);
			toggle_videomode();
			clrscr();
			toggle_videomode();
		} else if (*c == 's') {
			save_items();
			save_credits();
			cprintf("Statefile/Creditfile gesichert, druecke RETURN...\r\n");
			get_input();
		} else if (*c == 'd') {
			/* enable/disable printing */
			printing = (printing == 1 ? 0 : 1);
			cprintf("Drucken ist nun %s, druecke RETURN...\r\n", 
				(printing == 1 ? "eingeschaltet" : "ausgeschaltet"));
			get_input();
		} else if (*c == 'g') {
			credit_manager();
		} else if (*c == 'z') {
			set_time_interactive();
		} else if (*c == 'q')
			break;
	}
	cprintf("BYEBYE\r\n");
}