#include <stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
int mgetline(char line[],int lim);
int c,len1;
char buf[30];
int bufp=0;
 double val[100];int vali=0;

int len;
char line[30];
void push(char buf[],int size ){
    printf("im in push\n");
    val[vali]=atof(buf);
    memset(buf, ' ', size);
    printf("in val of index %d,i have %f\n",vali,val[vali]);
    vali++;
    
}
double pop(){
    printf("in pop the value of i is %d\n th value is %f\n",vali,val[vali]);
    double d=val[vali];
    vali--;
    return d;
    
}
int i=0;
int type(){
    printf("2 im in type\n");
    printf("%d %d\n",len ,i);
   if(line[i]=='\0') return EOF;
    
   
        printf("line[%d]=%c\n",i,line[i]);
        if(line[i]==' '){
            i++;
            
        }
        if(!isdigit(line[i])&&line[i]!='.'){
        return line[i];}
        while(isdigit(line[i]) || line[i]=='.'){
            buf[bufp]=line[i];
            //printf("buf[%d]=%c\n",bufp,buf[1]);
            printf("buf[%d]=%c\n",bufp,buf[bufp]);
            i++;
            bufp++;
            printf(" in buf %s\n",buf);
            
        }
        buf[bufp]='\0';
        return 0;
        
        
        
    }
    
    

int main(){
    int j=0;
    int lim=30;
    len=(mgetline(line,lim)-1);
    len1=len;
    
   //int t=type();
    while(line[j]!='\0'){
        printf("1 im in main\n");
        int t=type();
        switch(t){
            case 0:
            push(buf,sizeof(buf));break;
            case '+':
            vali--;
            printf("3 in case + %f\n",pop()+pop());
           break;
            
        }
        len=len-1;
        j++;
        
        
    }
    return 0;
}
int mgetline(char line[],int lim){
    int i;
    printf("enter character\n");
    
   
    for(i=0;(c=getchar())!=EOF && c!='\n'&& i<lim;i++){
         printf("%c\n",c);
        line[i]=c;
        printf("%s\n",line);
    }
    if(c=='\n'){
        line[i]=c;
        
    }
    i++;
    line[i]='\0';
    printf("in line %s\n",line);
    return i;
}


