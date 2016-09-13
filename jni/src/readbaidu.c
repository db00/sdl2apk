/**
 * @file readbaidu.c
 gcc -I"../SDL2_image/" -I"../SDL2_ttf" -I"../SDL2_mixer/" readbaidu.c urlcode.c ipstring.c files.c matrix.c array.c tween.c ease.c base64.c sprite.c httploader.c mystring.c -lssl -lcrypto  -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lSDL2 -I"../SDL2/include/" -lm -D debug_readloader && ./a.out
 diagram 英式与美式发音
http://fanyi.baidu.com/gettts?lan=uk&text=diagram&spd=2&source=alading
http://fanyi.baidu.com/gettts?lan=en&text=diagram&spd=2&source=alading
http://tts.baidu.com/text2audio?lan=zh&pid=101&ie=UTF-8&text=%E7%99%BE
 *  
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2015-08-18
 */

#include "readbaidu.h"



int Sound_init()
{
	/* Open the audio device */
	//if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 512) < 0)
	//if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,AUDIO_U16LSB, 2, 512) < 0)
	//if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,AUDIO_S16LSB, 2, 512) < 0)
	//if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,AUDIO_U16MSB, 2, 512) < 0)
	//if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,AUDIO_S16MSB, 2, 512) < 0)
	//if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,AUDIO_S16, 2, 512) < 0)
	//if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,AUDIO_S32LSB, 2, 512) < 0)
	//if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,AUDIO_U16SYS, 2, 512) < 0)
	//if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,AUDIO_S16SYS, 2, 512) < 0)
	//if (Mix_OpenAudio(11025,AUDIO_S16SYS, 2, 512) < 0)
	//if (Mix_OpenAudio(11025,MIX_DEFAULT_FORMAT, 2, 4096) < 0)
	if (Mix_OpenAudio(16000,MIX_DEFAULT_FORMAT, 2, 4096) < 0)
		//if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,AUDIO_U16, 2, 512) < 0)
	{
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		return 2;
	}
	/* Set the music volume */
	Mix_VolumeMusic(MIX_MAX_VOLUME);
	/* Set the external music player, if any */
	Mix_SetMusicCMD(SDL_getenv("MUSIC_CMD"));
	return 0;
}



void Sound_clear()
{
	if( Mix_PlayingMusic() ) {
		Mix_FadeOutMusic(1500);
		SDL_Delay(1500);
	}
	Mix_CloseAudio();
}

int Sound_playData(char * data,int data_length)
{
	Sound_init();
	Mix_Music *music = NULL;
	while(1)
	{
		/* Load the requested music file */
		music = Mix_LoadMUS_RW(SDL_RWFromConstMem(data,data_length), SDL_TRUE);
		if(music){
			if(Mix_PlayMusic(music,0) == -1)
			{
				fprintf(stderr,"play failure ,%s\n",(char*)Mix_GetError());  
				break;
			}
			while(Mix_PlayingMusic() ) {
				SDL_Delay(100);
			}
			Mix_FreeMusic(music);
			music = NULL;
		}else{
			Sound_clear();
			return 1;
		}
		break;
	}
	Sound_clear();
	return 0;
}


int Sound_playFile(char * fileName)
{
	Sound_init();
	Mix_Music *music = NULL;
	while(1)
	{
		/* Load the requested music file */
		//music = Mix_LoadMUS_RW(SDL_RWFromConstMem(data,data_length), SDL_TRUE);
		if ( music == NULL ) {
			music = Mix_LoadMUS(fileName);
			if(music==NULL){
				fprintf(stderr,"Load %s failure ,%s\n",fileName,(char*)Mix_GetError());
				char * cmd = malloc(1024);
				memset(cmd,0,1024);
#ifdef __ANDROID__
				sprintf(cmd,"am start -n com.android.music/.MediaPlaybackActivity -d %s &",fileName);
#else
				sprintf(cmd,"mplayer %s ",fileName);
#endif
				system(cmd);
				free(cmd);
				break;
			}
		}

		if(music){
			if(Mix_PlayMusic(music,0) == -1)
			{
				fprintf(stderr,"play %s failure ,%s\n",fileName,(char*)Mix_GetError());  
				break;
			}
			while(Mix_PlayingMusic() ) {
				SDL_Delay(100);
			}
			Mix_FreeMusic(music);
			music = NULL;
		}
		break;
	}
	Sound_clear();
	return 0;
}

