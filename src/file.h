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

#ifndef FILE_H
#define FILE_H

#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#endif

#define FILE_BUFFER_SIZE 1024

#ifdef _WIN32
#define FILE_SEPARATOR '\\'
#else
#define FILE_SEPARATOR '/'
#endif

long fileSize(FILE *file);
void fileBuffer(FILE *file, char **buffer, long *size);
int fileSendRange(FILE *file, int socket, int start, int end);
int fileSend(FILE *file, int socket);
int fileMove(char *old, char *fresh);
int fileRemove(char *path);

int fileFolder(char *path);
void fileFolderBuffer(char *path, char **base, int *size);
int fileFolderRemove(char *path);
int fileFolderSet(char *path);

void fileOpenPath(char *path);

#endif
