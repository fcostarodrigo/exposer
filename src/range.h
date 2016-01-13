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

#ifndef RANGE_H
#define RANGE_H

#include <stdio.h>
#include <string.h>

enum rangeParseState{
  stateBegin,
  stateUnit,
  stateEqual,
  stateRange,
  stateSufix,
  stateEnd,
  stateLeft,
  stateDash,
  stateRight,
  stateDone
};

struct range{
  char *string, *next;
  int start, end, size;
  enum{unrecognized, unsatisfiable, statusOk} status;
};

void rangePrint(struct range range);
struct range rangeMake(char *string, int size);
void rangeParse(struct range *range);

#endif
