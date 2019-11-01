#ifndef LOGFILE_H
#define LOGFILE_H

//#define LOGFILE_OUT_TO_STDOUT //For debugging output
//#define LOGFILE_FLUSH_EVERY_LINE

//Open the file ready for writing - return non-zero on success, errno set
int logfile_init(char *name);

//Write a null-terminated string to the file, prepended with a line number and appended with \n
//Be sure to call logfile_init() first
//Return a negative value on failure
int logfile_write(char *s);

//Force a flush of all written lines
void logfile_flush();

#endif //LOGFILE_H
