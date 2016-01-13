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

#include "header.h"

void headerCopy(struct header header, char **buffer, int *size){
  int i, j, k;

  size[0] = 0;
  size[0] = size[0] + strlen(header.method) + 1;
  size[0] = size[0] + strlen(header.path) + 2;
  size[0] = size[0] + strlen(header.version) + 2;
  for(i = 0; i < ATTRIBUTES_N; ++i){
    if(!header.attributes[i]) continue;
    size[0] = size[0] + strlen(attributes[i]) + 2;
    size[0] = size[0] + strlen(header.attributes[i]) + 2;
  }
  size[0] = size[0] + 2;

  buffer[0] = malloc(size[0]);
  i = 0;

  for(j = 0; header.method[j]; ++j){
    buffer[0][i++] = header.method[j];
  }
  buffer[0][i++] = ' ';

  buffer[0][i++] = '/';
  for(j = 0; header.path[j]; ++j){
    buffer[0][i++] = header.path[j];
  }
  buffer[0][i++] = ' ';

  for(j = 0; header.version[j]; ++j){
    buffer[0][i++] = header.version[j];
  }
  buffer[0][i++] = '\r';
  buffer[0][i++] = '\n';

  for(k = 0; k < ATTRIBUTES_N; ++k){
    if(!header.attributes[k]) continue;
    for(j = 0; attributes[k][j]; ++j){
      buffer[0][i++] = attributes[k][j];
    }
    buffer[0][i++] = ':';
    buffer[0][i++] = ' ';
    for(j = 0; header.attributes[k][j]; ++j){
      buffer[0][i++] = header.attributes[k][j];
    }
    buffer[0][i++] = '\r';
    buffer[0][i++] = '\n';
  }

  buffer[0][i++] = '\r';
  buffer[0][i++] = '\n';
}

char *headerErrorString(enum headerParserState error){
  switch(error){
    case methodBegin:
      return "Method does not begin with upper case letter.";

    case methodEnd:
      return "Method has a non upper case letter.";

    case pathBegin0:
      return "Invalid beginning of path.";

    case pathHost1:
      return "First \"/\" not followed by a \"/\" in URL in path.";

    case versionBegin:
      return "Version does not begin with \"H\".";

    case versionEnd:
      return "Version does not end with \"\\n\" or \"\\r\".";

    case versionEndStandard0:
    case versionEndStandard2:
      return "At the end of version \"\\n\" does not follow \"\\r\".";

    case attributeNameBegin:
      return "Attribute name can't start with white space or \":\".";

    case attributeValueEndStandard0:
    case attributeValueEndStandard2:
      return "At the end of value \"\\n\" does not follow \"\\r\".";

    default:
      return NULL;
  }
  return NULL;
}

void headerPrint(struct header header){
  int i;

  printf("\nHeader.\n");

  switch(header.status){
    printf("Status: ");
    case incomplete:
      printf("Incomplete.\n");
      break;

    case invalid:
      printf("Invalid: %s\n", headerErrorString(header.error));
      break;

    case ok:
      printf("OK.\n");
      break;
  }
  
  printf("i: %d\n", header.i);
  printf("Error: %d\n", header.error);
  printf("State: %d\n", header.state);

  if(header.state > methodEnd)  printf("Method: %s\n", header.method);
  if(header.state > pathEnd)    printf("Path: %s\n", header.path);
  if(header.state > versionEnd) printf("Version: %s\n", header.version);

  for(i = 0; i < ATTRIBUTES_N; ++i){
    if(!header.attributes[i]) continue;
    printf("%s: %s\n", attributes[i], header.attributes[i]);
  }

  printf("\n");
}

struct header headerMake(){
  struct header header;
  memset(&header, 0, sizeof(header));
  return header;
}

void headerSwapBuffer(struct header *header, char *buffer){
  int i;

  if(buffer == header->buffer) return;

  if(header->method){
    header->method = &buffer[header->method - header->buffer];
  }
  if(header->path){
    header->path = &buffer[header->path - header->buffer];
  }
  if(header->version){
    header->version = &buffer[header->version - header->buffer];
  }

  for(i = 0; i < ATTRIBUTES_N; ++i){
    if(header->attributes[i]){
      header->attributes[i] = &buffer[header->attributes[i] - header->buffer];
    }
  }

  if(header->attribute.name){
    header->attribute.name = &buffer[header->attribute.name - header->buffer];
  }

  header->buffer = buffer;
}

