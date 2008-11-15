/* 
 * RGB2R-C128-Kassenprogramm
 * (c) 2007-2008 phil_fry, sECuRE, sur5r
 * See LICENSE for license information
 *
 */
#include <peekpoke.h>
#include <stdlib.h>
#include <stdio.h>
#include "general.h"
#include <stdint.h>

char *get_time() {
	uint32_t h = PEEK(0x00A0) * 65536,
		m = PEEK(0x00A1) * 256,
		s = PEEK(0x00A2);
	static char buffer[9];
	BYTE hrs, min;

	h = (h + m + s) / 60;
	hrs = (h / 3600);
	h -= ((uint32_t)hrs * (uint32_t)3600);
	min = (h / 60);
	h -= (min * 60);
	
	sprintf(buffer, "%02d:%02d:%02d", hrs, min, (BYTE)h);
	return buffer;
}

void set_time(BYTE hrs, min, sec) {
	uint32_t added = ((uint32_t)sec + ((uint32_t)min * (uint32_t)60) + ((uint32_t)hrs * (uint32_t)3600)) * (uint32_t)60;
	uint32_t lowbit = (added & 0xFF);
	uint32_t middlebit = (added >> 8) & 0xFF;
	uint32_t highbit = (added >> 16) & 0xFF;

	POKE(0x00A0, (BYTE)highbit);
	POKE(0x00A1, (BYTE)middlebit);
	POKE(0x00A2, (BYTE)lowbit);
}
