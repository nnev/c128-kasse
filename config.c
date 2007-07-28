#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "general.h"
#include "config.h"

unsigned long int money = 0;
unsigned long int num_items = 0;
BYTE num_credit_items = 0;
unsigned long int items_sold = 0;
BYTE printer_port = 4;
struct status_t status[MAX_ITEMS+1];
struct credits_t credits[MAX_CREDIT_ITEMS+1];
#define REAL_DATA
#ifdef REAL_DATA
void load_config();


void load_items(){
    FILE* f;
    char line[80];
    char * sep;
    BYTE lfn = 219;
//  cbm_open(lfn, (BYTE)8, (BYTE)0, "items,r");
    f = fopen("items","r");
    for (num_items=0; num_items < MAX_ITEMS && !feof(f); num_items++) {
        fgets(line, 79, f);
        sep = strchr(line, '=');
        strncpy(status[num_items].item_name, line, sep-line);
        status[num_items].price = atoi(sep+1);
        status[num_items].times_sold = 0; 
    }
    fclose(f);
}

/**
 * must be called after load_items()
 */
void load_state(){
    FILE* f;
    char line[80];
    char * sep;
    char i, j;
    f = fopen("state", "r");
    if (f==NULL){
    	printf("cannot open state\n");
    	return;
    }
    while (!feof(f)) {
    	fgets(line, 79, f);
    	sep = strchr(line, '=');
    	*(line + (sep-line)) = 0;
        for (i=0; i< MAX_ITEMS; i++) {
        	if (strcmp(line, status[i].item_name)==0) {
        		status[i].times_sold = atoi(sep+1);
        		break;
        	}
        }
    }
}

void save_state(){}

void load_config() {
}

#else

void load_config() {
}

void load_items() {
	BYTE c;
	num_items=2;
	strcpy(status[0].item_name, "cola");
	status[0].price = 230;
	status[0].times_sold = 0;
	strcpy(status[1].item_name, "mate");
	status[1].price = 150;
	status[1].times_sold = 0;
	for (c = 2; c < MAX_ITEMS; ++c)
		status[c].item_name[0] = 0;
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
