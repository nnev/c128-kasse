#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "general.h"

char *filter = NULL;
uc filter_len;

void print_credit_screen() {
	uc i = 0;
	clrscr();
	printf("C128-Kassenprogramm: Credit Manager\n\n");
	for (; i < num_credit_items; ++i)
		if (filter == NULL || strncmp(credits[i].nickname, filter, filter_len) == 0)
			printf("Guthaben %s: %d Cents\n", credits[i].nickname, credits[i].credit);
	printf("\nBefehle: n) Neues Guthaben f) Filtern z) Zurueck\n");
}

/* Guthabenverwalter */
void credit_manager() {
	int negative = 1;
	uc c;
	char *nickname, *credits_input;
	/* credits_int is a stupid name, but overlaps with struct credits_t credits; else */
	int credits_int;
	while (1) {
		print_credit_screen();
		c = getchar();
		if (c == 'n') {
			printf("Nickname eingeben:\n");
			nickname = get_input();
			if (nickname[0] == '\0') {
				free(nickname);
				continue;
			}
			printf("\nGuthaben eingeben:\n");
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
				credits[num_credit_items].nickname = nickname;
				credits[num_credit_items].credit = credits_int;
				++num_credit_items;
			}
			if (negative == -1)
				--credits_input;
			free(credits_input);
		} else if (c == 'f') {
			printf("Filter eingeben:\n");
			if (filter != NULL)
				free(filter);
			filter = get_input();
			if (filter[0] == '0') {
				free(filter);
				filter = NULL;
			} else filter_len = strlen(filter);
		} else if (c == 'z')
			break;
	}
}
