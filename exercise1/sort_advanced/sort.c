#include "stdlib.h"
#include "stdio.h"
#include "string.h"

char out_buf[21];

int strcmp_void(const void *str1, const void *str2) {
	return strcmp((char *) str1, (char *) str2);
}


int main(int argc, char ** argv) {
	size_t strings_length = 10;
	char *strings = (char *) malloc(strings_length * 21);
	const realloc_multiplier = 2;

	if (!strings)
		return 1;

	int i = 0;
	while (true) {
		if (i >= strings_length) { //Realloc input buffer
			size_t length_new = strings_length * realloc_multiplier;
			char *strings_new = (char *) malloc(length_new * 21);

			if (!strings_new)
				return 1;

			memcpy(strings_new, strings, strings_length * 21);
			free(strings);
			strings = strings_new;
			strings_length = length_new;
		}

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

		i++;
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

	free(strings);
	return 0;
}