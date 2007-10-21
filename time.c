#include <peekpoke.h>
#include <stdlib.h>
#include <stdio.h>
#include "general.h"

char *get_time() {
	long int h = PEEK(0x00A0) * 65536,
		m = PEEK(0x00A1) * 256,
		s = PEEK(0x00A2);
	static char buffer[9];
	BYTE hrs, min;
	h = (h + m + s) / 60;
	hrs = (h / 3600);
	h -= (hrs * 3600);
	min = (h / 60);
	h -= (min * 60);
	sprintf(buffer, "%02d:%02d:%02d", hrs, min, (int)h);
	return buffer;
}

void set_time(BYTE hrs, min, sec) {
	long int added = ((long int)sec + ((long int)min * (long int)60) + ((long int)hrs * (long int)3600)) * (long int)60;
	POKE(0x00A0, (BYTE)(added / 65536));
	POKE(0x00A1, (BYTE)(added / 256));
	POKE(0x00A2, (BYTE)added);
}
