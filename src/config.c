/* 
 * RGB2R-C128-Kassenprogramm
 * (c) 2007 phil_fry, sECuRE, sur5r
 * See LICENSE for license information
 *
 */
#define _IS_CONFIG_C

#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "kasse.h"
#include "general.h"
#include "config.h"

/* undocumented function which scratches files */
unsigned char __fastcall__ _sysremove(const char *name);

unsigned long int money = 0;
unsigned long int items_sold = 0;
BYTE printer_port = 4;
static bool items_exists = false;
static bool credits_exists = false;
struct status_array_t status;
struct credits_array_t credits;

/* 
 * Checks if items/credits-files are existing to avoid having to recover
 * the error state of the drive (we'd have to if we would just access the
 * files directly)
 *
 */
static void lookup_needed_files() {
	BYTE lfn = 8;
	struct cbm_dirent *dirent;

	if (cbm_opendir(lfn, (BYTE)8) != 0) {
		cprintf("could not open directory\r\n");
		return;
	}
	while (cbm_readdir(lfn, dirent) == 0) {
		if (strncmp(dirent->name, "log", 3) == 0)
			log_num++;
		if (strcasecmp(dirent->name, "items") == 0)
			items_exists = true;
		if (strcasecmp(dirent->name, "credits") == 0)
			credits_exists = true;
	}
	cbm_closedir(lfn);
}

void load_items() {
	if (items_exists)
		cbm_load("items", (BYTE)8, &status);
	else
		memset(&status, 0, sizeof(struct status_array_t));
}

void load_credits() {
	if (credits_exists)
		cbm_load("credits", (BYTE)8, &credits);
	else
		memset(&credits, 0, sizeof(struct credits_array_t));
}

void save_items() {
	if (items_exists)
		_sysremove("items");
	cbm_save("items", (BYTE)8, &status, sizeof(struct status_array_t));
	items_exists = true;
}

void save_credits() {
	if (credits_exists)
		_sysremove("credits");
	cbm_save("credits", (BYTE)8, &credits, sizeof(struct credits_array_t));
	credits_exists = true;
}

void load_config() {
	lookup_needed_files();
}
