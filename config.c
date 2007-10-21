#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "general.h"
#include "config.h"

unsigned long int money = 0;
unsigned long int num_items = 0;
BYTE num_credit_items = 0;
unsigned long int items_sold = 0;
BYTE printer_port = 4;
struct status_t status[MAX_ITEMS+1];
struct credits_t credits[MAX_CREDIT_ITEMS+1];
static bool files_existing = false;

bool lookup_needed_files() {
	BYTE lfn = 8;
	BYTE files_existing = 0;
	struct cbm_dirent *dirent;

	if (cbm_opendir(lfn, (BYTE)8) != 0) {
		cprintf("could not open directory\r\n");
		return false;
	}
	while (cbm_readdir(lfn, dirent) == 0)
		if (	strcasecmp(dirent->name, "items") == 0 ||
			strcasecmp(dirent->name, "credits") == 0)
			files_existing++;
	cbm_closedir(lfn);
	return (files_existing == 2);
}

void load_items() {
	BYTE rc;
	char *nn;
	cprintf("load_items()\r\n");
	if (files_existing) {
		rc = cbm_load("items", (BYTE)8, status);
		cprintf("rc = %d\r\n", rc);
		nn = get_input();
	}
//		cbm_load("items", (BYTE)8, NULL);
}

void save_items() {
	cbm_save("items", (BYTE)8, status, sizeof(struct status_t) * MAX_ITEMS);
	files_existing = true;
}

void load_credits() {
	if (files_existing)
		cbm_load("credits", (BYTE)8, credits);
}

void save_credits() {
	cbm_save("credits", (BYTE)8, credits, sizeof(struct credits_t) * MAX_CREDIT_ITEMS);
	files_existing = true;
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
	files_existing = lookup_needed_files();
}
