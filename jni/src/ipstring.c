/**
 * @file ipstring.c
 gcc -Wall -g -D testip array.c mystring.c ipstring.c && ./a.out
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
	if(hostname && isIpString(hostname))
		return hostname;
	//return NULL;

#ifdef __WIN32__
	WORD wVersion;
	WSADATA WSAData;
	wVersion=MAKEWORD(2,2);
	WSAStartup(wVersion,&WSAData);
#endif
	struct addrinfo hints;
	struct addrinfo *res, *cur;
	int ret;
	struct sockaddr_in *addr;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family   = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags    = AI_PASSIVE;

	ret = getaddrinfo(hostname, NULL,&hints,&res);
	if (ret){
		fprintf(stderr,"getaddrinfo:%s --> %s\n", hostname,gai_strerror(ret));
		return NULL;
	}
	for (cur = res; cur != NULL; cur = cur->ai_next) {
		char ipbuf[32];
		memset(ipbuf,0,sizeof(ipbuf));
		addr = (struct sockaddr_in *)cur->ai_addr;
		if(res->ai_family == AF_INET){//IPv4
			inet_ntop(AF_INET, &addr->sin_addr, ipbuf, 16);
			if(isIpString(ipbuf))
			{
				printf("SDL get %s ip %s\n",hostname ,ipbuf);
				freeaddrinfo(res);
				return contact_str(ipbuf,"");
			}
		}if(res->ai_family == AF_INET6){
			// Found IPv6 address
			inet_ntop(AF_INET6,
					&(((struct sockaddr_in *)addr)->sin_addr),
					ipbuf, 32);
			printf("IPv6: %s\n", ipbuf);
			return contact_str(ipbuf,"");
		}
	}
	freeaddrinfo(res);
	return NULL;
}

#ifdef testip
int  main(int argc, char **argv)
{
	domain2ipString("baidu.com");
	return 0;
}
#endif
