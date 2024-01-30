/*#include <fcntl.h>
#include <stdarg.h>
// The functions available from stdio.h are implemented here.
//#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

#ifdef NULL
#undef NULL
#endif

#define NULL 0
#define EOF (-1)
#define BUFSIZ 1024
#define OPEN_MAX 20 /* max # files open at once //

typedef struct _iobuf {
    int cnt;    /* characters left//
    char *ptr;  /* next character position //
    char *base; /* location of the buffer //
    int flag;   /* mode of the file access //
    int fd;     /* file descriptor //
} FILE;

extern FILE _iob[OPEN_MAX];

#define stdin (&_iob[0])
#define stdout (&_iob[1])
#define stderr (&_iob[2])

enum _flags {
    _READ = 01,
    /* file open for reading */ /* binary 1 //
    _WRITE = 02,
    /* file open for writing */ /* binary 10 //
    _UNBUF = 03,
    /* file is unbuffered */ /* binary 11 //
    _EOF = 010,
    /* EOF has occurred on this file */ /* binary 1000 //
    _ERR = 020,
    /* error occurred on this file */ /* binary 10000//
};

int _fillbuf(FILE *);

int _flushbuf(int, FILE *);

#define feof(p) (((p)->flag & _EOF) != 0)
#define ferror(p) (((p)->flag & _ERR) != 0)
#define fileno(p) ((p)->fd)

#define getc(p) (--(p)->cnt >= 0 ? (unsigned char)*(p)->ptr++ : _fillbuf(p))

#define putc(x, p) (--(p)->cnt >= 0 ? *(p)->ptr++ = (x) : _flushbuf((x), p))

#define getchar() getc(stdin)
#define putchar(x) putc((x), stdout)

#define PERMS 0666 /* RW for owner, group and others //

/* fopen: open file, return file ptr //

FILE *fopen(char *name, char *mode) {
    int fd;
    FILE *fp;

    if (*mode != 'r' && *mode != 'w' && *mode != 'a')//a means append
        return NULL;


    for (fp = _iob; fp < _iob + OPEN_MAX; fp++)
        if ((fp->flag & (_READ | _WRITE)) == 0)/*flag determines the mode,if its neither read nor write break it//
            break; /* found free slot //

    if (fp >= _iob + OPEN_MAX) /* no free slots //
        return NULL;

    if (*mode == 'w')
        fd = creat(name, PERMS);
    else if (*mode == 'a') {
        if ((fd = open(name, O_WRONLY, 0)) == -1)
            fd = creat(name, PERMS);
        lseek(fd, 0L, 2);/*lseek(fd,long offset,origin)//org=0 means from start,1 means from mid,2 means from end//
    } else
        fd = open(name, O_RDONLY, 0);

    if (fd == -1) /* couldn't access name //
        return NULL;

    fp->fd = fd;
    fp->cnt = 0;
    fp->base = NULL;
    fp->flag = (*mode == 'r') ? _READ : _WRITE;
    return fp;
}

/* _fillbuf: allocate and fill input buffer //

int _fillbuf(FILE *fp) {
    int bufsize;

    /* this is a bit operation //

    if ((fp->flag & (_READ | _EOF | _ERR)) != _READ)
        return EOF;

    /* this is a bit operation //

    bufsize = (fp->flag & _UNBUF) ? 1 : BUFSIZ;

    if (fp->base == NULL) /* no buffer yet //
        if ((fp->base = (char *) malloc(bufsize)) == NULL)
            return EOF; /* can't get buffer //

    fp->ptr = fp->base;
    fp->cnt = read(fp->fd, fp->ptr, bufsize);/* fp ptr tells where the data has to go or come from//bufsize-no of bytes to be transferred//returns no of count of bytes transferred*/

    /* these are bit operations //

    if (--fp->cnt < 0) {
        if (fp->cnt == -1)
            fp->flag |= _EOF;
        else
            fp->flag |= _ERR;
        fp->cnt = 0;
        return EOF;
    }

    return (unsigned char) *fp->ptr++;
}

/* _flushbuf: flush a buffer //
int _flushbuf(int c, FILE *f) {
    int num_written, bufsize;
    unsigned char uc = c;

    if ((f->flag & (_WRITE | _EOF | _ERR)) != _WRITE) {
        return EOF;
    }

    if (f->base == NULL && ((f->flag & _UNBUF) == 0)) {
        /* no buffer yet //
        if ((f->base = malloc(BUFSIZ)) == NULL)
            /* could not allocate a buffer, so try unbuffered.//
            f->flag |= _UNBUF;
        else {
            f->ptr = f->base;
            f->cnt = BUFSIZ - 1;
        }
    }

    if (f->flag & _UNBUF) {
      
        f->ptr = f->base = NULL;
        f->cnt = 0;
        if (c == EOF) {
            return EOF;
        }
        num_written = write(f->fd, &uc, 1);
        bufsize = 1;
    } else {
        /* buffered write */
        /*if ( c!= EOF) {
                f->ptr = uc;
        } //
        bufsize = (int) (f->ptr - f->base);
        num_written = write(f->fd, f->base, bufsize);
        f->ptr = f->base;
        f->cnt = BUFSIZ - 1;
    }

    if (num_written == bufsize)
        return c;
    else {
        f->flag |= _ERR;
        return EOF;
    }
}
/*
typedef struct _iobuf {
    int cnt;    //characters left //
    char *ptr;  // next character position //
    char *base; /* location of the buffer //
    int flag;   /* mode of the file access//
    int fd;     /* file descriptor//
} FILE;
in FILE _iob[OPEN_MAX]  first 3 index-->0,1,2 is occupied as stdin,stdout,stderr others are assigned as 0.

#define stdin (&_iob[0])
#define stdout (&_iob[1])
#define stderr (&_iob[2])
//

FILE _iob[OPEN_MAX] = {/* stdin, stdout, stderr //
        {0, (char *) 0, (char *) 0, _READ,           0},
        {0, (char *) 0, (char *) 0, _WRITE,          1},
        {0, (char *) 0, (char *) 0, _WRITE | _UNBUF, 2}};

int main(int argc, char *argv[]) {
FILE *fp;
char c;
while(--argc>0)
if((fp=fopen(*++argv,"r"))==NULL)
return 1;
else{
    //int c;
    while ((c = getchar()) != 'x') {
        putchar(c);
    }}
}
/*
#define getchar() getc(stdin)
#define putchar(x) putc((x), stdout)
#define getc(p) (--(p)->cnt >= 0 ? (unsigned char)*(p)->ptr++ : _fillbuf(p))

#define putc(x, p) (--(p)->cnt >= 0 ? *(p)->ptr++ = (x) : _flushbuf((x), p))


*/


/*

#define NULL 0
#define EOF (-1)
#define OPEN_MAX 20

typedef struct _iobuf {
	int cnt;
	char *ptr;
	char *base;
	int read;
	int write;
	int unbuf;
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

/***********
 *  fopen  *
 ***********/

#include <fcntl.h>
#include <unistd.h>
#define PERMS 0666

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

/**************
 *  _fillbuf  *
 **************/

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

/**************************
 *  stdin, stdou, stderr  *
 **************************/

FILE _iob[OPEN_MAX] = { { 0, (char *)0, (char *)0, 1, 0, 0, 0, 0, 0 },
			{ 0, (char *)0, (char *)0, 0, 1, 0, 0, 0, 1 },
			{ 0, (char *)0, (char *)0, 0, 1, 1, 0, 0, 2 } };

/****************
 *  main (cat)  *
 ****************/

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









