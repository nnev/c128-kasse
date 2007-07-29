#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "general.h"

/*
 * Liest (maximal 31) Zeichen ein, bis Enter gedrückt wird. Der zurückgegebene 
 * char-pointer muss anschließend ge-free()d werden!
 *
 */
char *get_input() {
	BYTE i = 0;
	char *output = malloc(32 * sizeof(char));
	BYTE c;
	if (output == NULL) {
		perror("malloc()");
		exit(1);
	}
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

char * format_euro(char * s, int maxlen, int cent){
	int tmp = cent;
	int len = 5; // 1 char at least + 4 (== strlen(",EUR"))
	while ((tmp/=10) > 0)
		len++;
	if (len>maxlen)
		return NULL;
//	sprintf(s, "%d,%dEUR", cent/100, cent%100);
	return s;
}

void c128_perror(BYTE c, char *msg) {
	cprintf("\r\nError (Code %d) while: %s/r\nOS Error = %d\r\n", c, msg, _oserror);
}
