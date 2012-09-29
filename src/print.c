/*
 * © 2007-2009 phil_fry, sECuRE, sur5r
 * See LICENSE for license information
 *
 */
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <cbm.h>

#include "general.h"
#define _IS_PRINT
#include "print.h"

/* NOTE: undocumented function which scratches files
   We need to use this function because linking unistd.h
   makes our program break at runtime.
 */
unsigned char __fastcall__ _sysremove(const char *name);

void print_the_buffer() {
	BYTE c;
RETRY:
	c = cbm_open((BYTE)4, (BYTE)4, (BYTE)0, NULL);
	if (c != 0) {
		c128_perror(c, "cbm_open(printer)");
		if (retry_or_quit() == 'q')
			exit(1);

		goto RETRY;
	}
	c = cbm_write((BYTE)4, print_buffer, strlen(print_buffer));
	if (c != strlen(print_buffer)) {
		c128_perror(c, "write(printer)");
		if (retry_or_quit() == 'q')
			exit(1);
		goto RETRY;
	}
	cbm_close((BYTE)4);
	log_file(print_buffer);
}

void print_header() {
	sprintf(print_buffer, "%c--------------------------------------------------------------------------------\r", 17);
	print_the_buffer();

	sprintf(print_buffer, "%c#kauf Uhrzeit  - Ding      - Preis      - Restguthaben - # - Nickname\r", 17);
	print_the_buffer();

	sprintf(print_buffer, "%c--------------------------------------------------------------------------------\r", 17);
	print_the_buffer();

}

void log_file(const char *s) {
	/* A log-entry has usually 50 bytes, so we take 64 bytes.
	   Because files are wrapped (log.0, log.1, ...) every 100
	   lines, we don't need more than 100 * 64 bytes. */
	char *buffer = malloc(sizeof(char) * 64 * 100);
	char filename[8];
	int read = 0;
	unsigned int c;

	if (buffer == NULL) {
		cprintf("No memory available\n");
		exit(1);
	}

	buffer[0] = '\0';
	if (((++log_lines_written) % 100) == 0)
		log_num++;
	sprintf(filename, "log-%d", log_num);
	/* Don't read log if there were no lines written before */
	if (log_lines_written != 1) {
		if ((c = cbm_open((BYTE)8, (BYTE)8, (BYTE)0, filename)) != 0) {
			c128_perror(c, "cbm_open(log)");
			exit(1);
		}
		read = cbm_read((BYTE)8, buffer, sizeof(char) * 64 * 100);
		cbm_close((BYTE)8);
		_sysremove(filename);
	}
	if ((c = cbm_open((BYTE)8, (BYTE)8, (BYTE)1, filename)) != 0) {
		c128_perror(c, "cbm_open(log)");
		exit(1);
	}
	if (read < 0) {
		cprintf("Could not read existing logfile (read returned %d)\n", read);
		exit(1);
	}
	strcpy(buffer+read, s);
	c = cbm_write((BYTE)8, buffer, read+strlen(s));
	if (c != (read+strlen(s))) {
		cprintf("Could not save logfile (wrote %d bytes, wanted %d bytes), please make sure the floppy is not full!\n", c, (read+strlen(s)));
		exit(1);
	}
	cbm_close((BYTE)8);
	free(buffer);
}
