/**
 * @file searhdict.c
 gcc -D test_searchdict searhdict.c readbaidu.c input.c textfield.c dict.c music.c urlcode.c base64.c myregex.c files.c httploader.c ease.c tween.c utf8.c ipstring.c sprite.c array.c mystring.c matrix.c mysurface.c -lssl -lcrypto -lm -lpthread -lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_image -I"../SDL2_mixer/"  -I"../SDL2_ttf/" -I"../SDL2_image/" -I"../SDL2/include/" && ./a.out 
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2016-09-22
 */

#include "searhdict.h"
#include "datas.h"

int add_history(char *word)
{
	if(history_db && regex_match(word,"/^[a-z-]*$/i")) 
	{
		int rc=0;
		rc = add_new_word(word);
		printf("\r\n-------------------id:%d\r\n",rc);
		rc = add_to_history(rc);
		if(!rc)printf("\nsql_result_str:%s",history_db->result_str);
	}
	return 0;
}

Sprite * curlistSprite = NULL;
Sprite * dictContainer= NULL;
Input * input = NULL;
TextField * textfield = NULL;
Dict * dict = NULL;
Array * history_str_arr;
Sprite * history_btn;


void Redraw(char *text) {
	if(text){
		textfield = TextField_setText(textfield,text);
		//TextField_setScrollV(textfield,TextField_getMaxScrollV(textfield));
	}
	UserEvent_new(SDL_USEREVENT,0,Stage_redraw,NULL);//Stage_redraw
}




void open_dict()
{
	if(dict==NULL)
	{
		dict = Dict_new();
		dict->name = "oxford-gb";
	}
}

char * showExplain(char *explain)
{
	if(explain){
		char *tmp = regex_replace_all(explain,"([^a-zA-Z])( [\\*0-9]+ )","$1\n$2");
		free(explain);
		explain = tmp;
		TextField_setText(textfield,explain);
	}
	if(curlistSprite)
	{
		curlistSprite->visible = SDL_FALSE;
		UserEvent_new(SDL_USEREVENT,0,Stage_redraw,NULL);//Stage_redraw
	}

	return explain;
}

static void * readWordUs(void * _key)
{
	Word * word = _key;
	Sound_playEng(word->word,2);
	return NULL;
}
static void * readWordEn(void * _key)
{
	Word * word = _key;
	Sound_playEng(word->word,1);
	return NULL;
}

int getMean(Word*word)
{
	printf("selected Word: %s\n",word->word);
	//return 0;
	if(word==NULL)
		return 0;
	open_dict();
	char * explain = NULL;
	explain = Dict_getMean(dict,word);
	showExplain(explain);
	if(word->word)
	{
		pthread_t thread;//创建不同的子线程以区别不同的客户端  
		if(pthread_create(&thread, NULL, readWordUs, word)!=0)//创建子线程  
		{  
			perror("pthread_create");  
		}
		pthread_detach(thread);
		//Sound_playEng(word->word,2);
		add_history(word->word);
	}
	return 0;
}

int searchWord(char* _word)
{
	if(_word && strlen(_word)){
		open_dict();
		char * explain = NULL;
		Word *word = Dict_getWord(dict,_word);
		if(word)
		{
			pthread_t thread;//创建不同的子线程以区别不同的客户端  
			if(pthread_create(&thread, NULL, readWordEn, word)!=0)//创建子线程  
			{  
				perror("pthread_create");  
			}
			pthread_detach(thread);
			//Sound_playEng(word->word,1);
		}
		explain = Dict_getMean(dict,word);
		showExplain(explain);
		if(word){
			add_history(word->word);
		}
	}
	return 0;
}

