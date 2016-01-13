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

#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "attributes.h"
#include "url.h"

#define HEADER_ERROR_BUFFER_SIZE 47

enum headerParserState{
  start,
  methodBegin,
  methodEnd,
  pathBegin0,
  pathHost0,
  pathHost1,
  pathHost2,
  pathBegin1,
  pathRoot0,
  pathRoot1,
  pathRoot2,
  pathEnd,
  versionBegin,
  versionEnd,
  versionEndStandard0,
  versionEndStandard1,
  versionEndStandard2,
  versionEndNonStandard,
  attributeNameBegin,
  attributeNameEnd,
  attributeValueBegin,
  attributeValueEnd,
  attributeValueEndStandard0,
  attributeValueEndStandard1,
  attributeValueEndStandard2,
  attributeValueEndNonStandard,
  end,
  endStandard0,
  endStandard1,
  endStandard2,
  endNonStandard
};

enum headerStatus{
  incomplete,
  invalid,
  ok
};

struct header{
  int i;

  enum headerStatus status;
  enum headerParserState state;
  enum headerParserState error;

  char *buffer;
  char *method, *path, *version;
  char *attributes[ATTRIBUTES_N];

  struct{char *name; int i;} attribute;
};

void headerCopy(struct header header, char **buffer, int *size);
void headerParse(struct header *header, char *buffer, int size);
void headerSwapBuffer(struct header *header, char *buffer);
char *headerErrorString(enum headerParserState error);
void headerPrint(struct header header);
struct header headerMake();

#endif

