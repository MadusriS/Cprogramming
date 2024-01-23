#include <stdio.h>
#include<string.h>
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

    while (fgets(line1, MAXLINE, file1) != NULL && fgets(line2, MAXLINE, file2) != NULL) {
        lineNumber++;

        if (strcmp(line1, line2) != 0) {
            printf("Files differ at line %d:\n", lineNumber);
            printf("File 1: %s", line1);
            printf("File 2: %s", line2);
            return; // Exit the function on the first difference found
        }
    }

    if (feof(file1) && feof(file2)) {
        printf("Files are identical.\n");
    } else {
        printf("Files have different lengths.\n");
    }
}
