#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "general.h"

char *filter = NULL;
BYTE filter_len;

void print_credit_screen() {
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
	BYTE c;
	char *nickname, *credits_input;
	/* credits_int is a stupid name, but overlaps with struct credits_t credits; else */
	int credits_int;
	while (1) {
		print_credit_screen();
		c = getchar();
		if (c == 'n') {
			cprintf("Nickname eingeben:\r\n");
			nickname = get_input();
			if (nickname[0] == '\0') {
				free(nickname);
				continue;
			}
			cprintf("\r\nGuthaben eingeben:\r\n");
			credits_input = get_input();
			if (credits_input[0] == '\0') {
				free(credits_input);
				continue;
			}
			if (credits_input[0] == '-') {
				negative = -1;
				++credits_input;
			}
			credits_int = atoi(credits_input) * negative;
			if (credits_int > 0) {
				strcpy(credits.credits[credits.num_items].nickname, nickname);
				credits.credits[credits.num_items].credit = credits_int;
				++credits.num_items;
			}
			if (negative == -1)
				--credits_input;
			free(credits_input);
		} else if (c == 'f') {
			cprintf("Filter eingeben:\r\n");
			if (filter != NULL)
				free(filter);
			filter = get_input();
			if (filter[0] == '0') {
				free(filter);
				filter = NULL;
			} else filter_len = strlen(filter);
		} else if (c == 'z' || c == 'q')
			break;
	}
}
