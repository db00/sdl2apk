/**
 *
 gcc -g -I"../SDL2/include/" files.c array.c mystring.c dict.c myregex.c regex.c -lm -Ddebug -DSTDC_HEADERS  -lmingw32 -lSDL2main -lSDL2 && a help
 gcc  -Wall files.c array.c mystring.c dict.c myregex.c -lm -Ddebug  && ./a.out help
 gcc  -Wall files.c mystring.c dict.c myregex.c regex.c -lm -Ddebug -DSTDC_HEADERS && a nude
 gcc -Wall mystrings.c dict.c myregex.c regex.c -lm -Ddebug -DSTDC_HEADERS -o ~/a && ~/a
 gcc -Wall dict.c myregex.c -Ddebug && ./a.out
 gcc -Wall dict.c -Ddebug && a fun
 > dict.txt
 * @file readall.c
 *  
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2013-11-13
 */
#include "dict.h"

Dict * Dict_new()
{
	Dict * dict= (Dict*)malloc(sizeof(Dict));
	memset(dict,0,sizeof(*dict));
	return dict;
}


void Dict_freeWords(Dict*dict)
{
	if(dict->words)
	{
		int i= 0;
		Word *curWord = dict->words;
		while(curWord && i < dict->numIndexDealed)
		{
			if(curWord->word)
				free(curWord->word);
			++i;
			curWord ++;
		}
		free(dict->words);
		dict->words = NULL;
	}
}

void Dict_free(Dict*dict)
{
	if(dict){
		Dict_freeWords(dict);
		if(dict->index_file)
		{
			fclose(dict->index_file);
			dict->index_file= NULL;
		}
		if(dict->file)
		{
			fclose(dict->file);
			dict->file = NULL;
		}
		if(dict->name)
		{
			//free(dict->name);
			dict->name= NULL;
		}
		free(dict);
	}
}

inline static int to_int(unsigned char *from_int) {
	return *(from_int+3)+(*(from_int+2)<<8)+(*(from_int+1)<<16)+(*from_int<<24);
}


int Dict_readIndexFile(Dict *dict)
{
	if(dict == NULL) 
		return -1;
	if(dict->file == NULL){
		return -2;
		dict->file = Dict_open(dict);
	}

	if(dict->words == NULL)
	{
		dict->words = (Word*)malloc(sizeof(Word)*dict->wordcount);
		memset(dict->words,0,sizeof(Word)*dict->wordcount);
	}

	unsigned char* buffer = (unsigned char*)malloc(dict->idxfilesize +1);//read the index file
	memset(buffer,0,dict->idxfilesize+1);

	rewind(dict->index_file);
	fread(buffer,dict->idxfilesize,1,dict->index_file);

	unsigned char *head,*tail;
	tail = buffer;
	head = buffer;
	int it=0;//cur pos;
	int numWords = dict->numIndexDealed;//the len of word arr;
	for(; it < dict->idxfilesize; it++)
	{
		Word*curWord = NULL;
		if(*head == '\0')//reach the end of the word
		{
			curWord = dict->words + numWords;

			size_t wordLength = head-tail+1;
			if(curWord->word){
				free(curWord->word);
			}
			curWord->word = malloc(wordLength+1);
			memset(curWord->word,0,wordLength+1);

			strncpy(curWord->word,(char*)tail,wordLength);//save the word to arr;
			curWord->offset = to_int(head+1);//the offset of the word mean; 
			curWord->length = to_int(head+5);//the length of the word mean;
			//if(900<numWords && numWords < 910) printf("%s:%d,%d\n",curWord->word,curWord->offset,curWord->length);
			head += 9;
			tail = head;
			numWords ++;

			if(numWords==dict->wordcount)break;//reach the end of file 
		}else{
			head++;
		}
	}
	fclose(dict->index_file);
	dict->index_file= NULL;
	if(buffer)free(buffer);
	buffer = NULL;
	return 0;
}

