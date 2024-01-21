#include <stdio.h>

void removeComments() {
    int ch, nextCh;

    while ((ch = getchar()) != EOF) {
        if (ch == '/') {
            if ((nextCh = getchar()) == '/') {
         
                while ((ch = getchar()) != '\n' && ch != EOF)
                    ;
            } else if (nextCh == '*') {
                
                while ((ch = getchar()) != '*' || (nextCh = getchar()) != '/')
                    ;
            } else {
                putchar(ch);
                putchar(nextCh);
            }
        } else {
            putchar(ch);
        }
    }
}

int main() {
    removeComments();
    return 0;
}/*
#include <stdio.h>
void rcomment(int c);
void incomment(void);
void insicomm();
int main(void) {
    int c, d;
while ((c = getchar()) != EOF)
        rcomment(c);
    return 0;
}
void rcomment(int c) {
    int d;

    if (c == '/') {
        if ((d = getchar()) == '*')
            incomment();
        else if (d == '/') {      
            insicomm();
        } else {	
            putchar(c);
            putchar(d);}
        }else putchar(c);}
void incomment() {
    int c, d;
    c = getchar();
    d = getchar();

    while (c != '*' || d != '/') {
        c = d;
        d = getchar();
    }
}  
void insicomm(){
	while(getchar()!='\n');}*/
