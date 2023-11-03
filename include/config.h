#include "general.h"

#ifndef CONFIG_H
#define CONFIG_H

#define MAX_ITEMS 30
#define MAX_ITEM_NAME_LENGTH 19

/* Eingenommes Geld in Cent */
extern int32_t money;
extern int32_t items_sold;
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
#define MAX_CREDIT_ITEMS 75
#define MAX_CREDIT_NAME_LENGTH 10
#define CREDITS_PER_PAGE 30
struct credits_t {
  char nickname[MAX_CREDIT_NAME_LENGTH + 1];
  /* Guthaben (in Cent) */
  signed int credit;
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
