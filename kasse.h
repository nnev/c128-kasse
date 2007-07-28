/* Abkürzung */
#define uc	unsigned char

/* Anzahl Einträge */
#define NUM_ITEMS	15

/* Eingenommes Geld in Cent */
static unsigned long int money = 0;
static unsigned long int items_sold = 0;

/* Datenstruktur der verkauften Einträge */
struct status_t {
	char *item_name;
	/* Wieviel kostet der Eintrag (in Cent)? */
	unsigned int preis;
	/* Wie oft wurde er verkauft */
	unsigned int times_sold;
};

static struct status_t status[NUM_ITEMS];
