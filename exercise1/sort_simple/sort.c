#include "stdlib.h"
#include "stdio.h"
#include "string.h"

char strings[20][21]; //21 to include the \0
char out_buf[21];

int strcmp_void(const void *str1, const void *str2) {
	return strcmp((char *) str1, (char *) str2);
}

int main(int argc, char ** argv) {
	int i;
	for (i = 0; i < 20; i++) {
		char *read_to = (char *) (strings + i); //addr of next string
		if (!fgets(read_to, 21, stdin)) {
			i--;
			break;
		}

		for (int j = 0; j < 20; j++) {
			if (strings[i][j] == '\n') { //Remove the \n
				strings[i][j] = '\0';
				break;
			}
		}
	}

	qsort(strings, i + 1, 21, strcmp_void);

	for (int i2 = 0; i2 < i + 1; i2++) {
		for (int j2 = 0; j2 < 20; j2++) {
			char c = strings[i2][j2];
			if (!c) {
				out_buf[j2] = '\n'; //Add back the \n
				out_buf[j2 + 1] = '\0';
				break;
			}

			out_buf[j2] = c;
		}
		printf("%s", out_buf);
	}

	return 0;
}