void headerParse(struct header *header, char *buffer, int size){
  headerSwapBuffer(header, buffer);
  char *name;

  while(header->status == incomplete && header->i < size){
    switch(header->state){
      case start:
        switch(buffer[header->i]){
          case ' ': case '\t': case '\n': case '\r':
            header->i = header->i + 1;
            break;

          default:
            header->state = methodBegin;
        }
        break;

      case methodBegin:
        switch(buffer[header->i]){
          case 'Q': case 'W': case 'E': case 'R': case 'T': case 'Y': case 'U':
          case 'I': case 'O': case 'P': case 'A': case 'S': case 'D': case 'F':
          case 'G': case 'H': case 'J': case 'K': case 'L': case 'Z': case 'X': 
          case 'C': case 'V': case 'B': case 'N': case 'M':
            header->method = &buffer[header->i];
            header->state = methodEnd;
            header->i = header->i + 1;
            break;

          default:
            header->error = header->state;
            header->state = end;
        }
        break;

      case methodEnd:
        switch(buffer[header->i]){
          case 'Q': case 'W': case 'E': case 'R': case 'T': case 'Y': case 'U':
          case 'I': case 'O': case 'P': case 'A': case 'S': case 'D': case 'F':
          case 'G': case 'H': case 'J': case 'K': case 'L': case 'Z': case 'X': 
          case 'C': case 'V': case 'B': case 'N': case 'M':
            header->i = header->i + 1;
            break;

          case '\t': case ' ': 
            header->state = pathBegin0;
            buffer[header->i] = '\0';
            header->i = header->i + 1;
            break;

          default:
            header->error = header->state;
            header->state = end;
        }
        break;

      case pathBegin0:
        switch(buffer[header->i]){
          case 'h':
            header->state = pathHost0;
            break;

          case '/':
            header->i = header->i + 1;
            header->state = pathBegin1;
            break;

          case '\t': case ' ': 
            header->i = header->i + 1;
            break;

          default:
            header->error = header->state;
            header->state = end;
        }
        break;

      case pathHost0:
        switch(buffer[header->i]){
          case '/':
            header->state = pathHost1;
            header->i = header->i + 1;
            break;

          default:
            header->i = header->i + 1;
        }
        break;

      case pathHost1:
        switch(buffer[header->i]){
          case '/':
            header->state = pathHost2;
            header->i = header->i + 1;
            break;

          default:
            header->error = header->state;
            header->state = end;
        }
        break;

      case pathHost2:
        switch(buffer[header->i]){
          case '/':
            header->state = pathBegin1;
            header->i = header->i + 1;
            break;

          default:
            header->i = header->i + 1;
        }
        break;

      case pathBegin1:
        switch(buffer[header->i]){
          case 'r':
            header->path = &buffer[header->i];
            header->state = pathRoot0;
            header->i = header->i + 1;
            break;

          default:
            header->path = &buffer[header->i];
            header->state = pathEnd;
        }
        break;

      case pathRoot0:
        switch(buffer[header->i]){
          case 'o':
            header->state = pathRoot1;
            header->i = header->i + 1;
            break;

          default:
            header->state = pathEnd;
        }
        break;

      case pathRoot1:
        switch(buffer[header->i]){
          case 'o':
            header->state = pathRoot2;
            header->i = header->i + 1;
            break;

          default:
            header->state = pathEnd;
        }
        break;

      case pathRoot2:
        switch(buffer[header->i]){
          case 't':
            header->state = pathEnd;
            header->i = header->i + 1;
            header->path = &buffer[header->i];
            break;

          default:
            header->state = pathEnd;
        }
        break;

      case pathEnd:
        switch(buffer[header->i]){
          case '\t': case ' ': 
            header->state = versionBegin;
            buffer[header->i] = '\0';
            URLDecode(header->path);
            header->i = header->i + 1;
            break;

          default:
            header->i = header->i + 1;
        }
        break;

      case versionBegin:
        switch(buffer[header->i]){
          case '\t': case ' ': 
            header->i = header->i + 1;
            break;

          case 'H':
            header->version = &buffer[header->i];
            header->state = versionEnd;
            header->i = header->i + 1;
            break;

          default:
            header->error = header->state;
            header->state = end;
        }
        break;

      case versionEnd:
        switch(buffer[header->i]){
          case 'H': case 'T': case 'P': case '.': case '/':
          case '1': case '2': case '3':
          case '4': case '5': case '6':
          case '7': case '8': case '9': case '0':
            header->i = header->i + 1;
            break;

          case '\n':
            header->state = versionEndNonStandard;
            buffer[header->i] = '\0';
            header->i = header->i + 1;
            break;

          case '\r':
            header->state = versionEndStandard0;
            buffer[header->i] = '\0';
            header->i = header->i + 1;
            break;

          default:
            header->error = header->state;
            header->state = end;
        }
        break;

      case versionEndStandard0:
        switch(buffer[header->i]){
          case '\n':
            header->state = versionEndStandard1;
            header->i = header->i + 1;
            break;

          default:
            header->error = header->state;
            header->state = end;
        }
        break;

      case versionEndStandard1:
        switch(buffer[header->i]){
          case '\r':
            header->state = versionEndStandard2;
            header->i = header->i + 1;
            break;

          default:
            header->state = attributeNameBegin;
        }
        break;

      case versionEndStandard2:
        switch(buffer[header->i]){
          case '\n':
            header->i = header->i + 1;
            header->status = ok;
            break;

          default:
            header->error = header->state;
            header->state = end;
        }
        break;

      case versionEndNonStandard:
        switch(buffer[header->i]){
          case '\n':
            header->i = header->i + 1;
            header->status = ok;
            break;

          default:
            header->state = attributeNameBegin;
        }
        break;

      case attributeNameBegin:
        switch(buffer[header->i]){
          case '\t': case ' ': case ':':
            header->error = header->state;
            header->state = end;
            break;

          default:
            header->attribute.name = &buffer[header->i];
            header->state = attributeNameEnd;
        }
        break;

      case attributeNameEnd:
        switch(buffer[header->i]){
          case ':':
            buffer[header->i] = '\0';
            header->attribute.i = attributesGetIndex(header->attribute.name);
            if(header->attribute.i == -1){
              name = header->attribute.name;
              fprintf(stderr, "Unrecognized attribute %s.\n", name);
            }
            buffer[header->i] = ':';
            header->state = attributeValueBegin;
            header->i = header->i + 1;
            break;

          default:
            buffer[header->i] = tolower(buffer[header->i]);
            header->i = header->i + 1;
        }
        break;

      case attributeValueBegin:
        switch(buffer[header->i]){
          case '\t': case ' ':
            header->i = header->i + 1;
            break;

          default:
            if(header->attribute.i != -1){
              header->attributes[header->attribute.i] = &buffer[header->i];
            }
            header->state = attributeValueEnd;
            header->i = header->i + 1;
        }
        break;

      case attributeValueEnd:
        switch(buffer[header->i]){
          case '\n':
            header->state = attributeValueEndNonStandard;
            header->i = header->i + 1;
            break;

          case '\r':
            header->state = attributeValueEndStandard0;
            header->i = header->i + 1;
            break;

          default:
            header->i = header->i + 1;
        }
        break;

      case attributeValueEndStandard0:
        switch(buffer[header->i]){
          case '\n':
            header->state = attributeValueEndStandard1;
            header->i = header->i + 1;
            break;

          default:
            header->error = header->state;
            header->state = end;
        }
        break;

      case attributeValueEndStandard1:
        switch(buffer[header->i]){
          case '\r':
            header->state = attributeValueEndStandard2;
            if(header->attribute.i != -1){
              buffer[header->i - 2] = '\0';
            }
            header->i = header->i + 1;
            break;

          case '\t': case ' ':
            header->state = attributeValueEnd;
            header->i = header->i + 1;
            break;

          default:
            header->state = attributeNameBegin;
            if(header->attribute.i != -1){
              buffer[header->i - 2] = '\0';
            }
        }
        break;

      case attributeValueEndStandard2:
        switch(buffer[header->i]){
          case '\n':
            header->i = header->i + 1;
            header->status = ok;
            break;

          default:
            header->error = attributeValueEndStandard2;
            header->state = end;
        }
        break;

      case attributeValueEndNonStandard:
        switch(buffer[header->i]){
          case '\n':
            if(header->attribute.i != -1){
              buffer[header->i - 1] = '\0';
            }
            header->i = header->i + 1;
            header->status = ok;
            break;

          case '\t': case ' ':
            header->state = attributeValueEnd;
            header->i = header->i + 1;
            break;

          default:
            header->state = attributeNameBegin;
            if(header->attribute.i != -1){
              buffer[header->i - 1] = '\0';
            }
        }
        break;

      case end:
        switch(buffer[header->i]){
          case '\r':
            header->state = endStandard0;
            header->i = header->i + 1;
            break;

          case '\n':
            header->state = endNonStandard;
            header->i = header->i + 1;
            break;

          default:
            header->i = header->i + 1;
        }
        break;

      case endNonStandard:
        switch(buffer[header->i]){
          case '\n':
            header->status = invalid;
            header->i = header->i + 1;
            break;

          default:
            header->state = end;
        }
        break;

      case endStandard0:
        switch(buffer[header->i]){
          case '\n':
            header->state = endStandard1;
            header->i = header->i + 1;
            break;

          default:
            header->state = end;
        }
        break;

      case endStandard1:
        switch(buffer[header->i]){
          case '\r':
            header->state = endStandard2;
            header->i = header->i + 1;
            break;

          default:
            header->state = end;
        }
        break;

      case endStandard2:
        switch(buffer[header->i]){
          case '\n':
            header->status = invalid;
            header->i = header->i + 1;
            break;

          default:
            header->state = end;
        }
        break;
    }
  }
}
