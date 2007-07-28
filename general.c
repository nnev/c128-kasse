#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "general.h"

char *get_input() {
	uc i = 0;
	char *output = malloc(32 * sizeof(char));
	uc c;
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
