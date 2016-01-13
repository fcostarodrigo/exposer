#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/date.h"

int main(void) {
    char string[1024];

    time_t source = 784111777l;
    time_t target;

    int number, i;

    printf("date test> Testing seconds parse.\n");
    assert(dateSeconds(NULL, &number) == 0);
    assert(dateSeconds(strcpy(string, "a"), &number) == 0);
    assert(dateSeconds(strcpy(string, "-1"), &number) == 0);
    assert(dateSeconds(strcpy(string, "0"), &number) == 1);
    assert(dateSeconds(strcpy(string, "60"), &number) == 0);
    assert(dateSeconds(strcpy(string, "10"), &number) == 1);
    assert(number == 10);

    printf("date test> Testing minutes parse.\n");
    assert(dateMinutes(NULL, &number) == 0);
    assert(dateMinutes(strcpy(string, "a"), &number) == 0);
    assert(dateMinutes(strcpy(string, "-1"), &number) == 0);
    assert(dateMinutes(strcpy(string, "0"), &number) == 1);
    assert(dateMinutes(strcpy(string, "60"), &number) == 0);
    assert(dateMinutes(strcpy(string, "10"), &number) == 1);
    assert(number == 10);

    printf("date test> Testing hours parse.\n");
    assert(dateHours(NULL, &number) == 0);
    assert(dateHours(strcpy(string, "a"), &number) == 0);
    assert(dateHours(strcpy(string, "-1"), &number) == 0);
    assert(dateHours(strcpy(string, "0"), &number) == 1);
    assert(dateHours(strcpy(string, "24"), &number) == 0);
    assert(dateHours(strcpy(string, "10"), &number) == 1);
    assert(number == 10);

    printf("date test> Testing day parse.\n");
    assert(dateDay(NULL, &number) == 0);
    assert(dateDay(strcpy(string, "a"), &number) == 0);
    assert(dateDay(strcpy(string, "-1"), &number) == 0);
    assert(dateDay(strcpy(string, "0"), &number) == 0);
    assert(dateDay(strcpy(string, "32"), &number) == 0);
    assert(dateDay(strcpy(string, "10"), &number) == 1);
    assert(number == 10);

    printf("date test> Testing week parse.\n");
    assert(dateWeek(NULL, &number) == 0);
    assert(dateWeek(strcpy(string, "a"), &number) == 0);
    for (i = 0; dateWeeks[i]; ++i) {
        assert(dateWeek(dateWeeks[i], &number) == 1);
        assert(number == i);
    }
    for (i = 0; dateWeeksShort[i]; ++i) {
        assert(dateWeek(dateWeeksShort[i], &number) == 1);
        assert(number == i);
    }

    printf("date test> Testing month parse.\n");
    assert(dateMonth(NULL, &number) == 0);
    assert(dateMonth(strcpy(string, "a"), &number) == 0);
    for (i = 0; dateMonths[i]; ++i) {
        assert(dateMonth(dateMonths[i], &number) == 1);
        assert(number == i);
    }
    for (i = 0; dateMonthsShort[i]; ++i) {
        assert(dateMonth(dateMonthsShort[i], &number) == 1);
        assert(number == i);
    }

    printf("date test> Testing year parse.\n");
    assert(dateYear(NULL, &number) == 0);
    assert(dateYear(strcpy(string, "a"), &number) == 0);
    assert(dateYear(strcpy(string, "-1"), &number) == 1);
    assert(dateYear(strcpy(string, "0"), &number) == 1);
    assert(dateYear(strcpy(string, "2016"), &number) == 1);
    assert(number == 2016);

    printf("date test> Testing date parse.\n");
    assert(dateParse(NULL, &target) == 0);
    assert(dateParse(strcpy(string, "a"), &target) == 0);

    strcpy(string, "Sun, 06 Nov 1994 08:49:37 GMT");
    assert(dateParse(string, &target) == 1);
    assert(source == target);

    strcpy(string, "Sunday, 06-Nov-94 08:49:37 GMT");
    assert(dateParse(string, &target) == 1);
    assert(source == target);

    strcpy(string, "Sun Nov  6 08:49:37 1994");
    assert(dateParse(string, &target) == 1);
    assert(source == target);

    strcpy(string, "Sunn, 06 Nov 1994 08:49:37 GMT");
    assert(dateParse(string, &target) == 0);

    strcpy(string, "Sun, 0 Nov 1994 08:49:37 GMT");
    assert(dateParse(string, &target) == 0);

    strcpy(string, "Sun, 06 Nove 1994 08:49:37 GMT");
    assert(dateParse(string, &target) == 0);

    strcpy(string, "Sun, 06 Nov -1 08:49:37 GMT");
    assert(dateParse(string, &target) == 0);

    strcpy(string, "Sun, 06 Nov 1994 24:49:37 GMT");
    assert(dateParse(string, &target) == 0);

    strcpy(string, "Sun, 06 Nov 1994 08:60:37 GMT");
    assert(dateParse(string, &target) == 0);

    strcpy(string, "Sun, 06 Nov 1994 08:49:60 GMT");
    assert(dateParse(string, &target) == 0);

    strcpy(string, "Sun, 06 Nov 1994 08:49:60 GMTZ");
    assert(dateParse(string, &target) == 0);

    strcpy(string, "Sat, 06 Nov 1994 08:49:60 GMT");
    assert(dateParse(string, &target) == 0);

    strcpy(string, "Tue, 31 Nov 2015 08:49:60 GMT");
    assert(dateParse(string, &target) == 0);

    return 0;
}
