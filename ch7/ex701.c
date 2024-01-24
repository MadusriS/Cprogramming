#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#define MAXLINE 100
void compareFiles(FILE *, FILE *);
int main(int argc, char *argv[]) {
    FILE *fp1, *fp2;
    

    if (argc == 1) {
        /* no args; copy standard input */
        printf("Please provide two files");
        return 1;
    } else
        while (--argc > 0) {
            if ((fp1 = fopen(argv[1], "r")) == NULL) {
                printf("cat: can't open %s\n", *argv);
                return 1;
            }

            if ((fp2 = fopen(argv[2], "r")) == NULL) {
                printf("cat: can't open %s\n", *argv);
                return 1;
            }
            compareFiles(fp1, fp2);
            fclose(fp1);
            fclose(fp2);
        }

    return 0;
}

void compareFiles(FILE *file1, FILE *file2) {
    char line1[MAXLINE];
    char line2[MAXLINE];
    int lineNumber = 0;
    if (fgets(line2, MAXLINE, file2) == NULL) {
        printf("Error reading from file2.\n");
        return;
    }
    /*The issue in your code is that the fgets function reads the newline character ('\n') from file2 and includes it in line2. When you use strstr to search for line2 within line1, the newline character makes it less likely to find an exact match.

To fix this issue, you can remove the newline character from line2 before comparing. Additionally, you should handle the case where line2 is longer than MAXLINE, as fgets might read a partial line if it reaches the maximum character limit.*/

    // Remove the newline character from line2
    size_t len2 = strlen(line2);
    if (len2 > 0 && line2[len2 - 1] == '\n') {
        line2[len2 - 1] = '\0';
    }
    
    while (fgets(line1, MAXLINE, file1) != NULL) {
    
    // Remove the newline character from line1
    size_t len1 = strlen(line1);
        if (len1 > 0 && line1[len1 - 1] == '\n') {
            line1[len1 - 1] = '\0';
        }
        lineNumber++;    
                           
        if (strstr(line1, line2) !=NULL) {
            printf("pattern found at line %d:\n", lineNumber);
            printf("File 1: %s", line1);
            printf("File 2: %s", line2);
            return; 
        }
    }

    if (feof(file1) && feof(file2)) {
        printf("Files are identical.\n");
    } else {
        printf("Files have different lengths.\n");
    }
}
