#include "general.h"

#ifndef CONFIG_H
#define CONFIG_H

#define MAX_ITEMS 32
#define MAX_ITEM_NAME_LENGTH 9
#define MAX_CREDIT_ITEMS 75
#define CREDITS_PER_PAGE 10

/* Eingenommes Geld in Cent */
extern unsigned long int money;
extern unsigned long int items_sold;
extern BYTE printer_port;

/* Datenstruktur der verkauften Einträge */
struct status_t {
  char item_name[MAX_ITEM_NAME_LENGTH + 1];
  /* Wieviel kostet der Eintrag (in Cent)? */
  unsigned int price;
  /* Wie oft wurde er verkauft */
  unsigned int times_sold;
};

struct status_array_t {
  BYTE num_items;
  unsigned int transaction_id;
  struct status_t status[MAX_ITEMS];
};

/* Datenstruktur für die Guthaben */
#define NICKNAME_MAX_LEN 10
struct credits_t {
  char nickname[NICKNAME_MAX_LEN + 1];
  /* Guthaben (in Cent) */
  unsigned int credit;
};

struct credits_array_t {
  BYTE num_items;
  struct credits_t credits[MAX_CREDIT_ITEMS];
};

#ifndef IS_CONFIG_C
extern struct status_array_t status;
extern struct credits_array_t credits;
#endif

/* Lädt Dinge wie die Druckeradresse */
void load_config(void);
void load_items(void);
void load_credits(void);
// void dump_state(void);

void save_items(void);
void save_credits(void);
#endif /* CONFIG_H */
