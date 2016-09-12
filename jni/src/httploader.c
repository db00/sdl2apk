/**
 *
 gcc -Wall -I"../SDL2_image/" -I"../SDL2_ttf" mysurface.c urlcode.c textfield.c files.c array.c matrix.c tween.c ease.c base64.c ipstring.c sprite.c httploader.c mystring.c  -lssl -lcrypto  -lSDL2_image -lSDL2_ttf -lSDL2 -I"../SDL2/include/" -lm -D debug_httploader &&./a.out
 gcc -Wall -I"../SDL2_image/" -I"../SDL2_ttf" -I"include" mysurface.c textfield.c files.c array.c matrix.c tween.c ease.c base64.c ipstring.c sprite.c httploader.c mystring.c -L"lib" -lssl -lcrypto -lwsock32 -lgdi32 -lSDL2_image -lSDL2_ttf -lmingw32 -lSDL2main -lSDL2 -I"../SDL2/include/" -lm -D debug_httploader && a
 > a.txt
 */
#include "httploader.h"


/**
 *
 * send a string (sendStr) to fd, 
 * return a server repond
 * contentLength save number of bytes of the respond data 
 */
char *ssls(int fd,char*sendStr,int*contentLength)
{
#ifdef HEADER_SSL_H 
	int n,ret;
	SSL *ssl;
	SSL_CTX *contex;
	SSL_load_error_strings();
	SSL_library_init();
	contex = SSL_CTX_new(SSLv23_client_method());
	if ( contex == NULL ){
		printf("init SSL contex failed:%s\n", ERR_reason_error_string(ERR_get_error()));
	}
	ssl = SSL_new(contex);
	if ( ssl == NULL ){
		printf("new SSL with created contex failed:%s\n", ERR_reason_error_string(ERR_get_error()));
	}
	ret = SSL_set_fd(ssl, fd);
	if ( ret == 0 ){
		printf("add SSL to tcp socket failed:%s\n", ERR_reason_error_string(ERR_get_error()));
	}
	/* PRNG */
	RAND_poll();
	while ( RAND_status() == 0 ){
		unsigned short rand_ret = rand() % 65536;
		RAND_seed(&rand_ret, sizeof(rand_ret));
	}
	/* SSL Connect */
	ret = SSL_connect(ssl);
	if( ret != 1 ){
		printf("SSL connection failed:%s\n", ERR_reason_error_string(ERR_get_error()));
	}
	// https socket write.
	SSL_write(ssl, sendStr, strlen(sendStr));
	char *data = malloc(1024);
	memset(data,0,1024);
	int received = 0;
	while((n = SSL_read(ssl, data+received, 1024-1)) > 0){
		data[(received+n)] = '\0';
		received += n;
		data = realloc(data,received+1024);
	}
	if(contentLength)*contentLength = received;
	if(n != 0){
		printf("SSL read failed:%s\n", ERR_reason_error_string(ERR_get_error()));
	}
	// close ssl tunnel.
	ret = SSL_shutdown(ssl); 
	if( ret != 1 ){
		close(fd);
		printf("SSL shutdown failed:%s\n", ERR_reason_error_string(ERR_get_error()));
	}

	// clear ssl resource.
	SSL_free(ssl); 
	SSL_CTX_free(contex);
	ERR_free_strings();
	return data;
#endif
	return NULL;
}

void RequestData_parseLine(RequestData*header,char * line)
{
	if(line==NULL)return;
	//printf("%s\n",line);
	char * s = "Content-Type: ";
	if(strncasecmp(line,s,strlen(s))==0)
	{
		printf("%s---->%s\n",s,line+strlen(s));
		header->contentType = line + strlen(s);
		return;
	}
	s = "HTTP/1.";
	if(strncasecmp(line,s,strlen(s))==0)
	{
		header->statusCode = atoi(line + strlen(s)+2);
		printf("\n%s---->%d\n",s,header->statusCode);
		return;
	}
	s = "Content-Length: ";
	if(strncasecmp(line,s,strlen(s))==0)
	{
		printf("%s---->%s\n",s,line+strlen(s));
		header->contentLength= atoi(line + strlen(s));
		return;
	}
	s = "Date: ";
	if(strncasecmp(line,s,strlen(s))==0)
	{
		//printf("%s---->%s\n",s,line+strlen(s));
		header->date= (line + strlen(s));
		return;
	}
	s = "Authorization: ";
	if(strncasecmp(line,s,strlen(s))==0)
	{
		printf("%s---->%s\n",s,line+strlen(s));
		header->Authorization = (line + strlen(s));
		return;
	}
}

