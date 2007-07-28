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

void c128_perror(BYTE c, char *msg) {
	printf("\nError (Code %d) while: %s\nOS Error = %s\n", c, msg, _oserror);
}
