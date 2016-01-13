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

#include "request.h"

void requestExecute(struct connection *connection){
  char *attribute, *slash, *path;
  struct header header;
  int size;

  header = connection->header;

  if(header.status == invalid){
    responseBadRequest(connection[0], headerErrorString(header.error));
    return;
  }

  if(strcmp(header.version, "HTTP/1.1") == 0){
    if(header.attributes[ATTRIBUTES_HOST] == NULL){
      responseBadRequest(connection[0], "Missing host.");
      return;
    }
  }

  attribute = header.attributes[ATTRIBUTES_CONNECTION];
  if(attribute && strcmp(attribute, "close") == 0){
    connection->close = 1;
  }

  if(strcmp(header.version, "HTTP/1.0") == 0){
    connection->close = 1;
  }

  if(connection->header.path[0] == '\0'){
    connection->header.path = "index.html";
    header = connection->header;
  }

  if(libPrefix("extern", connection->header.path)){
    responseExtern(connection);
    return;
  }

  if(strcmp("folder/get", connection->header.path) == 0){
    responseFolderGet(connection[0]);
    connection->close = 1;
    return;
  }

  if(strcmp("folder/set", connection->header.path) == 0){
    responseFolderSet(connection[0]);
    connection->close = 1;
    return;
  }

  if(strcmp("folder/remove", connection->header.path) == 0){
    responseFolderRemove(connection[0]);
    connection->close = 1;
    return;
  }

  if(strcmp("folder/rename", connection->header.path) == 0){
    responseFolderRename(connection[0]);
    connection->close = 1;
    return;
  }

  if(strcmp("file/get", connection->header.path) == 0){
    responseFileGet(connection[0]);
    connection->close = 1;
    return;
  }

  if(strcmp("file/set", connection->header.path) == 0){
    responseFileSet(connection);
    connection->close = 1;
    return;
  }

  if(strcmp("file/add", connection->header.path) == 0){
    responseFileAdd(connection);
    connection->close = 1;
    return;
  }

  if(strcmp("file/remove", connection->header.path) == 0){
    responseFileRemove(connection[0]);
    connection->close = 1;
    return;
  }

  if(strcmp("file/rename", connection->header.path) == 0){
    responseFileRename(connection[0]);
    connection->close = 1;
    return;
  }

  if(strcmp("file/open", connection->header.path) == 0){
    responseFileOpen(connection[0]);
    connection->close = 1;
    return;
  }

  if(strcmp("shell", connection->header.path) == 0){
    responseShell(connection[0]);
    connection->close = 1;
    return;
  }

  if(strcmp("quit", connection->header.path) == 0){
    connection->close = 1;
    connection->quit = 1;
    responseQuit(connection[0]);
    return;
  }

  if(strcmp("argv", connection->header.path) == 0){
    responseArgv(connection[0]);
    return;
  }

  path = NULL;
  if((slash = strrchr(connection->argv[0], FILE_SEPARATOR))){
    size = strlen(header.path) + strlen(connection->argv[0]);
    path = malloc(size);
    if(!path){
      responseServerError(connection[0], "Error allocating memory.");
      return;
    }
    size = (int)(slash - connection->argv[0]);
    sprintf(path, "%.*s/%s", size, connection->argv[0], header.path);
    connection->header.path = path;
  }

  if(strcmp("GET", header.method) == 0){
    responseFile(connection[0]);
    if(path) free(path);
    return;
  }

  if(strcmp("HEAD", header.method) == 0){
    responseFile(connection[0]);
    if(path) free(path);
    return;
  }

  if(strcmp("POST", header.method) == 0){
    responseFile(connection[0]);
    if(path) free(path);
    return;
  }

  responseNotImplemented(connection);
  printf("Method %s unrecognized.\n", connection->header.method);
  if(path) free(path);
}
