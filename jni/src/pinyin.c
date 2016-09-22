/**
 * @file pinyin.c
 gcc -g -Wall -I"../SDL2_mixer/" -I"../SDL2/include/" -lSDL2 -lSDL2_mixer music.c pinyin.c utf8.c urlcode.c files.c array.c base64.c myregex.c ipstring.c httploader.c mystring.c  -lssl -lcrypto  -lm -D debug_pinyin &&./a.out
 gcc -I"../SDL2_image/" -I"../SDL2_ttf" -I"../SDL2_mixer/" readbaidu.c urlcode.c ipstring.c files.c matrix.c array.c tween.c ease.c base64.c sprite.c httploader.c mystring.c -lssl -lcrypto  -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lSDL2 -I"../SDL2/include/" -lm -D debug_readloader && ./a.out
 *  
 *
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2016-09-06
 */
//http://hanyu.baidu.com/zici/s?wd=%E7%A9%B4&tupu=01

#include "pinyin.h"

#if 0
static char * loadUrl(char * url);
static char * domain = "http://xh.5156edu.com/";
#endif
static char * shengdiao = "āáǎàōóǒòēéěèīíǐìūúǔùǖǘǚǜ";
static char * shengArr = "aoeiuü";
//āáǎàōóǒòēéěèīíǐìūúǔùǖǘǚǜü

//a1 => ā
char * numToShengdiao(char *s)
{
	char * ret = NULL;
	char *pinyin = getSubStr(s,0,strlen(s)-1);
	char diao = s[strlen(s)-1];
	if('0'>=diao || diao>= '9')
		diao = 0;
	else
		diao -= '0';
	if(diao){
		Array * shengArr = UTF8_each(shengdiao);
		Array_prints(shengArr);
		printf("%s:%s,%d --> ",s,pinyin,diao);
		char * a = strstr(pinyin,"a");
		char * o = strstr(pinyin,"o");
		char * e = strstr(pinyin,"e");
		char * i = strstr(pinyin,"i");
		char * u = strstr(pinyin,"u");
		char * v = strstr(pinyin,"v");
		char * yu = strstr(pinyin,"ü");
		if(a){
			diao-=1;
			ret = regex_replace_all(pinyin, "/a/" , Array_getByIndex(shengArr,diao));
		}else if(o){
			diao+=3;
			ret = regex_replace_all(pinyin, "/o/" , Array_getByIndex(shengArr,diao));
		}else if(e){
			diao+=7;
			ret = regex_replace_all(pinyin, "/e/" , Array_getByIndex(shengArr,diao));
		}else if(i && u){
			if(i>u){
				diao+=11;
				ret = regex_replace_all(pinyin, "/i/" , Array_getByIndex(shengArr,diao));
			}else{
				diao+=15;
				ret = regex_replace_all(pinyin, "/u/" , Array_getByIndex(shengArr,diao));
			}
		}else if(i){
			diao+=11;
			ret = regex_replace_all(pinyin, "/i/" , Array_getByIndex(shengArr,diao));
		}else if(u){
			diao+=15;
			ret = regex_replace_all(pinyin, "/u/" , Array_getByIndex(shengArr,diao));
		}else if(v){
			diao+=19;
			ret = regex_replace_all(pinyin, "/v/" , Array_getByIndex(shengArr,diao));
		}else if(yu){
			diao+=19;
			ret = regex_replace_all(pinyin, "/ü/" , Array_getByIndex(shengArr,diao));
		}
		Array_freeEach(shengArr);
	}else{
		ret = contact_str(s,"");
	}
	free(pinyin);
	printf("%s\n",ret);
	return ret;
}


