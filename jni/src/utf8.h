#ifndef utf8_h
#define utf8_h


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>

#include "mystring.h"
#include "array.h"

int UTF8_numByte(char * utf8);
size_t UTF8_length(char * utf8);
Array * UTF8_each(const char * utf8);
char * UTF8_readFrom(char* inbuf,size_t * outlen,char * coding);
char * UTF8_encodeTo(char* inbuf,size_t * outlen,char * coding);

#endif
