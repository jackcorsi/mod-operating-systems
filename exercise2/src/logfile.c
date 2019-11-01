#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <sys/file.h>

#include "logfile.h"

volatile unsigned long long line_no = 0;
FILE *file = NULL;
pthread_mutex_t mutex;

int logfile_init(char *name) {
    errno = pthread_mutex_init(&mutex, NULL);
    if (errno)
        return -1;

#ifndef LOGFILE_OUT_TO_STDOUT
    file = fopen(name, "a");
    if (!file) {
        printf("Line 20\n");
        return -1;
    }
#else
    file = stdout;
#endif

    //Try to establish an exclusive lock on the file, so hopefully no other processes will race us
    errno = flock(fileno(file), LOCK_EX | LOCK_NB);
    if (errno)
        return -1;
    else
        return 0;
}

int logfile_write(char *s) {
    //Increment line_no and write as an atomic operation
    pthread_mutex_lock(&mutex);
    int result = fprintf(file, "%llu %s\n", line_no++, s);
    pthread_mutex_unlock(&mutex);

#ifdef LOGFILE_FLUSH_EVERY_LINE
    fflush(file);
#endif

    return result;
}

void logfile_flush() {
    fflush(file);
}
