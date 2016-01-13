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

#include "file.h"

int fileSize(FILE *file, long *size) {
    long originalPosition;

    originalPosition = ftell(file);
    if (originalPosition == -1) {
        fprintf(stderr, "file> Error getting index in file.\n");
        return 0;
    }

    if (fseek(file, 0, SEEK_END) == -1) {
        fprintf(stderr, "file> Error seeking file.\n");
        return 0;
    }

    size[0] = ftell(file);
    if (size[0] == -1) {
        fprintf(stderr, "file> Error getting index in file.\n");
        return 0;
    }

    if (fseek(file, originalPosition, SEEK_SET) == -1) {
        fprintf(stderr, "file> Error seeking file.\n");
        return 0;
    }

    return 1;
}

int fileArray(FILE *file, struct array *array) {
    long toLoad;
    long loaded;

    arrayClean(array);

    if (!fileSize(file, &toLoad)) {
        fprintf("file> Error getting file size.\n");
        return 0;
    }

    if (fseek(file, 0, SEEK_SET) == -1) {
        fprintf(stderr, "file> Error rewinding file.\n");
        return 0;
    }

    if (!arrayInitSize(array, sizeof(char), toLoad)) {
        fprintf("file> Error creating array.\n");
        return 0;
    }

    while (toLoad) {
        loaded = fread(array->end, sizeof(char), toLoad, file);
        arrayAddSize(&array, loaded);
        toLoad = toLoad - loaded;
    }

    return 1;
}

int fileSendRange(FILE *file, int socket, long start, long end) {
    struct array array;
    long toLoad;
    long loaded;
    long toSend;
    long sent;

    if (fseek(file, start, SEEK_SET) == -1) {
        fprintf(stderr, "file> Error seeking file.\n");
        return 0;
    }

    if (!arrayInit(&array)) {
        fprintf(stderr, "file> Error creating array.\n");
        return 0;
    }

    toLoad = end - start;

    while (toLoad && !feof(file)) {

        if (toLoad > array->capacity) {
            loaded = fread(array->end, sizeof(char), array->capacity, file);
        } else {
            loaded = fread(array->end, sizeof(char), toLoad, file);
        }
        
        arrayAddSize(&array, loaded);
        toLoad = toLoad - loaded;
        toSend = loaded;
        while (toSend) {
            printf("Start sending.\n");
            sent = send(socket, array->data, arraySize(&array), 0);
            printf("%.*sEnd sending.\n", sent, array->data);

            if (sent == -1) {
                arrayFree(&array);
                if (errno == ECONNRESET) {
                    return 1;
                } else {
                    fprintf(stderr, "file> Error \"%s\" ", strerror(errno));
                    fprintf(stderr, "sending data.\n");
                    return 0;
                }
            } else {
                toSend = toSend - sent;
                arrayForget(&array, sent);
            }
        }
        arrayReset(&array);
    }

    return arrayFree(&array);
}

int fileSend(FILE *file, int socket) {
    long size;

    if (!fileSize(file, &size)) {
        fprintf("file> Error getting file size.\n");
        return 0;
    }

    return fileSendRange(file, socket, 0, size);
}

#ifdef _WIN32

int fileMove(char *old, char *fresh) {
    return MoveFile(old, fresh);
}

int fileRemove(char *path) {
    return DeleteFile(path);
}

int fileFolder(char *path, int *folder) {
    DWORD dwAttrs = GetFileAttributes(path);

    if (dwAttrs == INVALID_FILE_ATTRIBUTES) {
        fprintf(stderr, "Error getting file %s information.\n", path);
        return 0;
    }

    folder[0] = dwAttrs & FILE_ATTRIBUTE_DIRECTORY;
    return 1;
}

int fileFolderRemove(char *path) {
    return RemoveDirectory(path);
}

int fileFolderSet(char *path) {
    return CreateDirectory(path, NULL);
}

#else

int fileMove(char *old, char *fresh) {
    return !rename(old, fresh);
}

int fileRemove(struct array path) {
    return !unlink(path.data);
}

int fileFolder(struct array path, int *folder) {
    struct stat meta;

    if(stat(path.data, &meta) == -1){
        fprintf(stderr, "Error \"%s\" ", strerror(errno));
        fprintf(stderr, "getting file %s information.\n", path);
        return 0;
    }

    folder[0] = meta.st_mode & S_IFDIR;
    return 1;
}

int fileFolderRemove(struct array path) {
    return !rmdir(path.data);
}

int fileFolderSet(struct array path) {
    return !mkdir(path.data, 0700);
}

#endif

