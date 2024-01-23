/*non graphic characters are non visiblw echaracters like space ,blank,nextline ,graphic characters are like alphabets and all..*/
#include <ctype.h>
#include <stdio.h>
#define MAXLINE 100
#define OCTLEN 6  

/* inc : increment position counter for output */
int inc(int pos, int n) {
    if (pos + n < MAXLINE)
        return pos + n;
    else {
        putchar('\n');
        return n;
    }
}

//print arbitrary input in a sensible way 
int main(void) {
    int c, pos;

    pos = 0; // position in the line 

    while ((c = getchar()) != EOF)
        if (iscntrl(c) || c == ' ')//iscntrl()  -- checks for nongraphic characters
         {
          
            pos = inc(pos, OCTLEN);
            printf("\\%03o", c);/*Putting it all together, printf("\\%03o", c); is printing the octal representation of the character c with a minimum width of three characters, padded with leading zeros if necessary, and prefixed with a backslash. This helps in visually distinguishing and formatting the output of non-graphic characters or blanks in the program.*
            /* newline character */
            if (c == '\n')
                pos = 0;
            putchar('\n');
        } else {
            // graphic character 
            pos = inc(pos, 1);
            putchar(c);
        }
    return 0;
}