static SDL_mutex *mutex = NULL;

int Sound_playUrl(void *url,char * _name)
{
	printf("Sound_playUrl:%s,%s\n",(char*)url,_name);
	URLRequest * urlrequest = Httploader_load((char*)url);
	if(urlrequest
			&& urlrequest->statusCode == 200 
			&& urlrequest->data)
	{
		char *data = urlrequest->data;
		size_t data_length = urlrequest->respond->contentLength;


		if (SDL_LockMutex(mutex) < 0) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't lock mutex: %s", SDL_GetError());
			URLRequest_clear(urlrequest);
			return 1;
		}

		char * fileName = malloc(strlen(url));
		memset(fileName,0,strlen(url));
#ifdef __ANDROID__
		sprintf(fileName,"/sdcard/%s.mp3",_name);
#else
		sprintf(fileName,"sound/%s.mp3",_name);
#endif
		printf("\n%s\n",fileName);

		if(Sound_playData(data,data_length)==0)
		{
			printf("data play successfully!\n");
		}else{
			if(writefile(fileName,data,data_length)==0) {
				printf("writefile successfully!\n");
			}
			Sound_playFile(fileName);
		}

		free(fileName);
		if (SDL_UnlockMutex(mutex) < 0) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't unlock mutex: %s", SDL_GetError());
			URLRequest_clear(urlrequest);
			return 1;
		}
	}
	URLRequest_clear(urlrequest);
	return 0;
}

int Sound_play(void *url)
{
	char * fileName = malloc(strlen(url));
	memset(fileName,0,strlen(url));
	char * p1 = strstr(url,"&text=");
	char * p2 = url+strlen(url);
	char _name[64];
	if(p1!=NULL)
	{
		p1 += strlen("&text=");
		memset(_name,0,64);
		snprintf(_name,p2-p1+1,"%s",p1);
	}else{
		return 2;
	}

	Sound_playUrl(url,_name);
	return 0;
}


/**
 *
 * type:1 uk
 * 		2 us
 */
void READ_loadSound(char *word,int type)
{
	if(mutex==NULL){
		if ((mutex = SDL_CreateMutex()) == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create mutex: %s\n", SDL_GetError());
			return ;
		}
	}

	SDL_Thread *thread;
	static char url[1024];
	memset(url,0,1024);
	char *_type;
	if(type==1){
		_type="uk";
	}else if(type==2){
		_type="en";
	}else{
		sprintf(url,"http://tts.baidu.com/text2audio?lan=zh&pid=101&ie=UTF-8&text=%s",word);
		thread = SDL_CreateThread(Sound_play, "Sound_play", (void *)url);
		if(thread)
			SDL_DetachThread(thread);
		return;
	}
	sprintf(url,"http://fanyi.baidu.com/gettts?lan=%s&spd=2&source=alading&text=%s",_type,word);
	thread = SDL_CreateThread(Sound_play, "Sound_play", (void *)url);
	if(thread)
		SDL_DetachThread(thread);
	return;
	/*
	   if (NULL == thread) {
	   printf("\nSDL_CreateThread failed: %s\n", SDL_GetError());
	   } else {
	   int threadReturnValue;
	   SDL_WaitThread(thread, &threadReturnValue);
	   printf("\nThread returned value: %d", threadReturnValue);
	   }
	   */
}



#ifdef debug_readloader
int main(int argc, char *argv[])
{
	if ( SDL_Init(SDL_INIT_AUDIO) < 0 ) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		return(255);
	}
	if(mutex==NULL){
		if ((mutex = SDL_CreateMutex()) == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create mutex: %s\n", SDL_GetError());
			return 0;
		}
	}
#if 0
	//阿
	Sound_playFile("sound/pinyin/ni3.mp3");
	Sound_playFile("sound/pinyin/hao3.mp3");
	Sound_playFile("sound/pinyin/a1.mp3");
	//SDL_Delay(100); return 0;

	char c = 'a';
	char s[2];
	s[1]='\0';
	while(c<='z')
	{
		s[0]=c++;
		READ_loadSound(s,1);
	}
	//READ_loadSound("help",1);
	//READ_loadSound("工",0);
	//SDL_Delay(2000); return 0;

