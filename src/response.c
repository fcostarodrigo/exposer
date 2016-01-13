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

#include "response.h"

void responseFailure(struct connection connection){
  char *date;
  int size;

  char string[
    RESPONSE_200_N +
    RESPONSE_SERVER_N +
    RESPONSE_CONNECTION_CLOSE_N +
    RESPONSE_NO_CACHE_N +
    RESPONSE_DATE_N +
    RESPONSE_ACCEPT_RANGES_N +
    RESPONSE_CONTENT_TYPE_N +
    RESPONSE_CONTENT_LENGTH_N +
    RESPONSE_CRLF_N +
    10
  ];

  char format[] =
    RESPONSE_200
    RESPONSE_SERVER
    RESPONSE_CONNECTION_CLOSE
    RESPONSE_NO_CACHE
    RESPONSE_DATE
    RESPONSE_ACCEPT_RANGES
    RESPONSE_CONTENT_TYPE_BUILD(text/plain)
    RESPONSE_CONTENT_LENGTH_BUILD(10)
    RESPONSE_CRLF
    "Failure!\r\n";

  date = responseDate();
  size = sprintf(string, format, date);
  responseBuffer(connection, string, size);
}

void responseSuccess(struct connection connection){
  char *date;
  int size;

  char string[
    RESPONSE_200_N +
    RESPONSE_SERVER_N +
    RESPONSE_CONNECTION_CLOSE_N +
    RESPONSE_NO_CACHE_N +
    RESPONSE_DATE_N +
    RESPONSE_ACCEPT_RANGES_N +
    RESPONSE_CONTENT_TYPE_N +
    RESPONSE_CONTENT_LENGTH_N +
    RESPONSE_CRLF_N +
    10
  ];

  char format[] =
    RESPONSE_200
    RESPONSE_SERVER
    RESPONSE_CONNECTION_CLOSE
    RESPONSE_NO_CACHE
    RESPONSE_DATE
    RESPONSE_ACCEPT_RANGES
    RESPONSE_CONTENT_TYPE_BUILD(text/plain)
    RESPONSE_CONTENT_LENGTH_BUILD(10)
    RESPONSE_CRLF
    "Success!\r\n";

  date = responseDate();
  size = sprintf(string, format, date);
  responseBuffer(connection, string, size);
}

void responseInteger(struct connection connection, int x){
  char *date;
  int size;

  char string[
    RESPONSE_200_N +
    RESPONSE_SERVER_N +
    RESPONSE_CONNECTION_CLOSE_N +
    RESPONSE_NO_CACHE_N +
    RESPONSE_DATE_N +
    RESPONSE_ACCEPT_RANGES_N +
    RESPONSE_CONTENT_TYPE_N +
    RESPONSE_CONTENT_LENGTH_N +
    RESPONSE_CRLF_N +
    RESPONSE_INT_N +
    RESPONSE_CRLF_N
  ];

  char format[] =
    RESPONSE_200
    RESPONSE_SERVER
    RESPONSE_CONNECTION_CLOSE
    RESPONSE_NO_CACHE
    RESPONSE_DATE
    RESPONSE_ACCEPT_RANGES
    RESPONSE_CONTENT_TYPE_BUILD(text/plain)
    RESPONSE_CONTENT_LENGTH
    RESPONSE_CRLF
    RESPONSE_INT
    RESPONSE_CRLF;

  date = responseDate();
  size = sprintf(string, format, date, libIntegerLength(x) + 2, x);
  responseBuffer(connection, string, size);
}

