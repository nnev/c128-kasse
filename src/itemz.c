/*
 * RGB2R-C128-Kassenprogramm
 * (c) 2007-2008 phil_fry, sECuRE, sur5r
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

/* NOTE: dummy entry to fix linking */
int log_num = 0;
int log_lines_written = 0;

static void itemz_print_screen() {
	BYTE i;
	char buffer[10];

	clrscr();
	cprintf("itemz (phil_fry, sECuRE, sur5r)\r\n\r\n");
	cprintf("Datei: ITEMS\r\n\r\n");
	for (i = 0; i < status.num_items; i++) {
		if (format_euro(buffer, 10, status.status[i].price) != buffer) {
			cprintf("Error: Could not format price %d\r\n", status.status[i].price);
			exit(1);
		}
		cprintf("Eintrag %x: %s (%s, %d mal verkauft)\r\n",
			i, status.status[i].item_name, buffer, status.status[i].times_sold);
	}
	cprintf("\r\nn) Neu d) Loeschen s) Speichern m) Credit Modus q) Beenden\r\n");
}

static void new_item() {
	char *input, *name;
	int price;

	if (status.num_items == MAX_ITEMS) {
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
	memset(status.status[status.num_items].item_name, '\0', MAX_ITEM_NAME_LENGTH+1);
	strncpy(status.status[status.num_items].item_name, name, MAX_ITEM_NAME_LENGTH);
	status.status[status.num_items].price = price;
	status.status[status.num_items].times_sold = atoi(input);
	status.num_items++;
	free(name);
}

static void _delete_item(BYTE num) {
	memset(status.status[num].item_name, '\0', MAX_ITEM_NAME_LENGTH);
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

static void itemz_manager(){
	char *c;
	while(1){
		itemz_print_screen();
		c = get_input();
		switch (*c) {
			case 'n':
				new_item(); break;
			case 'd':
				delete_item(); break;
			case 's':
				save_items(); break;
			case 'm':
				return; // switch to credit mode
			case 'q':
				exit(0);
			default:
				cprintf("Unbekannter Befehl, druecke RETURN...\r\n");
				get_input(); 
		}
	}
}

int main() {
	if (VIDEOMODE == 40)
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
		itemz_manager();
		credit_manager();
	}
	return 0;
}
