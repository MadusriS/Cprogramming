#include<stdio.h>
void c_strncat(char *s1, char *s2, char *t, int n) {
    while (*s1) {
        *t++ = *s1++;
    }
    while (n-- > 0) {
        *t++ = *s2++;
    }

    *t = '\0';
}
int c_strncmp(char *s, char *t, int n) {
    for (; *s == *t; s++, t++)
        if (*s == '\0' || --n <= 0)
            return 0;
    return *s - *t;

}
void c_strncpy(char *s, char *t, int n) {
    while (*s && n-- > 0)
        *t++ = *s++;

    *t = '\0';
}

int main()
{
    char s[30]="hello";
    char s1[30]="hai";
    char t[30]="world";
    int n=4;
    int n2;
    c_strncpy(s,t,n);
    printf("%s %s\n",s,t);
    n2=c_strncmp(s,t,n);
    printf("%s %d  %s\n",s,n2,t);
    //c_strncpy(s,t,n);
    //printf("%s %s\n",s,t);
    c_strncat(s,s1,t,n);
    printf("%s  %s %s\n",s,s1,t);
    

    return 0;
}

