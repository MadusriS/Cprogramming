/*reverse polish calculator:
#include <stdio.h>
#include <ctype.h>
int buf[100]={0};
int bufp=0;
int val[100]={0};
int valp=0;
void push( int c){
    printf("2.push \n");
    buf[bufp]=c;
    bufp++;
    printf("push bufp=%d\n",bufp);
}
int pop(){
    printf("3.pop\n");
    if(bufp>0){
         printf("pop bufp=%d\n",bufp);
    return buf[--bufp];
    }
   
    
}
int getch(){
    printf("1.getchar\n");
   
        printf("1.getchar else \n");
        int n=getchar();
        return (n==EOF)?EOF:(char)n;
       // return n;
    
    
}

int main()
{   
    int c,op2;
    
    while((c=getch())!=EOF){
        if(isdigit(c)){
            push(c-'0');
            //break;
        }
        else{
            switch(c){
                case '+':
                int c1=pop();
                int d=pop();
                printf("3 bufp=%d\n",bufp);
                push(c1+d);
                printf("finaly%d",buf[bufp]);
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
           
            default:
                printf("error: unknown command ");
                break;
                
                
                
            }
        }
        
        
        
        
    }
     if (bufp > 0) {
        printf("Result: %d\n", pop());
    } else {
        printf("Error: Stack empty, no result\n");
    }

    return 0;
}
*/
#include <stdio.h>
#include <stdlib.h> 

#define MAXOP 100  
#define NUMBER '0' 

int getop(char[]);

void push(double);

double pop(void);


int main() {
    int type;
    double op2;
    char s[MAXOP];
    while ((type = getop(s)) != EOF) {
        switch (type) {
            case NUMBER:
                push(atof(s));
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
}

#define MAXVAL 100

int sp = 0;
double val[MAXVAL];

void push(double f) {
    if (sp < MAXVAL)
        val[sp++] = f;
    else
        printf("error:stack full, cant push %g\n", f);
}

double pop(void) {
    if (sp > 0)
        return val[--sp];
    else {
        printf("error: stack empty\n");
        return 0.0;
    }
}

#include <ctype.h>

int getch(void);

void ungetch(int);

int getop(char s[]) {
    int i, c;

    while ((s[0] = c = getch()) == ' ' || c == '\t');
    s[1] = '\0';

    i = 0;
    if (!isdigit(c) && c != '.' && c != '-')
        return c;

    if (c == '-')
        if (isdigit(c = getch()) || c == '.')
            s[++i] = c;
        else {
            if (c != EOF)
                ungetch(c);
            return '-';
        }

    if (isdigit(c))
        while (isdigit(s[++i] = c = getch()));

    if (c == '.')
        while (isdigit(s[++i] = c = getch()));

    s[i] = '\0';
    if (c != EOF)
        ungetch(c);
    return NUMBER;
}

#define BUFSIZE 100

char buf[BUFSIZE];
int bufp = 0;

int getch(void) {
    char c;
    if (bufp > 0) {
        return buf[--bufp];
    } else {
        scanf("%c", &c);
        return c;
    }
}

void ungetch(int c) {
    if (bufp >= BUFSIZE)
        printf("ungetch: too many characters\n");
    else
        buf[bufp++] = c;
}
