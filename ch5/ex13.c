#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#define MAX_LINES 5000
#define MAX_LINE_LENGTH 500

char lines[MAX_LINES][MAX_LINE_LENGTH];
int line_lengths[MAX_LINES];
int line_count = 0;

int mgetline(char s[], int lim) {
    int c, i;
    for (i = 0; i < lim - 1 && (c = getchar()) != EOF && c != '\n'; ++i)
        s[i] = c;
    
    if (c == '\n') {
        s[i] = c;
        ++i;
    }

    s[i] = '\0';
    return i;
}

int main(int argc, char *argv[]) {
    char s[MAX_LINE_LENGTH];
    int n;

    if (argc == 1)
        n = 10;
    else if (argc == 2 && (*++argv)[0] == '-')
        n = atoi(argv[0] + 1);
    else {
        printf("Usage: tail [-n]\n");
        return 1;
    }

    while (mgetline(s, MAX_LINE_LENGTH) > 0) {
        if (line_count < MAX_LINES) {
            strcpy(lines[line_count], s);
            line_lengths[line_count] = strlen(s);
            line_count++;
        } else {
            printf("Warning: Exceeded maximum number of lines.\n");
            break;
        }
    }

    int start = (line_count - n) >= 0 ? (line_count - n) : 0;

    for (int i = start; i < line_count; i++) {
        printf("%s", lines[i]);
    }

    return 0;
}

