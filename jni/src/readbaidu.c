/**
 * @file readbaidu.c
 gcc -g -I"../SDL2_image/" -I"../SDL2_ttf" -I"../SDL2_mixer/" myregex.c readbaidu.c music.c urlcode.c ipstring.c files.c matrix.c array.c tween.c ease.c base64.c sprite.c httploader.c mystring.c -lssl -lcrypto  -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lSDL2 -I"../SDL2/include/" -lm -D debug_readloader && ./a.out
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



char * getEngUrl(char * s,int type)
{
	char * url = NULL;
	if(type==1)
	{
		url = "http://fanyi.baidu.com/gettts?lan=en&spd=2&source=alading&text=";
	}else if(type==2){
		url = "http://fanyi.baidu.com/gettts?lan=uk&spd=2&source=alading&text=";
	}else{//chinese
		url= "http://tts.baidu.com/text2audio?lan=zh&pid=101&ie=UTF-8&text=";
	}
	return contact_str(url,s);
}

char * getEngPath(char * s,int type)
{
	char * path = NULL;
#ifdef __ANDROID__
	char * _path = "/sdcard/sound/";
#else
	char * _path = "~/sound/";
#endif
	if(type==1){
		path = contact_str(_path,"uk/");
	}else if(type==2){
		path = contact_str(_path,"us/");
	}else{
		path = contact_str(_path,"pinyin/");
	}
	_path = contact_str(path,s);
	free(path);
	path = contact_str(_path,".mp3");
	free(_path);
	return path;
}

static int plays(char *url,char * fileName)
{
	SDL_Log("plays:%s,%s",url,fileName);
	size_t data_length = 0;
	char * data = loadUrl(url,&data_length);
	if(fileName){
		if(data && data_length>0)
			if(writefile(fileName,data,data_length)==0) {
				if(strcmp(fileName+strlen(fileName)-4,".mp3")==0)
				{
					if(stage->sound==NULL){
						//stage->sound=Sound_new(16000);
						stage->sound=Sound_new(44100);
					}
					if(Sound_playFile(stage->sound,fileName))
						stage->sound = NULL;
					free(data);
					return 0;
				}
				SDL_Log("writefile successfully!\n");
			}
	}
	if(data)
	{
		if(stage->sound==NULL){
			//stage->sound=Sound_new(16000);
			stage->sound=Sound_new(44100);
		}
		if(Sound_playData(stage->sound,data,data_length))
		{
			stage->sound = NULL;
			SDL_Log("data play Error!\n");
		}
		free(data);
	}
	return 0;
}


/**
 *
 * type:1 uk
 * 		2 us
 */
void READ_loadSound(char *word,int type)
{
	char * url = getEngUrl(word,type);
	char * fileName = getEngPath(word,type);
	plays(url,fileName);
	free(url);
	free(fileName);
	return;
}

void Sound_playEng(char * s,int type)
{
	char * engPath = getEngPath(s,type);
	if(!!fileExists(engPath))
	{
		printf("file %s exists!\n",engPath);
		if(stage->sound==NULL){
			//stage->sound=Sound_new(16000);
			stage->sound=Sound_new(44100);
		}
		if(Sound_playFile(stage->sound,engPath))
			stage->sound = NULL;
	}else{
		READ_loadSound(s,type);
	}
	free(engPath);
}



#ifdef debug_readloader
int main(int argc, char *argv[])
{
	if ( SDL_Init(SDL_INIT_AUDIO) < 0 ) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		return(255);
	}
	Stage_init(1);
	//SDL_Log("%s",loadUrl("http://www.baidu.com/",NULL));
	//fflush(stdout);
	Sound_playEng("black",1);
	return 0;
#if 0
	//阿
	Sound_playFile(NULL,"sound/pinyin/ni3.mp3");
	Sound_playFile(NULL,"sound/pinyin/hao3.mp3");
	Sound_playFile(NULL,"sound/pinyin/a1.mp3");
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
	char * arr[] = {"zai","na",NULL};
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
		int j=0;
		while(j<4)
		{
			char * _w = malloc(32);
			memset(_w,0,32);
			sprintf(_w,"%s",w);
			sprintf(_w+strlen(_w),"%d",++j);
			char * name2 = getEngPath(_w,0);
			free(_w);
			memset(_url,0,128);
			sprintf(_url,format,w,j);
			plays(_url,name2);
			free(name2);
		}
		memset(_url,0,128);
		sprintf(_url,format2,w);
		char * _name2 = getEngPath(w,0);
		plays(_url,_name2);
		free(_name2);
		++i;
	}
	SDL_Delay(1000);
	return 0;
}
#endif