RequestData *RequestData_new(char* _s)
{
	RequestData* header = malloc(sizeof(*header));
	memset(header,0,sizeof(*header));
	if(_s == NULL)return header;
	char * data = _s;
	sprintf(data,"%s",_s);
	char * s = strtok(data,"\r\n");
	while(s){
		RequestData_parseLine(header,s);
		s = strtok(NULL,"\r\n");
	}
	return header;
}
void RequestData_clear(RequestData *header)
{
	if(header)
	{
		if(header->Authorization)
		{
			free(header->Authorization);
		}
		if(header->data)
		{
			free(header->data);
		}
		free(header);
	}
}
void URLRequest_clear(URLRequest* urlrequest)
{
	if(urlrequest){
		if(urlrequest->path){
			free(urlrequest->path);
			urlrequest->path = NULL;
		}
		if(urlrequest->data){
			free(urlrequest->data);
			urlrequest->data= NULL;
		}
		if(urlrequest->request){
			RequestData_clear(urlrequest->request);
			urlrequest->request= NULL;
		}
		if(urlrequest->respond){
			RequestData_clear(urlrequest->respond);
			urlrequest->respond= NULL;
		}
		free(urlrequest);
	}

}

URLRequest * URLRequest_new(char *_url)
{
	printf("%s\n",_url);
	URLRequest *urlrequest = malloc(sizeof(URLRequest));
	memset(urlrequest,0,sizeof(URLRequest));
	urlrequest->url = _url;
	urlrequest->path = append_str(NULL,"/");
	if(_url == NULL)return urlrequest;
	if(_url){
		char * url = malloc(strlen(_url)+1);
		urlrequest->url = url;
		*(url + strlen(_url))='\0';
		memcpy(url,_url,strlen(_url));

		char * s = "http://";
		int len = strlen(s);
		char *p = url + len ;

		if(strncasecmp(url,s,len)==0)
		{
			urlrequest->host = p;
			char* start = strstr(p,"/");
			if(start){
				*start= '\0';
				p = start + 1;
			}
			start = strstr(urlrequest->host,":");
			if(start){
				*start = '\0';
				urlrequest->port = atoi(start+1);
			}else{
				urlrequest->port = 80;
			}

			if(urlrequest->host && isIpString(urlrequest->host)==0){
				urlrequest->ip = domain2ipString(urlrequest->host);
				if(urlrequest->ip == NULL){
					printf("network ERROR! XXXXXXXXXXXXXXXXX\n");
					urlrequest->port = 0;
					return urlrequest;
				}
			}else if(urlrequest->host){
				urlrequest->ip = malloc(strlen(urlrequest->host)+1);
				memset(urlrequest->ip,0,strlen(urlrequest->host)+1);
				memcpy(urlrequest->ip,urlrequest->host,strlen(urlrequest->host));
			}else{
				printf("not found host!\n");
			}
		}else{
			s = "https://";
			len = strlen(s);
			p = url + len ;

			if(strncasecmp(url,s,len)==0)
			{
				urlrequest->isHttps = 1;
				urlrequest->host = p;
				char* start = strstr(p,"/");
				if(start){
					*start= '\0';
					p = start + 1;
				}
				start = strstr(urlrequest->host,":");
				if(start){
					*start = '\0';
					urlrequest->port = atoi(start+1);
				}else{
					urlrequest->port = 443;
				}

				if(urlrequest->host && isIpString(urlrequest->host)==0){
					urlrequest->ip = domain2ipString(urlrequest->host);
					if(urlrequest->ip == NULL){
						printf("network ERROR! XXXXXXXXXXXXXXXXX\n");
						urlrequest->port = 0;
						return urlrequest;
					}
				}else if(urlrequest->host){
					urlrequest->ip = malloc(strlen(urlrequest->host)+1);
					memset(urlrequest->ip,0,strlen(urlrequest->host)+1);
					memcpy(urlrequest->ip,urlrequest->host,strlen(urlrequest->host));
				}else{
					printf("not found host!\n");
				}
			}else{
				printf("protocal not support!\n");
			}
		}


		if(p - url < strlen(_url))
		{
			char * encodedPath = url_encode(p,strlen(p),NULL,1);
			char * path = contact_str(urlrequest->path,encodedPath);
			free(urlrequest->path);
			free(encodedPath);
			urlrequest->path = path;
		}

		printf("urlrequest->host:%s\n",urlrequest->host);
		printf("urlrequest->port:%d\n",urlrequest->port);
		printf("urlrequest->path:%s\n",urlrequest->path);
		printf("urlrequest->ip:%s\n",urlrequest->ip);
	}
	if(urlrequest->ip == NULL)
		urlrequest->port = 0;
	return urlrequest;
}

