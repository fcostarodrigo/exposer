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

#include "url.h"

void URLDecode(char *url){
  char string[] = "0123456789abcdef";
  char *left, *right;
  int i;

  for(i = 0; url[i] && url[i + 1] && url[i + 2]; ++i){
    if(url[i] != '%') continue;
    if(!(left = strchr(string, tolower(url[i + 1])))) continue;
    if(!(right = strchr(string, tolower(url[i + 2])))) continue;

    url[i] = 16 * (left - string) + (right - string);
    memmove(&url[i + 1], &url[i + 3], strlen(&url[i + 3]) + 1);
  }
}
