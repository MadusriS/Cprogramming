#include <stdio.h>
#include<string.h>
int flag=0;
char * strrev(char t[]){
    char temp;
    for(int i=0;i<strlen(t)/2;i++){
        temp=t[i];
        t[i]=t[strlen(t)-1-i];
        t[strlen(t)-1-i]=temp;
        
        
    }
    return t;
    
}
void str1end(char *s,char *t){
   
    t=strrev(t);
    printf("%s\n",t);
    int i,j;
    
    
    for(i=0,j=strlen(s)-1;i<strlen(t);i++,j--){
        if(s[j]!=t[i]){
            flag=1;
    }
}}
int strend(char *s, char *t) {
    int len;
    len = strlen(t);
    while (*s != '\0')
        ++s;
    --s;

    while (*t != '\0')
        ++t;

    --t;
    while (len > 0) {
        if (*t == *s) {
            --t;
            --s;
            --len;
        } else
            return 0;
    }
    if (len == 0)
        return 1;
}

int main()
{
    char s[30]="helloworld";
    char t[30]="jkl";
    int ret = strend(s, t);
    printf("ret %d", ret);
    str1end(s,t);
    if(flag==0)printf("t is there in string s end");
    else printf("no ,its not there");

    return 0;
}
