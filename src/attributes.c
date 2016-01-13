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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "attributes.h"

#ifndef DEBUG
#define DEBUG 0
#endif

char *attributes[] = {
    "accept",
    "accept-charset",
    "accept-datetime",
    "accept-encoding",
    "accept-language",
    "arg0",
    "arg1",
    "arg2",
    "authorization",
    "cache-control",
    "connection",
    "content-length",
    "content-md5",
    "content-type",
    "cookie",
    "date",
    "dnt",
    "expect",
    "from",
    "host",
    "if-match",
    "if-modified-since",
    "if-none-match",
    "if-range",
    "if-unmodified-since",
    "max-forwards",
    "origin",
    "pragma",
    "proxy-authorization",
    "range",
    "referer",
    "transfer-encoding",
    "upgrade",
    "user-agent",
    "via",
    "warning"
};

int attributesCompare(const void *first, const void *second) {
      char * const * firstCasted = first;
      char * const * secondCasted = second;

      return strcmp(firstCasted[0], secondCasted[0]);
}

int attributesIndex(char *attribute) {
    int size;
    char **key;

    size = sizeof(char *);
    key = &attribute;

    key = bsearch(key, attributes, ATTRIBUTES_N, size, attributesCompare); 

    if (key) {
        return key - attributes;
    }

    if (DEBUG) {
        printf("attributes> Attribute \"%s\" not found.\n", attribute);
    }

    return -1;
}