URLRequest * Httploader_request(URLRequest *urlrequest)
{
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested=MAKEWORD(2, 2);
	int ret=WSAStartup(wVersionRequested, &wsaData);
	if(ret!=0)
	{
		printf("WSAStartup() failed!\n");
		return urlrequest;
	}
#endif

	if(urlrequest==NULL)
		return NULL;
	if(urlrequest->ip==NULL || urlrequest->path==NULL || urlrequest->port==0){
		printf("get ip or port failed!\n");
		return urlrequest;
	}

	char *sendStr= NULL;
	if(urlrequest->isHttps){
		sendStr = append_str(NULL,"GET %s HTTP/1.0\r\nHost: %s\r\n",
				urlrequest->path,
				urlrequest->host
				);
	}else{
		sendStr = append_str(NULL,"GET %s HTTP/1.0\r\nHost: %s",
				urlrequest->path,
				urlrequest->host
				,urlrequest->port
				);
		if(urlrequest->port==80){
			sendStr = append_str(sendStr,"\r\n");
		}else{
			sendStr = append_str(sendStr,":%d\r\n"
					,urlrequest->port
					);
		}
	}

	if(urlrequest->request){
		if( urlrequest->request->Authorization){
			printf("Authorization:\r\n%s\r\n", urlrequest->request->Authorization);fflush(stdout);
			// Authorization: Basic a29kaTpzYmhhbWU=\r\n",
			sendStr = append_str(sendStr ,"Authorization: %s\r\n",
					urlrequest->request->Authorization
					);
		}

		if(urlrequest->request->data && urlrequest->request->contentLength>0){
			printf("contentLength:%d\r\n",urlrequest->request->contentLength);
			sendStr = append_str(sendStr,"Content-Length: %d\r\n",urlrequest->request->contentLength);
		}
	}
	sendStr = append_str(sendStr,"\r\n");
	if(urlrequest->request && urlrequest->request->data && urlrequest->request->contentLength>0){
		sendStr = append_str(sendStr,"%s\r\n\r\n",urlrequest->request->data);
	}

	printf("send:\r\n%s",sendStr);fflush(stdout);

	int sock;
	struct sockaddr_in echoserver;
	unsigned int echolen;

	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		printf("Failed to create socket");
		return urlrequest;
	}

	memset(&echoserver, 0, sizeof(echoserver));       /* Clear struct */
	echoserver.sin_family = AF_INET;                  /* Internet/IP */
	echoserver.sin_addr.s_addr = inet_addr(urlrequest->ip);  /* IP address */
	echoserver.sin_port = htons(urlrequest->port);       /* server port */
	if (connect(sock, (struct sockaddr *) &echoserver, sizeof(echoserver)) < 0) {
		printf("Failed to connect with server\n");
		return urlrequest;
	}else{
		printf("Connectted\n");
	}

	int received = 0;
	char *buffer = NULL;
	if(urlrequest->isHttps)
	{
		buffer = ssls(sock,sendStr,&received);
		if(buffer==NULL)
			return urlrequest;
	}else{
		echolen = strlen(sendStr);
		if (send(sock, sendStr, echolen, 0) != echolen) {
			printf("Mismatch in number of sent bytes");
			return urlrequest;
		}else{
			//printf("sent =====>\n%s---------------\n",sendStr);
		}

		const int _PAGE_SIZE = 64;//
		int numPage = 1;
		buffer = (char*)malloc(numPage*_PAGE_SIZE);
		memset(buffer,0,numPage*_PAGE_SIZE);
		fprintf(stdout, "Received: \n");

		int bytes = 0;
		while(1) {
			while(received+_PAGE_SIZE+1 >= numPage*_PAGE_SIZE){
				++numPage;
				//printf("numPage:%d\n",numPage);
				buffer = (char*)realloc(buffer,numPage*_PAGE_SIZE);
			}
			bytes = 0;
			if ((bytes = recv(sock, (char*)(buffer + received), numPage*_PAGE_SIZE - (received+1), 0)) <= 0) {
				printf("recv error:%d!!!!!!!!!!!\n",bytes);
				//printf("Failed to receive bytes from server %d\n",received);
				//return urlrequest;
				break;
			}
			received += bytes;
			buffer[received] = '\0';
			//fprintf(stdout, buffer);
			//fprintf(stdout, "received:%d\n",received);
		}
	}
	//printf("%s\n",buffer);fflush(stdout);

	char * headend = strstr(buffer,"\r\n\r\n");
	if(headend){ printf("header recv success!"); }
	//printf("=================================headend:%s\n",headend);
	//char * headend = memstr(buffer,numPage*_PAGE_SIZE,"\r\n\r\n");

	if(headend){
		*headend='\0';

		if(urlrequest->respond){
			RequestData_clear(urlrequest->respond);
			urlrequest->respond = NULL;
		}
		urlrequest->respond = RequestData_new(buffer);
		//printf("-------------------------bytesOfData:%d\n",urlrequest->respond->contentLength);
		fflush(stdout);
		//urlrequest->respond->contentLength = urlrequest->bytesOfData;
		urlrequest->statusCode = urlrequest->respond->statusCode;
		int dataReceived = 0;
		if(urlrequest->respond->contentLength > 0){
			urlrequest->bytesOfData = urlrequest->respond->contentLength;
			urlrequest->data = malloc(urlrequest->respond->contentLength+1);
			memset(urlrequest->data,0,urlrequest->respond->contentLength+1);
			if(urlrequest->isHttps){
				memcpy(urlrequest->data,headend+4,urlrequest->bytesOfData);
			}else{
				dataReceived = received-(headend-buffer)-4;
				memcpy(urlrequest->data,headend+4,dataReceived);
				while(dataReceived < urlrequest->respond->contentLength)
				{
					int bytes = 0;
					if ((bytes = recv(sock, urlrequest->data + dataReceived, urlrequest->respond->contentLength - dataReceived, 0)) < 1) {
						printf("recv data error!!!!!!!!!!!\n");
						break;
					}
					dataReceived += bytes;
				}
			}
		}else{
			dataReceived = received-(headend-buffer)-4;
			if(dataReceived > 0)
			{
				urlrequest->respond->contentLength = dataReceived;
				urlrequest->bytesOfData = urlrequest->respond->contentLength;
				urlrequest->data = malloc(urlrequest->respond->contentLength+1);
				memset(urlrequest->data,0,urlrequest->respond->contentLength+1);
				memcpy(urlrequest->data,headend+4,dataReceived);
			}
		}
	}else{
		fprintf(stdout, "no header\n");
	}
	//fprintf(stdout, "\n");
	close(sock);
	return urlrequest;
}

