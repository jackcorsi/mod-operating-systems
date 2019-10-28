#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>

#include "protocol.h"

const int BUFFER_PACKETS = 10;

void *main_client(void *sockfd_vptr);

int main(int argc, char **argv) {
    int sockfd; //Handle for the incoming connection socket

    if (argc != 2) {
        printf("Usage: %s portno", argv[0]);
        return 0;
    }

    long portno_l = strtol(argv[1], NULL, 10);
    if (portno_l < 0 || portno_l > 65535) {
        printf("Bad input: Valid port numbers exist in the range 0 to 65535");
        return 0;
    }
    short portno = (short) portno_l;

    sockfd = socket(AF_INET6, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror(NULL);
        return -1;
    }

    struct sockaddr_in6 server_addr = {0};
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_addr = in6addr_any;
    server_addr.sin6_port = htons(portno);

    if (!bind(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr))) {
        perror(NULL);
        return -1;
    }

    listen(sockfd, BUFFER_PACKETS); //Start listening for connections
    while(1) {
        //This is passed into the client thread as a pointer so needs to be allocated dynamically
        int *newsockfd = (int *) malloc(sizeof(int));
        if (!newsockfd) {
            fprintf(stderr, "Out of memory\n");
            return -1;
        }
        *newsockfd = accept(sockfd, NULL, NULL); //Accept incoming as a new socket
        if (*newsockfd < 0) {
            perror(NULL);
            return -1;
        }

        //Set the thread attributes as "detached" - automatically release its resources when it terminates
        pthread_attr_t thread_attr;
        if (pthread_attr_init(&thread_attr)) {
            perror(NULL);
            return -1;
        }
        if (pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED)) {
            perror(NULL);
            return -1;
        }

        pthread_t client_thread;
        errno = pthread_create(&client_thread, &thread_attr, main_client, newsockfd);
        if (errno) {
            perror(NULL);
            return -1;
        }
    }
}

void *main_client(void *sockfd_vptr) {
    int *sockfd = (int *) sockfd_vptr;
    char *buf = NULL; //The read-in buffer
    protocol_strlen_t buf_size = 0;

    while (1) {

    }
}
