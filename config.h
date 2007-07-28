#ifndef CONFIG_H_
#define CONFIG_H_

/* Eingenommes Geld in Cent */
extern unsigned long int money;
extern unsigned long int num_items;
extern unsigned char num_credit_items;
extern unsigned long int items_sold;


/* Datenstruktur der verkauften Einträge */
struct status_t {
	char *item_name;
	/* Wieviel kostet der Eintrag (in Cent)? */
	unsigned int price;
	/* Wie oft wurde er verkauft */
	unsigned int times_sold;
};

#define MAX_ITEMS 15
extern struct status_t* status;

/* Datenstruktur für die Guthaben */
struct credits_t {
	char *nickname;
	/* Guthaben (in Cent) */
	unsigned int credit;
};

#define MAX_CREDIT_ITEMS 75
extern struct credits_t *credits;

/* Lädt Dinge wie die Druckeradresse */
void load_config();
void load_items();
void load_state();
void load_credits();

void save_state();
void save_credits();
#endif /*CONFIG_H_*/
