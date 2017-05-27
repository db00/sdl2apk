/**
 * @file testwords.c
 gcc -g -D debug_test_word -I"../SDL2/include/" -I"../SDL2_ttf/" -lSDL2 -lsqlite3 -lz -lssl -lcrypto -lSDL2_ttf -lm utf8.c myfont.c matrix.c sprite.c textfield.c input.c urlcode.c dict.c update.c bytearray.c zip.c httploader.c ipstring.c base64.c sqlite.c myregex.c  testwords.c files.c mystring.c array.c datas.c && ./a.out
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2017-05-18
 */

#include "testwords.h"

static Sprite * testContainer= NULL;
static Input * input = NULL;
static TextField * textfield = NULL;

static int numIndex = 0;
static int numWords = 5;
static int minToPass = 5;
static Array * test_array;
static Dict * ec_dict = NULL;
static char * full_explain = NULL;


typedef struct TestWord{
	char * word;
	time_t date;
	int numRight;
}TestWord;

static TestWord * TestWord_new()
{
	TestWord * testword = malloc(sizeof(struct TestWord));
	memset(testword,0,sizeof(struct TestWord));
	return testword;
}
static void TestWord_free(TestWord * word)
{
	if(word)
	{
		if(word->word)
			free(word->word);
		free(word);
	}
}

static void test_word(TestWord * word);

static void write_config()
{
	char * s = "words=%d,pass=%d";
	int len = strlen(s)+20;
	char content[len];
	memset(content,0,len);
	sprintf(content,s,numWords,minToPass);
	char * file= decodePath("~/sound/test_config.txt");
	printf("%s\r\n",content);
	writefile(file,content,strlen(content));
}

static int get_test_config()
{
	char * file = "~/sound/test_config.txt";
	if(!fileExists(file)){
		char * content = "words=5,pass=5";
		writefile(file,content,strlen(content));
	}
	char *url = decodePath("~/sound/test_config.txt");
	SDL_Log("%s\n",url);
	if(url){
		size_t len;
		char *s = readfile(url,&len);
		SDL_Log("content:%s\n",s);
		//while(s[len-1]=='\r' || s[len-1]=='\n') s[len-1]='\0';
		Array * arr = string_split(s,",");
		if(arr){
			int i = 0;
			while(i<arr->length)
			{
				char * item = Array_getByIndex(arr,i);
				if(strncmp(item,"words=",6)==0)
					numWords = atoi(item+6);
				else if(strncmp(item,"pass=",5)==0)
					minToPass = atoi(item+5);
				++i;
			}
		}
		free(s);
		Array_clear(arr);
		free(url);
		url = NULL;
		printf("%d,%d\r\n",numWords,minToPass);
		return 0;
	}
	return -1;
}

static void adds(Array * data,time_t * date)
{
	if(data)
	{
		Array * names = Array_getByIndex(data,0);
		if(names==NULL){
			printf("no names Array");
			return;
		}
		int nCount = names->length;
		int i = 0;
		int wordCol=0;
		int numCol=0;
		int dateCol=0;
		while(i<nCount)
		{
			char * curName =Array_getByIndex(names,i);
			if(strcmp(curName,"word")==0)
			{
				wordCol = i+1;
				//printf("\r\n column_name:%s:%d,length:%d\r\n",curName,i+1,wordsArr->length);
			}else if(strcmp(curName,"numTest")==0){
				numCol = i+1;
			}else if(strcmp(curName,"date")==0){
				dateCol = i+1;
			}
			++i;
		}
		if(wordCol && numCol && dateCol)
		{
			Array * wordsArr = Array_getByIndex(data,wordCol);
			Array * numsArr = Array_getByIndex(data,numCol);
			Array * dateArr = Array_getByIndex(data,dateCol);
			if(wordsArr && wordsArr->length>0)
			{
				if(test_array==NULL)
				{
					test_array = Array_new();
				}
				int j = 0;
				while(j<wordsArr->length)
				{
					char * word = Array_getByIndex(wordsArr,j);

					int k = 0;
					int isIn = 0;
					while(k<test_array->length)
					{
						TestWord * testword = Array_getByIndex(test_array,k);
						if(strcmp(testword->word,word)==0)
						{
							isIn = 1;
							break;
						}
						++k;
					}

					if(!isIn)
					{
						TestWord * testword = TestWord_new();
						testword->word = append_str(NULL,"%s",word);
						testword->numRight = atoi((char*)Array_getByIndex(numsArr,j));
						testword->date = atoi((char*)(Array_getByIndex(dateArr,j)));
						Array_push(test_array,testword);
					}
					++j;
				}
			}
		}
	}
}

