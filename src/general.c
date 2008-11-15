/* 
 * RGB2R-C128-Kassenprogramm
 * (c) 2007-2008 phil_fry, sECuRE, sur5r
 * See LICENSE for license information
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>

#include "general.h"

/*
 * Liest (maximal 31) Zeichen ein, bis Enter gedrückt wird.
 * Vorsicht: Es wird ein statischer Buffer benutzt, sodass man
 * das Ergebnis via strdup() retten muss, bevor man get_input()
 * erneut aufruft
 *
 */
char *get_input() {
	BYTE i = 0;
	static char output[32];
	BYTE c;
	memset(output, '\0', 32);
	while (1) {
		if (i == 31)
			break;
		c = getchar();
		if (c == 13)
			break;
		else output[i++] = c;
	}
	return output;
}

char *format_euro(char *s, int maxlen, int cent){
	int tmp = cent;
	int len = strlen(",EUR");
	while ((tmp /= 10) > 0)
		++len;
	if (len >= maxlen)
		return NULL;
	// workaround to produce a leading zero for cents.. %0.2d won't work 
	sprintf(s, "%d,%s%dEUR", cent / 100, ((cent%100)<10?"0":""), cent % 100);
	return s;
}

void c128_perror(BYTE c, char *msg) {
	cprintf("\r\nError (Code %d) while: %s\r\nOS Error = %d\r\n", c, msg, _oserror);
}
