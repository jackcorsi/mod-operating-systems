all: client serverSingle serverThreaded
.PHONY: all

client: client.o
	gcc -g -D_POSIX_SOURCE -Wall -Werror -pedantic -std=c99 -D_GNU_SOURCE -pthread -o client client.o -lreadline

client.o: src/client.c src/protocol.h
	gcc -g -D_POSIX_SOURCE -Wall -Werror -pedantic -std=c99 -D_GNU_SOURCE -pthread -c src/client.c -lreadline

serverThreaded: server.o logfile.o
	gcc -g -D_POSIX_SOURCE -Wall -Werror -pedantic -std=c99 -D_GNU_SOURCE -pthread \
	-o serverThreaded server.o logfile.o -lreadline

serverSingle: server.o logfile.o
	gcc -g -D_POSIX_SOURCE -Wall -Werror -pedantic -std=c99 -D_GNU_SOURCE -pthread \
	-o serverSingle server.o logfile.o -lreadline

server.o: src/server.c src/protocol.h src/logfile.h
	gcc -g -D_POSIX_SOURCE -Wall -Werror -pedantic -std=c99 -D_GNU_SOURCE -pthread \
	-o server.o -c src/server.c -lreadline

logfile.o: src/logfile.c src/logfile.h
	gcc -g -D_POSIX_SOURCE -Wall -Werror -pedantic -std=c99 -D_GNU_SOURCE -pthread \
    -o logfile.o -c src/logfile.c -lreadline