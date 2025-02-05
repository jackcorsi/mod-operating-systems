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
    if (!file)
        return -1;
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
    //Increment line_no and write, as an atomic operation
    if ((errno = pthread_mutex_lock(&mutex)))
        return -1;

    int result = fprintf(file, "%llu %s\n", line_no++, s);

    pthread_mutex_unlock(&mutex);

#ifdef LOGFILE_FLUSH_ON_WRITE
    fflush(file);
#endif

    return result;
}

void logfile_flush() {
    fflush(file);
}