FILE * Dict_open(Dict *dict)
{
#ifdef __ANDROID__
	char rootpath[]="/sdcard/";
#else
#ifdef linux
	char rootpath[]="./";
#else
	char rootpath[]="";
#endif
#endif
	char *_file = dict->name;
	dict->index_file = fopen(append_str(NULL,"%s%s/%s%s",rootpath,_file,_file,".idx"),"rb");
	char *dict_file= append_str(NULL,"%s%s/%s%s",rootpath,_file,_file,".dict");
	char *info_file= append_str(NULL,"%s%s/%s%s",rootpath,_file,_file,".ifo");

	char* file_content = NULL;
	if(info_file){
		file_content = readfile(info_file,NULL);
		free(info_file);
		info_file = NULL;
	}

	if(file_content){
		char *curpara = strtok(file_content,"\r\n");
		while(curpara){
			char *wordcount = "wordcount=";
			char *idxfilesize = "idxfilesize=";
			if(strncasecmp(curpara,wordcount,strlen(wordcount))==0) {
				dict->wordcount=atoi(curpara+strlen(wordcount));
				//printf("dict->wordcount:%d,",dict->wordcount);
			}else if(strncasecmp(curpara,idxfilesize,strlen(idxfilesize))==0) {
				dict->idxfilesize=atoi(curpara+strlen(idxfilesize));
				//printf("dict->idxfilesize:%d,",dict->idxfilesize);
			}
			curpara = strtok(NULL,"\r\n");
		}
		free(file_content);
		file_content = NULL;
	}

	dict->file = fopen(dict_file,"rb");
	if(Dict_readIndexFile(dict))
	{
		printf("open dict index_file error!\n");
	}
	return dict->file;
}

int Dict_getWordIndex(Dict *dict,char * target_word)
{
	if(target_word== NULL || dict == NULL)
		return -1;

	if(dict->file== NULL)
		dict->file = Dict_open(dict);

	if(dict->words==NULL)
		return -1;

	//二分法查找
	int head=0;
	int tail=dict->wordcount;
	int cur=tail/2;
	while(1)
	{
		if(cur<0)
			return 0;
		if(cur>=dict->wordcount)
			return dict->wordcount -1;
		int cmp = strcasecmp(target_word,dict->words[cur].word);
		//int cmp = 
		if(0 == cmp)
		{
			int _i=0;
			int first = cur + _i;
			while(_i!=0 && cur+_i>=0 && strcasecmp(target_word,dict->words[cur+_i].word)==0)
			{
				first = cur + _i;
				if(strcmp(target_word,dict->words[cur+_i].word)==0)
				{
					cur = cur+_i;
					return cur;
				}
				--_i;
			}
			_i = 1;
			while(cur+_i<dict->wordcount && strcasecmp(target_word,dict->words[cur+_i].word)==0)
			{
				if(strcmp(target_word,dict->words[cur+_i].word)==0)
				{
					cur = cur+_i;
					return cur;
				}
				++_i;
			}
			return first;
		}else if(0 > cmp){
			tail = cur-1;
		}else{
			head = cur+1;
		}
		cur=(tail+head)/2;

		if(tail<head){
			if(abs(cur)>=dict->wordcount)
				cur = dict->wordcount-1;
			return -(cur);
		}
	}
	return -1;
}

Word*Dict_contains(Dict *dict,char *word)
{
	int id = Dict_getWordIndex(dict,word);
	if(id>=0){
		Word* retword = (Word*)(dict->words+id);
		return retword;
	}
	return NULL;
}


Word*Dict_getWord(Dict *dict,char * target_word)
{
	int id = abs(Dict_getWordIndex(dict,target_word));
	Word* retword = (Word*)(dict->words+id);
	return retword;
}




char *Dict_getByIndex(Dict * dict,int id){
	if(dict==NULL)
		return NULL;

	if(dict->file==0){
		dict->file = Dict_open(dict);
	}

	Word*word = dict->words + id;
	if(word == NULL)
		return NULL;

	/*printf("word:%s,offset:%d,wordLength:%d\n",word->word,word->offset,word->length);*/
	//printf("%s\r\n",word->word);
	fflush(stdout);

	rewind(dict->file);

	if(0 != fseek(dict->file,word->offset,SEEK_SET)){
		printf("seek word error\n");
		return NULL;
	}
	int len = word->length+1;
	char * explain = (char*)malloc(len);
	memset(explain,0,len);
	fread(explain,word->length,1,dict->file);
	return explain;
}

