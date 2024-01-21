#include <stdio.h>

#define MAXLINE 1000
#define LIMIT 30



int _getline(char line[], int lim);

int main(void) {
    int len;
    char line[MAXLINE];

    while ((len = _getline(line, MAXLINE)) > 0) {
        if (len > LIMIT)
            printf("%s", line);
    }

    return 0;
}

int _getline(char line[], int lim) {
    int i, c;

    for (i = 0; i < lim - 1 && (c = getchar()) != EOF && c != '\n'; ++i)
        line[i] = c;

    if (c == '\n') {
        line[i] = c;
        ++i;
    }
    line[i] = '\0';

    return i;
}

