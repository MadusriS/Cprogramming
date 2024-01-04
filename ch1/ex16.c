#include <stdio.h>


#define MAX 1000


int get_line(char arr[], int lim);

void copy(char to[], const char from[]);

int main() {

    int len, max;
    char line[MAX];
    char longest[MAX];

    max = 0;
    while ((len = get_line(line, MAX)) > 0) {

        if (len > max) {

            max = len;
            copy(longest, line);
        }
    }

    if (max > 0) {

        printf("length = %i, string=%s", max, longest);
    }

    return 0;
}


int get_line(char arr[], int lim) {

    int c, i;

    for (i = 0; i < lim - 1 && (c = getchar()) != EOF && c != '\n'; ++i) {

        arr[i] = c;
    }
    if (c == '\n') {

        arr[i] = c;
        ++i;

    } else {

       
        while ((c = getchar() != EOF) && c != '\n') {

            ++i;
        }

        if (c == '\n') {

            arr[i] = c;
            ++i;
        }
    }

    arr[i] = '\0';
    return i;
}


void copy(char to[], const char from[]) {

    int i;

    i = 0;

    while ((to[i] = from[i]) != '\0') {

        ++i;
    }
}

