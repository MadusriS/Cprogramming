/*flushing:
Flushing refers to the act of writing any buffered data to its intended destination, such as a file or a display. In the context of file I/O, when data is written to a file, it is often not immediately physically written to the disk. Instead, it might be held in a buffer (temporary storage) to improve efficiency. Flushing ensures that any buffered data is actually written or output.*/
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/*
    Exercise 8-4. The standard library function int fseek(FILE *fp, long offset, int origin) is identical to lseek except that fp is a file pointer instead of a file
    descriptor and the return value is an int status, not a position. Write fseek. Make sure that your fseek coordinates properly with the buffering done for the other
    functions of the library.
*/

#ifdef NULL
    #undef NULL
#endif

typedef struct _iobuf
{
    int cnt;    // characters left
    char *ptr;  // next character position
    char *base; // location of buffer
    int flag;   // mode of file access
    int fd;     // file descriptor
} FILE;

enum _flags {
    _READ = 01,  // file open for reading
    _WRITE = 02, // file open for writing
    _UNBUF = 04, // file is unbuffered//whether the buffer is free or not-in that case directly acces the file charcaters
    _EOF = 010,  // EOF has occurred on this file
    _ERR = 020   // error occurred on this file
};

#define NULL 0
#define EOF (-1)
#define BUFSIZ 1024
#define OPEN_MAX 20 // max #files open at once
#define stdin (&_iob[0])
#define stdout (&_iob[1])
#define stderr (&_iob[2])

extern FILE _iob[OPEN_MAX];
int _fillbuf(FILE *fp);
int _flushbuf(int c, FILE *fp);

#define feof(p) (((p)->flag & _EOF) == _EOF)
#define ferror(p) (((p)->flag & _ERR) == _ERR)
#define fileno(p) ((p)->fd)
#define getc(p) (--(p)->cnt >= 0 ? (unsigned char) *(p)->ptr++ : _fillbuf(p))//cnt of remaining characters 
#define putc(x, p) (--(p)->cnt >= 0 ? *(p)->ptr++ = (x) : _flushbuf((x), p))
#define getchar() getc(stdin)
#define putchar(x) putc((x), stdout)
#define PERMS 0666 // RW for owner, group, others
#define MAXERRORMSG 1500

FILE *fopen(char *name, char *mode);
int fflush(FILE *fp);
int fclose(FILE *fp);
void error(char *msg);
int fseek(FILE *fp, long offset, int origin);

FILE _iob[OPEN_MAX] = // stdin, stdout, stderr
{
    { 0, (char *) 0, (char *) 0, _READ, 0 },
    { 0, (char *) 0, (char *) 0, _WRITE, 1 },
    { 0, (char *) 0, (char *) 0, _WRITE | _UNBUF, 2 }
};

int main(int argc, char *argv[])
{
    char msg[MAXERRORMSG];
    FILE *fpIn, *fpOut;
    if (argc == 3)
    {
        if ((fpIn = fopen(*++argv, "r")) == NULL) // input file
            error(strcat(strcat(msg, "error: couldn't open file "), *argv)); // the double strcat adds "error..." to msg and then filename to msg after "error..." string
        if ((fpOut = fopen(*++argv, "w")) == NULL) // output file
            error(strcat(strcat(msg, "error: couldn't write to file "), *argv)); // the double strcat adds "error..." to msg and then filename to msg after "error..." string
    }
    else
        error("usage: ./myfseek input_file output_file");

    if (fseek(fpIn, -1, SEEK_END) == EOF)//-1 means traverse from right to left
        error("failed to seek to last char of input file");
    if (fseek(fpOut, 15, SEEK_CUR) == EOF)
        error("failed to seek 15 bytes past the start/end of the output file");
    putc(getc(fpIn), fpOut);

    if (fseek(fpIn, 1, SEEK_SET) == EOF)
        error("failed to seek to first char of input file");
    if (fseek(fpOut, -16, SEEK_END) == EOF) // if it was -17, lseek would trigger an error since it can't seek to before the file (-1)
        error("failed to seek 16 bytes back from the end of the output file (which is the start of the file)");
    putc(getc(fpIn), fpOut);

    fclose(fpIn); // close the input file
    fclose(fpOut); // close the output file
    exit(0);
}

