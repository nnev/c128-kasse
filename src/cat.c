#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

#include "general.h"

#define x2(x) (buffer[x] < 10 ? "0" : ""), buffer[x]

int main() {
	char *filename = NULL;
	FILE *file;
	unsigned int c;
	unsigned char i;
	char buffer[8];
	char readable[9];

	memset(readable, '\0', 9);

	while (1) {
		clrscr();
		while (filename == NULL || *filename == '\0') {
			printf("Please enter filename (q to exit):\r\n");
			filename = get_input();
		}
		if (*filename == 'q')
			return 0;
		c = 0;
		if ((file = fopen(filename, "r")) == NULL) {
			printf("Could not open file\r\n");
			continue;
		}
		clrscr();
		while (!feof(file)) {
			if (fgets(buffer, 8, file) != buffer) {
				printf("Could not read from file, bailing out\r\n");
				return 1;
			}
			for (i = 0; i < 8; i++)
				if (	(buffer[i] >= 0x41 && buffer[i] <= 0x5A) ||
					(buffer[i] >= 0xC1 && buffer[i] <= 0xDA) ||
					(buffer[i] >= 0x30 && buffer[i] <= 0x39))
					readable[i] = buffer[i];
				else readable[i] = 0x2E;
			printf("%s%x |%s%x %s%x %s%x %s%x %s%x %s%x %s%x %s%x| %s \n",
				(c <= 0xF ? "0" : ""), c,
				x2(0), x2(1), x2(2), x2(3),
				x2(4), x2(5), x2(6), x2(7),
				readable);
			c++;
			if ((c % 20) == 0) {
				get_input();
				clrscr();
			}
		}
		fclose(file);
		filename = NULL;
		printf("File finished, press RETURN...\n");
		get_input();
	}
}
