#include <stdio.h>
int bre, brk, par;
void incomment();
void inquote(int c);
void search(int c);
int main(void) {
    int c;
    while ((c = getchar()) != EOF)
        if (c == '/')
            if ((c = getchar()) == '*')
                incomment();
            else
                search(c);
        else if (c == '\'' || c == '"')
            inquote(c);
        else
            search(c);

    if (bre < 0) {
        printf("Unmatched Braces\n");
        bre = 0;
    } else if (brk < 0) {
        printf("Unmatched brackets\n");
        brk = 0;
    } else if (par < 0) {
        printf("Unmatched parenthesis\n");
        par = 0;
    }

    if (bre > 0)
        printf("Unmatched braces\n");
    else if (brk > 0)
        printf("Unmatched brackets\n");
    else if (par > 0)
        printf("Unmatched parenthesis\n");

    return 0;
}
void incomment() {
    int c, d;
    c = getchar();
    d = getchar();
    while (c != '*' || d != '/') {
        c = d;
        d = getchar();
    }
}

void inquote(int c) {
    int d;
    putchar(c);
    while ((d = getchar()) != c)
        if (d == '\\')
            getchar();
}

void search(int c) {
        if (c == '{')
        --bre;
    else if (c == '}')
        ++bre;
    else if (c == '[')
        --brk;
    else if (c == ']')
        ++brk;
    else if (c == '(')
        --par;
    else if (c == ')')
        ++par;
}