int stageMouseY =0;
void selectedEvent(SpriteEvent*e)
{
	if(e==NULL)
		return;

	Sprite*sprite = e->target;
	SDL_Event* event = e->e;

	if(sprite==NULL || sprite->parent==NULL)
		return;

	Word * word = sprite->obj;
	//printf("---------------------%s\n",word->word);
	switch(e->type)
	{
		case SDL_MOUSEBUTTONDOWN:
			stageMouseY = event->button.y;
			sprite->parent->obj= NULL;
			return;
			break;
		case SDL_MOUSEBUTTONUP:
			if(word==NULL)
			{
				return;
			}
			if(sprite->parent->obj)
				return;
			if(abs(stageMouseY - event->button.y)>2)
				return;
			break;
	}

	//return;
	Sprite_removeEventListener(sprite,e->type,selectedEvent);
	//Sprite_removeEventListener(sprite,SDL_MOUSEBUTTONDOWN,selectedEvent);

	getMean(word);
}



Sprite * makeWordBtn(Word * word)
{
	int fontSize = 20*stage->stage_h/320;
	//printf("fontSize: (%d===%d)", fontSize,h);
	Sprite * sprite = Sprite_newText(word->word,fontSize,0x0,0xffffffff);
	sprite->obj= word;
	//sprite->filter = 0;
	if(sprite->name)
		free(sprite->name);
	sprite->name = contact_str("",word->word);
	Sprite_addEventListener(sprite,SDL_MOUSEBUTTONDOWN,selectedEvent);
	Sprite_addEventListener(sprite,SDL_MOUSEBUTTONUP,selectedEvent);
	return sprite;
}

Sprite * appendWordBtn(Word * word,int end)
{
	Sprite * sprite = makeWordBtn(word);
	if(curlistSprite->children && curlistSprite->children->length>0)
	{
		Sprite * lastSprite = NULL;
		if(end){
			lastSprite = Sprite_getChildByIndex(curlistSprite,curlistSprite->children->length-1);
		}else{
			lastSprite = Sprite_getChildByIndex(curlistSprite,0);
		}
		if(lastSprite)
		{
			if(end){
				sprite->y = lastSprite->y + lastSprite->h + 4;
			}else{
				sprite->y = lastSprite->y - sprite->h - 4;
			}
		}
	}
	if(end){
		Sprite_addChild(curlistSprite,sprite);
	}else{
		Sprite_addChildAt(curlistSprite,sprite,0);
	}
	return sprite;
}

void changeWordList()
{
	//printf("curlistSprite height: %d\n",curlistSprite->h);
	if(curlistSprite->children && curlistSprite->children->length>0)
	{
		Sprite * lastSprite = NULL;
		lastSprite = Sprite_getChildByIndex(curlistSprite,0);
		while(lastSprite->y + curlistSprite->y>0)
		{
			Word * _word = lastSprite->obj;
			int index = _word->index;
			if(index<=1)
				break;
			Word * word = Dict_getWordByIndex(dict,index-1); 
			appendWordBtn(word,0);
			lastSprite = Sprite_getChildByIndex(curlistSprite,0);
		}
		lastSprite = Sprite_getChildByIndex(curlistSprite,curlistSprite->children->length-1);

		while(lastSprite->y + curlistSprite->y + lastSprite->h < stage->stage_h)
		{
			Word * _word = lastSprite->obj;
			int index = _word->index + 1;
			if(index>= dict->wordcount)
				break;
			Word * word = Dict_getWordByIndex(dict,index); 
			appendWordBtn(word,1);
			lastSprite = Sprite_getChildByIndex(curlistSprite,curlistSprite->children->length-1);
		}
	}
}


