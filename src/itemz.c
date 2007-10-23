/*
 * RGB2R-C128-Kassenprogramm
 * (c) 2007 phil_fry, sECuRE, sur5r
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

enum itemz_mode { MODE_ITEMS, MODE_CREDITS };

static enum itemz_mode mode = MODE_ITEMS;
static BYTE current_credits_page = 0;

static void print_screen() {
	BYTE i, pages;
	char buffer[10];

	clrscr();
	cprintf("itemz (phil_fry, sECuRE, sur5r)\r\n\r\n");
	if (mode == MODE_ITEMS) {
		cprintf("Datei: ITEMS\r\n\r\n");
		for (i = 0; i < status.num_items; i++) {
			if (format_euro(buffer, 10, status.status[i].price) != buffer) {
				cprintf("Error: Could not format price %d\r\n", status.status[i].price);
				exit(1);
			}
			cprintf("Eintrag %x: %s (%s, %d mal verkauft)\r\n",
				i, status.status[i].item_name, buffer, status.status[i].times_sold);
		}
		cprintf("\r\nn) Neu d) Loeschen s) Speichern m) Modus\r\n");
	} else {
		/* 16 entries per page */
		pages = (credits.num_items / 16);
		if (current_credits_page > pages)
			current_credits_page = pages;
		cprintf("Datei: CREDITS (Seite %d von %d)\r\n\r\n", current_credits_page, pages);
		for (i = (current_credits_page * 16); i < credits.num_items && i < ((current_credits_page+1) * 16); i++) {
			if (format_euro(buffer, 10, credits.credits[i].credit) != buffer) {
				cprintf("Error: Could not format credit %d\r\n", credits.credits[i].credit);
				exit(1);
			}

			cprintf("%d: %s: %s\r\n", i, credits.credits[i].nickname, buffer);
		}
		cprintf("\r\nn) Neu d) Loeschen b) Seite hoch f) Seite runter\r\ne) Aendern s) Speichern m) Modus\r\n");
	}
}

static void new_item() {
	char *input, *name;
	int price;

	if (status.num_items == 16) {
		cprintf("\rEs ist bereits die maximale Anzahl an Eintraegen erreicht, druecke RETURN...\r\n");
		input = get_input();
		return;
	}

	cprintf("\rName des Eintrags:\r\n");
	if ((input = get_input()) == NULL || *input == '\0')
		return;
	name = strdup(input);
	cprintf("\r\nPreis in Cents:\r\n");
	if ((input = get_input()) == NULL || *input == '\0' || (price = atoi(input)) == 0)
		return;
	cprintf("\r\nWie oft schon verkauft? [0] \r\n");
	if ((input = get_input()) == NULL || *input == '\0')
		return;
	strcpy(status.status[status.num_items].item_name, name);
	status.status[status.num_items].price = price;
	status.status[status.num_items].times_sold = atoi(input);
	status.num_items++;
	free(name);
}

static void _delete_item(BYTE num) {
	memset(status.status[num].item_name, '\0', 10);
	status.status[num].price = 0;
	status.status[num].times_sold = 0;
}

static void delete_item() {
	char *input;
	BYTE num, last;

	cprintf("\r Welcher Eintrag soll geloescht werden?\r\n");
	if ((input = get_input()) == NULL || *input == '\0')
		return;
	num = atoi(input);
	if (status.num_items > 1) {
		/* Swap last item with this one and delete the last one to avoid holes */
		last = (status.num_items - 1);
		strcpy(status.status[num].item_name, status.status[last].item_name);
		status.status[num].price = status.status[last].price;
		status.status[num].times_sold = status.status[last].times_sold;
		_delete_item(last);
	} else {
		/* Just delete it */
		_delete_item(num);
	}
	status.num_items--;
}

static void new_credit() {
	char *input, *name;
	int credit;

	if (credits.num_items == 75) {
		cprintf("\rEs ist bereits die maximale Anzahl an Eintraegen erreicht, druecke RETURN...\r\n");
		input = get_input();
		return;
	}

	cprintf("\rNickname:\r\n");
	if ((input = get_input()) == NULL || *input == '\0')
		return;
	name = strdup(input);
	cprintf("\r\nGuthaben in Cents:\r\n");
	if ((input = get_input()) == NULL || *input == '\0' || (credit = atoi(input)) == 0)
		return;
	strcpy(credits.credits[credits.num_items].nickname, name);
	credits.credits[credits.num_items].credit = credit;
	credits.num_items++;
	free(name);
}

static void _delete_credit(BYTE num) {
	memset(credits.credits[num].nickname, '\0', 11);
	credits.credits[num].credit = 0;
}

static void delete_credit() {
	char *input;
	BYTE num, last;

	cprintf("\r Welcher Eintrag soll geloescht werden?\r\n");
	if ((input = get_input()) == NULL || *input == '\0')
		return;
	num = atoi(input);
	if (credits.num_items > 1) {
		/* Swap last item with this one and delete the last one to avoid holes */
		last = (credits.num_items - 1);
		strcpy(credits.credits[num].nickname, credits.credits[last].nickname);
		credits.credits[num].credit = credits.credits[last].credit;
		_delete_credit(last);
	} else {
		/* Just delete it */
		_delete_credit(num);
	}
	credits.num_items--;
}

int main() {
	char *c;
	toggle_videomode();
	credits.num_items = 0;
	status.num_items = 0;
	cprintf("itemz loading...\n");
	load_config();
	cprintf("itemz: loading ITEMS...\n");
	load_items();
	cprintf("itemz: loading CREDITS...\n");
	load_credits();
	while (1) {
		print_screen();
		c = get_input();
		if (mode == MODE_ITEMS) {
			if (*c == 'n')
				new_item();
			else if (*c == 'd')
				delete_item();
			else if (*c == 's')
				save_items();
			else if (*c == 'm')
				mode = MODE_CREDITS;
			else {
				cprintf("Unbekannter Befehl, druecke RETURN...\r\n");
				c = get_input();
			}
		} else {
			if (*c == 'm')
				mode = MODE_ITEMS;
			else if (*c == 'n')
				new_credit();
			else if (*c == 'd')
				delete_credit();
			else if (*c == 's')
				save_credits();
			else if (*c == 'f') {
				if (current_credits_page < (credits.num_items / 16))
					current_credits_page++;
			} else if (*c == 'b') {
				if (current_credits_page > 0)
					current_credits_page--;
			}
			else {
				cprintf("Unbekannter Befehl, druecke RETURN...\r\n");
				c = get_input();
			}
		}
	}
	return 0;
}
