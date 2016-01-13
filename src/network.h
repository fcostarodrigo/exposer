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

#ifndef MY_SOCKET_H
#define MY_SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#endif

#include "http.h"
#include "browser.h"

#define NETWORK_QUEUE 20
#define NETWORK_CHECK_DELAY 5

struct networkThread{
  int *stop;
  int socket;
  char **argv;
};

int networkServe(char **argv);
void networkMakeSocket(int *socket, char *portString);
struct addrinfo *networkParseAddress(char *host, char *service);
int networkOpenSocket(struct addrinfo *address);
int networkGetPort(int socket, struct sockaddr *address, size_t size);

#ifdef _WIN32
DWORD WINAPI networkServeOne(LPVOID lpParam);
#endif

#endif
