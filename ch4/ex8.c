#include <stdio.h>

int buf = EOF;

int getch(void) {
    int c;
    if (buf != EOF) {
        c = buf;
        buf = EOF;
    } else {
        c = getchar();
    }
    return c;
}

void ungetch(int c) {
    if (buf != EOF) {
        printf("ungetch: too many characters\n");
    } else {
        buf = c;
    }
}

int main(void) {
    int c;

    c = '*';
    ungetch(c);

    while ((c = getch()) != EOF) {
        putchar(c);
    }

    return 0;
}

