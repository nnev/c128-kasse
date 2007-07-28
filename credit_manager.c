#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "general.h"

char *filter = NULL;

void print_credit_screen() {
	uc i = 0;
	clrscr();
	printf("C128-Kassenprogramm: Guthabenverwaltung\n\n");
	for (; i < num_credit_items; ++i)
		printf("Guthaben für %s: %d Cents\n", credits[i].nickname, credits[i].credit);
	printf("\nBefehle: f) Filtern q) Back to main screen\n");
}

/* Guthabenverwalter */
void credit_manager() {
	uc c;
	while (1) {
		print_credit_screen();
		c = getchar();
		if (c == 'f') {
			printf("Filter eingeben:\n");
			if (filter != NULL)
				free(filter);
			filter = get_input();
			if (filter[0] == '0') {
				free(filter);
				filter = NULL;
			}
		} else if (c == 'q')
			break;
	}
}
