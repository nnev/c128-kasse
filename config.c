#include <stdlib.h>
#include "general.h"
#include "config.h"

unsigned long int money = 0;
unsigned long int num_items = 0;
uc num_credit_items = 0;
unsigned long int items_sold = 0;
struct status_t status[MAX_ITEMS+1];
struct credits_t credits[MAX_CREDIT_ITEMS+1];

void load_config() {
}

void load_items() {
	uc c;
	num_items=2;
	status[0].item_name = "cola";
	status[0].price = 230;
	status[0].times_sold = 0;
	status[1].item_name = "mate";
	status[1].price = 150;
	status[1].times_sold = 0;
	for (c = 2; c < MAX_ITEMS; ++c)
		status[c].item_name = NULL;
	
}

void load_state() {
	status[0].times_sold=23;	
	status[1].times_sold=42;	
}

void load_credits() {
}

void save_state() {
}

void save_credits() {
}
