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

#include "range.h"

void rangePrint(struct range range){
  printf("Range\n");
  printf("string = %s\n", range.string);
  printf("next = %s\n", range.next);
  printf("start = %d\n", range.start);
  printf("end = %d\n", range.end);
  printf("size = %d\n", range.size);
  switch(range.status){
    case unrecognized:
      printf("status = %s\n", "unrecognized");
      break;

    case unsatisfiable:
      printf("status = %s\n", "unsatisfiable");
      break;

    case statusOk:
      printf("status = %s\n", "ok");
      break;
  }
  printf("\n");
}

struct range rangeMake(char *string, int size){
  struct range range;

  range.string = string;
  range.next = NULL;
  range.start = 0;
  range.end = 0;
  range.size = size;
  range.status = unrecognized;

  return range;
}

void rangeParse(struct range *range){
  enum rangeParseState state = stateBegin;
  int sufixSize;
  int i = 0;

  if(range->next){
    range->string = range->next;
    range->next = NULL;
    state = stateRange;
  }

  range->status = statusOk;

  while(state != stateDone){
    switch(state){
      case stateBegin:
        switch(range->string[i]){
          case '\0':
            range->status = unrecognized;
            state = stateDone;
            break;

          case ' ': case '\t':
            i = i + 1;
            break;

          default:
            state = stateUnit;
        }
        break;

      case stateUnit:
        if(strlen(range->string + i) < strlen("bytes")){
          range->status = unrecognized;
          state = stateDone;
        }
        else if(memcmp("bytes", range->string + i, strlen("bytes")) != 0){
          range->status = unrecognized;
          state = stateDone;
        }
        else{
          state = stateEqual;
        }
        break;

      case stateEqual:
        switch(range->string[i]){
          case '\0':
            range->status = unrecognized;
            state = stateDone;
            break;

          case '=':
            i = i + 1;
            state = stateRange;
            break;

          default:
            i = i + 1;
        }
        break;

      case stateRange:
        switch(range->string[i]){
          case '\0':
            range->status = unrecognized;
            state = stateDone;
            break;

          case '-':
            i = i + 1;
            state = stateSufix;
            break;

          case '1': case '2': case '3':
          case '4': case '5': case '6':
          case '7': case '8': case '9': case '0':
            state = stateLeft;
            break;

          default:
            i = i + 1;
        }
        break;

      case stateSufix:
        switch(range->string[i]){
          case '\0':
            range->status = unrecognized;
            state = stateDone;
            break;

          case '1': case '2': case '3':
          case '4': case '5': case '6':
          case '7': case '8': case '9': case '0':
            sscanf(range->string + i, "%d", &sufixSize);
            if(!sufixSize) range->status = unsatisfiable;
            range->start = range->size - sufixSize;
            range->end = range->size;
            state = stateEnd;
            break;

          default:
            i = i + 1;
        }
        break;

      case stateEnd:
        switch(range->string[i]){
          case '\0':
            state = stateDone;
            break;

          case ',':
            range->next = range->string + i + 1;
            state = stateDone;
            break;

          default:
            i = i + 1;
        }
        break;

      case stateLeft:
        sscanf(range->string + i, "%d", &(range->start));
        if(range->start >= range->size) range->status = unsatisfiable;
        state = stateDash;
        break;

      case stateDash:
        switch(range->string[i]){
          case '\0':
            state = stateDone;
            break;

          case '-':
            i = i + 1;
            state = stateRight;
            break;

          default:
            i = i + 1;
        }
        break;

      case stateRight:
        switch(range->string[i]){
          case '1': case '2': case '3':
          case '4': case '5': case '6':
          case '7': case '8': case '9': case '0':
            sscanf(range->string + i, "%d", &(range->end));
            range->end = range->end + 1;
            if(range->end > range->size) range->end = range->size;
            state = stateEnd;
            break;

          case '\0':
            range->end = range->size;
            state = stateDone;
            break;

          case ',':
            range->next = range->string + i + 1;
            range->end = range->size;
            state = stateDone;
            break;

          default:
            i = i + 1;
        }
        break;

      case stateDone:
        break;
    }
  }
}