void responseExtern(struct connection *connection){
  char *bufferHeader, *attribute, *host, *slash;
  char buffer[RESPONSE_BUFFER_SIZE];
  struct addrinfo *address;
  int socket, size, load;

  host = connection->header.path + strlen("extern/");
  slash = strchr(host, '/');
  if(slash){
    slash[0] = '\0';
    slash = slash + 1;
  }
  else{
    slash = "";
  }

  address = networkParseAddress(host, "http");
  if(!address){
    responseServerError(connection[0], "Error parsing the host.");
    return;
  }

  socket = networkOpenSocket(address);
  if(socket == -1){
    responseServerError(connection[0], "Error creating socket.");
    freeaddrinfo(address);
    return;
  }

  if(connect(socket, address->ai_addr, address->ai_addrlen) == -1){
    responseServerError(connection[0], strerror(errno));
    freeaddrinfo(address);
    return;
  }

  freeaddrinfo(address);

  connection->header.path = slash;
  connection->header.attributes[ATTRIBUTES_CONNECTION] = "close";
  connection->header.attributes[ATTRIBUTES_HOST] = host;
  connection->header.attributes[ATTRIBUTES_REFERER] = host;
  headerCopy(connection->header, &bufferHeader, &size);

  printf("start sending to foreign host\n");
  send(socket, bufferHeader, size, 0);
  printf("%.*send sending to foreign host\n", size, bufferHeader);
  free(bufferHeader);

  bodyPass(connection, socket, 0);

  while((load = recv(socket, buffer, RESPONSE_BUFFER_SIZE, 0))){
    printf("start sending\n");
    send(connection->socket, buffer, load, 0);
    printf("%.*send sending\n", load, buffer);
  }

  close(socket);
}

void responseFolderGet(struct connection connection){
  char *path, *buffer, *date;
  int size, length;

  char string[
    RESPONSE_200_N +
    RESPONSE_SERVER_N +
    RESPONSE_CONNECTION_CLOSE_N +
    RESPONSE_NO_CACHE_N +
    RESPONSE_DATE_N +
    RESPONSE_ACCEPT_RANGES_N +
    RESPONSE_CONTENT_TYPE_N +
    RESPONSE_CONTENT_LENGTH_N +
    RESPONSE_CRLF_N
  ];

  char format[] = 
    RESPONSE_200
    RESPONSE_SERVER
    RESPONSE_CONNECTION_CLOSE
    RESPONSE_NO_CACHE
    RESPONSE_DATE
    RESPONSE_ACCEPT_RANGES
    RESPONSE_CONTENT_TYPE_BUILD(text/plain)
    RESPONSE_CONTENT_LENGTH
    RESPONSE_CRLF;

  path = connection.header.attributes[ATTRIBUTES_ARG0];
  if(!path){
    responseBadRequest(connection, "Path not provided.");
    return;
  }
  URLDecode(path);

  if(!fileFolder(path)){
    responseBadRequest(connection, "Not a folder.");
    return;
  }

  fileFolderBuffer(path, &buffer, &length);
  if(!buffer){
    responseServerError(connection, "Error creating folder buffer.");
    return;
  }

  date = responseDate();
  size = sprintf(string, format, date, length);
  responseBuffer(connection, string, size);

  responseBuffer(connection, buffer, length);

  free(buffer);
}

void responseFolderSet(struct connection connection){
  char *path, *slash;

  path = connection.header.attributes[ATTRIBUTES_ARG0];
  if(!path){
    responseBadRequest(connection, "No path provided.\n");
    return;
  }
  URLDecode(path);

  slash = strrchr(path, FILE_SEPARATOR);
  if(slash){
    slash[0] = '\0';
  }

  if(fileFolderSet(path)){
    responseSuccess(connection);
  }
  else{
    responseServerError(connection, strerror(errno));
  }
}

void responseFolderRemove(struct connection connection){
  char *path, *slash;

  path = connection.header.attributes[ATTRIBUTES_ARG0];
  if(!path){
    responseBadRequest(connection, "No path provided.\n");
    return;
  }
  URLDecode(path);

  slash = strrchr(path, FILE_SEPARATOR);
  if(slash){
    slash[0] = '\0';
  }

  if(fileFolderRemove(path)){
    responseSuccess(connection);
  }
  else{
    responseServerError(connection, strerror(errno));
  }
}

void responseFolderRename(struct connection connection){
  responseFileRename(connection);
}

