#include <stdio.h>

#define TAB_SPACES 8

int main() {
    int c;
    int pos = 0;
    int nosp;

    while ((c = getchar()) != EOF) {
        if (c == '\t') {
            nosp = TAB_SPACES - (pos % TAB_SPACES);

            while (nosp > 0) {
                putchar('#');
                nosp--;
                pos++;
            }
        } else if (c == '\n') {
            putchar(c);
            pos = 0;
        } else {
            putchar(c);
            pos++;
        }
    }

    return 0;
}