// opens file. Returns NULL if could not open file/bad mode provided, otherwise returns file ptr
FILE *fopen(char *name, char *mode)
{
    int fd;
    FILE *fp;

    if (*mode != 'r' && *mode != 'w' && *mode != 'a')
        return NULL;
    for (fp = _iob; fp < _iob + OPEN_MAX; fp++)
        if ((fp->flag & (_READ | _WRITE)) == 0) // if both _READ and _WRITE bits not set
            break;
    if (fp >= _iob + OPEN_MAX) // no free slots
        return NULL;
    
    if (*mode == 'w')
        fd = creat(name, PERMS);
    else if (*mode == 'a' && (fd = open(name, O_APPEND, 0)) == -1) // open in append mode. Causes write to always seek to the end before writing, regardless of lseek usage
        fd = creat(name, PERMS); // if files doesn't exist and creat is used to create it, it is possible for lseek to change the position of data written.
        // lseek(fd, 0L, 2); this is unnecessary 
    else
        fd = open(name, O_RDONLY, 0);
    if (fd == -1) // couldn't access name
        return NULL;
    fp->fd = fd;
    fp->cnt = 0;
    fp->base = NULL;
    fp->flag = (*mode == 'r') ? _READ : _WRITE; // remove all flags and set only _READ or _WRITE
    return fp;
}

// allocate and fill input buffer. If error or EOF, return EOF, otherwise return next char in buffer
int _fillbuf(FILE *fp)
{
    if ((fp->flag & (_READ | _EOF | _ERR)) != _READ) // if _READ is not set or _EOF or _ERR is set
        return EOF; // only _READ should be set out of those three. Return EOF
    int bufsize = (fp->flag & _UNBUF) ? 1 : BUFSIZ; // get buffer size//*un buf says whether there is space or not,if no space directly accaess the file*/ 
    if (fp->base == NULL) // no buffer yet
        if ((fp->base = (char *) malloc(bufsize)) == NULL) // create buffer
            return EOF; // failed to create buffer, return EOF
    fp->ptr = fp->base; // reset ptr to base since all chars in buffer have already been read
    fp->cnt = read(fp->fd, fp->ptr, bufsize); // store number of chars read from fd in cnt. Overwrite buffer with up to bufsize number of chars in buffer pointed to by ptr
    if (--fp->cnt < 0) // if cnt - 1 is less than 0
    {
        if (fp->cnt == -1) // if == -1, reached EOF
            fp->flag |= _EOF; // turn on _EOF bit
        else
            fp->flag |= _ERR; // if < -1, error occurred turn on _ERR bit
        fp->cnt = 0; // reset number to indicate no chars left without having negative number
        return EOF;
    }
    return (unsigned char) *fp->ptr++; // if read was successful, return char read from input 
}

// calls fflush to write unread buffered data to output. Stores char in new buffer. Returns EOF if error, otherwise 0
int _flushbuf(int c, FILE *fp)
{
    if (fp == NULL)
        return EOF; // invalid pointer provided
    else if (fflush(fp) == EOF)
        return EOF; // an error occurred in fflush
    *fp->ptr++ = (char) c; // store provided char in buffer. Make sure to cast to the int to char since buffer is based on size of char
    fp->cnt--; // update the counter for the number of chars that can fit in the buffer (just stored one)
    return 0;
}

// if fp is write FILE, writes unwritten buffer to output. if fp == NULL, flushes all write FILES. Returns EOF if error or read FILE provided, otherwise 0.
int fflush(FILE *fp)
{
    if (fp == NULL) // if fp == NULL, then flush all buffers
    {
        int result = 0;
        for (int i = 0; i < OPEN_MAX; i++) // go through all FILEs in _iob. Can't intelligently break from loop early since an older FILE can be closed before a newer one
            if (((&_iob[i])->flag & _WRITE) == _WRITE && fflush(&_iob[i]) == EOF) // if _WRITE flag set, flush it. Also, if fflush returns error, update result
                result = EOF;
        return result; // if any error occurred, return EOF, otherwise return 0
    }
    else if (fp < _iob || fp >= _iob + OPEN_MAX)
        return EOF; // invalid pointer provided
    else if ((fp->flag & (_WRITE | _ERR | _READ)) != _WRITE) // if _WRITE is not set or _ERR or _READ is set.
        return EOF; // only _WRITE should be set out of those three. Return EOF

    int bufsize = (fp->flag & _UNBUF) ? 1 : BUFSIZ; // get buffer size
    if (fp->base == NULL) // no buffer yet so nothing to flush. Create buffer to store provided char
    {
        if ((fp->base = (char *) malloc(bufsize)) == NULL)
        {
            fp->flag |= _ERR; // turn on _ERR bit
            return EOF; // malloc failed to allocate a buffer
        }
    }
    else // buffer exists, so write contents to file
    {
        int n = fp->ptr - fp->base; // gets number of characters in buffer
        if (write(fp->fd, fp->base, n) != n)
        {
            fp->flag |= _ERR; // turn on _ERR bit
            return EOF; // error writing buffer, return EOF
        }
    }
    fp->ptr = fp->base; // reset ptr to base of buffer since data was already written or the buffer was just created
    fp->cnt = bufsize; // update the counter for the number of chars that can fit in the buffer so putc will work correctly
    return 0;
}

