#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

#include "general.h"
#include "config.h"
#include "kasse.h"
// conf
// drucker 4 oder 5
// pricee+getraenke
//
// graphic 4,0,10



/* Hauptbildschirm ausgeben */
void print_screen() {
	uc i = 0;
	clrscr();
	printf("C128-Kassenprogramm\n\n");
	printf("Eingenommen: %d Euro, Verkauft: %d Flaschen\n\n", money * 100, items_sold);
	for (; i < num_items; ++i)
		printf("Item %x: %s (%d Cents, %d mal verkauft)\n", i, status[i].item_name, status[i].price, status[i].times_sold);
	printf("\nBefehle: s) Save Data\tp) Toggle Printing\n");
}

/* Druckt eine entsprechende Zeile aus */
void print_log(uc n, int einheiten, char *nickname) {
	/* Format: 
	   Transaction-ID (Anzahl verkaufter Einträge, inklusive des zu druckenden!)
	   Uhrzeit (TODO)
	   Eintragname (= Getränk)
	   Preis (in Cents)
	   Anzahl
	   Nickname (falls es vom Guthaben abgezogen wird)
	   */
	printf("[%d] UHRZEIT - %s - %d - %d - an %s\n", items_sold, status[n].item_name, status[n].price, einheiten, (nickname != NULL ? nickname : "Unbekannt"));
}

/* Dialog, der einen durch's Abrechnen der Einträge führt */
void buy(uc n) {
	int negative = 1;
	char entered[5] = {'1', 0, 0, 0, 0};
	uc i = 0;
	uc c;
	int einheiten;
	if (status[n].item_name == NULL)
		printf("ERROR: No such item\n");
	else {
		printf("Wieviel Einheiten \"%s\"?\n", status[n].item_name);
		while (1) {
			c = getchar();
			if (c == 13)
				break;
			else if (c == 45 && i == 0)
				negative = -1;
			else if (c > 47 && c < 58)
				entered[i++] = c;
		}
		einheiten = atoi(entered) * negative;
		status[n].times_sold += einheiten;
		money += status[n].price * einheiten;
		items_sold += einheiten;
		// TODO: NULL in nickname des guthabenden ändern
		print_log(n, einheiten, NULL);
	}
}

int main() {
	static uc c;
	while (1) {
		/* Bildschirm anzeigen */
		print_screen();
		/* Tastatureingaben abfragen */
		c = getchar();
		/* und eventuell weitere Dialoge anzeigen */
		if (c > 47 && c < 58)
			buy(c - 48);
		else if (c == 's') {
			save_state();
			printf("Statefile saved, press ANYKEY to continue...\n");
			getchar();
		} else if (c == 'p') {
			printing = (printing == 1 ? 0 : 1);
			printf("Printing is now %s, press ANYKEY to continue...\n", (printing == 1 ? "on" : "off"));
			getchar();
		} else if (c == 'q')
			break;
	}
	printf("BYEBYE\n");
}
