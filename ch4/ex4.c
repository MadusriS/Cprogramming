#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#define MAXVAL 100

int sp = 0;
double val[MAXVAL];

void push(double f) {
    if (sp < MAXVAL) val[sp++] = f;
    else printf("error: stack full, can't push %g\n", f);
}

double pop(void) {
    if (sp > 0) return val[--sp];
    else {
        printf("error: stack empty\n");
        return 0.0;
    }
}

int main() {
    char input[100];
    double op2;

    while (scanf("%s", input) != EOF) {
        if (isdigit(input[0]) || (input[0] == '-' && isdigit(input[1])))
            push(atof(input));
        else {
            switch (input[0]) {
                case '+': push(pop() + pop()); break;
                case '*': push(pop() * pop()); break;
                case '-': op2 = pop(); push(pop() - op2); break;
                case '/': op2 = pop(); 
                          if (op2 != 0.0) push(pop() / op2);
                          else printf("error: zero divisor\n");
                          break;
                case '%': op2 = pop(); 
                          if (op2 != 0.0) push(fmod(pop(), op2));
                          else printf("error: zero divisor\n");
                          break;
                case '?': op2 = pop(); printf("\t%.8g\n", op2); push(op2); break;
                case 'c': sp = 0; break;
                case 'd': op2 = pop(); push(op2); push(op2); break;
                case 's': op2 = pop(); double temp = pop(); push(op2); push(temp); break;
                case '\n': printf("\t%.8g\n", pop()); break;
                default: printf("error: unknown command %s\n", input); break;
            }
        }
    }
    return 0;
}

