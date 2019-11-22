#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define MAX_LINE_LEN 50

void *thread_func(void *param);

int device_fd = -1;
FILE *out = NULL;

int wait = 1;

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s filename randomseed\n", argv[0]);
        return 0;
    }

    unsigned int seed = strtoul(argv[2], NULL, 10);
    printf("Using seed %u\n", seed);
    srandom(seed);

    device_fd = open(argv[1], O_RDWR);
    if (device_fd == -1) {
        perror("Couldn't open device file");
        return 1;
    }

    FILE *in1 = fopen("custom_test/test1_1.in", "r");
    if (!in1) {
        perror("test1_1");
        return 1;
    }

    FILE *in2 = fopen("custom_test/test1_2.in", "r");
    if (!in2) {
        perror("test1_2");
        return 1;
    }

    out = fopen("custom_test1.out", "w");
    if (!out) {
       perror("test1.out");
       return 1;
    }

    pthread_attr_t thread_attr;
    if (pthread_attr_init(&thread_attr))
        perror("line 7");

    if (pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE))
        perror("Line 10");

    pthread_t t1;
    pthread_t t2;

    errno = pthread_create(&t1, &thread_attr, thread_func, in1);
    if (errno) {
        perror("Failed to create thread 1");
        return 1;
    }

    errno = pthread_create(&t2, &thread_attr, thread_func, in2);
    if (errno) {
        perror("Failed to create thread 2");
        return 1;
    }

    wait = 0;

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
}

void *thread_func(void *param) {
    char buf[MAX_LINE_LEN];
    FILE *in = param;

    while (wait);

    while (1) {
        if (random() < RAND_MAX / 2) {
            //write a line
            if (!fgets(buf, MAX_LINE_LEN - 1, in)) {
                perror("Failed to read from in");
                break;
            }
            size_t len = strlen(buf);

            if (len > 0) {
                if (write(device_fd, buf, len + 1) == -1) {
                    perror("Failed to write to the device");
                    break;
                }
            } else {
                printf("Warning: zero length string\n");
            }

        } else {
            //read a line
            if (read(device_fd, buf, MAX_LINE_LEN) == -1) {
                perror("Failed to read from the device");
                continue;
            }

            fputs(buf, out);
            fflush(out);
        }
    }

    fclose(param);
    return NULL;
}