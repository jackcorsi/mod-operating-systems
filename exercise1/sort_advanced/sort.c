#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#define RESIZE_MULTIPLIER 2

int strings_size = 10;
char **strings = NULL; //Array of string pointers, resized as necessary
int strings_last = -1; //Last populated index

int bigbuffer_size = 10;
char *bigbuffer = NULL;

void cleanup() {
	for (int i = 0; i <= strings_last; i++)
		free(strings[i]);

	free(strings);
	free(bigbuffer);
}

int string_comparator(const void *s1, const void *s2) { //These will be pointers to the strings array (i.e. pointers to pointers)
	char **str1 = (char **) s1;
	char **str2 = (char **) s2;
	return strcmp(*str1, *str2);
}

int main(int argc, char ** argv) {
	strings = (char **) malloc(sizeof(char *) * strings_size);
	bigbuffer = (char *) malloc(bigbuffer_size);

	int i_next = 0; //Character position in line
	while (1) {
		int next = getchar();

		if (next != '\n' && next != EOF) {
			if (i_next >= bigbuffer_size) { //Resize bigbuffer
				bigbuffer_size *= RESIZE_MULTIPLIER;
				char *bigbuffer_new = (char *) realloc(bigbuffer, bigbuffer_size);
				if (!bigbuffer_new) {
					cleanup();
					return -1;
				}
				bigbuffer = bigbuffer_new;
			}

			char c = next;
			bigbuffer[i_next++] = c; //Write string to bigbuffer first (so most lines will only need one malloc)
		} else {
			//--Transfer line out of bigbuffer--
			
			if (strings_last + 1 >= strings_size) { //Make space for another string pointer
				strings_size *= RESIZE_MULTIPLIER;
				char **strings_new = (char **) realloc(strings, strings_size * sizeof(char *));
				if (!strings_new) {
					cleanup();
					return -1;
				}
				strings = strings_new;
			}

			char *string = (char *) malloc(sizeof(char[i_next + 1])); //We need +1 to append a \0
			if (!string) {
				cleanup();
				return -1;
			}

			memcpy(string, bigbuffer, i_next);
			string[i_next] = '\0';

			strings[++strings_last] = string;

			if (next == EOF) {
				if (i_next == 0) //Empty last line detection
					free(strings[strings_last--]); //Ctrl+Z

				break;
			}

			i_next = 0;
		}
	}

	qsort(strings, strings_last + 1, sizeof(char *), string_comparator);

	for (int i = 0; i <= strings_last; i++)
		printf("%s\n", strings[i]);

	cleanup();
	return 0;
}