//ā => a1
char * pinyinFormat(char * s)
{
	if(s==NULL || strlen(s)==0)
		return NULL;
	int diao = 0;
	char * ret = malloc(strlen(s)+2);
	memset(ret,0,strlen(s)+2);

	Array * cArr = UTF8_each(s);
	Array * aArr = UTF8_each(shengdiao);

	if(cArr)
	{
		int i = 0;
		while(i<cArr->length)
		{
			char * c = Array_getByIndex(cArr,i);
			if(diao==0 && c!=NULL)
			{
				char * p = strstr(shengdiao,c);
				if(p)
				{//replace
					int k = 0;
					while(k<aArr->length)
					{
						if(strcmp(c,Array_getByIndex(aArr,k))==0)
							break;
						++k;
					}
					int aoeiu = k/4;
					diao = (k%4) + 1;
					if(aoeiu<5)
						sprintf(ret+strlen(ret),"%c",shengArr[aoeiu]);
					else
						sprintf(ret+strlen(ret),"ü");
				}else{
					sprintf(ret+strlen(ret),"%s",c);
					++i;
					continue;
				}
			}else{
				sprintf(ret+strlen(ret),"%s",c);
			}
			++i;
		}
		Array_freeEach(cArr);
		Array_freeEach(aArr);
		cArr = NULL;
		aArr = NULL;
	}


	if(diao)
	{
		sprintf(ret+strlen(ret),"%d",diao);
	}
	//printf("%s",ret);
	return ret;
}

char * getHZpinyin(char *s)
{
	char * txt = readfile("~/sound/pinyin.txt",NULL);
	char * p = strstr(txt,s);
	if(p==NULL)
		return NULL;
	char * end = NULL;
	while(*p!='\n' && p>txt)
	{
		if(end==NULL && *p==':')
			end = p;
		p--;
	}
	char *py = getSubStr(p,1,end-p-1);
	free(txt);
	return py;
}

Array * toFormatPinyin(char*s)
{
	Array * pinyinArr = NULL;
	Array * words = UTF8_each(s);
	if(words)
	{
		int i = 0;
		while(i<words->length)
		{
			char * hz = Array_getByIndex(words,i);
			if(hz)
			{
				char * pinyin = getHZpinyin(hz);
				//if(pinyin)
				{
					char * fpinyin = pinyinFormat(pinyin);
					free(pinyin);
					pinyinArr = Array_push(pinyinArr,fpinyin);
				}
			}
			++i;
		}
	}
	return pinyinArr;
}

Array * toPinyin(char*s)
{
	Array * pinyinArr = NULL;
	Array * words = UTF8_each(s);
	if(words)
	{
		int i = 0;
		while(i<words->length)
		{
			char * hz = Array_getByIndex(words,i);
			if(hz)
			{
				char * pinyin = getHZpinyin(hz);
				if(pinyin)
				{
					pinyinArr = Array_push(pinyinArr,pinyin);
				}
			}
			++i;
		}
	}
	return pinyinArr;
}




#if 0
FILE * file;
#include "iconv.h"
char * gbk2utf(char* inbuf,size_t * outlen){
	size_t inlen =strlen(inbuf);
	iconv_t cd=iconv_open("UTF-8","GBK");
	char*outbuf=(char*)malloc(inlen*4);
	memset(outbuf,0,inlen*4);
	char*in=inbuf;
	char*out=outbuf;
	*outlen=inlen*4;
	iconv(cd,&in,(size_t*)&inlen,&out,outlen);
	*outlen=strlen(outbuf);
	//printf("%s\n",outbuf);
	//free(outbuf);
	iconv_close(cd);
	return outbuf;
}

/**
 *
 */
