#include <stdio.h>
#include <string.h> 

#define MAXLINE 1000

int my_getline(char s[], int lim) {
    int c, i;
    for (i = 0; i < lim - 1 && (c = getchar()) != EOF && c != '\n'; ++i)
        s[i] = c;
    if (c == '\n') {
        s[i] = c;
        ++i;
    }
    s[i] = '\0';
    return i;
}

int main() {
    int len;
    int max = 0;
    char line[MAXLINE];
    char longest[MAXLINE];

    while ((len = my_getline(line, MAXLINE)) > 1) {
        if (len > max) {
            max = len;
            strncpy(longest, line, MAXLINE);
        }
    }

    if (max > 0) {
        printf("%d:%s", max, longest);
    }

    return 0;
}

