#include <stdio.h>

char daytab[2][13] = {
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

int isleapyr(int year);

int dayoftheyr(int year, int month, int day);

void monthday(int year, int yearday, int *month, int *day);

int main(void) {
    int yearday, month, day;

    yearday = dayoftheyr(1988, 2, 29);
    if (yearday != -1)
        printf("year day of 1988, 2, 29 is %d\n", yearday);
    else
        printf(" year day of 1988, 2, 29 is Invalid date\n");

    yearday = dayoftheyr(2020, 2, 31);
    if (yearday != -1)
        printf("year day of 2020, 2, 31%d\n", yearday);
    else
        printf("year day of 2020, 2, 31 Invalid date\n");

    monthday(1988, 60, &month, &day);
    if (month != -1 && day != -1)
        printf(" the month and day of 1988, 60 is   Month: %d, Day: %d\n", month, day);
    else
        printf(" the month and day of 1988, 60 is   Invalid date\n");

    monthday(1999, 366, &month, &day);
    if (month != -1 && day != -1)
        printf(" the month and day of 1999, 366 is   Month: %d, Day: %d\n", month, day);
    else
        printf("the month and day of 1999, 366 Invalid date\n");

    return 0;
}

int isleapyr(int year) {
    return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

int dayoftheyr(int year, int month, int day) {
    if (year < 1 || month < 1 || month > 12 || day < 1 || day > daytab[isleapyr(year)][month])
        return -1;

    int leap = isleapyr(year);
    char *p;
    p = &daytab[leap][1];
    month = month - 1;

    while (month >= 1) {
        day += *p;
        ++p;
        --month;
    }
    return day;
}

void monthday(int year, int yearday, int *month, int *day) {
    if (year < 1 || yearday < 1 || yearday > (isleapyr(year) ? 366 : 365)) {
        *month = -1;
        *day = -1;
        return;
    }

    char *p;
    int leap = isleapyr(year);
    p = &daytab[leap][1];

    while (yearday > *++p) {
        yearday -= *p;
    }

    *month = p - daytab[leap];
    *day = yearday;
    return;
}

