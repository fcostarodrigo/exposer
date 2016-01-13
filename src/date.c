/*
    Copyright 2015 Rodrigo Fernandes da Costa

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Author: Rodrigo Fernandes da Costa

    E-mail: rodrigo5244@gmail.com
*/

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>

#include "date.h"

#ifndef DEBUG
#define DEBUG 0
#endif

char *dateWeeks[] = {
    "sunday", "monday", "tuesday", "wednesday",
    "thursday", "friday", "saturday", NULL
};

char *dateWeeksShort[] = {
    "sun", "mon", "tue", "wed", "thu", "fri", "sat", NULL
};

char *dateMonths[] = {
    "january", "february", "march", "april", "may", "june",
    "july", "august", "september", "october", "november", "december", NULL
};

char *dateMonthsShort[] = {
    "jan", "feb", "mar", "apr", "may", "jun",
    "jul", "aug", "sep", "oct", "nov", "dec", NULL
};

int dateSeconds(char *string, int *number) {
    char *left;

    if (string == NULL) {
        return 0;
    }

    number[0] = strtol(string, &left, 10);

    if (left == string || number[0] < 0 || number[0] > 59) {
        return 0;
    }

    return 1;
}

int dateMinutes(char *string, int *number) {
    char *left;

    if (string == NULL) {
        return 0;
    }

    number[0] = strtol(string, &left, 10);

    if (left == string || number[0] < 0 || number[0] > 59) {
        return 0;
    }

    return 1;
}

int dateHours(char *string, int *number) {
    char *left;

    if (string == NULL) {
        return 0;
    }

    number[0] = strtol(string, &left, 10);

    if (left == string || number[0] < 0 || number[0] > 23) {
        return 0;
    }

    return 1;
}

int dateDay(char *string, int *number) {
    char *left;

    if (string == NULL) {
        return 0;
    }

    number[0] = strtol(string, &left, 10);

    if (left == string || number[0] < 1 || number[0] > 31) {
        return 0;
    }

    return 1;
}

int dateWeek(char *string, int *number) {
    int i;

    if (string == NULL) {
        return 0;
    }

    for (i = 0; dateWeeks[i]; ++i) {
        if (strcmp(string, dateWeeks[i]) == 0) {
            number[0] = i;
            return 1;
        }
    }

    for (i = 0; dateWeeksShort[i]; ++i) {
        if (strcmp(string, dateWeeksShort[i]) == 0) {
            number[0] = i;
            return 1;
        }
    }

    return 0;
}

int dateMonth(char *string, int *number) {
    int i;

    if (string == NULL) {
        return 0;
    }

    for (i = 0; dateMonths[i]; ++i) {
        if (strcmp(string, dateMonths[i]) == 0) {
            number[0] = i;
            return 1;
        }
    }

    for (i = 0; dateMonthsShort[i]; ++i) {
        if (strcmp(string, dateMonthsShort[i]) == 0) {
            number[0] = i;
            return 1;
        }
    }

    return 0;
}

int dateYear(char *string, int *number) {
    char *left;

    if (string == NULL) {
        return 0;
    }

    number[0] = strtol(string, &left, 10);

    if (left == string) {
        return 0;
    }

    return 1;
}

int dateZone(char *string, int *number) {
    if (string == NULL) {
        return 0;
    }

    if (strcmp(string, "GMT") == 0) {
        number[0] = 0;
        return 1;
    }

    return 1;
}

int dateLeapYear(int year) {
    if (year % 4 == 0) {
        if (year % 100 == 0) {
            if (year % 400 == 0) {
                return 1;
            }
            return 0;
        }
        return 1;
    }
    return 0;
}

int dateDayCheck(int day, int month, int year) {
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    days[1] = days[1] + dateLeapYear(year);

    return (day <= days[month]);
}

int dateWeekCheck(int day, int week, int month, int year) {
    int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    int calculated;

    month = month + 1;

    if (month < 3) {
        year = year - 1;
    }

    calculated = (year + year/4 - year/100 + year/400 + t[month-1] + day) % 7;

    return calculated == week;
}

time_t dateFixTimeZone(struct tm meta) {
    time_t temp;

    temp = mktime(&meta);
    meta.tm_hour = meta.tm_hour - ((gmtime(&temp))->tm_hour - meta.tm_hour);
    temp = mktime(&meta);

    if (DEBUG) {
        printf("date> Parsed as %ld.\n", temp);
    }

    return temp;
}

int dateParse(char *string, time_t *timestamp) {
    const char delimiters[] = " :-,";
    char *token;
    struct tm meta;
    int error;
    int i;

    struct {
        int seconds, minutes, hours, day, week, month, year, zone;
    } date;

    if (string == NULL) {
        if (DEBUG) {
            printf("date> Parse with invalid string.\n");
        }
        return 0;
    }

    if (DEBUG) {
        printf("date> Parsing \"%s\".\n", string);
    }

    for (i = 0; string[i]; ++i) {
        string[i] = tolower(string[i]);
    }

    error = 0;

    if (!error && !dateWeek(strtok(string, delimiters), &date.week)) {
        error = 1;
    }

    token = strtok(NULL, delimiters);

    if (!error && dateDay(token, &date.day)) {
        if (!error && !dateMonth(strtok(NULL, delimiters), &date.month)) {
            error = 1;
        }
        if (!error && !dateYear(strtok(NULL, delimiters), &date.year)) {
            error = 1;
        }
        if (!error && !dateHours(strtok(NULL, delimiters), &date.hours)) {
            error = 1;
        }
        if (!error && !dateMinutes(strtok(NULL, delimiters), &date.minutes)) {
            error = 1;
        }
        if (!error && !dateSeconds(strtok(NULL, delimiters), &date.seconds)) {
            error = 1;
        }
        if (!error && !dateZone(strtok(NULL, delimiters), &date.zone)) {
            error = 1;
        }
    } else {
        if (!error && !dateMonth(token, &date.month)) {
            error = 1;
        }
        if (!error && !dateDay(strtok(NULL, delimiters), &date.day)) {
            error = 1;
        }
        if (!error && !dateHours(strtok(NULL, delimiters), &date.hours)) {
            error = 1;
        }
        if (!error && !dateMinutes(strtok(NULL, delimiters), &date.minutes)) {
            error = 1;
        }
        if (!error && !dateSeconds(strtok(NULL, delimiters), &date.seconds)) {
            error = 1;
        }
        if (!error && !dateYear(strtok(NULL, delimiters), &date.year)) {
            error = 1;
        }
    }

    if (!error && date.year >= 70 && date.year < 100) {
        date.year = date.year + 1900;
    }

    if (!error && !dateDayCheck(date.day, date.month, date.year)) {
        error = 1;
    }

    if (!error && !dateWeekCheck(date.day, date.week, date.month, date.year)) {
        error = 1;
    }

    if (error) {
        if (DEBUG) {
            printf("date> Could not parse date.\n");
        }
        return 0;
    }

    if (date.year >= 100) {
        date.year = date.year - 1900;
    }

    meta.tm_sec = date.seconds;
    meta.tm_min = date.minutes;
    meta.tm_hour = date.hours;
    meta.tm_mday = date.day;
    meta.tm_mon = date.month;
    meta.tm_year = date.year;
    meta.tm_wday = date.week;

    timestamp[0] = dateFixTimeZone(meta);

    return 1;
}