void responseFile(struct connection connection){
  char *date, *type;
  FILE *file;
  int size;

  char string[
    RESPONSE_200_N +
    RESPONSE_SERVER_N +
    RESPONSE_NO_CACHE_N +
    RESPONSE_DATE_N +
    RESPONSE_ACCEPT_RANGES_N +
    RESPONSE_CONTENT_TYPE_N +
    RESPONSE_CONTENT_LENGTH_N +
    RESPONSE_CRLF_N
  ];

  char format[] =
    RESPONSE_200
    RESPONSE_SERVER
    RESPONSE_NO_CACHE
    RESPONSE_DATE
    RESPONSE_ACCEPT_RANGES
    RESPONSE_CONTENT_TYPE
    RESPONSE_CONTENT_LENGTH
    RESPONSE_CRLF;

  file = fopen(connection.header.path, "rb");
  if(!file){
    responseNotFound(connection);
    return;
  }

  if(!responsePartialFile(connection, file)){
    date = responseDate();
    type = responseContentType(connection);
    size = fileSize(file);
    size = sprintf(string, format, date, type, size);
    responseBuffer(connection, string, size);

    if(strcmp(connection.header.method, "HEAD") != 0){
      fileSend(file, connection.socket);
    }
  }


  fclose(file);
}

void responseFileGet(struct connection connection){
  char *path;

  path = connection.header.attributes[ATTRIBUTES_ARG0];
  if(!path){
    responseBadRequest(connection, "Path not provided.\n");
    return;
  }
  URLDecode(path);
  connection.header.path = path;

  responseFile(connection);
}

void responseFileSets(struct connection *connection, char *mode){
  char *attribute, *string, *path;
  FILE *file;
  int load;

  path = connection->header.attributes[ATTRIBUTES_ARG0];
  if(!path){
    responseBadRequest(connection[0], "Path not provided.\n");
    return;
  }
  URLDecode(path);

  if(strcmp("POST", connection->header.method) == 0){
    attribute = connection->header.attributes[ATTRIBUTES_CONTENT_LENGTH];
    if(!attribute){
      responseBadRequest(connection[0], "Content not provided.\n");
      return;
    }
  }
  else{
    string = connection->header.attributes[ATTRIBUTES_ARG0 + 1];
    if(!string){
      responseBadRequest(connection[0], "Content not provided.\n");
      return;
    }
    URLDecode(string);
  }

  fileOpenPath(path);
  file = fopen(path, mode);
  if(!file){
    responseNotFound(connection[0]);
    return;
  }

  if(strcmp("POST", connection->header.method) == 0){
    load = atoi(attribute);
    bodyWrite(connection, load, 0, file);
  }
  else{
    load = fwrite(string, sizeof(char), strlen(string), file);
  }

  responseInteger(connection[0], load);

  fclose(file);
}

void responseFileSet(struct connection *connection){
  responseFileSets(connection, "wb");
}

void responseFileAdd(struct connection *connection){
  responseFileSets(connection, "ab");
}

void responseFileRemove(struct connection connection){
  char *path;

  path = connection.header.attributes[ATTRIBUTES_ARG0];
  if(!path){
    responseBadRequest(connection, "No path provided.\n");
    return;
  }
  URLDecode(path);

  if(fileRemove(path)){
    responseSuccess(connection);
  }
  else{
    responseServerError(connection, strerror(errno));
  }
}

void responseFileRename(struct connection connection){
  char *fresh, *old;

  old = connection.header.attributes[ATTRIBUTES_ARG0];
  if(!old){
    responseBadRequest(connection, "Old path not provided.\n");
    return;
  }
  URLDecode(old);

  fresh = connection.header.attributes[ATTRIBUTES_ARG0 + 1];
  if(!fresh){
    responseBadRequest(connection, "New path not provided.\n");
    return;
  }
  URLDecode(fresh);

  if(fileMove(old, fresh)){
    responseSuccess(connection);
  }
  else{
    responseServerError(connection, strerror(errno));
  }
}