char *Dict_getMean(Dict* dict,Word* word)
{
	if(dict->file==NULL){
		dict->file = Dict_open(dict);
	}
	if(dict->file ==NULL)
		return "can not Dict_open";

	if(word == NULL){
		printf("#ERROR:not such word\n");
		return NULL;
	}

	printf("word:%s,offset:%d,wordLength:%d\n",word->word,word->offset,word->length);
	//return NULL;
	if(0 != fseek(dict->file,word->offset,SEEK_SET)){
		printf("seek word error\n");
		return NULL;
	}

	char *explain = NULL;
	if(word && word->length && word->word){
		int len = word->length+2 + strlen(word->word);
		explain = (char*)malloc(len);
		memset(explain,0,len);
		sprintf(explain,"%s",word->word);
		sprintf(explain+strlen(explain),"\n");
		len = fread(explain+strlen(explain),1,word->length,dict->file);
		if(len != word->length)
			return NULL;
	}
	return explain;
}

char *Dict_explain(Dict* dict,char* target_word)
{
	if(dict->file==NULL){
		dict->file = Dict_open(dict);
	}
	if(dict->file ==NULL)
		return "can not Dict_open";

	Word*word = Dict_getWord(dict,target_word);

	return Dict_getMean(dict,word);
}

Word**Dict_getWordList(Dict*dict,char*s,int * numWords)
{
	int target_index = abs(Dict_getWordIndex(dict,s));
	printf("%d,",target_index);
	fflush(stdout);
	if(target_index >= dict->wordcount)
		target_index = dict->wordcount-1;
	int i = 0;
	Word**ret = (Word**)malloc(sizeof(Word*)*(*numWords));
	memset(ret,0,sizeof(Word*)*(*numWords));
	while(i<*numWords){
		if(i+target_index >= dict->wordcount)
			break;
		ret[i] = dict->words + target_index + i;
		if(ret[i]==NULL || ret[i]->word==NULL || strlen(ret[i]->word)==0)
			break;
		++i;
	}
	*numWords = i;
	return ret;
}

#ifdef debug
#ifdef WIN32
#include "SDL.h"
#endif
int main(int argc,char**argv)
{

	Dict * dict = Dict_new();
	dict->name = "oxford-gb";
	int numWords = 10;
	int _i=0;
	Word**wordlist = Dict_getWordList(dict,"zz",&numWords);
	while(_i<numWords)
	{
		Word*word = wordlist[_i];
		if(word)
			printf("%s\n",word->word);
		++_i;
	}


	//return 0;
	char* explain;
	explain = Dict_explain(dict,"go");
	if(explain==NULL)return 0;
	printf("explaination:%s\n",explain);
	fflush(stdout);
	Dict_free(dict);
	explain = regex_replace_all(explain,"([^a-zA-Z])( [\\*0-9]+ )","$1\r\n$2");
#ifdef WIN32
		//SDL_setenv("SDL_VIDEODRIVER","dummy",1);
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0) {
	}
	char*explain2 = SDL_iconv_string("gb2312", "UTF-8", explain, SDL_strlen(explain) + 1);
	//char*explain2 = SDL_iconv_string("UTF-8","gb2312", explain, SDL_strlen(explain) + 1);
	free(explain);
	explain = explain2;
#endif
	printf("explaination:%s\n",explain);
	return 0;
	fflush(stdout);
	free(explain);
	dict = Dict_new();
	dict->name = "oxford-gb";
	int i =1;
	/*while(i < dict->wordcount)*/
	while(i < 10)
	{
		explain = regex_replace_all(Dict_getByIndex(dict,i)," \\* ","\r\n");
		printf("%s\r\n",explain);

		fflush(stdout);
		free(explain);
		++i;
	}
	/*getchar();*/
	return 0;
}
#endif