int fileFolderArray(struct array path, struct array *array) {
    HANDLE entries = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA entry;
    struct array query;

    if (!arrayInitSize(&query, sizeof(char), strlen(path) + 2)) {
        fprintf(stderr, "file> Error creating array.\n");
        return 0;
    }
    arrayPush(&query, path->data, arraySize(&path) - 1);
    arrayPush(&query, "*", 2);

void fileFolderBuffer(char *path, char **base, int *size){
#ifdef _WIN32
    HANDLE entries = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA entry;
    int pass = 0;
    base[0] = NULL;
    size[0] = 0;
    char *query;

  query = malloc(strlen(path) + 2);
  if(!query){
    fprintf(stderr, "Error allocating memory.\n");
    return;
  }
  sprintf(query, "%s\\*", path);

  while(pass != 2){
    size[0] = 0;
    entries = FindFirstFile(query, &entry);
    if(entries == INVALID_HANDLE_VALUE){
      fprintf(stderr, "Error reading entries of folder.\n");
      return;
    }
    do{
      if(entry.cFileName[0] == '.') continue;
      if(entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
        if(pass){
          sprintf(base[0] + size[0], "%s%c\n", entry.cFileName, FILE_SEPARATOR);
        }
        size[0] = size[0] + 1;
      }
      else{
        if(pass){
          sprintf(base[0] + size[0], "%s\n", entry.cFileName);
        }
      }
      size[0] = size[0] + strlen(entry.cFileName) + 1;
    }while(FindNextFile(entries, &entry) != 0);
    if(!pass){
      base[0] = malloc(size[0] + 1);
      if(!base[0]){
        size[0] = 0;
        free(query);
        fprintf(stderr, "Error allocating memory.\n");
        return;
      }
    }
    pass = pass + 1;
    size[0] = size[0] - 1;
  }
  free(query);
#else
  struct{char **base; int *size; int n;} buffer;
  struct{char *base; int size;} name;
  struct{
    struct{struct stat file; struct dirent *folder; } meta;
    struct{char *base; int size; int n;} path;
  }entry;
  struct{
    struct{char *base; int size;} path;
    DIR *meta;
  }folder;
  char *error;

  buffer.base = base;
  buffer.size = size;
  buffer.n = 0;

  buffer.size[0] = FILE_BUFFER_SIZE;
  buffer.base[0] = malloc(buffer.size[0] * sizeof(char));
  if(!buffer.base[0]){
    fprintf(stderr, "Error allocating memory.\n");
    buffer.size[0] = 0;
    return;
  }

  entry.path.size = FILE_BUFFER_SIZE;
  entry.path.base = malloc(entry.path.size);
  if(!entry.path.base){
    fprintf(stderr, "Error allocating memory.\n");
    free(buffer.base[0]);
    buffer.base[0] = NULL;
    buffer.size[0] = 0;
    return;
  }

  folder.path.base = path;
  folder.path.size = strlen(path);
  folder.meta = opendir(folder.path.base);
  if(!folder.meta){
    error = "Error \"%s\" getting folder information.\n";
    fprintf(stderr, error, strerror(errno));
    free(entry.path.base);
    free(buffer.base[0]);
    buffer.base[0] = NULL;
    buffer.size[0] = 0;
    return;
  }

  while((entry.meta.folder = readdir(folder.meta))){
    name.base = entry.meta.folder->d_name;
    if(strcmp(".", name.base) == 0) continue;
    if(strcmp("..", name.base) == 0) continue;
    name.size = strlen(name.base);

    if(folder.path.base[folder.path.size - 1] == '/'){
      entry.path.n = folder.path.size + name.size + 1;
    }
    else{
      entry.path.n = folder.path.size + name.size + 2;
    }

    if(entry.path.size < entry.path.n){
      entry.path.size = entry.path.n * 2;
      entry.path.base = realloc(entry.path.base, entry.path.size);
      if(!entry.path.base){
        fprintf(stderr, "Error allocating memory.\n");
        free(buffer.base[0]);
        buffer.base[0] = NULL;
        buffer.size[0] = 0;
        return;
      }
    }

    if(folder.path.base[folder.path.size - 1] == '/'){
      if(sprintf(entry.path.base, "%s%s", folder.path.base, name.base) < 0){
        fprintf(stderr, "Error writing folder entry path.\n");
        continue;
      }
    }
    else{
      if(sprintf(entry.path.base, "%s/%s", folder.path.base, name.base) < 0){
        fprintf(stderr, "Error writing folder entry path.\n");
        continue;
      }
    }

    if(stat(entry.path.base, &entry.meta.file) == -1){
      error = "Error \"%s\" getting file information.\n";
      fprintf(stderr, error, strerror(errno));
      continue;
    }

    if(buffer.size[0] < buffer.n + name.size + 2){
      buffer.size[0] = (buffer.n + name.size + 2) * 2;
      buffer.base[0] = realloc(buffer.base[0], buffer.size[0]);
      if(!buffer.base[0]){
        fprintf(stderr, "Error allocating memory.\n");
        free(entry.path.base);
        buffer.size[0] = 0;
        return;
      }
    }

    memcpy(buffer.base[0] + buffer.n, name.base, name.size);
    buffer.n = buffer.n + name.size;
    if(entry.meta.file.st_mode & S_IFDIR){
      buffer.base[0][buffer.n++] = '/';
    }
    buffer.base[0][buffer.n++] = '\n';
  }

  if(buffer.n){
    buffer.size[0] = buffer.n - 1;
  }
  else{
    buffer.size[0] = 0;
  }

  closedir(folder.meta);
  free(entry.path.base);
#endif
}

void fileOpenPath(char *path){
  char *slash;

  slash = path;
  while((slash = strchr(slash + 1, FILE_SEPARATOR))){
    slash[0] = '\0';
    fileFolderSet(path);
    slash[0] = FILE_SEPARATOR;
  }
}