URLRequest * Httploader_load(char *url)
{
	if(url == NULL)
		return NULL;
	URLRequest *urlrequest = URLRequest_new(url);
	return Httploader_request(urlrequest);
}


URLRequest*URLRequest_setAuthorization(URLRequest*urlrequest,char*userName,char*pswd)
{
	if(urlrequest==NULL)
		return NULL;
	if(urlrequest->request == NULL)
		urlrequest->request = RequestData_new(NULL);
	if(urlrequest->request->Authorization){
		free(urlrequest->request->Authorization);
		urlrequest->request->Authorization = NULL;
	}
	char *authorization =  append_str(NULL,"%s:%s",userName,pswd);
	char *Authorization= base64_encode(authorization,strlen(authorization));
	free(authorization);
	urlrequest->request->Authorization = append_str(NULL,"Basic %s",Authorization);
	free(Authorization);
	return urlrequest;
}


#ifdef debug_httploader
#include "mysurface.h"
//#include "sprite.h"
int main(int argc, char *argv[]) 
{

	Stage_init(1);
	URLRequest * urlrequest = NULL;
	int statusCode;
#if 1
	Sprite * sprite = Sprite_new();
	sprite->surface = Httploader_loadimg("http://res1.huaien.com/images/tx.jpg");
	Sprite_addChild(stage,sprite);

	//URLRequest * urlrequest = Httploader_load("http://zp.huaien.com/AjaxFlashTXY/GetTXYInfo.do?spaceId=100016");
	// http://picture.youth.cn/qtdb/201508/W020150811198543071000.jpg
	urlrequest = Httploader_load("https://www.baidu.com/");
	statusCode = urlrequest->statusCode;
	if((statusCode >= 200 && statusCode<300) || statusCode==304){
		if(urlrequest->respond->contentLength == strlen(urlrequest->data))
		{
			printf("repond data:\n%s\n",urlrequest->data);
			printf("repond data:\n%d\n",urlrequest->respond->contentLength);
			fflush(stdout);
		}
	}
	URLRequest_clear(urlrequest);
	urlrequest = NULL;
#endif

	//URLRequest *urlrequest = URLRequest_new("http://127.0.0.1:8080/jsonrpc?request={\"method\":\"Application.Quit\",\"params\":{},\"id\":1,\"jsonrpc\":\"2.0\"}");
	//URLRequest *urlrequest = URLRequest_new("http://127.0.0.1:8080/jsonrpc?request={\"method\":\"System.Reboot\",\"params\":{},\"id\":1,\"jsonrpc\":\"2.0\"}");
	//URLRequest *urlrequest = URLRequest_new("http://127.0.0.1:8080/jsonrpc?request={\"method\":\"System.EjectOpticalDrive\",\"params\":{},\"id\":1,\"jsonrpc\":\"2.0\"}");
	//URLRequest *urlrequest = URLRequest_new("http://127.0.0.1:8080/jsonrpc?request={\"method\":\"Addons.GetAddons\",\"params\":{},\"id\":1,\"jsonrpc\":\"2.0\"}");
	//URLRequest *urlrequest = URLRequest_new("http://127.0.0.1:8080/jsonrpc?request={\"method\":\"Addons.GetAddonDetails\",\"params\":{\"addonid\":\"metadata.common.fanart.tv\"},\"id\":1,\"jsonrpc\":\"2.0\"}");
	//URLRequest *urlrequest = URLRequest_new("http://127.0.0.1:8809/");
	//URLRequest *urlrequest = URLRequest_new("http://127.0.0.1:8809/?f=nohup&p1=kodi");
	//URLRequest *urlrequest = URLRequest_new("http://127.0.0.1:8080/jsonrpc?request={\"method\":\"Input.SendText\",\"params\":{\"text\":\"Kodi\",\"done\":true},\"id\":1,\"jsonrpc\":\"2.0\"}");
	//URLRequest *urlrequest = URLRequest_new("http://127.0.0.1:8080/jsonrpc?request={\"method\":\"Input.ExecuteAction\",\"params\":{\"action\":\"back\"},\"id\":1,\"jsonrpc\":\"2.0\"}");
	//URLRequest *urlrequest = URLRequest_new("http://127.0.0.1:8080/jsonrpc?request={\"method\":\"Playlist.GetPlaylists\",\"params\":{},\"id\":1,\"jsonrpc\":\"2.0\"}");
	//URLRequest_setAuthorization(urlrequest,"kodi","sbhame");
	//URLRequest_setAuthorization(urlrequest,"test","test");
	urlrequest = URLRequest_new("http://xh.5156edu.com/xhzdmp3abc/nü3.mp3");
	urlrequest = Httploader_request(urlrequest);
	statusCode = urlrequest->statusCode;
	if((statusCode >= 200 && statusCode<300) || statusCode==304){
		if(urlrequest->respond->contentLength == strlen(urlrequest->data))
		{
			printf("repond data:\n%s\n",urlrequest->data);
			printf("repond datalength:\n%d\n",urlrequest->respond->contentLength);
			fflush(stdout);
		}
	}
	URLRequest_clear(urlrequest);
	urlrequest = NULL;
	Stage_loopEvents();
	exit(0);
	return 0;
}
#endif
