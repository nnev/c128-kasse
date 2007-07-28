#ifndef CONFIG_H_
#define CONFIG_H_

/* Eingenommes Geld in Cent */
extern unsigned long int money;
extern unsigned long int num_items;
extern unsigned long int items_sold;


/* Datenstruktur der verkauften Einträge */
struct status_t {
	char key;
	char *item_name;
	/* Wieviel kostet der Eintrag (in Cent)? */
	unsigned int price;
	/* Wie oft wurde er verkauft */
	unsigned int times_sold;
};

#define MAX_ITEMS 15
extern struct status_t* status;

// unklar bis jetzt was das tun wird
void load_config();

void load_items();
void load_state();
void save_state();
#endif /*CONFIG_H_*/
