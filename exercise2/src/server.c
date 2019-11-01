#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "protocol.h"
#include "logfile.h"

#define CLIENT_STATUS_MSGS //Log clients in and out

const int BUFFER_PACKETS = 10;

void *main_client(void *sockfd_vptr); //Client thread routine
int is_string_numeric(char *s); //Return 0 if no

#ifdef CLIENT_STATUS_MSGS
volatile unsigned long long next_client_id = 1;
pthread_mutex_t client_id_mutex = PTHREAD_MUTEX_INITIALIZER;

unsigned long long assign_client_id() {
    if (pthread_mutex_lock(&client_id_mutex))
        return 0;

    unsigned long long id = next_client_id++;

    pthread_mutex_unlock(&client_id_mutex);

    return id;
}
#endif

int main(int argc, char **argv) {
    int sockfd; //Handle for the incoming connection socket

    if (argc != 3) {
        printf("Usage: %s portno filename\n", argv[0]);
        return 0;
    }

    int open_err = logfile_init(argv[2]);
    if (open_err) {
        perror("Failed to open the specified file");
        return 1;
    }

    if (!is_string_numeric(argv[1])) {
        printf("Invalid port number\n");
        return 0;
    }

    long portno_l = strtol(argv[1], NULL, 10);
    if (portno_l < 0 || portno_l > 65535) {
        printf("Bad input: Valid port numbers exist in the range 0 to 65535");
        return 0;
    }
    uint16_t portno = portno_l;

    //Boilerplate
    sockfd = socket(AF_INET6, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Failed to create socket");
        return 1;
    }

    struct sockaddr_in6 server_addr = {0};
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_addr = in6addr_any;
    server_addr.sin6_port = htons(portno);

    if (bind(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr))) {
        perror("Failed to bind socket");
        close(sockfd);
        return 1;
    }

    listen(sockfd, BUFFER_PACKETS); //Start listening for connections
    while(1) {
        //This is passed into the client thread as a pointer so needs to be allocated dynamically
        int *newsockfd = (int *) malloc(sizeof(int));
        if (!newsockfd) {
            fprintf(stderr, "Out of memory\n");
            free(newsockfd);
            close(sockfd);
            return 1;
        }
        *newsockfd = accept(sockfd, NULL, NULL); //Accept incoming as a new socket
        if (*newsockfd < 0) {
            perror("Failed to accept connection from the socket");
            free(newsockfd);
            continue;
        }

        //Set the thread attributes as "detached" - automatically release its resources when it terminates
        //"continue" to try to keep serving existing clients, although realistically if these fail our days are numbered
        pthread_attr_t thread_attr;
        if (pthread_attr_init(&thread_attr)) {
            perror("Failed to initialise pthread attributes");
            close(*newsockfd);
            free(newsockfd);
            continue;
        }
        if (pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED)) {
            perror("Failed to make pthread detached");
            close(*newsockfd);
            free(newsockfd);
            continue;
        }

        pthread_t client_thread;
        errno = pthread_create(&client_thread, &thread_attr, main_client, newsockfd);
        if (errno) {
            perror("Failed to create a new thread for the client");
            close(*newsockfd);
            free(newsockfd);
        }
    }

    //There's no specified way to actually stop the server, so I guess we just won't clean up?
    //The client thread should at least be leak free
}

void *main_client(void *sockfd_vptr) {
    int *sockfd = (int *) sockfd_vptr;
    FILE *sockfile = fdopen(*sockfd, "r");
    if (!sockfile) {
        perror("Failed to open the network socket as a file");
        close(*sockfd);
        free(sockfd);
        return NULL;
    }

#ifdef CLIENT_STATUS_MSGS
    unsigned long long my_id;
    printf("Client %llu connected\n", my_id = assign_client_id());
#endif

    char *buf = NULL; //Line reading buffer
    size_t buf_size = 0;

    while (1) {
        int continuation_byte_read = fgetc(sockfile);
        if (continuation_byte_read == EOF) {
            errno = ferror(sockfile);
            perror("Failed to read continuation byte");
            break;
        }

        unsigned char continuation_byte = (unsigned char) continuation_byte_read;
        if (continuation_byte == PROTOCOL_FINISHED) {
#ifdef CLIENT_STATUS_MSGS
            printf("Client %llu has finished\n", my_id);
            fflush(stdout);
#endif
            break;
        } else if (continuation_byte != PROTOCOL_MORE_STRINGS) {
            fprintf(stderr, "Protocol error, received continuation byte %u", continuation_byte);
            break;
        }

        ssize_t len = getdelim(&buf, &buf_size, '\0', sockfile);
        if (len == -1) {
            perror("Failed to read in log line");
            break;
        }

        if (logfile_write(buf) < 0)
            perror("File write operation failed");

    }

    logfile_flush();
    free(buf);
    fclose(sockfile);
    close(*sockfd);
    free(sockfd);
    return NULL;
}

int is_string_numeric(char *s) {
    size_t len = strlen(s);
    for (size_t i = 0; i < len; i++) {
        if (!isdigit(s[i]))
            return 0;
    }
    return 1;
}
