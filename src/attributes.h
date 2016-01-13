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

#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#define ATTRIBUTES_ACCEPT 0
#define ATTRIBUTES_ACCEPT_CHARSET 1
#define ATTRIBUTES_ACCEPT_DATETIME 2
#define ATTRIBUTES_ACCEPT_ENCODING 3
#define ATTRIBUTES_ACCEPT_LANGUAGE 4
#define ATTRIBUTES_ARG0 5
#define ATTRIBUTES_ARG1 6
#define ATTRIBUTES_ARG2 7
#define ATTRIBUTES_AUTHORIZATION 8
#define ATTRIBUTES_CACHE_CONTROL 9
#define ATTRIBUTES_CONNECTION 10
#define ATTRIBUTES_CONTENT_LENGTH 11
#define ATTRIBUTES_CONTENT_MD5 12
#define ATTRIBUTES_CONTENT_TYPE 13
#define ATTRIBUTES_COOKIE 14
#define ATTRIBUTES_DATE 15
#define ATTRIBUTES_DNT 16
#define ATTRIBUTES_EXPECT 17
#define ATTRIBUTES_FROM 18
#define ATTRIBUTES_HOST 19
#define ATTRIBUTES_IF_MATCH 20
#define ATTRIBUTES_IF_MODIFIED_SINCE 21
#define ATTRIBUTES_IF_NONE_MATCH 22
#define ATTRIBUTES_IF_RANGE 23
#define ATTRIBUTES_IF_UNMODIFIED_SINCE 24
#define ATTRIBUTES_MAX_FORWARDS 25
#define ATTRIBUTES_ORIGIN 26
#define ATTRIBUTES_PRAGMA 27
#define ATTRIBUTES_PROXY_AUTHORIZATION 28
#define ATTRIBUTES_RANGE 29
#define ATTRIBUTES_REFERER 30
#define ATTRIBUTES_TRANSFER_ENCODING 31
#define ATTRIBUTES_UPGRADE 32
#define ATTRIBUTES_USER_AGENT 33
#define ATTRIBUTES_VIA 34
#define ATTRIBUTES_WARNING 35
#define ATTRIBUTES_N 36

extern char *attributes[];

int attributesCompare(const void *first, const void *second);
int attributesIndex(char *attribute);

#endif
