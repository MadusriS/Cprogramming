#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<stdlib.h>
#define NUMBER '0' 
#define MAXVAL 100
#define BUFSIZE 100
#define MAXLINE 1000
#define MAXOP 100
int getch(void);
void ungetch(int);
int getop(char *);
void push(double);
double pop(void);
int mgetline(char *s, int lim);
int strindex(char *s, char *t);
int atoiv2(char *);
void itoav2(int n, char *s);
void rev(char *);
int sp = 0;
int bufp = 0;
double val[MAXVAL];
char buf[BUFSIZE];
char pattern[] = "ould"; 
int main(void) {
    char line[MAXLINE];
    int found = 0;
while ((mgetline(line, MAXLINE)) > 0)
        if (strindex(line, pattern) >= 0) {
            printf("%s\n", line);
            found++;
        }

    char *s = "1234";
    int ret;
    ret = atoiv2(s);
    printf("%d\n", ret);
    char s1[100];
    int i = 12345;
    itoav2(i, s1);
    rev(s1);
    printf("%s\n", s1);
    char *s2 = "This is a line";
    char *t = "is";
    ret = 0;
    ret = strindex(s2, t);
    printf("%d\n", ret);
    int type;
    double op2;
    char s3[MAXOP];
    while ((type = getop(s3)) != EOF) {
        switch (type) {
            case NUMBER:
                push(atof(s3));
                break;
            case '+':
                push(pop() + pop());
                break;
            case '*':
                push(pop() * pop());
                break;
            case '-':
                op2 = pop();
                push(pop() - op2);
                break;
            case '/':
                op2 = pop();
                if (op2 != 0.0)
                    push(pop() / op2);
                else
                    printf("error: zero divisor\n");
                break;
            case '\n':
                printf("\t%.8g\n", pop());
                break;
            default:
                printf("error: unknown command %s\n", s);
                break;
        }
    }
    return 0;
}
int atoiv2(char *s) {
    int n, sign;

    for (; isspace(*s); s++)  
        ;
    sign = (*s == '-') ? -1 : 1;

    if (*s == '+' || *s == '-')
        s++;
    for (n = 0; isdigit(*s); s++)
        n = 10 * n + *s - '0';

    return sign * n;
}
void push(double f) {
    if (sp < MAXVAL)
        val[sp++] = f;
    else
        printf("error: stack full,can't push %g\n", f);
}
double pop(void) {
    if (sp > 0)
        return val[--sp];
    else {
        printf("error: stack empty \n");
        return 0.0;
    }
}
int getop(char *s) {
    int c;

    while ((*s = c = getch()) == ' ' || c == '\t');
    *(s + 1) = '\0';

    if (!isdigit(c) && c != '.')
        return c; 
    if (isdigit(c))
        while (isdigit(*++s = c = getch()));

    if (c == '.')
        while (isdigit(*++s = c = getch()));

    *s = '\0';

    if (c != EOF)
        ungetch(c);
    return NUMBER;
}
int getch(void) {
    return (bufp > 0) ? buf[--bufp] : getchar();
}
void ungetch(int c) {
    if (bufp >= BUFSIZE)
        printf("ungetch: too many characters\n");
    else
        buf[bufp++] = c;
}
void itoav2(int n, char *s) {
    int sign;
    char *t = s;
    if ((sign = n) < 0)
        n = -n;
    do {
        *s++ = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0)
        *s++ = '-';
    *s = '\0';

}
void rev(char *s) {
    int c;
    char *t;

    for (t = s + (strlen(s) - 1); s < t; s++, t--) {
        c = *s;
        *s = *t;
        *t = c;
    }
}
int mgetline(char *s, int lim) {
    int c;
    char *t = s;

    while (--lim > 0 && (c = getchar()) != 'X' && c != '\n')
        *s++ = c;

    if (c == '\n')
        *s++ = c;
    *s = '\0';

    return s - t;
}
int strindex(char *s, char *t) {
    char *b = s;
    char *p, *r;

    for (; *s != '\0'; s++) {
        for (p = s, r = t; *r != '\0' && *p == *r; p++, r++);

        if (r > t && *r == '\0')
            return s - b;
    }
    return -1;
}