//<a class='fontbox' href='/html3/5477.html'>呵<
int getlines2(char * sdata)
{
	char  *data = getStrBtw(sdata,"font_14","</table>",2);
	//printf("matched_arr len: %s\n",data);
	Array *arr = string_split(data,"=font_14");
	if(arr==NULL)
		return 1;
	int j = 0;
	while(j<arr->length)
	{
		Array *matched_arr= Array_new();
		char * str = Array_getByIndex(arr,j);
		char * pinyin = getStrBtw(str,">","<",0);
		if(pinyin==NULL)
		{
			break;
		}
		printf("%s\n",pinyin);
		fwrite(pinyin,1,strlen(pinyin),file);
		fwrite(":",1,1,file);
		int len = regex_search_all(str,"//html3/[0-9]+\\.html\'>[^<]+/",matched_arr);
		//printf("matched_arr len: %d\n",(int)len);
		int i = 0;
		while(i<len)
		{
			char *s = Array_getByIndex(matched_arr,i);
			//printf("%s\n",s); //"html2/p102.html">ai
			char * end = strrchr(s,'\'');
			char * path = getSubStr(s,1,end-s-1);
			char * name = getSubStr(s,end-s+2,9);
			printf("%s,%s\n",path,name);
			fwrite(name,1,strlen(name),file);
			free(path);
			free(name);
			++i;
		}
		fwrite("\n",1,1,file);
		free(pinyin);
		regex_matchedarrClear(matched_arr);
		++j;
	}
	free(data);
	return 0;
}

//<a class='fontbox' href="html2/p102.html">ai
int getlines(char * data)
{
	Array *matched_arr= Array_new();
	int len = regex_search_all(data,"/\"html2/[a-z]+[0-9]+\\.html\">[a-z]+/",matched_arr);
	printf("matched_arr len: %d\n",(int)len);
	int i = 0;
	while(i<len)
	{
		char *s = Array_getByIndex(matched_arr,i);
		//printf("%s\n",s); //"html2/p102.html">ai
		char * end = strrchr(s,'"');
		char * path = getSubStr(s,1,end-s-1);
		char * name = getSubStr(s,end-s+2,9);
		//printf("%s,%s\n",path,name);
		char * url = contact_str(domain,path);
		free(path);
		char * _data = loadUrl(url);
		size_t z;
		char * utf = gbk2utf(_data,&z);
		free(_data);

		getlines2(utf);
		/*
		//printf("data:%s",_data);
		if(writefile("pinyin1.txt",_data,strlen(_data))==0) {
		printf("writefile successfully!\n");
		fflush(stdout);
		}
		*/

		free(utf);
		free(url);
		free(name);
		++i;
		//return 0;
	}
	regex_matchedarrClear(matched_arr);
	return 0;
}

#endif
int playHzPinyin(char * s)
{
	Array * pinyinArr = toFormatPinyin(s);
	if(pinyinArr)
	{
		int i = 0;
		while(i<pinyinArr->length)
		{
			char * pinyin = Array_getByIndex(pinyinArr,i);
			if(pinyin){
				char * pinyinFile0 = contact_str("~/sound/pinyin/",pinyin);
				char * pinyinFile = pinyinFile = contact_str(pinyinFile0,".ogg");
				free(pinyinFile0);
				Sound_playFile(NULL,pinyinFile);
				free(pinyinFile);
			}else{
				//SDL_Delay(400);
			}
			++i;
		}
	}
	Array_freeEach(pinyinArr);
	return 0;
}
//http://zidian.911cha.com/duoyinzi_%d.html

#if 0
void loadDouyinzi()
{/*{{{*/
	FILE * file2 = fopen("duoyinzi.txt","w");
	int i=0;
	while(i<25)
	{
		char * format1 = "http://zidian.911cha.com/duoyinzi.html";
		char * format2 = "http://zidian.911cha.com/duoyinzi_%d.html";
		char url[128];
		memset(url,0,128);
		if(i==0){
			sprintf(url,"%s",format1);
		}else{
			sprintf(url,format2,i);
		}
		char * data = loadUrl(url);
		//<li><a href="./NDJzcQ==.html" target="_blank">难</a> <a href="./NDJzcQ==.html" target="_blank" class="gray noline">(nán、nàn、nuó)</a></li>
		char * table = getStrBtw(data,"l3","</div>",2);
		free(data);
		//writefile("duoyinzi.html",table,strlen(table));
		//printf("%s",table);

		Array *matched_arr= Array_new();
		int len = regex_search_all(table,"/>[^<]+</a>/",matched_arr);
		//printf("matched_arr len: %d\n",(int)len);
		int j = 0;
		while(j<len)
		{
			char *s = (char*)Array_getByIndex(matched_arr,j);
			char * zi = getStrBtw(s,">","<",0);
			//printf("%s\n",zi);
			fwrite(zi,1,strlen(zi),file2);
			free(zi);
			++j;

		}
		//return;
		++i;
	}
	fclose(file2);
}/*}}}*/