void changeHistoryList()
{
	//printf("curlistSprite height: %d\n",curlistSprite->h);
	if(history_str_arr==NULL || history_str_arr->length<=0)
		return;

	//char * curWord;
	//Word* word;
	if(curlistSprite->children ==NULL || curlistSprite->children->length<=0)
	{
		char * curWord = Array_getByIndex(history_str_arr,0);
		if(curWord==NULL)
			return;
		printf("\r\n curWord:%s\r\n",curWord);

		open_dict();
		Word * word = Dict_getWord(dict,curWord);
		if(word==NULL)
			return;
		appendWordBtn(word,0);
	}
	//fflush(stdout); return;

	if(curlistSprite->children && curlistSprite->children->length>0)
	{
		Sprite * lastSprite = NULL;
		lastSprite = Sprite_getChildByIndex(curlistSprite,curlistSprite->children->length-1);
		while(lastSprite->y + curlistSprite->y + lastSprite->h < stage->stage_h)
		{
			int index = curlistSprite->children->length;
			if(index>= history_str_arr->length)
				break;

			char * curWord = Array_getByIndex(history_str_arr,index);
			printf("\r\n curWord:%s\r\n",curWord);
			Word * word = Dict_getWord(dict,curWord);

			appendWordBtn(word,1);
			lastSprite = Sprite_getChildByIndex(curlistSprite,curlistSprite->children->length-1);
		}
	}
}

void show_history_list(SpriteEvent*e)
{
	Sprite*target = e->target;
	SDL_Event* event = e->e;

	Array_clear(history_str_arr);
	history_str_arr = NULL;

	char * data =  get_history();
	if(data==NULL)
		return;
	cJSON* pRoot = cJSON_Parse(data);
	if(pRoot){
		int nCount = cJSON_GetArraySize ( pRoot); 

		int i;
		for(i=0;i<nCount; i++){
			if(history_str_arr==NULL)
				history_str_arr = Array_new();

			cJSON *child =  cJSON_GetArrayItem(pRoot, i);
			cJSON * w = cJSON_GetObjectItem(child,"word");
			//char * word = cJSON_Print(w);;
			char * word = append_str(NULL,w->valuestring);
			history_str_arr = Array_push(history_str_arr,word);
			//printf("\n---------->%s,%s\n",word,(char*)Array_getByIndex(history_str_arr,i));
			//fflush(stdout);

			child = child->next;
		}
		cJSON_Delete(pRoot);
		pRoot = NULL;
		//printf("---------->%d,%s",nCount,cJSON_Print(pRoot));
	}


	if(curlistSprite){
		Sprite_removeChildren(curlistSprite);
		curlistSprite->x = 0;
		curlistSprite->y = input->sprite->h;
	}
	if(strcmp(history_btn->obj,"历史"))
	{
		history_btn->obj = "历史";
	}else{
		history_btn->obj = "字典";
		changeHistoryList();
	}
	UserEvent_new(SDL_USEREVENT,0,Stage_redraw,NULL);//Stage_redraw
}

void mouseMoves(SpriteEvent*e)
{
	Sprite*target = e->target;
	SDL_Event* event = e->e;

	if(event->motion.state){
		//if(abs(event->motion.xrel)<20 && abs(event->motion.yrel)<20)
		{
			target->x += event->motion.xrel;
			target->y += event->motion.yrel;
		}
		int stageSize = stage->stage_w>stage->stage_h?stage->stage_w:stage->stage_h;
		if(abs(stageMouseY-event->motion.y)>stageSize/320 || abs(event->motion.xrel)>stageSize/320){
			target->obj= e;
		}
		Sprite_limitPosion(target,target->dragRect);
		if(target==curlistSprite)
		{
			if(strcmp(history_btn->obj,"历史")==0)
			{
				changeWordList();
			}else{
				changeHistoryList();
			}
		}
		Redraw(NULL);
	}
}
Sprite * makeWordlist(char * curWord)
{
	if(curWord==NULL || strlen(curWord)==0)
	{
		return NULL;
	}
	if(curlistSprite){
		Sprite_removeChildren(curlistSprite);
		curlistSprite->x = 0;
		curlistSprite->y = input->sprite->h;
	}

	open_dict();
	int numWords = 1;
	int _i=0;
	Word * word = Dict_getWord(dict,curWord);
	if(word){
		appendWordBtn(word,1);
		//printf("%s,%d\n",word->word,btn->h);
		changeWordList();
	}
	//Array_clear(wordlist);
	return curlistSprite;
}


