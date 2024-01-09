#include <stdio.h>
#include <string.h>

#define MAXBUF 100
#define MAXLINE 100

int bufp = 0;
int buf[MAXBUF];
void ungetch(int c) {
    if (bufp >= MAXBUF)
        printf("ungetch: too many characters\n");
    else
        buf[bufp++] = c;
}

int getch(void) { return (bufp > 0) ? buf[--bufp] : getchar(); }
int mgetline(char s[], int lim) {
    int i, c;

    for (i = 0; i < lim - 1 && (c = getchar()) != EOF && c != '\n'; ++i)
        s[i] = c;

    if (c == '\n')
        s[i++] = c;

    s[i] = '\0';
    return i;
}
void ungets(char s[]) {
    int i;

    i = strlen(s);

    while (i > 0)
        ungetch(s[--i]);
}
int main(void) {
    char line[MAXLINE];
    int c;

    mgetline(line, MAXLINE);

    ungets(line);

    while ((c = getch()) != EOF)
        putchar(c);

    return 0;
}
