/*
File Pointer vs. File Descriptor:

File Pointer (FILE*): A file pointer is an abstraction provided by the C standard library to represent a stream (input or output) associated with a file. The FILE structure typically contains information about the file, such as its current position, buffering details, status flags, and a file descriptor. You interact with file streams using functions like fread, fwrite, fseek, etc.

File Descriptor (int): A file descriptor, on the other hand, is a low-level integer identifier used by the operating system to represent an open file or I/O channel. File descriptors are used in lower-level system calls like read, write, and lseek. While file pointers (FILE*) provide a higher-level abstraction, file descriptors are more closely tied to the operating system's file management.

In the case of fseek, it provides a way to move the file position indicator associated with a file stream (FILE*). The lseek function, which operates with file descriptors, performs a similar function at a lower level. The fseek function allows you to seek within a file using a file pointer (FILE*) and is designed to coordinate with the buffering mechanisms used by other standard library functions.*/
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

