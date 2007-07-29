#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <cbm.h>

#include "general.h"
#include "config.h"
#include "kasse.h"
#include "credit_manager.h"
#include "time.h"
// drucker 4 oder 5
// graphic 4,0,10

char print_buffer[81];

/* Hauptbildschirm ausgeben */
void print_screen() {
	BYTE i = 0;
	char *time = get_time();
	clrscr();
	cprintf("C128-Kassenprogramm\r\n\r\nUhrzeit: %s (wird nicht aktualisiert\r\nEingenommen: %ld Cents, Verkauft: %ld Flaschen, Drucken: %s\r\n\r\n", time, money, items_sold, (printing == 1 ? "ein" : "aus"));
	free(time);
	for (; i < num_items; ++i)
		cprintf("Eintrag %x: %s (%d Cents, %d mal verkauft)\r\n", i, status[i].item_name, status[i].price, status[i].times_sold);
	cprintf("\r\nBefehle: s) Daten sichern d) Drucken umschalten\r\ng) Guthabenverwaltung z) Zeit setzen\r\n");
}

void log_file(char * s) {
	FILE * f;
	if (s==NULL)
		return;
	f = fopen("log", "a");
	if (f==NULL)
		c128_perror(23, "kann logfile nicht oeffnen");
	fputs(s, f);
	fclose(f);
}

/* Druckt eine entsprechende Zeile aus */
void print_log(BYTE n, int einheiten, char *nickname) {
	BYTE c;
	/* Format: 
	   Transaction-ID (Anzahl verkaufter Einträge, inklusive des zu druckenden!)
	   Uhrzeit (TODO)
	   Eintragname (= Getränk)
	   Preis (in Cents)
	   Anzahl
	   Nickname (falls es vom Guthaben abgezogen wird)
	   */
//	sprintf(print_buffer, "[%d] UHRZEIT - %s - %d - %d - an %s\r\n", items_sold, status[n].item_name, status[n].price, einheiten, (nickname != NULL ? nickname : "Unbekannt"));
	c = cbm_open((BYTE)4, (BYTE)4, (BYTE)0, NULL);
	if (c != 0) {
		c128_perror(c, "cbm_open(printer)");
		save_state();
		exit(1);
	}
	c = cbm_write((BYTE)4, print_buffer, strlen(print_buffer));
	if (c != strlen(print_buffer)) {
		c128_perror(c, "write(printer)");
		save_state();
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
	BYTE c, nickname_len, single_match;
	int einheiten;
	char *nickname;
	if (status[n].item_name == NULL)
		cprintf("FEHLER: Diese Einheit existiert nicht.\r\n");
	else {
		cprintf("Wieviel Einheiten \"%s\"?\r\n", status[n].item_name);
		while (1) {
			c = getchar();
			if (c == 13)
				break;
			else if (c == '-'&& i == 0)
				negative = -1;
			else if (c > 47 && c < 58)
				entered[i++] = c;
		}
		einheiten = atoi(entered) * negative;
		cprintf("\r\nAuf ein Guthaben kaufen? Wenn ja, Nickname eingeben:\r\n");
		nickname = get_input();
		if (nickname[0] == '\0') {
			free(nickname);
			nickname = NULL;
		} else {
			nickname_len = strlen(nickname);
			/* go through credits and remove the amount of money or set nickname
			 * to NULL if no such credit could be found */
			for (c = 0; c < num_credit_items; ++c)
				if (strncmp(nickname, credits[c].nickname, nickname_len) == 0) {
					if (++matches == 2)
						break;
					else single_match = c;
				}
			if (matches == 1) {
				if (credits[single_match].credit < (status[n].price * einheiten)) {
					cprintf("Sorry, %s hat nicht genug Geld :-(\r\n", nickname);
					free(nickname);
					return;
				} else {
					/* Geld abziehen */
					credits[single_match].credit -= (status[n].price * einheiten);
					cprintf("\r\nVerbleibendes Guthaben fuer %s: %d Cents. Druecke ANYKEY...\r\n", nickname, credits[single_match].credit);
					getchar();
				}
			} else if (matches == 0) {
				// TODO
			} else {
				free(nickname);
				nickname = NULL;
			}
		}
		status[n].times_sold += einheiten;
		money += status[n].price * einheiten;
		items_sold += einheiten;
		if (printing == 1)
			print_log(n, einheiten, nickname);
		if (nickname != NULL)
			free(nickname);
	}
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
	free(time);
}

int main() {
	BYTE c;
	/* Zeit erstmalig setzen */
	set_time_interactive();
	POKE(216, 255);
	/* Konfigurationsdatei laden */
	load_config();
	/* Einträge (=Getränke) laden */
	load_items();
	/* Zustand laden */
	load_state();
	/* Guthaben laden */
//	load_credits();
	while (1) {
		/* Bildschirm anzeigen */
		print_screen();
		/* Tastatureingaben abfragen */
		c = getchar();
		/* und eventuell weitere Dialoge anzeigen */
		if (c > 47 && c < 58)
			buy(c - 48);
		else if (c == 's') {
			/* Zustandsdatei schreiben */
			save_state();
//			save_credits();
			cprintf("Statefile/Creditfile gesichert, druecke ANYKEY...\r\n");
			getchar();
		} else if (c == 'd') {
			/* Drucken an- oder ausschalten */
			printing = (printing == 1 ? 0 : 1);
			cprintf("Drucken ist nun %s, druecke ANYKEY...\r\n", (printing == 1 ? "eingeschaltet" : "ausgeschaltet"));
			getchar();
		} else if (c == 'g') {
			/* Guthabenverwalter aufrufen */
			credit_manager();
		} else if (c == 'z') {
			/* Zeit setzen */
			set_time_interactive();
		} else if (c == 'q')
			break;
	}
	cprintf("BYEBYE\r\n");
}
