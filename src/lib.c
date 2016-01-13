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

#include "lib.h"

int libIntegerLength(int x){
  if(x == 0){
    return 1;
  }

  if(x < 0){
    return floor(log10(abs(x))) + 2;
  }

  return floor(log10(x)) + 1;
}

int libPrefix(char *prefix, char *string){
  int i;
  for(i = 0; prefix[i] && prefix[i] == string[i]; ++i);
  return prefix[i] == '\0';
}