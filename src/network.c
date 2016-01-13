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

#include "network.h"

void networkMakeSocket(int *socket, char *portString){
  struct{char *string; int number;} port;
  struct addrinfo *address;

  port.string = portString;
  port.number = 1024;
  socket[0] = 0;

  while(!socket[0]){
    sprintf(port.string, "%d", port.number);

    address = networkParseAddress("127.0.0.1", port.string);
    if(!address){
      fprintf(stderr, "Error parsing address.\n");
      return;
    }

    socket[0] = networkOpenSocket(address);
    if(!socket[0]){
      fprintf(stderr, "Error opening socket.\n");
      freeaddrinfo(address);
    }

    if(!networkGetPort(socket[0], address->ai_addr, address->ai_addrlen)){
      freeaddrinfo(address);
      close(socket[0]);
      socket[0] = 0;
    }

    port.number = port.number + 1;
  }

  freeaddrinfo(address);
}

struct addrinfo *networkParseAddress(char *host, char *service){
  struct addrinfo *address;
  struct addrinfo hints;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if(getaddrinfo(host, service, &hints, &address)){
    fprintf(stderr, "Error parsing address.\n");
    return NULL;
  }

  return address;
}

int networkOpenSocket(struct addrinfo *address){
  int protocol1 = address->ai_family;   /* IPv4 */
  int protocol2 = address->ai_socktype; /* TCP */
  int protocol3 = address->ai_protocol; /* port */
  int openedSocket;

  openedSocket = socket(protocol1, protocol2, protocol3);
  if(openedSocket == -1){
    fprintf(stderr, "Error creating socket.\n");
    return 0;
  }

  return openedSocket;
}

int networkGetPort(int socket, struct sockaddr *address, size_t size){
  if(bind(socket, address, size) == -1){
    return 0;
  }

  return 1;
}

int networkServe(char **argv){
  struct{struct sockaddr address; int socket; socklen_t size;} client;
  struct{int socket; char port[5];} server;
  struct{int pipe[2]; int flag;} stop;
  struct networkThread *thread;
  struct timeval timeout;
  fd_set socketSet;
  int children = 0;
  int pid = 1;
  int i;

  networkMakeSocket(&server.socket, server.port);
  if(!server.socket){
    fprintf(stderr, "Error creating socket.\n");
    return 0;
  }

  if(!browserOpen(server.port)){
    fprintf(stderr, "Error opening browser.\n");
    close(server.socket);
    return 0;
  }

  if(listen(server.socket, NETWORK_QUEUE) == -1){
    fprintf(stderr, "Error listening.\n");
    close(server.socket);
    return 0;
  }

  printf("Ready on %s.\n", server.port);

  stop.flag = 0;
#ifndef _WIN32
  if(pipe(stop.pipe) == -1){
    fprintf(stderr, "Error %s setting pipe.\n", strerror(errno));
    close(server.socket);
    return 0;
  }
  if(fcntl(stop.pipe[0], F_SETFL, O_NONBLOCK) == -1){
    fprintf(stderr, "Error %s setting pipe.\n", strerror(errno));
    close(server.socket);
    close(stop.pipe[0]);
    close(stop.pipe[1]);
    return 0;
  }
#endif
  FD_ZERO(&socketSet);

  while(!stop.flag){

    /* Set timeout. */
    timeout.tv_sec = NETWORK_CHECK_DELAY;
    timeout.tv_usec = 0;
    FD_SET(server.socket, &socketSet);
    if(select(server.socket + 1, &socketSet, NULL, NULL, &timeout) == -1){
      fprintf(stderr, "Error \"%s\" waiting on socket.\n", strerror(errno));
      break;
    }

    if(FD_ISSET(server.socket, &socketSet)){

      /* Accept connection. */
      client.size = sizeof(client.address);
      client.socket = accept(server.socket, &client.address, &client.size);
      if(client.socket == -1){
        fprintf(stderr, "Error accepting connection.\n");
        break;
      }
      fflush(stdout);
#ifdef _WIN32
      thread = malloc(sizeof(struct networkThread));
      thread->stop = &stop.flag;
      thread->socket = client.socket;
      thread->argv = argv;
      CreateThread(NULL, 0, networkServeOne, thread, 0, NULL);
    }
  }
  closesocket(server.socket);
#else
      pid = fork();
      if(pid < 0){
        fprintf(stderr, "Error forking.\n");
        sleep(3);
        continue;
      }
      else{
        children = children + 1;
      }

      /* Children code. */
      if(pid == 0){
        printf("new socket connection.\n");
        stop.flag = httpConnect(client.socket, argv);
        if(stop.flag == -1){
          fprintf(stderr, "Error handling HTTP connection.\n");
          close(client.socket);
          break;
        }
        if(stop.flag == 1){
          write(stop.pipe[1], &stop.flag, sizeof(int));
        }
        stop.flag = 1;
        printf("end socket connection.\n");
      }

      close(client.socket);
    }

    else{
      read(stop.pipe[0], &stop.flag, sizeof(int));
    }
  }

  close(stop.pipe[0]);
  close(stop.pipe[1]);
  close(server.socket);
  if(pid) for(i = 0; i < children; ++i) wait(NULL);
#endif
  return 1;
}

#ifdef _WIN32
DWORD WINAPI networkServeOne(LPVOID lpParam){
  struct networkThread *pipe = (struct networkThread *)lpParam;

  printf("new socket connection.\n");
  pipe->stop[0] = httpConnect(pipe->socket, pipe->argv);
  printf("end socket connection.\n");
  closesocket(pipe->socket);
  free(pipe);
}
#endif