static time_t lastdate=0;
static Array * get_test_array(int start,int _numWords)
{
	Array * data = get_test_list(start,_numWords);
	adds(data,NULL);
	if(data && data->length>=_numWords)
		return test_array;
	if(test_array == NULL || test_array->length<numWords)
	{//review remembered words
		if(test_array==NULL)
			start = 0;
		else
			start = test_array->length;

		if(lastdate==0){
			lastdate = time(NULL)-24*3600;
		}
		data = get_review_list(lastdate,numWords-start);
		//data = get_review_list(lastdate,numWords);
		adds(data,&lastdate);
		if(data && data->length+start>=numWords)
			return test_array;
	}
	//return test_array;
	if(test_array == NULL || test_array->length<numWords)
	{//find random word in dict
		if(test_array==NULL){
			test_array = Array_new();
		}
		int i = test_array->length;
		while(i<numWords)
		{
			srand(time(NULL));
			int ran = rand()% Dict_getNumWords(ec_dict);
			Word * word = Dict_getWordByIndex(ec_dict,ran);
			char * _word = word->word;
			if(strlen(_word)>2 && regex_match(_word,"/^[a-z]*$/i"))
			{
				if(add_new_word(_word,0)==0)
				{//not in the list of database

					TestWord * testword = TestWord_new();
					testword->word = append_str(NULL,"%s",_word);
					testword->numRight = 0;
					testword->date = time(NULL);
					Array_push(test_array,testword);
					++i;
				}
			}
		}
	}
	return test_array;
}

static char * starStrings(int i)
{
	char * s = malloc(i+1);
	memset(s,'*',i+1);
	s[i]='\0';
	return s;
}

static void test_next()
{
	++numIndex;
	if(numIndex>=test_array->length)
		numIndex=0;
	TestWord * _s = Array_getByIndex(test_array,numIndex);
	test_word(_s);
	Input_setText(input,"");
}

static void change_wordRight(TestWord * word,int i)
{
	change_word_rights(word->word,i);
	word->numRight = i;
	word->date = time(NULL);
}


static void check_word(char * s)
{
	if(strlen(s)<2)
	{
		if(strcmp(s,"q")==0){
			testContainer->visible = SDL_FALSE;
			showSearchDict(1);
		}
		return;
	}
	if(regex_match(s,"/^[0-9]{1,}[wp]$/i")){
		int len = strlen(s);
		if(input->value[len-1]=='w'){
			numWords = atoi(s);
			if(numWords>100)
				numWords=100;
			else if(numWords<5)
				numWords = 5;
		}else{
			minToPass = atoi(s);
			if(minToPass>10)
				minToPass = 10;
			else if(numWords<1)
				minToPass = 1;
		}
		Input_setText(input,"");
		write_config();
		return;
	}else if(regex_match(s,"/^([0-9]{1,3}\\.){3}[0-9]{1,3}k$/i")){
		char * file = "~/sound/host";
		char ip[32];
		memset(ip,0,sizeof(ip));
		snprintf(ip,strlen(input->value)-2,"%s",input->value);
		writefile(file,ip,strlen(ip));
		Input_setText(input,ip);
		return;
	}
	TestWord * origin_word = Array_getByIndex(test_array,numIndex);
	char * curWord = regex_replace_all(s,"/[^a-z]/i","");
	char * right_answer = regex_replace_all(origin_word->word,"/[^a-z]/i","");
	int numRight = origin_word->numRight;
	int lasttestTime = origin_word->date;
	printf("numRight:%d\r\n",numRight);
	if(strcasecmp(right_answer,curWord)==0)
	{
		++numRight;
		char * right_s = contact_str(s," √ ");
		Input_setText(input,right_s);
		free(right_s);
		change_wordRight(origin_word,numRight);
		printf("right!\r\n");
		if(numRight>=minToPass || lasttestTime < time(NULL)-3600*24){
			add_remembered_word(origin_word->word,1);
			TestWord_free(origin_word);
			Array_removeByIndex(test_array,numIndex);
			int len = test_array->length;
			get_test_array(len,numWords-len);
			--numIndex;
		}
	}else{
		numRight = 0;
		add_remembered_word(origin_word->word,0);
		change_wordRight(origin_word,0);
		//TextField_setText(textfield,"");
		printf("wrong!\r\n");
		printf("%s\r\n",input->value);
		char * wrong_s = contact_str(s," × ");
		char * tmp = regex_replace_all(wrong_s,"/[\r\n]/img","");
		Input_setText(input,tmp);
		free(wrong_s);
		free(tmp);
	}
	free(right_answer);
	free(curWord);
	TextField_setText(textfield,full_explain);
}

