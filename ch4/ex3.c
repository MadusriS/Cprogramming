#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#define MAX_STACK 100

int sp = 0;
double stack[MAX_STACK];

void push(double val) {
    if (sp < MAX_STACK)
        stack[sp++] = val;
    else
        printf("error: stack full, can't push %g\n", val);
}

double pop(void) {
    if (sp > 0)
        return stack[--sp];
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
        else if (input[0] == '+')
            push(pop() + pop());
        else if (input[0] == '-')
            if (sp > 1) {
                op2 = pop();
                push(pop() - op2);
            } else {
                push(-pop());
            }
        else if (input[0] == '*')
            push(pop() * pop());
        else if (input[0] == '/') {
            op2 = pop();
            if (op2 != 0.0)
                push(pop() / op2);
            else
                printf("error: zero divisor\n");
        }
        else if (input[0] == '%') {
            op2 = pop();
            if (op2 != 0.0)
                push(fmod(pop(), op2));
            else
                printf("error: zero divisor\n");
        }
        else if (input[0] == '\n')
            printf("Result: %.8g\n", pop());
        else
            printf("error: unknown command %s\n", input);
    }
    return 0;
}

