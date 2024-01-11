#include <stdio.h>
#include<string.h>
char r[30];
int i=0;
void rev(char s[],int len){
    if(len<0){ printf("%s\n",r);return;}
    
    r[len]=s[i];
    i++;
    rev(s,len-1);
    
    
    
    
    
}
int main()
{
    char *s;
    s="helloworld";
    int len=strlen(s);
    printf("%d",len);
    rev(s,len-1);

    return 0;
}