static void keyupEvent(SpriteEvent* e){
	if(regex_match(input->value,"/ [×√]/")){
		test_next();
		return;
	}
	if(strlen(input->value)==0)
		return;
	if(testContainer==NULL || testContainer->visible==0)
		return;
	SDL_Event *event = e->e;
	const char * kname = SDL_GetKeyName(event->key.keysym.sym);
	if(!strcmp(kname,"Menu"))
		return;
	switch (event->key.keysym.sym)
	{
		case SDLK_MENU:
			return;
			break;
		case SDLK_RETURN:
			if(strlen(input->value)>1){
				if(regex_match(input->value,"/ [×√]/")){
					test_next();
				}else{
					int l= strlen(input->value);
					char v[l];
					memset(v,0,l);
					memcpy(v,input->value,l-1);
					while( v[strlen(v)-1]=='\r'|| v[strlen(v)-1]=='\n')
						v[strlen(v)-1]='\0';
					check_word(v);
				}
			}else{
				//Input_setText(input,"输入单词，回车查询！");
			}
			break;
		default:
			break;
	}
	UserEvent_new(SDL_USEREVENT,0,Stage_redraw,NULL);//this line is equal to the following code block.
}

static void test_word(TestWord * word)
{
	if(word==NULL)
		return;
	printf("%d:%s\r\n",numIndex,word->word);
	fflush(stdout);
	if(word->word && strlen(word->word)){
		Word * _word;
		_word = Dict_getWord(ec_dict,word->word);
		char * explain = NULL;
		explain = Dict_getMean(ec_dict,_word);
		char * tmp = regex_replace_all(explain,"([^\r\n][:?!\\.\\*]) ","$1\n");
		free(explain);
		if(full_explain)
			free(full_explain);
		full_explain = regex_replace_all(tmp,"([^a-zA-Z,;\r\n])( [\\*0-9]{1,2} )","$1\n$2");
		free(tmp);
		char * numStars = starStrings(strlen(word->word));
		int len = strlen(word->word)+8;
		char patt[len];
		memset(patt,0,len);
		sprintf(patt,"/%s?/i",word->word);
		tmp = regex_replace_all(full_explain,patt,numStars);
		free(numStars);
		//char * test_explain = regex_replace_all(tmp,"/\\/[^\\/]*\\//ig"," ");
		char * test_explain = regex_replace_all(tmp,"/\\/[^0-9\r\n\\/]*\\//ig"," ");
		printf("test_explain:%s\r\n",test_explain);
		printf("full_explain:%s\r\n",full_explain);
		//explain = tmp;
		//TextField_setText(textfield,full_explain);
		//
		//

		if(testContainer==NULL){
			testContainer = Sprite_new();
			testContainer->surface = Surface_new(1,1);
			char pixels[4] ={'\0','\0','\0','\xff'};
			memcpy(testContainer->surface->pixels,(char*)pixels,sizeof(pixels));
			Sprite_addChild(stage->sprite,testContainer);
			testContainer->w = stage->stage_w;
			testContainer->h = stage->stage_h;
			if(input==NULL)
			{
				input = Input_new(stage->stage_w,min(stage->stage_h/10,50));
				//input->textChangFunc = textChangFunc;
				//Sprite_addEventListener(input->sprite,SDL_MOUSEBUTTONDOWN,show_list);//click to show a list
				Sprite_addChild(testContainer,input->sprite);
				stage->focus = input->sprite;
			}
			textfield = TextField_new();
			textfield->sprite->canDrag = 1;
			textfield->w = stage->stage_w;
			//Sprite_addEventListener(textfield->sprite,SDL_MOUSEBUTTONDOWN,stopInput); 
			Sprite_addChildAt(testContainer,textfield->sprite,0);
			textfield->y = input->sprite->h;
			textfield->h = stage->stage_h - textfield->y;
		}
		testContainer->visible = SDL_TRUE;
		TextField_setText(textfield,test_explain);free(test_explain);
		Sprite_addEventListener(stage->sprite,SDL_KEYUP,keyupEvent); 
	}
}

void startTest(int b)
{
	if(b){
		if(ec_dict==NULL)
		{
			ec_dict = Dict_new();
			ec_dict->name = "oxford-gb";
			if(!fileExists("~/sound/oxford-gb/"))
			{
				//Loading_show(1,"loading oxford ......");
				loadAndunzip("https://git.oschina.net/db0/kodi/raw/master/oxford.zip","~/sound/");
			}
		}
		get_test_config();
		get_test_array(0,numWords);
		numIndex = 0;
		TestWord * s = Array_getByIndex(test_array,numIndex);
		test_word(s);
	}else{
		if(testContainer)
			testContainer->visible = SDL_FALSE;
		if(input)
			Input_setText(input,"");
	}
	//UserEvent_new(SDL_USEREVENT,0,Stage_redraw,NULL);//this line is equal to the following code block.
}

#ifdef debug_test_word
int main()
{
	Stage_init(1);
	startTest();


	Stage_loopEvents();
	return 0;
}
#endif
