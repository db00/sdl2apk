#ifndef _httploader_h
#define _httploader_h

#include <stdio.h>
#include <stdlib.h>
#ifdef linux
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#endif
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#ifdef WIN32
#include <ws2tcpip.h>
#include <winsock2.h>
#include <windows.h>
#endif

#include "base64.h"
#include "urlcode.h"
#include "ipstring.h"
#include "mystring.h"

//https://github.com/guardianproject/openssl-android.git
//#ifdef HEADER_SSL_H 
#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
//#endif


typedef struct RequestData
{
	unsigned int statusCode;
	char * contentType;
	char * server;
	char * date;
	char * Authorization;

	unsigned int contentLength;
	char * data;
}RequestData;

typedef struct URLRequest
{
	char * url;
	char * ip;
	char * host;
	unsigned int statusCode;
	unsigned int port;
	int isHttps;
	char * path;

	char * data;//downloaded data
	int bytesOfData;//downloaded data Length;
	RequestData * request;
	RequestData * respond;
}URLRequest;

int isIpString(char*host);
char * domain2ipString(char * hostname);
RequestData *RequestData_new(char* _s);
void RequestData_parseLine(RequestData*header,char * line);
void RequestData_clear(RequestData *header);
void URLRequest_clear(URLRequest* urlrequest);

URLRequest * URLRequest_new(char *_url);
URLRequest * Httploader_request(URLRequest *urlrequest);
URLRequest * Httploader_load(char *url);
URLRequest * URLRequest_setAuthorization(URLRequest*urlrequest,char*userName,char*pswd);


#endif
