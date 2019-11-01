#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <readline/readline.h>

#include "protocol.h"

//Comment the call to disable
#define PRINT_SOCKET_CREATION_ERROR() //perror(NULL);
#define PRINT_SOCKET_CONNECT_ERROR() //perror(NULL);

int sockfd = 0; //Handle for the socket

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s hostname portno\n", argv[0]);
        return 0;
    }

    //Boilerplate taken from the examples
    struct addrinfo parse_hints = {0};
    parse_hints.ai_family = AF_UNSPEC;
    parse_hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *parse_result;
    int parse_error = getaddrinfo(argv[1], argv[2], &parse_hints, &parse_result);
    if (parse_error) {
        printf("Couldn't interpret hostname/port number: %s\n", gai_strerror(parse_error));
        return 0;
    }

    struct addrinfo *addr_current = parse_result;
    for (; addr_current; addr_current = addr_current->ai_next) { //Traverse the list of possible addrs
        sockfd = socket(addr_current->ai_family, addr_current->ai_socktype, addr_current->ai_protocol);
        if (sockfd < 0) {
            PRINT_SOCKET_CREATION_ERROR()
            continue;
        }

        if (connect(sockfd, addr_current->ai_addr, addr_current->ai_addrlen)) {
            PRINT_SOCKET_CONNECT_ERROR()
            close(sockfd);
            continue;
        }

        break;
    }
    freeaddrinfo(parse_result);

    if (!addr_current) {
        fprintf(stderr, "Failed to connect to the specified host\n");
        return -1;
    }

    char *line;
    while (1) {
        line = readline(NULL);

        if (!line) {
            if (write(sockfd, &PROTOCOL_FINISHED, sizeof(PROTOCOL_FINISHED)) == -1)
                perror("Failed while sending a terminating message to the server");

            break;
        }

        if (write(sockfd, &PROTOCOL_MORE_STRINGS, sizeof(PROTOCOL_MORE_STRINGS)) == -1) {
            perror("Failed to send continuation byte to server");
            free(line);
            break;
        }

        size_t len = strlen(line);

        if (write(sockfd, line, len + 1) == -1) { //Send the string to the server
            perror("Failed to send the line to the server");
            free(line);
            break;
        }
        free(line);
    }

    free(line);
    close(sockfd);
}
