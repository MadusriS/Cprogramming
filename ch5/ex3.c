#include <stdio.h>
#include<string.h>
void strcating(char *s,char *t){
    int i,j;
    int lens=strlen(s);
    int lent=strlen(t);
    for(i=0;i<lens;i++){printf("%c\n",s[i]);}
    for(j=0;i<lens+lent;j++,i++){
        s[i]=t[j];
        printf("%c\n",s[i]);
    }
    s[i]='\0';
    printf("%s",s);
    
}
void mystrcat(char *s, char *t) {
    while (*s != '\0')
        s++;
                
    while ((*s = *t) != '\0') {
        s++;
        t++;
    }
}


int main()
{
   char s1[30]="hello";
   char s[30]="hai";
   char *t="world";
   strcating(s1,t);
   mystrcat(s,t); 
   printf("the string is%s\n",s);

    return 0;
}

