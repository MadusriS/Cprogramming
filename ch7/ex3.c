/*Functionality:

printf is a comprehensive and standard function in the C library that supports a wide range of format specifiers for various data types.
minprintf is a simplified version created for educational purposes and supports a limited set of format specifiers (%d, %f, %s, %o, %x) compared to the full functionality of printf.
Implementation:

printf is a built-in function provided by the C standard library, and its implementation may vary across different C libraries. It is a part of the standard I/O library.
minprintf is a custom function defined in your code snippet. It is a simplified and minimalistic implementation of a printf-like function with reduced features.
Variable Arguments Handling:

Both functions use variable arguments (ellipsis ...), but the way they handle these arguments is different.
printf relies on the standard C library functions from the <stdarg.h> header, such as va_start, va_arg, and va_end.
minprintf also uses <stdarg.h> and follows a similar approach for handling variable arguments but has a simplified implementation.
Format Specifiers:

printf supports a wide range of format specifiers for various data types (e.g., %d for integers, %f for floats, %s for strings, etc.).
minprintf supports a subset of format specifiers, including %d, %f, %s, %o, and %x, focusing on basic types.
In summary, printf is a comprehensive and standardized function for formatted output in the C library, while minprintf is a custom and simplified version with reduced functionality, designed for educational purposes.*/
/* minprintf: minimalistic printf function */
#include <stdarg.h>
#include <stdio.h>

void minprintf(char *fmt, ...);/*The three dots (...) in the function declaration void minprintf(char *fmt, ...); represent the ellipsis or variable arguments in C. This feature allows a function to accept a variable number of arguments. In other words, it enables the function to take a variable number of parameters after the fixed parameters specified in the function declaration.

*/

int main(void) {
    char *a = "Hello,World";
    minprintf("%s", a);
    int i = 10;
    minprintf("%d\n", i);
    int b = 011;
    minprintf("b in octal: %o, and in decimal: %d\n", b, b);
    int h = 10;
    minprintf("h in hex: %x, and in decimal: %d\n", h, h);
    return 0;
}

void minprintf(char *fmt, ...) {
    va_list ap; /* points to each unnamed arg in turn */
    char *p, *sval;
    int ival;
    double dval;

    va_start(ap, fmt); /* make ap point to 1st unnamed arg */

    for (p = fmt; *p; p++) {
        if (*p != '%') {
            putchar(*p);
            continue;
        }

        switch (*++p) {
            case 'd':
                ival = va_arg(ap, int);
                printf("%d", ival);
                break;
            case 'f':
                dval = va_arg(ap, double);
                printf("%f", dval);
                break;
            case 's':
                for (sval = va_arg(ap, char *); *sval; sval++)
                    putchar(*sval);
                break;
            case 'o':
                ival = va_arg(ap, int);
                printf("%o", ival);
                break;
            case 'x':
                ival = va_arg(ap, int);
                printf("%x", ival);
                break;
            default:
                putchar(*p);
                break;
        }
    }
    va_end(ap); /* clean up when done */
}
