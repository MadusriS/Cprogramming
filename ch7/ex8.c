#include <stdio.h>

void printFile(FILE *file, const char *filename, int *pageCount);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s file1 file2 ...\n", argv[0]);
        return 1;
    }

    int pageCount = 0;

    for (int i = 1; i < argc; i++) {
        FILE *file = fopen(argv[i], "r");

        if (file == NULL) {
            printf("Unable to open file: %s\n", argv[i]);
            continue;
        }

        printFile(file, argv[i], &pageCount);

        fclose(file);
        pageCount++; // Increment page count for the next file
    }

    return 0;
}

void printFile(FILE *file, const char *filename, int *pageCount) {
    printf("\n=== Page %d: %s ===\n", *pageCount + 1, filename);

    char line[100];
    int lineCount = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
        lineCount++;

        // Start a new page after a certain number of lines 
        if (lineCount == 20) {
            lineCount = 0;
            (*pageCount)++;
            printf("\n=== Page %d: %s ===\n", *pageCount + 1, filename);
        }
    }

    printf("\n"); }

