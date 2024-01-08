#include <stdio.h>
#include <string.h>
#define MAXLIMIT 1000
void itoa(int n, char s[], int w);
void reverse(char s[]);
int main(void) {
    int number, width;
    char str[MAXLIMIT];
    number = -323464;
    width = 10;
    itoa(number, str, width);
    printf("%s", str);
    return 0;}
void itoa(int n, char s[], int w) {
    int i, sign;
    if ((sign = n) < 0)
        n = -n;
    i = 0;
    do {
        s[i] = (n % 10) + '0';
	i++;
    } while ((n /= 10) > 0);
    if (sign < 0){
        s[i] = '-';i++;}
    while (i < w){
        s[i] = ' ';
    i++;}
    s[i] = '\0';
    reverse(s);}
void reverse(char s[]) {
    int i, j, temp;
    for (i = 0, j = strlen(s) - 1; i < j; i++, j--)
        temp = s[i], s[i] = s[j], s[j] = temp;
}