void responseFileOpen(struct connection connection){
  char *argument, *command;

  argument = connection.header.attributes[ATTRIBUTES_ARG0];
  if(!argument){
    responseBadRequest(connection, "Missing argument.");
    return;
  }
  URLDecode(argument);
  command = malloc(strlen(argument) + strlen(LIB_START) + 2);
  if(!command){
    responseServerError(connection, "Error allocating memory.");
    return;
  }

  if(!sprintf(command, "%s %s", LIB_START, argument)){
    responseServerError(connection, "Error building string.");
    free(command);
    return;
  }

  if(system(command)){
    responseFailure(connection);
  }
  else{
    responseSuccess(connection);
  }

  free(command);
}

void responseShell(struct connection connection){
  char *command;

  command = connection.header.attributes[ATTRIBUTES_ARG0];
  if(!command){
    responseBadRequest(connection, "Missing argument.");
    return;
  }
  URLDecode(command);

  responseInteger(connection, system(command));
}

void responseQuit(struct connection connection){
  char *date;
  int size;

  char string[
    RESPONSE_200_N +
    RESPONSE_SERVER_N +
    RESPONSE_CONNECTION_CLOSE_N +
    RESPONSE_NO_CACHE_N +
    RESPONSE_DATE_N +
    RESPONSE_ACCEPT_RANGES_N +
    RESPONSE_CONTENT_TYPE_N +
    RESPONSE_CONTENT_LENGTH_N +
    RESPONSE_CRLF_N +
    6 +
    RESPONSE_CRLF_N
  ];

  char format[] =
    RESPONSE_200
    RESPONSE_SERVER
    RESPONSE_CONNECTION_CLOSE
    RESPONSE_NO_CACHE
    RESPONSE_DATE
    RESPONSE_ACCEPT_RANGES
    RESPONSE_CONTENT_TYPE_BUILD(text/plain)
    RESPONSE_CONTENT_LENGTH_BUILD(6)
    RESPONSE_CRLF
    "Bye!"
    RESPONSE_CRLF;

  date = responseDate();
  size = sprintf(string, format, date);
  responseBuffer(connection, string, size);
}

void responseArgv(struct connection connection){
  int size, i, j, k, length;
  char *date, *buffer;

  char string[
    RESPONSE_200_N +
    RESPONSE_SERVER_N +
    RESPONSE_NO_CACHE_N +
    RESPONSE_DATE_N +
    RESPONSE_ACCEPT_RANGES_N +
    RESPONSE_CONTENT_TYPE_N +
    RESPONSE_CONTENT_LENGTH_N +
    RESPONSE_CRLF_N
  ];

  char format[] = 
    RESPONSE_200
    RESPONSE_SERVER
    RESPONSE_NO_CACHE
    RESPONSE_DATE
    RESPONSE_ACCEPT_RANGES
    RESPONSE_CONTENT_TYPE_BUILD(text/plain)
    RESPONSE_CONTENT_LENGTH
    RESPONSE_CRLF;

  for(size = i = 0; connection.argv[i]; ++i){
    size = size + strlen(connection.argv[i]) + 1;
    if(fileFolder(connection.argv[i])){
      size = size + 1;
    }
  }

  buffer = malloc(size);
  if(!buffer){
    responseServerError(connection, "Error allocating memory.");
    return;
  }

  for(k = i = 0; connection.argv[i]; ++i){
    for(j = 0; connection.argv[i][j]; ++j){
      buffer[k++] = connection.argv[i][j];
    }
    if(fileFolder(connection.argv[i])){
      if(buffer[k - 1] == FILE_SEPARATOR){
        size = size - 1;
      }
      else{
        buffer[k++] = FILE_SEPARATOR;
      }
    }
    buffer[k++] = '\n';
  }

  date = responseDate();
  length = sprintf(string, format, date, size - 1);
  responseBuffer(connection, string, length);

  responseBuffer(connection, buffer, size - 1);

  free(buffer);
}

