#ifndef CONFIG_H_
#define CONFIG_H_

/* Eingenommes Geld in Cent */
unsigned long int money = 0;
unsigned long int num_items = 0;
unsigned long int items_sold = 0;


/* Datenstruktur der verkauften Einträge */
struct status_t {
	char key;
	char *item_name;
	/* Wieviel kostet der Eintrag (in Cent)? */
	unsigned int price;
	/* Wie oft wurde er verkauft */
	unsigned int times_sold;
};

#define MAX_ITEMS 16
static struct status_t status[MAX_ITEMS];

// unklar bis jetzt was das tun wird
void load_config();

void load_items();
void load_state();
void save_state();
#endif /*CONFIG_H_*/
