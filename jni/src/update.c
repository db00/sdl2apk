/**
 *
 gcc update.c tween.c ease.c array.c textfield.c base64.c ipstring.c mystring.c sprite.c files.c httploader.c matrix.c -lm -I"../SDL2/include/" -I"../SDL2_image/" -I"../SDL2_ttf/" -lSDL2_ttf -lSDL2_image -lSDL2 -lssl -lcrypto -D debug_update && ./a.out
 */
#include "update.h"

void Update_clear(UPDATE*update)
{
	if(update){
		if(update->data)
		{
			free(update->data);
		}
		free(update);
	}
}
UPDATE * Update_decode(UPDATE*update,char *data)
{
	if(data==NULL || strlen(data)==0)
		return update;
	char *p = strtok(data,"\r\n");
	while(p)
	{
		//SDL_Log("strtok:%s\n",p);
		char * s = "version:";
		char * s1 = "apk:";
		char * s2 = "modify:";
		if(strncmp(s,p,strlen(s))==0){
			update->version = p+strlen(s);
			//SDL_Log("%s == %s\n",s,update->version);
		}else if(strncmp(s1,p,strlen(s1))==0){
			update->path= p+strlen(s1);
			//SDL_Log("%s == %s\n",s1,update->path);
		}else if(strncmp(s2,p,strlen(s2))==0){
			update->modify= p+strlen(s2);
			//SDL_Log("%s == %s\n",s2,update->modify);
		}
		p = strtok(NULL,"\r\n");
	}
	return update;
}
void Update_loadApk(UPDATE*update)
{
	char *url = update->path;
	URLRequest * urlrequest = Httploader_load(url);
	if(urlrequest->statusCode == 200){
		char filename[128];
		memset(filename,0,128);
#ifdef __ANDROID__
		sprintf(filename,"/sdcard/update_%s",(char*)(strrchr(update->path,'/')+1));
#else
		sprintf(filename,"../../update_%s",(char*)(strrchr(update->path,'/')+1));
#endif
		if(writefile(filename,urlrequest->data,urlrequest->respond->contentLength)==0)
		{
			char a[1024];
			memset(a,0,1024);
			sprintf(a,"pm install -r %s &",filename);
			SDL_Log("%s\n",a);
#ifdef __ANDROID__
			//system(a);
#endif
		}
	}
}

void Update_init()
{
	char*cur_data = NULL;
#ifdef __ANDROID__
	cur_data = sdlreadfile("version.txt",NULL);
#else
	cur_data = readfile("../../assets/version.txt",NULL);
#endif
	if(cur_data){
		SDL_Log("LOCAL VERSION_DATA:%s",cur_data);
		Update_decode(&cur,cur_data);
		cur.data = cur_data;
	}else{
		SDL_Log("XXXXXXXXXX read version file ERROR!\n");
		//Update_clear(&cur);
		return;
	}
	char *url = "https://git.oschina.net/db0/SDL2apk/raw/master/assets/version.txt";
	URLRequest * urlrequest = Httploader_load(url);
	if(urlrequest && urlrequest->statusCode == 200 && urlrequest->data && strlen(urlrequest->data)){
		if(urlrequest->respond->contentLength == strlen(urlrequest->data))
		{
			if(urlrequest->data)
			{
				int len = strlen(urlrequest->data);
				char * net_data = (char*)malloc(len+1);
				memset(net_data,0,len+1);
				memcpy(net_data,urlrequest->data,len);
				Update_decode(&net,net_data);
				net.data = net_data;
			}else{
				//Update_clear(&cur);
				//Update_clear(&net);
				URLRequest_clear(urlrequest);
				return;
			}
			if(strcmp(cur.version,net.version))
			{
				SDL_Log("has new version:%s , url:%s\n",net.version,net.path);
				Update_loadApk(&net);
			}else{
				SDL_Log("no new version\n");
			}
			//SDL_Log("repond data:\n%s\n",urlrequest->data);
			//SDL_Log("repond data:\n%d\n",urlrequest->respond->contentLength);
			fflush(stdout);
		}
		URLRequest_clear(urlrequest);
		urlrequest = NULL;
	}else{
		SDL_Log("%s ERROR!\n",url);
	}
}

#ifdef debug_update
int main()
{
	Update_init();
	return 0;
}
#endif


