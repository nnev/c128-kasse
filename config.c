#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <stdio.h>
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
    char line[80];
    char *sep;
    BYTE lfn = 8;
	BYTE rc;
	int count=1;

    rc=cbm_open(lfn, (BYTE)8, (BYTE)0, "items,r");
	if(rc!=0)
	{
		cprintf("cannot open items\r\n");
		return;
	}
    for (num_items=0; num_items < MAX_ITEMS && count>0; num_items++) {
		count=cbm_read(lfn, line, 79);
        //fgets(line, 79, f);
        sep = strchr(line, '=');
        strncpy(status[num_items].item_name, line, sep-line);
        status[num_items].price = atoi(sep+1);
        status[num_items].times_sold = 0; 
    }
    cbm_close(lfn);
}

/**
 * must be called after load_items()
 */
void load_state(){
    char line[80];
    char * sep;
    char i;
	BYTE lfn=8;
	BYTE rc;
	int count=1;

	rc=cbm_open(lfn, (BYTE)8, (BYTE)0, "state,r");
    if (rc!=0){
    	cprintf("cannot open state\r\n");
    	return;
    }
    while (count>0) {
    	count=cbm_read(lfn,line,79);
		//fgets(line, 79, f);
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
    cbm_close(lfn);
}

void save_state(){
	int i;
	BYTE lfn=8;
	BYTE rc;
	int count=1;
	int size=1;
	char line[81];
	
	rc=cbm_open(lfn, (BYTE)8, (BYTE)0, "state,w");
    if (rc!=0){
    	c128_perror(23, "cannot open state file");
    	return;
    }
    for (i=0;i<num_items;i++)
	{
		memset(line,0, 81);
		size=sprintf(line, "%s=%d\n", status[i].item_name, status[i].times_sold);
		cbm_write(lfn, line, size);
    	//fprintf(f, "%s=%d\n",status[i].item_name, status[i].times_sold);
	}
	cbm_close(lfn);
}
/*
void dump_state(){
	FILE * f;
	char buf[128];
	int i, len;
	memset(buf, 0, 128);
	f = fopen("state", "r");
	len = fread(buf, 1, 128, f);
	cprintf("read %d bytes from state\n", len);
	fclose(f);
	for (i=0;i<len;i++)
		printf("%x ", buf[i]);
	cprintf("\n");
	
}
*/
void load_config() {
}
