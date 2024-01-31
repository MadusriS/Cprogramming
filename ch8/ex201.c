#define NULL 0
#define EOF (-1)
#define OPEN_MAX 20

typedef struct _iobuf {
	int cnt;// characters left
	char *ptr;
	char *base;
	int read;
	int write;
	int unbuf;/*/* unbuf-means the file stream should not use any buffering,it means every read or write operation will access the file directly without using any intermediatte buffer*/
	int eof;
	int err;
	int fd;
} FILE;
extern FILE _iob[OPEN_MAX];

#define stdin (&_iob[0])
#define stdout (&_iob[1])
#define stderr (&_iob[2])

FILE *fopen(char *name, char *mode);
int _fillbuf(FILE *);
int _flushbuf(int, FILE *);

#define feof(p) (((p)->flag & _EOF) != 0)
#define ferror(p) (((p)->flag & _ERR) != 0)
#define fileno(p) ((p)->fd)

#define getc(p) (--(p)->cnt >= 0 ? (unsigned char)*(p)->ptr++ : _fillbuf(p))
#define putc(x, p) write(p->fd, &x, 1);

#define getchar() getc(stdin)
#define putchar(x) putc(x, stdout)



#include <fcntl.h>
#include <unistd.h>
#define PERMS 0666
/*In the context of the provided code, PERMS is a constant defined as 0666. The 0666 octal value corresponds to the decimal value 438.

In Unix-like operating systems, especially when dealing with file permissions, octal values are commonly used. The 0666 permission value represents read (4), write (2), and execute (1) permissions for the file owner, group, and others. However, since it's used in the context of file creation, the execute permission is not applicable.
So, breaking down 0666:
Owner permissions: Read (4) + Write (2) = 6
Group permissions: Read (4) + Write (2) = 6
Others permissions: Read (4) + Write (2) = 6
Thus, 0666 grants read and write permissions to the owner, the group, and others.

*/

FILE *fopen(char *name, char *mode)
{
	int fd;
	FILE *fp;

	if (*mode != 'r' && *mode != 'w' && *mode != 'a')
		return NULL;
	for (fp = _iob; fp < _iob + OPEN_MAX; fp++)
		if (!fp->read && !fp->write)
			break;
	if (fp >= _iob + OPEN_MAX)
		return NULL;
	if (*mode == 'w')
		fd = creat(name, PERMS);
	else if (*mode == 'a') {
		if ((fd = open(name, O_WRONLY, 0)) == -1)
			fd = creat(name, PERMS);
		lseek(fd, 0L, 2);
	} else
		fd = open(name, O_RDONLY, 0);
	if (fd == -1)
		return NULL;
	fp->fd = fd;
	fp->cnt = 0;
	fp->base = NULL;
	fp->read = (*mode == 'r');
	fp->write = !fp->read;
	fp->unbuf = fp->eof = fp->err = 0;
	return fp;
}



#include <stdlib.h>
#define BUFSIZE 1024

int _fillbuf(FILE *fp)
{
	int bufsize;

	if (!fp->read || fp->eof || fp->err)
		return EOF;
	bufsize = fp->unbuf ? 1 : BUFSIZE;
	if (fp->base == NULL)
		if ((fp->base = (char *)malloc(bufsize)) == NULL)
			return EOF;
	fp->ptr = fp->base;
	fp->cnt = read(fp->fd, fp->ptr, bufsize);
	if (--fp->cnt < 0) {
		if (fp->cnt == -1)
			fp->eof = 1;
		if (fp->cnt == -2)
			fp->err = 1;
		fp->cnt = 0;
		return EOF;
	}
	return (unsigned char)*fp->ptr++;
}



FILE _iob[OPEN_MAX] = { { 0, (char *)0, (char *)0, 1, 0, 0, 0, 0, 0 },//stdin,read
			{ 0, (char *)0, (char *)0, 0, 1, 0, 0, 0, 1 },//stdout,write
			{ 0, (char *)0, (char *)0, 0, 1, 1, 0, 0, 2 } };//srderr ,write|unbuf


int main(int argc, char *argv[])
{
	FILE *fp;
	char c;

	while (--argc > 0)
		if ((fp = fopen(*++argv, "r")) == NULL)
			return 1;
		else
			while ((c = getc(fp)) != EOF)
				putchar(c);
	return 0;
}
