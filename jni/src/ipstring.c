/**
 * @file ipstring.c
 gcc -Wall -I"../SDL2_image/" -I"../SDL2_ttf" -I"include" -L"lib" mysurface.c textfield.c files.c array.c matrix.c tween.c ease.c base64.c ipstring.c sprite.c httploader.c mystring.c lib/libeay32.dll.a lib/libssl32.dll.a -lgdi32 -lwsock32 -lssl -lssl32 -lcrypto -lSDL2_image -lSDL2_ttf -lmingw32 -lSDL2main -lSDL2 -I"../SDL2/include/" -lm -D debug_httploader && a
 *  
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2016-03-18
 */
#include "ipstring.h"

int isIpString(char*host)
{
	char *p= host;
	if(host && strlen(host)>3){
		while((*p != '\0') && (*p != ' '))
		{
			//printf("%c",*p);
			if(*p!='.' && isxdigit(*p)==0){
				return 0;
			}
			p++;
		}
		return 1;
	}
	return 0;
}

char * domain2ipString(char * hostname)
{
	if(isIpString(hostname))
		return hostname;

#ifdef __WIN32__
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested=MAKEWORD(2, 2);
	int ret=WSAStartup(wVersionRequested, &wsaData);
	if(ret!=0)
	{
		return NULL;
	}
#endif
	char * ipStr = NULL;
	char   *ptr, **pptr;
	struct hostent *hptr;
	if(ipStr == NULL){
		ipStr = malloc(32);
		memset(ipStr,0,32);
	}
	ptr = hostname;
	if((hptr = gethostbyname(ptr)) == NULL)
	{
		printf(" gethostbyname error for host:%s\n", ptr);
		free(ipStr);
		return NULL;
	}
	//printf("official hostname:%s\n",hptr->h_name);
	for(pptr = hptr->h_aliases; *pptr != NULL; pptr++)
	{
		//printf(" alias:%s\n",*pptr);
	}

	switch(hptr->h_addrtype)
	{
		case AF_INET:
		case AF_INET6:
			pptr=hptr->h_addr_list;
#ifdef __WIN32__
			memset(ipStr,0,32);
			sprintf(ipStr,"%d.%d.%d.%d",
					(hptr->h_addr_list[0][0]&0x00ff),
					(hptr->h_addr_list[0][1]&0x00ff),
					(hptr->h_addr_list[0][2]&0x00ff),
					(hptr->h_addr_list[0][3]&0x00ff));
#else
			inet_ntop(hptr->h_addrtype, *pptr, ipStr, 32);
			//for(; *pptr!=NULL; pptr++) printf(" address:%s\n", ipStr);
			inet_ntop(hptr->h_addrtype, hptr->h_addr, ipStr, 32);
			//printf("first ip address: %s\n", ipStr);
#endif
			//printf("get ip:%s\n",ipStr);
			if(isIpString(ipStr))
				return ipStr;
			break;
		default:
			printf("unknown address type\n");
			free(ipStr);
			return NULL;
			break;
	}
	//printf("hostname2ipstring: %s\n",ipStr);

	if(isIpString(ipStr))
		return ipStr;
	else{
		free(ipStr);
		ipStr = NULL;
	}
	return NULL;
}
