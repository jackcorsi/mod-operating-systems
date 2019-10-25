client: client.o
	gcc -g -D_POSIX_SOURCE -Wall -Werror -pedantic -std=c99 -D_GNU_SOURCE -pthread -o client client.o -lreadline

client.o: src/client.c
	gcc -g -D_POSIX_SOURCE -Wall -Werror -pedantic -std=c99 -D_GNU_SOURCE -pthread -c src/client.c -lreadline