void responseMultipart(struct connection connection, FILE *file, struct range range){
  char *rangeString, *type, *date;
  int size;

  char string[
    RESPONSE_206_N +
    RESPONSE_SERVER_N +
    RESPONSE_DATE_N +
    RESPONSE_NO_CACHE_N +
    RESPONSE_ACCEPT_RANGES_N +
    RESPONSE_CONTENT_TYPE_N +
    RESPONSE_CRLF_N
  ];

  char format[] =
    RESPONSE_206
    RESPONSE_SERVER
    RESPONSE_DATE
    RESPONSE_ACCEPT_RANGES
    RESPONSE_CONTENT_TYPE_BUILD(multipart/byteranges; boundary=byterange)
    RESPONSE_CRLF;

  char string2[
    11 +
    RESPONSE_CONTENT_TYPE_N +
    RESPONSE_CONTENT_RANGE_N +
    RESPONSE_CRLF_N
  ];

  char format2[] =
    "byterange\r\n"
    RESPONSE_CONTENT_TYPE
    RESPONSE_CONTENT_RANGE
    RESPONSE_CRLF;

  date = responseDate();
  size = sprintf(string, format, date);
  responseBuffer(connection, string, size);

  type = responseContentType(connection);
  rangeString = responseContentRange(range);
  size = sprintf(string, format, type, rangeString);
  responseBuffer(connection, string, size);

  if(strcmp(connection.header.method, "HEAD") != 0){
    fileSendRange(file, connection.socket, range.start, range.end);
    responseString(connection, "\r\n");
  }

  while(range.next){
    rangeParse(&range);
    rangeString = responseContentRange(range);
    size = sprintf(string, format, type, rangeString);
    responseBuffer(connection, string, size);

    if(strcmp(connection.header.method, "HEAD") != 0){
      fileSendRange(file, connection.socket, range.start, range.end);
      responseString(connection, "\r\n");
    }
  }
}

int responsePartialFile(struct connection connection, FILE *file){
  char *rangeString, *type, *date, *attribute;
  struct range range;
  int size;

  char string[
    RESPONSE_206_N +
    RESPONSE_SERVER_N +
    RESPONSE_DATE_N +
    RESPONSE_ACCEPT_RANGES_N +
    RESPONSE_CONTENT_TYPE_N +
    RESPONSE_CONTENT_LENGTH_N +
    RESPONSE_CONTENT_RANGE_N +
    RESPONSE_CRLF_N
  ];

  char format[] =
    RESPONSE_206
    RESPONSE_SERVER
    RESPONSE_DATE
    RESPONSE_ACCEPT_RANGES
    RESPONSE_CONTENT_TYPE
    RESPONSE_CONTENT_LENGTH
    RESPONSE_CONTENT_RANGE
    RESPONSE_CRLF;

  attribute = connection.header.attributes[ATTRIBUTES_RANGE];
  if(!attribute) return 0;
  range = rangeMake(attribute, fileSize(file));
  rangeParse(&range);
  if(range.status == unrecognized) return 0;
  if(range.status == unsatisfiable){
    responseRequestedRangeNotSatisfiable(connection, range);
    return 1;
  }

  if(range.next){
    responseMultipart(connection, file, range);
    return 1;
  }

  date = responseDate();
  type = responseContentType(connection);
  size = range.end - range.start;
  rangeString = responseContentRange(range);
  size = sprintf(string, format, date, type, size, rangeString);
  responseBuffer(connection, string, size);

  if(strcmp(connection.header.method, "HEAD") != 0){
    fileSendRange(file, connection.socket, range.start, range.end);
  }

  return 1;
}

void responseBadRequest(struct connection connection, char *reason){
  char *date;
  int size;

  char string[
    RESPONSE_400_N +
    RESPONSE_SERVER_N +
    RESPONSE_DATE_N +
    RESPONSE_ACCEPT_RANGES_N +
    RESPONSE_CONTENT_TYPE_N +
    RESPONSE_CONTENT_LENGTH_N +
    RESPONSE_CRLF_N +
    RESPONSE_STR_N +
    500
  ];

  char format[] =
    RESPONSE_400
    RESPONSE_SERVER
    RESPONSE_DATE
    RESPONSE_ACCEPT_RANGES
    RESPONSE_CONTENT_TYPE_BUILD(text/html)
    RESPONSE_CONTENT_LENGTH
    RESPONSE_CRLF
    "<!DOCTYPE html>\r\n"
    "<html>\r\n"
    "  <head>\r\n"
    "    <meta charset='UTF-8'>\r\n"
    "    <title>Error</title>\r\n"
    "  </head>\r\n"
    "  <body>\r\n"
    "    " RESPONSE_STR "\r\n"
    "  </body>\r\n"
    "</html>\r\n";

  char html[] = 
    "<!DOCTYPE html>\r\n"
    "<html>\r\n"
    "  <head>\r\n"
    "    <meta charset='UTF-8'>\r\n"
    "    <title>Error</title>\r\n"
    "  </head>\r\n"
    "  <body>\r\n"
    "    "  "\r\n"
    "  </body>\r\n"
    "</html>\r\n";

  date = responseDate();
  size = strlen(html) + strlen(reason);
  size = sprintf(string, format, date, size, reason);
  responseBuffer(connection, string, size);
}

