#include <stdio.h>
#include <string.h>
#include<math.h>
#include<stdlib.h>
#define MAXLINE 1000
void itoa(int n, char s[]);
void reverse(char s[]);
int main(void) {
    int number;
    char str[MAXLINE];
    number = -2147483648;
    printf("Integer %d is made as\n String:", number);
    itoa(number, str);
    printf("%s", str);
    return 0;
}
void itoa(int n, char s[]) {
    int i, sign;
    sign = n;
    i = 0;
    do {
        s[i] = abs(n % 10) + '0';
        i++;
    } while ((n /= 10) != 0);
    if (sign < 0){ 
        s[i] = '-';}
        i++;
    s[i] = '\0';
    reverse(s);
}

void reverse(char s[]) {
    int c, i, j;
    j=strlen(s);
//    printf("%s\n",s);
  //  printf("%d\n",j);
    for (i = 0, j = strlen(s)-1; i < j; i++, j--)
        c = s[i], s[i] = s[j], s[j] = c;
}
