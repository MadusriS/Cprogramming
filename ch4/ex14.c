#include<stdio.h>

#define swap(t, x, y) { t z;   \
                        z = x; \
                        x = y;  \
                        y = z; }

int main(void) {
    int x,y;
    x = 1;
    y = 2;
    printf("x= %d \t y= %d\n", x, y);
    swap(int, x, y);
    printf("x=%d \t y=%d\n", x, y);
}