void responseNotImplemented(struct connection *connection){
  char *date;
  int size;

  char string[
    RESPONSE_501_N +
    RESPONSE_SERVER_N +
    RESPONSE_DATE_N +
    RESPONSE_ACCEPT_RANGES_N +
    RESPONSE_CONTENT_TYPE_N +
    RESPONSE_CONTENT_LENGTH_N +
    RESPONSE_CRLF_N +
    18
  ];

  char format[] =
    RESPONSE_501
    RESPONSE_SERVER
    RESPONSE_DATE
    RESPONSE_ACCEPT_RANGES
    RESPONSE_CONTENT_TYPE_BUILD(text/plain)
    RESPONSE_CONTENT_LENGTH_BUILD(16)
    RESPONSE_CRLF
    "Not Implemented!\r\n";

  date = responseDate();
  size = sprintf(string, format, date);
  responseBuffer(connection[0], string, size);

  bodyPass(connection, 0, 0);
}

void responseNotModified(struct connection connection){
  char *date;
  int size;

  char string[
    RESPONSE_304_N +
    RESPONSE_SERVER_N +
    RESPONSE_DATE_N +
    RESPONSE_ACCEPT_RANGES_N +
    RESPONSE_CRLF_N
  ];

  char format[] =
    RESPONSE_304
    RESPONSE_SERVER
    RESPONSE_DATE
    RESPONSE_ACCEPT_RANGES
    RESPONSE_CRLF;

  date = responseDate();
  size = sprintf(string, format, date);
  responseBuffer(connection, string, size);
}

void responseRequestedRangeNotSatisfiable(struct connection connection, struct range range){
  char *date, *rangeString;
  int size;

  char string[
    RESPONSE_416_N +
    RESPONSE_SERVER_N +
    RESPONSE_DATE_N +
    RESPONSE_ACCEPT_RANGES_N +
    RESPONSE_CONTENT_TYPE_N +
    RESPONSE_CONTENT_LENGTH_N +
    RESPONSE_CONTENT_RANGE_N +
    RESPONSE_CRLF_N +
    34
  ];

  char format[] =
    RESPONSE_416
    RESPONSE_SERVER
    RESPONSE_DATE
    RESPONSE_ACCEPT_RANGES
    RESPONSE_CONTENT_TYPE_BUILD(text/plain)
    RESPONSE_CONTENT_LENGTH_BUILD(31)
    RESPONSE_CONTENT_RANGE
    RESPONSE_CRLF
    "Requested Range Not Satisfiable!\r\n";

  date = responseDate();
  rangeString = responseContentRange(range);
  size = sprintf(string, format, date, rangeString);
  responseBuffer(connection, string, size);
}

void responsePreconditionFailed(struct connection connection){
  char *date;
  int size;

  char string[
    RESPONSE_412_N +
    RESPONSE_SERVER_N +
    RESPONSE_DATE_N +
    RESPONSE_ACCEPT_RANGES_N +
    RESPONSE_CONTENT_TYPE_N +
    RESPONSE_CONTENT_LENGTH_N +
    RESPONSE_CRLF_N +
    22
  ];

  char format[] =
    RESPONSE_412
    RESPONSE_SERVER
    RESPONSE_DATE
    RESPONSE_ACCEPT_RANGES
    RESPONSE_CONTENT_TYPE_BUILD(text/plain)
    RESPONSE_CONTENT_LENGTH_BUILD(20)
    RESPONSE_CRLF
    "Precondition Failed!";

  date = responseDate();
  size = sprintf(string, format, date);
  responseBuffer(connection, string, size);
}

