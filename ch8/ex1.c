#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>  // Include this line for read, write, and close
#include <stdio.h>
#include <stdlib.h>
#define STDIN 0
#define STDOUT 1
#define STDERR 2
/* filecopy: copy file ifd to ofd */
void filecopy(int ifd, int ofd) {
    int n;
    char buf[BUFSIZ];
    /* read and write using the system calls. */

    while ((n = read(ifd, buf, BUFSIZ)) > 0)
        if (write(ofd, buf, n) != n) {
            printf("cat: write error");
        }
}

/* cat: concatenate files - read/write/open/close */

int main(int argc, char *argv[]) {
    int fd;//file descripter
    if (argc == 1) {
        /* get from stdin and write to stdout. */
        filecopy(STDIN, STDOUT);
    } else
        /* cat all the files one by one. */
        while (--argc > 0)
            if ((fd = open(*++argv, O_RDONLY)) == -1) {
               printf("cat:can't open ");
            } else {
                filecopy(fd, STDOUT);
                close(fd);
            }
    return 0;
}

