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

#ifndef RESPONSE_H
#define RESPONSE_H

#include <time.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#include <winsock2.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#endif

#include "lib.h"
#include "url.h"
#include "body.h"
#include "mime.h"
#include "date.h"
#include "range.h"
#include "network.h"
#include "connection.h"

/* Status */
#define RESPONSE_100 "HTTP/1.1 100 Continue\r\n"
#define RESPONSE_100_N 23
#define RESPONSE_200 "HTTP/1.1 200 OK\r\n"
#define RESPONSE_200_N 17
#define RESPONSE_206 "HTTP/1.1 206 Partial Content\r\n"
#define RESPONSE_206_N 30
#define RESPONSE_304 "HTTP/1.1 304 Not Modified\r\n"
#define RESPONSE_304_N 27
#define RESPONSE_400 "HTTP/1.1 400 Bad Request\r\n"
#define RESPONSE_400_N 26
#define RESPONSE_404 "HTTP/1.1 404 Not Found\r\n"
#define RESPONSE_404_N 24
#define RESPONSE_412 "HTTP/1.1 412 Precondition Failed\r\n"
#define RESPONSE_412_N 34
#define RESPONSE_416 "HTTP/1.1 416 Requested Range Not Satisfiable\r\n"
#define RESPONSE_416_N 46
#define RESPONSE_500 "HTTP/1.1 500 Server Error\r\n"
#define RESPONSE_500_N 27
#define RESPONSE_501 "HTTP/1.1 501 Not Implemented\r\n"
#define RESPONSE_501_N 30

/* Attributes */
#define RESPONSE_CONNECTION_CLOSE "Connection: close\r\n"
#define RESPONSE_CONNECTION_CLOSE_N 19
#define RESPONSE_NO_CACHE "Cache-Control: no-cache\r\n"
#define RESPONSE_NO_CACHE_N 25
#define RESPONSE_SERVER "Server: Rodrigo's server\r\n"
#define RESPONSE_SERVER_N 26
#define RESPONSE_DATE "Date: %s\r\n"
#define RESPONSE_DATE_N 37
#define RESPONSE_ACCEPT_RANGES "Accept-Ranges: bytes\r\n"
#define RESPONSE_ACCEPT_RANGES_N 22
#define RESPONSE_LAST_MODIFIED "Last-Modified: %s\r\n"
#define RESPONSE_LAST_MODIFIED_N 46
#define RESPONSE_CONTENT_TYPE "Content-Type: %s\r\n"
#define RESPONSE_CONTENT_TYPE_N 271
#define RESPONSE_CONTENT_TYPE_BUILD(type) "Content-Type: " #type "\r\n"
#define RESPONSE_CONTENT_LENGTH "Content-Length: %d\r\n"
#define RESPONSE_CONTENT_LENGTH_N 28
#define RESPONSE_CONTENT_LENGTH_BUILD(length) "Content-Length:" #length "\r\n"
#define RESPONSE_CONTENT_RANGE "Content-range: bytes %s\r\n"
#define RESPONSE_CONTENT_RANGE_N 55
#define RESPONSE_ACCEPT_RANGES "Accept-Ranges: bytes\r\n"
#define RESPONSE_ACCEPT_RANGES_N 22

/* Auxiliar */
#define RESPONSE_INT "%d"
#define RESPONSE_INT_N 11
#define RESPONSE_STR "%s"
#define RESPONSE_STR_N 255
#define RESPONSE_CRLF "\r\n"
#define RESPONSE_CRLF_N 2

#define RESPONSE_BUFFER_SIZE 1024

/* API responses auxiliar */
void responseFailure(struct connection connection);
void responseSuccess(struct connection connection);
void responseInteger(struct connection connection, int x);

/* API responses */
void responseExtern(struct connection *connection);
void responseFolderGet(struct connection connection);
void responseFolderSet(struct connection connection);
void responseFolderRemove(struct connection connection);
void responseFolderRename(struct connection connection);
void responseFile(struct connection connection);
void responseFileGet(struct connection connection);
void responseFileSets(struct connection *connection, char *mode);
void responseFileSet(struct connection *connection);
void responseFileAdd(struct connection *connection);
void responseFileRemove(struct connection connection);
void responseFileRename(struct connection connection);
void responseFileOpen(struct connection connection);
void responseShell(struct connection connection);
void responseQuit(struct connection connection);
void responseArgv(struct connection connection);

/* Responses */
void responseMultipart(struct connection connection, FILE *file, struct range range);
int responsePartialFile(struct connection connection, FILE *file);
void responseFile(struct connection connection);
void responseBadRequest(struct connection connection, char *reason);
void responseNotImplemented(struct connection *connection);
void responseNotModified(struct connection connection);
void responseRequestedRangeNotSatisfiable(struct connection connection, struct range range);
void responsePreconditionFailed(struct connection connection);
void responseNotFound(struct connection connection);
void responseServerError(struct connection connection, char *reason);
void responseContinue(struct connection connection);

/* Headers */
char *responseContentRange(struct range range);
char *responseLastModified(time_t date);
char *responseDate(void);
char *responseContentType(struct connection connection);

/* Auxiliar */
void responseBuffer(struct connection connection, char *buffer, int size);
void responseString(struct connection connection, char *string);

#endif
