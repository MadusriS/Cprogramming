#include <stdlib.h>
#include<stdio.h>

void itoa(int n, char s[]) {
    static int i;
    i=0;
    if (n / 10)
        itoa(n / 10, s);
    s[i++] = n % 10 + '0';
    s[i] = '\0';

}



int main(void) {
    int n;
    char s[30];
    n = 1234;
    itoa(n, s);
    printf("%s", s);
    return 0;
}


