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

static void print_credit_screen() {
	BYTE i = 0;
	clrscr();
	cprintf("C128-Kassenprogramm: Credit Manager\r\n\r\n");
	for (; i < credits.num_items; ++i)
		if (filter == NULL || strncmp(credits.credits[i].nickname, filter, filter_len) == 0)
			cprintf("Guthaben %s: %d Cents\r\n", credits.credits[i].nickname, credits.credits[i].credit);
	cprintf("\r\nBefehle: n) Neues Guthaben f) Filtern z) Zurueck\r\n");
}

/* Guthabenverwalter */
void credit_manager() {
	int negative = 1;
	char *c, *nickname, *credits_input;
	/* credits_int is a stupid name, but overlaps with struct credits_t credits; else */
	int credits_int;
	while (1) {
		print_credit_screen();
		c = get_input();
		if (c == NULL || *c == '\0')
			continue;
		if (*c == 'n') {
			cprintf("\r\nGuthaben eingeben:\r\n");
			credits_input = get_input();

			if (credits_input == NULL || credits_input[0] == '\0')
				continue;
			else if (credits_input[0] == '-') {
				negative = -1;
				++credits_input;
			}
			credits_int = atoi(credits_input) * negative;
			if (credits_int > 0) {
				cprintf("Nickname eingeben:\r\n");
				nickname = get_input();
				if (nickname == NULL || nickname[0] == '\0')
					continue;
				strncpy(credits.credits[credits.num_items].nickname, nickname, 9);
				credits.credits[credits.num_items].credit = credits_int;
				++credits.num_items;
			}
		} else if (*c == 'f') {
			cprintf("Filter eingeben:\r\n");
			filter = get_input();
			if (filter == NULL || *filter == 32 || (filter_len = strlen(filter)) == 0)
				filter = NULL;
		} else if (*c == 'z' || *c == 'q')
			break;
	}
}
