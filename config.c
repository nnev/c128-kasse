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
    FILE * f;
    char line[80];
    char * sep;
    char i, j;
    f = fopen("state", "r");
    if (f==NULL){
    	cprintf("cannot open state\r\n");
    	return;
    }
    while (!feof(f)) {
    	fgets(line, 79, f);
    	sep = strchr(line, '=');
    	if (sep==NULL)
    		continue;
    	*(line + (sep-line)) = 0;
        for (i=0; i< MAX_ITEMS; i++) {
        	if (strcmp(line, status[i].item_name)==0) {
        		status[i].times_sold = atoi(sep+1);
        		break;
        	}
        }
    }
    fclose(f);
}

void save_state(){
	FILE * f;
	int i;
	
	f = fopen("state", "w");
    if (f==NULL){
    	c128_perror(23, "cannot open state file");
    	return;
    }
    for (i=0;i<num_items;i++)
    	fprintf(f, "%s=%d\n",status[i].item_name, status[i].times_sold);
    fclose(f);
}
/*
void dump_state(){
	FILE * f;
	char buf[128];
	int i, len;
	memset(buf, 0, 128);
	f = fopen("state", "r");
	len = fread(buf, 1, 128, f);
	printf("read %d bytes from state\n", len);
	fclose(f);
	for (i=0;i<len;i++)
		printf("%x ", buf[i]);
	printf("\n");
	
}
*/
void load_config() {
}