void responseNotFound(struct connection connection){
  char *date;
  int size;

  char string[
    RESPONSE_404_N +
    RESPONSE_SERVER_N +
    RESPONSE_DATE_N +
    RESPONSE_ACCEPT_RANGES_N +
    RESPONSE_CONTENT_TYPE_N +
    RESPONSE_CONTENT_LENGTH_N +
    RESPONSE_CRLF_N +
    12
  ];

  char format[] =
    RESPONSE_404
    RESPONSE_SERVER
    RESPONSE_DATE
    RESPONSE_ACCEPT_RANGES
    RESPONSE_CONTENT_TYPE_BUILD(text/plain)
    RESPONSE_CONTENT_LENGTH_BUILD(10)
    RESPONSE_CRLF
    "Not found!\r\n";

  date = responseDate();
  size = sprintf(string, format, date);
  responseBuffer(connection, string, size);
}

void responseServerError(struct connection connection, char *reason){
  char *date;
  int size;

  char string[
    RESPONSE_500_N +
    RESPONSE_SERVER_N +
    RESPONSE_DATE_N +
    RESPONSE_ACCEPT_RANGES_N +
    RESPONSE_CONTENT_TYPE_N +
    RESPONSE_CONTENT_LENGTH_N +
    RESPONSE_CRLF_N +
    RESPONSE_STR_N +
    500
  ];

  char format[] =
    RESPONSE_500
    RESPONSE_SERVER
    RESPONSE_DATE
    RESPONSE_ACCEPT_RANGES
    RESPONSE_CONTENT_TYPE_BUILD(text/html)
    RESPONSE_CONTENT_LENGTH
    RESPONSE_CRLF
    "<!DOCTYPE html>\r\n"
    "<html>\r\n"
    "  <head>\r\n"
    "    <meta charset='UTF-8'>\r\n"
    "    <title>Error</title>\r\n"
    "  </head>\r\n"
    "  <body>\r\n"
    "    " RESPONSE_STR "\r\n"
    "  </body>\r\n"
    "</html>\r\n";

  char html[] = 
    "<!DOCTYPE html>\r\n"
    "<html>\r\n"
    "  <head>\r\n"
    "    <meta charset='UTF-8'>\r\n"
    "    <title>Error</title>\r\n"
    "  </head>\r\n"
    "  <body>\r\n"
    "    "  "\r\n"
    "  </body>\r\n"
    "</html>\r\n";

  date = responseDate();
  size = strlen(html) + strlen(reason);
  size = sprintf(string, format, date, size, reason);
  responseBuffer(connection, string, size);
}

void responseContinue(struct connection connection){
  responseString(connection, RESPONSE_100 RESPONSE_CRLF);
}

char *responseContentRange(struct range range){
  static char string[] = "4294967296-4294967296/4294967296";
  static char format[] = "%d-%d/%d";

  sprintf(string, format, range.start, range.end - 1, range.size);
  return string;
}

char *responseLastModified(time_t date){
  static char string[] = "AAA, 00 AAA 0000 00:00:00 AAA";
  static char format[] = "%a, %d %b %Y %T %Z";

  strftime(string, 30, format, gmtime(&date));
  return string;
}

char *responseDate(void){
  static char string[] = "AAA, 00 AAA 0000 00:00:00 AAA";
  static char format[] = "%a, %d %b %Y %T %Z";
  time_t date = time(NULL);

  strftime(string, 30, format, gmtime(&date));
  return string;
}

char *responseContentType(struct connection connection){
  return mime(connection.header.path);
}

void responseBuffer(struct connection connection, char *buffer, int size){
  printf("start sending\n%.*send sending\n", size, buffer);
  send(connection.socket, buffer, size, 0);
}

void responseString(struct connection connection, char *string){
  responseBuffer(connection, string, strlen(string));
}
