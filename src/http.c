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

#include "http.h"

int httpConnect(int socket, char **argv){
  struct connection connection;
  int receiving, size;
  char *base;

  printf("new http connection\n");
  connection.buffer.size = HTTP_BUFFER_SIZE;
  connection.buffer.base = malloc(connection.buffer.size);
  connection.buffer.offset = 0;
  connection.buffer.n = 0;
  connection.socket = socket;
  connection.argv = argv;
  connection.quit = 0;
  connection.close = 0;

  while(!connection.close && !connection.quit){

    /* Parse HTTP header. */
    connection.header = headerMake();
    while(connection.header.status == incomplete){

      /* Parse. */
      base = connection.buffer.base + connection.buffer.offset;
      size = connection.buffer.n - connection.buffer.offset;
      headerParse(&connection.header, base, size);

      if(connection.header.status != incomplete) break;

      /* Buffer not full. */
      if(connection.buffer.n != connection.buffer.size){
        base = connection.buffer.base + connection.buffer.n;
        size = connection.buffer.size - connection.buffer.n;
        printf("start receiving\n");
        receiving = recv(socket, base, size, 0);
        printf("%.*send receiving\n", receiving, base);
        if(receiving == 0){
          printf("end http connection\n");
          connection.close = 1;
          free(connection.buffer.base);
          return connection.quit;
        }
        if(receiving == -1){
          printf("end http connection\n");
          fprintf(stderr, "Error %s receiving data.\n", strerror(errno));
          connection.close = 1;
          free(connection.buffer.base);
          return connection.quit;
        }
        connection.buffer.n = connection.buffer.n + receiving;
        continue;
      }

      /* Make room resetting offset. */
      if(connection.buffer.offset){

        base = connection.buffer.base + connection.buffer.offset;
        size = connection.buffer.n - connection.buffer.offset;
        memmove(connection.buffer.base, base, size);
        connection.buffer.n = connection.buffer.n - connection.buffer.offset;
        connection.buffer.offset = 0;
        continue;
      }

      /* Buffer not big enough. */
      size = connection.buffer.size = connection.buffer.size * 2;
      connection.buffer.base = realloc(connection.buffer.base, size);
      if(connection.buffer.base == NULL){
        printf("end http connection\n");
        fprintf(stderr, "Not enough memory for HTTP header.\n");
        free(connection.buffer.base);
        printf("http close error\n");
        return -1;
      }
    }

    //headerPrint(connection.header);
    connection.buffer.offset = connection.buffer.offset + connection.header.i;
    requestExecute(&connection);
  }

  printf("end http connection\n");
  free(connection.buffer.base);
  return connection.quit;
}