// flushes any unwritten data for fp, discards any unread buffered input, frees any automatically allocated buffer, then closes the file. Returns EOF if error, otherwise 0
int fclose(FILE *fp)
{
    int result = 0;
    if (fp == NULL || fp < _iob || fp >= _iob + OPEN_MAX)
        return EOF; // invalid pointer provided
    if ((fp->flag & _WRITE) == _WRITE) // if file is for writing
        result = fflush(fp); // flush anything in buffer to output and store result
    if (fp->base != NULL) // if there is a buffer that malloc allocated (applies to both read and write FILEs)
        free(fp->base); // free it since we don't want a memory leak
    if (close(fp->fd) != 0) // close file (note: it's legal to close stdin, stdout, and stderr. Just reopen them (e.g. freopen) or exit program afterwards to prevent issues)
        result = EOF; // close returns 0 if no errors. So update result to error since it failed to close
    fp->cnt = fp->flag = 0; // reset cnt and flag to 0
    fp->fd = -1; // reset fd to -1 (which indicates error and should prevent improper usage of this FILE's fd)
    fp->ptr = fp->base = NULL; // and reset the pointers to NULL. All of this resetting makes the FILE object available for future fopen calls
    return result; // defaults to 0 unless fflush or close returned an error
}

// print an error message and exit. Since normal printf messages aren't coded, use putc for stderr
void error(char *msg)
{
    fflush(NULL); // flush all buffers before program exits and error is printed
    while (*msg != '\0')
        putc(*msg++, stderr); // write error chars to stderr
    putc('\n', stderr); // finish it off with '\n', just in case
    fflush(stderr); // make sure error message is printed
    exit(1); // close program
}

// sets file position specified by offset and origin; subsequent read or write calls will access data at the new position. returns EOF is error, otherwise 0
int fseek(FILE *fp, long offset, int origin)
{
    if (fp == NULL || fp < _iob || fp >= _iob + OPEN_MAX)
        return EOF; // invalid pointer provided
    else if (origin != SEEK_SET && origin != SEEK_CUR && origin != SEEK_END)
        return EOF; // invalid origin flag
    else if ((fp->flag & (_READ | _WRITE)) == 0 || (fp->flag & (_READ | _WRITE)) == _READ + _WRITE)
        return EOF; // niether _READ nor _WRITE set or both are set
    if ((fp->flag & _WRITE) == _WRITE) // _WRITE flag set
    {
        if (fflush(fp) == EOF) // flush write buffer before seeking
            return EOF; // if fflush returns error, return EOF
    }
    else // _READ flag set
    { // the FILE already read bufsize chars by calling read() and stored them in a buffer. However, some chars weren't read by the program and seek doesn't know this
        if (origin == SEEK_CUR) // the program doesn't know the current position in the file. fp->cnt is the number of unread chars in the buffer
        { // so if seeking based on current location (origin == SEEK_CUR), the offset needs to be subtracted by the number of unread buffered chars
            if (offset >= 0 && offset <= fp->cnt) // However, if offset is still in the buffered chars, just move within buffer to prevent unnecessary seeks and reads
            {
                fp->ptr += offset; // move pointer to the new position in the buffer by the offset
                fp->cnt -= offset; // update number of chars left in buffer after the move
                return 0; // return 0 to indicate no errors occurred
            }
            offset -= fp->cnt; // since not within the buffer, subtract cnt away from the provided offset negative numbers are acceptable 
        }
        fp->cnt = 0; // finally, update cnt make the buffer get discarded later. When cnt == 0, getc calls _fillbuf which overwrites the buffer with new content
    }   
    if (lseek(fp->fd, offset, origin) == -1) // if lseek returns -1, an error occurred
    {
        fp->flag |= _ERR; // so update the _ERR flag and return EOF
        return EOF; 
    }
    fp->flag &= ~_EOF; // since seek didn't return an error, unset EOF flag. Note: it's legal to seek past the end of a file, so turn off EOF flag when lseek is error free
    return 0;
}
