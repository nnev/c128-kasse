/* 
 * RGB2R-C128-Kassenprogramm
 * (c) 2007 phil_fry, sECuRE, sur5r
 * See LICENSE for license information
 *
 */
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "general.h"

static char *filter = NULL;
static BYTE filter_len;

static BYTE current_credits_page = 0;

static void credit_print_screen() {
	BYTE i, pages;
	char buffer[10];

	clrscr();
	cprintf("itemz (phil_fry, sECuRE, sur5r)\r\n\r\n");
	/* 16 entries per page */
	pages = (credits.num_items / 16);
	if (current_credits_page > pages)
		current_credits_page = pages;
	cprintf("Datei: CREDITS (Seite %d von %d)\r\n\r\n", current_credits_page, pages);
	for (i = (current_credits_page * 16); i < credits.num_items && i < ((current_credits_page+1) * 16); i++) {
		if (filter == NULL || strncmp(credits.credits[i].nickname, filter, filter_len) == 0) {
			if (format_euro(buffer, 10, credits.credits[i].credit) != buffer) {
				cprintf("Error: Could not format credit %d\r\n", credits.credits[i].credit);
				exit(1);
			}
	
			cprintf("%d: %s: %s\r\n", i, credits.credits[i].nickname, buffer);
		}
	}
	cprintf("\r\nn) Neu d) Loeschen p) Einzahlen b) Seite hoch f) Seite runter\r\ng) Filtern e) Aendern s) Speichern z) Zurueck\r\n");
}

struct credits_t *find_credit(char *name){
	int i;
	for (i = 0; i < credits.num_items; i++)
		if (strncmp(name, credits.credits[i].nickname, NICKNAME_MAX_LEN + 1) == 0)
			return &credits.credits[i];
	return NULL;
}

/*
 * when depositing money with this and returning to the main menu, the program
 * will crash with a message like the following:
 * 
 */
static void deposit_credit() {
	char *input;
	struct credits_t *credit;
	unsigned int deposit;

	cprintf("\r\nName:\r\n");
	if ((input = get_input()) == NULL || *input == '\0')
		return; // no name given
		
	if ((credit = find_credit(input)) == NULL)
		return; // cannot find named credit
	
	cprintf("\r\nEinzahlung in Cent:\r\n");
	if ((input = get_input()) == NULL || *input == '\0' || (deposit = atoi(input)) == 0)
		return;

	credit->credit += deposit;
	
	toggle_videomode();
	cprintf("%d Cent eingezahlt fuer %s.\r\nRestguthaben: %d\r\n", deposit, credit->nickname, credit->credit);
	toggle_videomode();
	cprintf("\r\nEinzahlung durchgefuehrt, drucke RETURN...\r\n");
	input = get_input();
	toggle_videomode();
	clrscr();
	toggle_videomode();
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

void credit_manager(){
	char *c;
	while(1){
		credit_print_screen();
		c = get_input();
		switch (*c) {
			case 'n':
				new_credit(); break;
			case 'd':
				delete_credit(); break;
			case 's':
				save_credits(); break;
			case 'f':
				if (current_credits_page < (credits.num_items / 16))
						current_credits_page++;
				break;
			case 'b':
				if (current_credits_page > 0)
					current_credits_page--;
				break;
			case 'p':
				deposit_credit(); break;
			case 'g':
				cprintf("Filter eingeben:\r\n");
				filter = get_input();
				if (filter == NULL || *filter == 32 || (filter_len = strlen(filter)) == 0)
					filter = NULL;
				break;
			case 'z':
				return; 
			default:
				cprintf("Unbekannter Befehl, druecke RETURN...\r\n");
				get_input(); 
		}
	}	
}
