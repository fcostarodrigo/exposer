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

#ifndef DATE_H
#define DATE_H

#include <time.h>

extern char *dateWeeks[];
extern char *dateWeeksShort[];
extern char *dateMonths[];
extern char *dateMonthsShort[];

int dateSeconds(char *string, int *number);
int dateMinutes(char *string, int *number);
int dateHours(char *string, int *number);
int dateDay(char *string, int *number);
int dateMonth(char *string, int *number);
int dateYear(char *string, int *number);
int dateZone(char *string, int *number);
int dateLeapYear(int year);
int dateDayCheck(int day, int month, int year);
int dateWeekCheck(int day, int week, int month, int year);
time_t dateFixTimeZone(struct tm meta);
int dateParse(char *string, time_t *timestamp);

#endif