#endif
void getDuoyinzi()
{
	char * data = readfile("~/sound/pinyin.txt",NULL);
	Array * arr = string_split(data,"\n");
	Array * pinyinArr = NULL;
	Array * hanziArr = NULL;
	char * douyinzis = NULL;
	if(arr)
	{
		//printf("%d\n",arr->length);
		int i = 0;
		while(i<arr->length)
		{
			char * line = Array_getByIndex(arr,i);
			if(line && strlen(line))
			{
				Array * arr2 = string_split(line,":");
				if(arr2 && arr2->length==2)
				{
					pinyinArr = Array_push(pinyinArr,Array_getByIndex(arr2,0));
					hanziArr = Array_push(hanziArr,Array_getByIndex(arr2,1));
				}else{
					printf("line %d not exists !\n",i);
				}
			}else{
				printf("line %d not exists!\n",i);
			}
			++i;
		}
		free(arr);
	}
	if(pinyinArr)
	{
		printf("%d\n",pinyinArr->length);
	}
	char * duoyinziPath = decodePath("~/sound/duoyinzi.txt");
	FILE * dfile = fopen(duoyinziPath,"wb");
	free(duoyinziPath);
	if(hanziArr)
	{
		printf("%d\n",hanziArr->length);
		int i = 0;
		while(i<hanziArr->length)
		{
			char * line = Array_getByIndex(hanziArr,i);
			Array * hzArr = UTF8_each(line);
			if(hzArr && hzArr->length)
			{
				int j = 0;
				while(j<hzArr->length)
				{
					char * hz = Array_getByIndex(hzArr,j);
					if(douyinzis && strstr(douyinzis,hz))
					{
						++j;
						continue;
					}
					int k = i+1;
					int isDouyinzi = 0;
					while(k<hanziArr->length)
					{
						char * line2 = Array_getByIndex(hanziArr,k);
						if(strstr(line2,hz))
						{
							if(isDouyinzi==0)
							{
								if(douyinzis==NULL)
								{
									douyinzis = contact_str("",hz);
								}else{
									char * ss = contact_str(douyinzis,hz);
									free(douyinzis);
									douyinzis = ss;
								}
								fwrite("\n",1,1,dfile);
								fwrite(hz,1,strlen(hz),dfile);
								fwrite(":",1,1,dfile);
								char * ying = Array_getByIndex(pinyinArr,i);
								fwrite(ying,1,strlen(ying),dfile);
							}
							fwrite(" ",1,1,dfile);
							char * ying2 = Array_getByIndex(pinyinArr,k);
							fwrite(ying2,1,strlen(ying2),dfile);
							isDouyinzi = 1;
						}
						++k;
					}

					++j;
				}
			}
			++i;
		}

	}
	fclose(dfile);
	Array_clear(pinyinArr);
	Array_clear(hanziArr);
	free(data);
}

