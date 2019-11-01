all: client serverSingle serverThreaded
.PHONY: all

client: client.o
	gcc -g -D_POSIX_SOURCE -Wall -Werror -pedantic -std=c99 -D_GNU_SOURCE -pthread -o client client.o -lreadline

client.o: src/client.c src/protocol.h
	gcc -g -D_POSIX_SOURCE -Wall -Werror -pedantic -std=c99 -D_GNU_SOURCE -pthread -c src/client.c -lreadline

serverThreaded: serverThreaded.o logfile.o
	gcc -g -D_POSIX_SOURCE -Wall -Werror -pedantic -std=c99 -D_GNU_SOURCE -pthread \
	-o serverThreaded serverThreaded.o logfile.o -lreadline

serverSingle: serverThreaded.o logfile.o
	gcc -g -D_POSIX_SOURCE -Wall -Werror -pedantic -std=c99 -D_GNU_SOURCE -pthread \
	-o serverSingle serverThreaded.o logfile.o -lreadline

serverThreaded.o: src/server_threaded.c src/protocol.h src/logfile.h
	gcc -g -D_POSIX_SOURCE -Wall -Werror -pedantic -std=c99 -D_GNU_SOURCE -pthread \
	-o serverThreaded.o -c src/server_threaded.c -lreadline

logfile.o: src/logfile.c src/logfile.h
	gcc -g -D_POSIX_SOURCE -Wall -Werror -pedantic -std=c99 -D_GNU_SOURCE -pthread \
    -o logfile.o -c src/logfile.c -lreadline