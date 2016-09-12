/**
 * @file ipstring.c
 gcc -Wall -I"../SDL2_image/" -I"../SDL2_ttf" -I"include" -L"lib" mysurface.c textfield.c files.c array.c matrix.c tween.c ease.c base64.c ipstring.c sprite.c httploader.c mystring.c lib/libeay32.dll.a lib/libssl32.dll.a -lgdi32 -lws2_32 -lwsock32 -lssl -lssl32 -lcrypto -lSDL2_image -lSDL2_ttf -lmingw32 -lSDL2main -lSDL2 -I"../SDL2/include/" -lm -D debug_httploader && a
 */
#ifndef __ipstring_h
#define __ipstring_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#ifdef linux
#include <netdb.h>
#include <arpa/inet.h>
#else
#ifdef __WIN32__
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock.h>
//#include <windows.h>
//#include <Ws2tcpip.h>
#endif
#endif

int isIpString(char*host);
char * domain2ipString(char * hostname);
#endif
