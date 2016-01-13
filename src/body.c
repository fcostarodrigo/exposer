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

#include "body.h"

void bodyPass(struct connection *connection, int socket, FILE *file){
  char *size, *attribute;
  int length;

  attribute = connection->header.attributes[ATTRIBUTES_TRANSFER_ENCODING];
  if(attribute && strcmp(attribute, "chunked") == 0){
    while((length = bodyHeader(connection))){
      if(length == -1){
        fprintf(stderr, "Error reading chunked body.\n");
        return;
      }
      bodyWrite(connection, length, socket, file);
    }
    bodyFooter(connection);
  }
  else{
    size = connection->header.attributes[ATTRIBUTES_CONTENT_LENGTH];
    if(size == NULL) return;
    bodyWrite(connection, atoi(size), socket, file);
  }
}

void bodyWrite(struct connection *connection, int size, int socket, FILE *file){
  int bufferSize = connection->buffer.size;
  char *base = connection->buffer.base;
  int n;

  while(size){
    n = connection->buffer.n - connection->buffer.offset;

    if(size < n) n = size;
    if(file) fwrite(base + connection->buffer.offset, sizeof(char), n, file);
    if(socket) send(socket, base + connection->buffer.offset, n, 0);
    size = size - n;

    connection->buffer.offset = connection->buffer.offset + n;
    if(connection->buffer.n == connection->buffer.offset && size){
      connection->buffer.n = connection->buffer.offset = 0;
      connection->buffer.n = recv(connection->socket, base, bufferSize, 0);
      if(!connection->buffer.n){
        size = 0;
        connection->close = 1;
      }
    }
  }
}

void bodyFooter(struct connection *connection){
  int size = connection->buffer.size;
  int i = connection->buffer.offset;
  int socket = connection->socket;

  enum {
    start,
    standard0, standard1, standard2,
    nonStandard,
    done
  } state = start;

  while(state != done){

    if(i >= connection->buffer.n){
      connection->buffer.n = recv(socket, connection->buffer.base, size, 0);
      if(!connection->buffer.n){
        connection->close = 1;
        state = done;
      }
      i = 0;
    }

    switch(state){
      case start:
        switch(connection->buffer.base[i]){
          case '\n':
            state = nonStandard;
            i = i + 1;
            break;

          case '\r':
            state = standard0;
            i = i + 1;
            break;

          default:
            i = i + 1;
        }
        break;

      case nonStandard:
        switch(connection->buffer.base[i]){
          case '\n':
            state = done;
            i = i + 1;
            break;

          default:
            state = start;
        }
        break;

      case standard0:
        switch(connection->buffer.base[i]){
          case '\n':
            state = standard1;
            i = i + 1;
            break;

          default:
            state = start;
        }
        break;

      case standard1:
        switch(connection->buffer.base[i]){
          case '\r':
            state = standard2;
            i = i + 1;
            break;

          default:
            state = start;
        }
        break;

      case standard2:
        switch(connection->buffer.base[i]){
          case '\n':
            state = done;
            i = i + 1;
            break;

          default:
            state = start;
        }
        break;

      default:
        fprintf(stderr, "Error parsing body footer.\n");
    }
  }

  connection->buffer.offset = i;
}

int bodyHeader(struct connection *connection){
  enum {hex, comment, beforeBody, body} state = hex;
  int size = connection->buffer.size;
  int i = connection->buffer.offset;
  int socket = connection->socket;
  int length = 0;

  while(state != body){

    if(i >= connection->buffer.n){
      connection->buffer.n = recv(socket, connection->buffer.base, size, 0);
      if(!connection->buffer.n){
        connection->close = 1;
        state = body;
      }
      i = 0;
    }

    switch(state){
      case hex:
        switch(connection->buffer.base[i]){
          case '1': case '2': case '3':
          case '4': case '5': case '6':
          case '7': case '8': case '9': case '0':
            length = length * 16;
            length = length + (connection->buffer.base[i] - '0');
            i = i + 1;
            break;

          case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            length = length * 16;
            length = length + (connection->buffer.base[i] - 'a' + 10);
            i = i + 1;
            break;

          case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            length = length * 16;
            length = length + (connection->buffer.base[i] - 'A' + 10);
            i = i + 1;
            break;

          case '\r':
            state = beforeBody;
            i = i + 1;
            break;

          case '\n':
            state = body;
            i = i + 1;
            break;

          case ';':
            state = comment;
            i = i + 1;
            break;

          default:
            i = i + 1;
        }
        break;

      case beforeBody:
        switch(connection->buffer.base[i]){
          case '\n':
            state = body;
            i = i + 1;
            break;

          default:
            fprintf(stderr, "No \"\\n\" following \"\\r\" in chunked body.\n");
            i = i + 1;
        }
        break;

      case comment:
        switch(connection->buffer.base[i]){
          case '\r':
            state = beforeBody;
            i = i + 1;
            break;

          case '\n':
            state = body;
            i = i + 1;
            break;

          default:
            i = i + 1;
        }
        break;

      default:
        fprintf(stderr, "Error parsing body header.\n");
    }
  }

  connection->buffer.offset = i;
  return length;
}