#endif
	//char * arr[] = {"a","ai","an","ang","ao","ba","bai","ban","bang","bao","bei","ben","beng","bi","bian","biao","bie","bin","bing","bo","bu","ca","cai","can","cang","cao","ce","cen","ceng","cha","chai","chan","chang","chao","che","chen","cheng","chi","chong","chou","chu","chua","chuai","chuan","chuang","chui","chun","chuo","ci","cong","cou","cu","cuan","cui","cun","cuo","da","dai","dan","dang","dao","de","den","dei","deng","di","dia","dian","diao","die","ding","diu","dong","dou","du","duan","dui","dun","duo","e","ei","en","eng","er","fa","fan","fang","fei","fen","feng","fo","fou","fu","ga","gai","gan","gang","gao","ge","gei","gen","geng","gong","gou","gu","gua","guai","guan","guang","gui","gun","guo","ha","hai","han","hang","hao","he","hei","hen","heng","hong","hou","hu","hua","huai","huan","huang","hui","hun","huo","ji","jia","jian","jiang","jiao","jie","jin","jing","jiong","jiu","ju","juan","jue","jun","ka","kai","kan","kang","kao","ke","ken","keng","kong","kou","ku","kua","kuai","kuan","kuang","kui","kun","kuo","la","lai","lan","lang","lao","le","lei","leng","li","lia","lian","liang","liao","lie","lin","ling","liu","long","lou","lu","lü","luan","lue","lüe","lun","luo","m","ma","mai","man","mang","mao","me","mei","men","meng","mi","mian","miao","mie","min","ming","miu","mo","mou","mu","na","nai","nan","nang","nao","ne","nei","nen","neng","ng","ni","nian","niang","niao","nie","nin","ning","niu","nong","nou","nu","nü","nuan","nüe","nuo","nun","o","ou","pa","pai","pan","pang","pao","pei","pen","peng","pi","pian","piao","pie","pin","ping","po","pou","pu","qi","qia","qian","qiang","qiao","qie","qin","qing","qiong","qiu","qu","quan","que","qun","ran","rang","rao","re","ren","reng","ri","rong","rou","ru","ruan","rui","run","ruo","sa","sai","san","sang","sao","se","sen","seng","sha","shai","shan","shang","shao","she","shei","shen","sheng","shi","shou","shu","shua","shuai","shuan","shuang","shui","shun","shuo","si","song","sou","su","suan","sui","sun","suo","ta","tai","tan","tang","tao","te","teng","ti","tian","tiao","tie","ting","tong","tou","tu","tuan","tui","tun","tuo","wa","wai","wan","wang","wei","wen","weng","wo","wu","xi","xia","xian","xiang","xiao","xie","xin","xing","xiong","xiu","xu","xuan","xue","xun","ya","yan","yang","yao","ye","yi","yin","ying","yo","yong","you","yu","yuan","yue","yun","za","zai","zan","zang","zao","ze","zei","zen","zeng","zha","zhai","zhan","zhang","zhao","zhe","zhei","zhen","zheng","zhi","zhong","zhou","zhu","zhua","zhuai","zhuan","zhuang","zhui","zhun","zhuo","zi","zong","zou","zu","zuan","zui","zun","zuo",NULL};
	char * arr[] = {"ge",NULL};
	//char * format = "http://xh.5156edu.com/xhzdmp3abc/%s%d.mp3";
	//char * format2 = "http://xh.5156edu.com/xhzdmp3abc/%s.mp3";
	char * format = "http://appcdn.fanyi.baidu.com/zhdict/mp3/%s%d.mp3";
	char * format2 = "http://appcdn.fanyi.baidu.com/zhdict/mp3/%s.mp3";
	int i=0;
	while(1)
	{
		char * w = arr[i];
		if(w==NULL)
			break;
		char _url[128];
		char name2[16];
		int j=0;
		while(j<4)
		{
			memset(_url,0,128);
			memset(name2,0,16);
			sprintf(_url,format,w,++j);
			sprintf(name2,"%s%d",w,j);
			Sound_playUrl(_url,name2);
		}
		memset(_url,0,128);
		sprintf(_url,format2,w);
		Sound_playUrl(_url,w);
		++i;
	}
	SDL_Delay(1000);
	return 0;
}
#endif
