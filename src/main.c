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

#ifdef _WIN32

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#include <winsock2.h>
#include <windows.h>
#endif

#include "network.h"

int main(int argc, char *argv[]){
  int status;

#ifdef _WIN32
  WSADATA wsaData;

  if(WSAStartup(MAKEWORD(1,1), &wsaData) != 0){
    fprintf(stderr, "WSAStartup failed.\n");
    return 1;
  }
#endif

  status = networkServe(argv);

#ifdef _WIN32
  WSACleanup();
#endif

  if(!status){
    fprintf(stderr, "Error serving.\n");
    return 1;
  }

  return 0;
}
