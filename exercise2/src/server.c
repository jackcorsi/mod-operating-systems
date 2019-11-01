#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

#include "protocol.h"
#include "logfile.h"

const int BUFFER_PACKETS = 10;

void *main_client(void *sockfd_vptr);

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

    long portno_l = strtol(argv[1], NULL, 10);
    if (portno_l < 0 || portno_l > 65535) {
        printf("Bad input: Valid port numbers exist in the range 0 to 65535");
        return 0;
    }
    uint16_t portno = portno_l;

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
            return 1;
        }
        *newsockfd = accept(sockfd, NULL, NULL); //Accept incoming as a new socket
        if (*newsockfd < 0) {
            perror("Failed to accept connection from the socket");
            return 1;
        }

        //Set the thread attributes as "detached" - automatically release its resources when it terminates
        pthread_attr_t thread_attr;
        if (pthread_attr_init(&thread_attr)) {
            perror("Failed to initialise pthread attributes");
            return 1;
        }
        if (pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED)) {
            perror("Failed to make pthread detached");
            return 1;
        }

        pthread_t client_thread;
        errno = pthread_create(&client_thread, &thread_attr, main_client, newsockfd);
        if (errno) {
            perror("Failed to create a new thread for the client");
            return 1;
        }
    }
}

void *main_client(void *sockfd_vptr) {
    int *sockfd = (int *) sockfd_vptr;
    FILE *sockfile = fdopen(*sockfd, "r");
    if (!sockfile) {
        perror("Failed to open the network socket as a file");
        close(*sockfd);
        return NULL;
    }

    char *buf = NULL;
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
            printf("Client finished logging\n");
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
            perror("Error: file write operation failed");

    }

    logfile_flush();
    free(buf);
    fclose(sockfile);
    close(*sockfd);
    free(sockfd);
    return NULL;
}
