#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("Usage: %s filename ioctl_num ioctl_param\n", argv[0]);
        return 0;
    }

    unsigned int num = strtoul(argv[2], NULL, 10);
    unsigned int param = strtoul(argv[3], NULL, 10);

    printf("Trigger ioctl %u with param %u on file %s (y/n)?\n", num, param, argv[1]);
    char response[2];
    if (!fgets(response, 2, stdin))
        return 0;

    if (strcmp(response, "y"))
        return 0;

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Couldn't open that file");
        return 1;
    }

    int result = ioctl(fd, num, param);
    if (result >= 0)
        printf("Result: %d\n", result);
    else {
        perror("Negative return code");
    }

    close(fd);
    return 0;
}