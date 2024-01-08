#include <stdio.h>

unsigned rtrot(unsigned x, int n);
int wordlength(void);

int main(void) {
    printf("%u", (unsigned)rtrot((unsigned)8, (int)1));

    return 0;
}


unsigned rtrot(unsigned x, int n) {
    unsigned rev;

    rev = x << (wordlength() - n);
    x = x >> n;
    x = x | rev;

    return x;
}

int wordlength(void) {
    int i;
    unsigned v = (unsigned)~0;

    for (i = 1; (v = v >> 1) > 0; i++)
        ;
    return i;
}
