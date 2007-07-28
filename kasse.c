#include <stdio.h>
#include <conio.h>

#include "kasse.h"
// conf
// drucker 4 oder 5
// preise+getraenke
//
// graphic 4,0,10



/* Hauptbildschirm ausgeben */
void print_screen() {
	uc i = 0;
	clrscr();
	printf("C128-Kassenprogramm\n\n");
	printf("Eingenommen: %.2d Euro, Verkauft: %d Flaschen\n\n", money * 100, items_sold);
	for (; i < NUM_ITEMS; ++i)
		printf("Item %x: %s (%d Cents, %d mal verkauft)\n", i, status[i].item_name, status[i].preis, status[i].times_sold);
	printf("\nBefehle: s) Save Data\n");
}

/* Wird ausgelagert */
void save_data() {
}

void buy(uc n) {
	static uc einheiten = 1;
	static uc c;
	if (status[n].item_name == NULL)
		printf("ERROR: No such item\n");
	else {
		printf("Wieviel Einheiten \"%s\"?\n", status[n].item_name);
		while (1) {
			c = getchar();
			if (c == 32)
				break;
			else if (c > 47 && c < 58)
				einheiten += (c - 48);
		}
		status[n].times_sold += einheiten;
		money += status[n].preis * einheiten;
		items_sold += einheiten;
	}
}

int main() {
	static uc c;
	/* TODO: remove */
	status[0].item_name = "cola";
	status[0].preis = 230;
	status[0].times_sold = 0;
	status[1].item_name = "mate";
	status[1].preis = 150;
	status[0].times_sold = 0;
	for (c = 2; c < NUM_ITEMS; ++c)
		status[c].item_name = NULL;
	while (1) {
		/* Bildschirm anzeigen */
		print_screen();
		/* Tastatureingaben abfragen */
		c = getchar();
		/* und eventuell weitere Dialoge anzeigen */
		if (c > 47 && c < 58)
			buy(c - 48);
		else if (c == 115)
			save_data();
		else if (c == 113)
			break;
	}
	printf("BYEBYE\n");
}
