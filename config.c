#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "general.h"
#include "config.h"

unsigned long int money = 0;
unsigned long int num_items = 0;
uc num_credit_items = 0;
unsigned long int items_sold = 0;
struct status_t status[MAX_ITEMS+1];
struct credits_t credits[MAX_CREDIT_ITEMS+1];

#ifdef REAL_DATA
void load_config();

void load_items(){
        FILE* f;
        char line[80];
        char * sep;
        f = fopen("items", "r");
        for (num_items=0; num_items < MAX_ITEMS && !feof(f); num_items++) {
                fgets(line, 79, f);
                sep = strchr(line, '=');
                strncpy(status[num_items].item_name, line, sep-line);
                status[num_items].price = atoi(sep+1);
                status[num_items].times_sold = 0; 
        }
        
}

void load_state(){
	status[0].times_sold=23;	
	status[1].times_sold=42;	
}
void save_state(){}

void load_config() {
}

#else

void load_items() {
	uc c;
	num_items=2;
	strcpy(status[0].item_name, "cola");
	status[0].price = 230;
	status[0].times_sold = 0;
	strcpy(status[1].item_name, "mate");
	status[1].price = 150;
	status[1].times_sold = 0;

}

void load_state() {
	status[0].times_sold=23;	
	status[1].times_sold=42;	
}

void load_credits() {
}

void save_state() {
}

void save_credits() {}
#endif