void textChangFunc(Input * input){
	if(dictContainer->visible && strlen(input->value)>0)
	{
		SDL_Log("text input changed!");
		makeWordlist(input->value);
		curlistSprite->visible = 1;
		Redraw(NULL);
	}
}
void stopInput(SpriteEvent* e){
	stage->focus = NULL;
	SDL_StopTextInput();
}

static void keyupEvent(SpriteEvent* e){
	if(dictContainer==NULL || dictContainer->visible==0)
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
			if(strlen(input->value)>0){
				searchWord(input->value);
				Input_setText(input,"");
			}else{
				Input_setText(input,"输入单词，回车查询！");
			}
			break;
		default:
			break;
	}
	history_btn->obj = "历史";
	Redraw(NULL);
}


void *uiThread(void *ptr){
	if(dictContainer==NULL){
		dictContainer = Sprite_new();
		dictContainer->surface = Surface_new(1,1);
		char pixels[4] ={'\0','\0','\0','\xff'};
		memcpy(dictContainer->surface->pixels,(char*)pixels,sizeof(pixels));
		Sprite_addChild(stage->sprite,dictContainer);
		dictContainer->w = stage->stage_w;
		dictContainer->h = stage->stage_h;

		textfield = TextField_new();
		textfield->sprite->canDrag = 1;
		Sprite_addChild(dictContainer,textfield->sprite);
		Sprite_addEventListener(textfield->sprite,SDL_MOUSEBUTTONDOWN,stopInput); 



		input = Input_new(stage->stage_w,stage->stage_h/10);
		input->textChangFunc = textChangFunc;
		Sprite_addChild(dictContainer,input->sprite);
		stage->focus = input->sprite;



		textfield->w = stage->stage_w;
		//textfield->h = stage->stage_h - input->sprite->h;
		textfield->y = input->sprite->h;


		if(curlistSprite==NULL){
			int numWords = 10;
			int fontSize = 16;
			curlistSprite = Sprite_new();
			SDL_Rect * rect = malloc(sizeof(*rect));
			rect->x = curlistSprite->x;
			rect->w = 0;
			//rect->y = input->sprite->y+input->textfield->h;
			//rect->h = fontSize*1.5*numWords;
			rect->y = -(((unsigned int)-1)/4);
			rect->h = (((unsigned int)-1)/2);
			curlistSprite->dragRect = rect;
			curlistSprite->surface = Surface_new(stage->stage_w,numWords*fontSize*1.5);
			Sprite_addChildAt(dictContainer,curlistSprite,1);
			Sprite_addEventListener(curlistSprite,SDL_MOUSEMOTION,mouseMoves);
		}

		SDL_StartTextInput();

		Sprite_addEventListener(stage->sprite,SDL_KEYUP,keyupEvent); 

		history_btn = Sprite_newText("历史",30,0x0,0xffffffff);
		history_btn->y = 100;
		Sprite_addEventListener(history_btn,SDL_MOUSEBUTTONUP,show_history_list);
		//printf("history_btn:(%s)\n",history_btn->obj);
		history_btn->x = stage->stage_w - history_btn->w;
		Sprite_addChild(dictContainer,history_btn);

	}
	if(history_db==NULL)
		init_db();
	//pthread_exit(NULL);  
	return NULL;
}


void showSearchDict(int b)
{
	uiThread(NULL);
	dictContainer->visible = b;
	if(b){
		Sprite_addChild(stage->sprite,dictContainer);
		stage->focus = input->sprite;
	}
}


#ifdef test_searchdict
int main()
{
	Stage_init(1);
	if(stage==NULL)return 0;
	showSearchDict(1);
	Stage_loopEvents();
	return 0;
}
#endif
