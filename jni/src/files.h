#ifndef _files_h
#define _files_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#elif linux 
#include <stdarg.h>
#include <sys/stat.h>
#endif
#include "array.h"

#ifdef _WIN32
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#elif linux
#define ACCESS access
#define MKDIR(a) mkdir((a),0755)
#else
#define ACCESS access
#define MKDIR(a) mkdir((a),0755)
#endif

Array * listDir2(const char *path,Array *suffixs);
Array * listDir(const char *path);
size_t fileSize(FILE*stream);
char* readfile(char * path,size_t * fileSize);
int writefile(char * path,char *data,size_t data_length);
const char * App_storageDir();
#endif
