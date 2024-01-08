#include <stdio.h>
int low(int c);
int main(void) {
    int c;
    while ((c = getchar()) != EOF) {
        putchar(low(c));
    }}
int low(int c) { return c >= 'A' && c <= 'Z' ? c + 'a' - 'A' : c; }