// 10 -> 一十
char * readNum(int num)
{
	if(num == 0)return contact_str("","零");
	Array *hzstr = UTF8_each("零一二三四五六七八九十");
	Array *wei = UTF8_each("个万亿");
	char s[32];
	memset(s,0,32);
	sprintf(s,"%d",num);
	
	int i = 0;
	int len = strlen(s);
	char * retstr = malloc(1);
	memset(retstr,0,1);
	while(i<len)
	{
		++i;
		char c= s[len-i];
		char * n = Array_getByIndex(hzstr,c-'0');
		if(i%4==1){
			if((i-1)/4 >0){
			   	char * _retstr = contact_str(Array_getByIndex(wei,(int)(i/4)),retstr);
				free(retstr);
				retstr = _retstr;
			}
			if(c!='0'){
			   	char * _retstr = contact_str(n,retstr);
				free(retstr);
				retstr = _retstr;
			}
		}else if(i%4==2){
			if(c == '0'){
			   	char * _retstr = contact_str("零",retstr);
				free(retstr);
				retstr = _retstr;
			}else{
			   	char * str1 = contact_str(n,"十");
			   	char * _retstr = contact_str(str1,retstr);
				free(str1);
				free(retstr);
				retstr = _retstr;
			}
		}else if(i%4==3){
			if(c== '0'){
			   	char * _retstr = contact_str("零",retstr);
				free(retstr);
				retstr = _retstr;
			}else{
			   	char * str1 = contact_str(n,"百");
			   	char * _retstr = contact_str(str1,retstr);
				free(str1);
				free(retstr);
				retstr = _retstr;
			}
		} else if(i%4==0){
			if(c== '0'){
			   	char * _retstr = contact_str("零",retstr);
				free(retstr);
				retstr = _retstr;
			}else{
			   	char * str1 = contact_str(n,"千");
			   	char * _retstr = contact_str(str1,retstr);
				free(str1);
				free(retstr);
				retstr = _retstr;
			}
		}
	}
	Array_freeEach(hzstr);
	Array_freeEach(wei);
	char *ret = regex_replace_all(retstr,"/(零)+$/g","");
	//printf(retstr);
	free(retstr);
	retstr = ret;
	ret = regex_replace_all(retstr,"/(零){2,}/","零");
	free(retstr);
	retstr = ret;
	return retstr;
}

#ifdef debug_pinyin
int main()
{
	printf("%s\n",readNum(8));
	printf("%s\n",readNum(44));
	printf("%s\n",readNum(1000));
	printf("%s\n",Array_joins(UTF8_each("hello苦短"),"-"));
	getDuoyinzi();
	//numToShengdiao("sui3");
	if ( SDL_Init(SDL_INIT_AUDIO) < 0 ) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		return(255);
	}
	printf("%s\n",getHZpinyin("丕"));
	return 0;
	Array_prints(toPinyin("民国"));
	Array_prints(toFormatPinyin("民国"));
	//playHzPinyin("球");
	printf("%s\n",getHZpinyin("合"));
	printf("%s\n",getHZpinyin("貌"));
	playHzPinyin("对。错");
	return 0;

	//pinyinFormat("nüè");

#if 0
	file = fopen("~/sound/pinyin.txt","w");
	char * data = loadUrl("http://xh.5156edu.com/pinyin.html");
	getlines(data);
	fclose(file);

	int statusCode;
	URLRequest * urlrequest = NULL;

	urlrequest = URLRequest_new("http://xh.5156edu.com/pinyin.html");
	urlrequest = Httploader_request(urlrequest);
	statusCode = urlrequest->statusCode;
	if((statusCode >= 200 && statusCode<300) || statusCode==304){
		if(urlrequest->respond->contentLength == strlen(urlrequest->data))
		{
			/*
			   printf("repond data:\n%s\n",urlrequest->data);
			   printf("repond datalength:\n%d\n",urlrequest->respond->contentLength);
			   if(writefile("~/sound/pinyin.txt",urlrequest->data,urlrequest->respond->contentLength)==0) {
			   printf("writefile successfully!\n");
			   fflush(stdout);
			   }
			   */
			getlines(urlrequest->data);
		}
	}
	URLRequest_clear(urlrequest);
	urlrequest = NULL;
#endif
	return 0;
}
#endif
