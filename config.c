#define _IS_CONFIG_C

#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "general.h"
#include "config.h"

unsigned long int money = 0;
unsigned long int items_sold = 0;
BYTE printer_port = 4;
static bool files_existing = false;
struct status_array_t status;
struct credits_array_t credits;

/* 
 * Checks if items/credits-files are existing to avoid having to recover
 * the error state of the drive (we'd have to if we would just access the
 * files directly)
 *
 */
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
	return (files_existing >= 2);
}

void load_items() {
	if (files_existing)
		cbm_load("items", (BYTE)8, &status);
}

void load_credits() {
	if (files_existing)
		cbm_load("credits", (BYTE)8, &credits);
}

void save_items() {
	cbm_save("items", (BYTE)8, &status, sizeof(struct status_array_t));
	files_existing = true;
}

void save_credits() {
	cbm_save("credits", (BYTE)8, &credits, sizeof(struct credits_array_t));
	files_existing = true;
}

void load_config() {
	files_existing = lookup_needed_files();
}
