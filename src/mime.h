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

#ifndef MIME_H
#define MIME_H

#include <string.h>
#include <stdlib.h>

#define MIME_N 538

extern char *mimeExentions[];
extern char *mimeTypes[];

int mimeCompare(const void *first, const void *second);
char *mime(char *path);

#endif
