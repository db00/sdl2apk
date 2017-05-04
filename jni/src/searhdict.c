/**
 * @file searhdict.c
 gcc -D test_searchdict searhdict.c readbaidu.c cJSON.c datas.c sqlite.c sqlite3.c input.c textfield.c dict.c music.c urlcode.c base64.c myregex.c files.c httploader.c ease.c tween.c utf8.c ipstring.c sprite.c array.c mystring.c matrix.c mysurface.c -lssl -lcrypto -lm -ldl -lpthread -lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_image -I"../SDL2_mixer/"  -I"../SDL2_ttf/" -I"../SDL2_image/" -I"../SDL2/include/" && ./a.out 
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2016-09-22
 *
 *
 * 各种时态单词查询
 * 生词测试
 * 单词导入
 * 图片
 * 左右抽屉
 *
 */

#include "searhdict.h"
#include "datas.h"
#include "besier.h"
#include "sdlstring.h"


Sprite * curlistSprite = NULL;
Sprite * dictContainer= NULL;
Sprite * sideBtns= NULL;
Input * input = NULL;
TextField * textfield = NULL;
Dict * ec_dict = NULL;
Dict * ce_dict = NULL;
Array * history_str_arr;
Sprite * explainContainer;

static enum DICT_STATS {
	DICT,
	HISTORY,
	NEW,
	REMEMBERED,
	END
} STATS;


int isInputRegexp()
{
	if(input && input->value && regex_match(input->value,"/^\\/.*\\/[img]*$/"))
		return 1;
	return 0;
}
int isCE(char * value)
{
	//if(value && 0==regex_match(value,"/^[a-z0-9()\\\/-]*[\r\n]*$/i"))
	if(value && strlen(value) && 0==regex_match(value,"/^[\x1-\x7f]*[\r\n]*$/i"))
		return 1;
	return 0;
}

void Redraw(char *text) {
	if(text){
		textfield = TextField_setText(textfield,text);
		//TextField_setScrollV(textfield,TextField_getMaxScrollV(textfield));
	}
	UserEvent_new(SDL_USEREVENT,0,Stage_redraw,NULL);//Stage_redraw
}




void open_dict()
{
	if(ec_dict==NULL)
	{
		ec_dict = Dict_new();
		ec_dict->name = "oxford-gb";
	}
	if(ce_dict==NULL)
	{
		ce_dict = Dict_new();
		ce_dict->name = "ce";
	}
}

char * showExplain(char *explain)
{
	if(explain){
		//char *tmp = regex_replace_all(explain,"([^a-zA-Z,;])( [\\*0-9]+ )","$1\n$2");
		//free(explain);
		//explain = regex_replace_all(tmp,"([:?!\\.])( )","$1\n$2");
		//free(tmp);
		//char *tmp = regex_replace_all(explain,"([^a-zA-Z0-9,;'>\\(\\)\\/ ~]|[:?!\\.])([ \\*0-9]{,2} )","$1\n$2");
		char *tmp = regex_replace_all(explain,"([^\r\n][:?!\\.\\*]) ","$1\n");
		free(explain);
		explain = regex_replace_all(tmp,"([^a-zA-Z,;\r\n])( [\\*0-9]{1,2} )","$1\n$2");
		//explain = tmp;
		TextField_setText(textfield,explain);
	}
	if(curlistSprite)
	{
		curlistSprite->visible = SDL_FALSE;
		UserEvent_new(SDL_USEREVENT,0,Stage_redraw,NULL);//Stage_redraw
	}

	return explain;
}

int add_history(char *word)
{
	if(history_db && strlen(word)>1 && regex_match(word,"/^[a-z-]*$/i")) 
	{
		int rc=0;
		rc = add_new_word(word);
		//printf("\r\n-------------------id:%d\r\n",rc);
		rc = add_to_history(rc);
		//if(!rc)printf("\nsql_result_str:%s",history_db->result_str);
	}
	return 0;
}
static void show_copied(char * word,int r)
{
	if(r==0)
	{
		//printf("\n%d,",r);fflush(stdout);
		int success = SDL_ShowSimpleMessageBox(
				SDL_MESSAGEBOX_ERROR,
				word,
				"copied",
				NULL);
		if (success == -1) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error Presenting MessageBox: %s\n", SDL_GetError());
		}
	}
}
static void read_out(SpriteEvent*e)
{
	Sprite*target = e->target;
	if(target==NULL || target->obj==NULL)
		return;
	SDL_Event* event = e->e;


	if(strcmp(target->obj,"粘贴")==0){
		char * s = getClipboardText();
		//printf("\n------%s,",s);fflush(stdout);
		if(s && strlen(s)>0)
		{
			Input_setText(input,s);
			//searchWord(s);
		}
		return;
	}

	char * word = input->value;
	if(word==NULL || strlen(word)<=0)
		return;

	//printf("read %s\n",word);
	if(strcmp(target->obj,"英音")==0){
		if(strlen(word)==0)
			return;
		Sound_playEng(word,1);
	}else if(strcmp(target->obj,"美音")==0){
		if(strlen(word)==0)
			return;
		Sound_playEng(word,2);
	}else if(strcmp(target->obj,"清除")==0){
		if(strlen(word)==0)
			return;
		Input_setText(input,"");
	}else if(strcmp(target->obj,"复制单词")==0){
		if(strlen(word)==0)
			return;
		int r= setClipboardText(input->value);
		show_copied(input->value,r);
	}else if(strcmp(target->obj,"复制解释")==0){
		if(strlen(word)==0)
			return;
		int r= setClipboardText(textfield->text);
		show_copied(textfield->text,r);
	}else if(strcmp(target->obj,"粘贴")==0){
		char * s = getClipboardText();
		//printf("\n------%s,",s);fflush(stdout);
		if(s && strlen(s)>0)
		{
			Input_setText(input,s);
			//searchWord(s);
		}
	}
	//Redraw(NULL);
}

Word * _getWord(char * _word)
{
	//if(_word==NULL || strlen(_word)==0) return NULL;
	if(isCE(_word)){
		return Dict_getWord(ce_dict,_word);
	}
	return Dict_getWord(ec_dict,_word);
}

int getMean(Word*word)
{
	if(word==NULL)
		return 0;
	open_dict();
	char * explain = NULL;
	if(isCE(word->word)){
		explain = Dict_getMean(ce_dict,word);
	}else{
		explain = Dict_getMean(ec_dict,word);
	}
	showExplain(explain);
	if(word->word)
	{
		if(!isInputRegexp())
			Input_setText(input,word->word);
		add_history(word->word);
	}
	if(sideBtns) sideBtns->visible = SDL_FALSE;
	return 0;
}


int searchWord(char* _word)
{
	if(_word && strlen(_word)){
		open_dict();
		Word * word;
		word = _getWord(_word);
		getMean(word);
	}
	return 0;
}


static int button_messagebox(char * word)
{
	if (word == NULL || strlen(word)==0) {
		return 1;
	}
	const SDL_MessageBoxButtonData buttons[] = {
		{
			SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT,
			0,
			"remembered"
		},
		{
			SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT,
			1,
			"not remebered"
		},
	};

	SDL_MessageBoxData data = {
		SDL_MESSAGEBOX_INFORMATION,
		NULL, /* no parent window */
		"change rememeber status",
		word,//data.message = word;
		2,
		buttons,
		NULL /* Default color scheme */
	};

	int button = -1;
	int success = 0;

	success = SDL_ShowMessageBox(&data, &button);
	if (success == -1) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error Presenting MessageBox: %s\n", SDL_GetError());
		return 1;
	}
	SDL_Log("Pressed button: %d, %s\n", button, button == 0 ? "熟" : "生");

	if(button==0){
		add_remembered_word(word,1);
	}else{
		add_remembered_word(word,0);
	}

	return button;
}


int stageMouseY =0;
int timestamp = 0;
void selectedEvent(SpriteEvent*e)
{
	if(e==NULL)
		return;

	Sprite*sprite = e->target;
	SDL_Event* event = e->e;

	if(sprite==NULL || sprite->parent==NULL)
		return;

	Word * word = sprite->obj;
	if(word==NULL)
		return;
	//printf("---------------------%s\n",word->word);
	switch(e->type)
	{
		case SDL_MOUSEBUTTONDOWN:
			timestamp = event->button.timestamp;
			//printf("timestamp:%d\n",timestamp);fflush(stdout);
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
	//Sprite_removeEventListener(sprite,SDL_MOUSEBUTTONDOWN,selectedEvent);

	int time_diff = event->button.timestamp-timestamp;
	if(time_diff<500)
	{
		Sprite_removeEventListener(sprite,e->type,selectedEvent);
		getMean(word);
	}else{
		//printf("time_diff:%d\n",time_diff);fflush(stdout);
		button_messagebox(word->word);
	}
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

	/*
	   Sprite * btn = Sprite_newText("加入熟词",fontSize/2,0xff000000,0xffffffff);
	   Sprite_addChild(sprite,btn);
	   btn->x = stage->stage_w-btn->w*2;
	   btn->y = fontSize/4;
	   */
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

void removeOuts()
{
	if(curlistSprite->children && curlistSprite->children->length>0)
	{
		Sprite * lastSprite = NULL;
		lastSprite = Sprite_getChildByIndex(curlistSprite,0);
		while(lastSprite->y + lastSprite->h + curlistSprite->y<0)
		{
			Word * _word = lastSprite->obj;
			Sprite_removeChildAt(curlistSprite,0);
			Sprite_destroy(lastSprite);
			lastSprite = Sprite_getChildByIndex(curlistSprite,0);
		}

		lastSprite = Sprite_getChildByIndex(curlistSprite,curlistSprite->children->length-1);
		while(lastSprite->y + curlistSprite->y > stage->stage_h)
		{
			Word * _word = lastSprite->obj;
			Sprite_removeChild(curlistSprite,lastSprite);
			Sprite_destroy(lastSprite);
			lastSprite = Sprite_getChildByIndex(curlistSprite,curlistSprite->children->length-1);
		}
	}
}

static Word * _getWordByIndex(char * curWord,int index)
{
	if(isCE(curWord)){
		return Dict_getWordByIndex(ce_dict,index); 
	}
	return Dict_getWordByIndex(ec_dict,index); 
}
void changeWordList()
{
	if(ec_dict==NULL)
		open_dict();
	int isReg = isInputRegexp();
	char * curWord = input->value;
	int isC_E= isCE(curWord);
	if(curlistSprite->children==NULL || curlistSprite->children->length==0)
	{//first match
		Word * word = NULL;
		if(isReg){
			if(isC_E){
				word = Dict_getWordByRegIndex(ce_dict,input->value,0); 
			}else{
				word = Dict_getWordByRegIndex(ec_dict,input->value,0); 
			}
		}else{
			word = _getWord(curWord);
		}
		if(word){
			Sprite * first = appendWordBtn(word,1);
			if(isReg) first->name = append_str(NULL,"first");
		}else{
			return;
		}
	}
	//printf("curlistSprite height: %d\n",curlistSprite->h);
	if(curlistSprite->children && curlistSprite->children->length>0)
	{
		Sprite * lastSprite = NULL;
		lastSprite = Sprite_getChildByIndex(curlistSprite,0);
		if(strcmp(lastSprite->name,"only")==0) return;
		while(lastSprite->y + curlistSprite->y>0)
		{//first button
			Word * _word = lastSprite->obj;
			int index = _word->index;
			if(index<=1)
				break;
			Word * word = NULL;
			if(isReg){
				if(strcmp(lastSprite->name,"first")==0) break;
				Word * p;
				p = _getWordByIndex(curWord,index-1); 
				if(isC_E){
					word = Dict_getWordByRegWordPrev(ce_dict,input->value,p); 
				}else{
					word = Dict_getWordByRegWordPrev(ec_dict,input->value,p); 
				}
			}else{
				word = _getWordByIndex(curWord,index-1); 
			}
			if(word){
				appendWordBtn(word,0);
				lastSprite = Sprite_getChildByIndex(curlistSprite,0);
			}else{
				if(isReg) lastSprite->name = append_str(NULL,"first");
				break;
			}
		}
		lastSprite = Sprite_getChildByIndex(curlistSprite,curlistSprite->children->length-1);

		while(lastSprite && lastSprite->y + curlistSprite->y + lastSprite->h < stage->stage_h)
		{//last button
			Word * _word = lastSprite->obj;
			int index = _word->index + 1;
			if(isC_E){
				if(index>= ce_dict->wordcount)
					break;
			}else{
				if(index>= ec_dict->wordcount)
					break;
			}
			Word * word = NULL;
			if(isReg){
				if(lastSprite->name && strcmp(lastSprite->name,"last")==0) break;
				if(isC_E){
					word = Dict_getWordByRegIndex(ce_dict,input->value,index-1); 
				}else{
					word = Dict_getWordByRegIndex(ec_dict,input->value,index-1); 
				}
				if(word==NULL){
					if(strcmp(lastSprite->name,"first")==0){
						lastSprite->name = append_str(NULL,"only"); break;
					}else{
						lastSprite->name = append_str(NULL,"last"); break;
					}
				}
			}else{
				word = _getWordByIndex(curWord,index);

			}
			if(word){
				appendWordBtn(word,1);
				lastSprite = Sprite_getChildByIndex(curlistSprite,curlistSprite->children->length-1);
			}else{
				break;
			}
		}
	}
	removeOuts();
}

int getIndexByValue(Array * array,char * word)
{
	if(array && array->length>0)
	{
		int i = 0;
		while(i<array->length)
		{
			char * s = Array_getByIndex(array,i);
			//printf("%s - %s\r\n",s,word);
			if(s && strlen(word)>0 && strcmp(s,word)==0)
				return i;
			++i;
		}
	}else{
		if(array==NULL){
			printf("array==NULL\n");
		}else{
			printf("array.length=%d\n",array->length);
		}
	}
	//printf("s=%s,%d\n",word,strlen(word));
	fflush(stdout);
	return -1;
}

static int noMoreAppend=0;
static int noMorePreppend=0;
static Array * get_more_list(char * word,int isAppend,int isFromCenter)
{
	int num = 20;
	char * compare = ">";
	if(isAppend)
	{
		printf(" isAppend\r\n");
		compare = "<";
	}
	if(isFromCenter || word==NULL)
	{
		if(history_str_arr)
			Array_clear(history_str_arr);
		history_str_arr = NULL;
	}
	if(word && strlen(word)==0)
		word = NULL;

	if(isFromCenter==0)
	{
		if(noMoreAppend && isAppend)
			return history_str_arr;
		if(noMorePreppend && (!isAppend))
			return history_str_arr;
	}
	if(isFromCenter){
		if(strcmp(compare,"<")==0)
			compare = "<=";
		else
			compare = ">=";
		noMoreAppend=0;
		noMorePreppend=0;
	}

	Array * data = NULL;
	if(STATS==HISTORY){
		if(word && strlen(word)>0)
			data = get_history_list(num,word,compare);
		if(isFromCenter && data==NULL)
			data = get_history_list(num,NULL,compare);
	}else if(STATS==REMEMBERED){
		if(word && strlen(word)>0)
			data = get_remembered_list(1,num,word,compare);
		if(isFromCenter && data==NULL)
			data = get_remembered_list(1,num,NULL,compare);
	}else if(STATS==NEW){
		if(word && strlen(word)>0)
			data = get_remembered_list(0,num,word,compare);
		if(isFromCenter && data==NULL)
			data = get_remembered_list(0,num,NULL,compare);
	}

	if(data)
	{
		Array * names = Array_getByIndex(data,0);
		if(names==NULL){
			printf("no names Array");
			return history_str_arr;
		}
		int nCount = names->length;
		int i = 0;
		while(i<nCount)
		{
			char * curName =Array_getByIndex(names,i);
			if(strcmp(curName,"word")==0)
			{
				Array * wordsArr = Array_getByIndex(data,i+1);
				if(wordsArr && wordsArr->length>0)
				{
					Array * a = Array_new();
					int j = 0;
					while(j<wordsArr->length)
					{
						if(isAppend){
							Array_push(a,append_str(NULL,"%s",Array_getByIndex(wordsArr,j)));
						}else{
							Array_push(a,append_str(NULL,"%s",Array_getByIndex(wordsArr,wordsArr->length-1-j)));
						}
						++j;
					}

					if(isAppend){
						history_str_arr = Array_concat(history_str_arr,a);
					}else{
						Array * tmpArr = NULL;
						tmpArr = Array_concat(a,history_str_arr);
						Array_clear(history_str_arr);
						history_str_arr = tmpArr;
					}
				}
				if(wordsArr->length < num)
				{
					if(isAppend)
						noMoreAppend = 1;
					else
						noMorePreppend = 1;
				}
				//printf("\r\n column_name:%s:%d,length:%d\r\n",curName,i+1,wordsArr->length);
				break;
			}
			++i;
		}
	}else{
		if(isAppend)
			noMoreAppend = 1;
		else
			noMorePreppend = 1;
	}
	return history_str_arr;
}

static void changeHistoryList()
{
	//printf("curlistSprite height: %d\n",curlistSprite->h);
	if(history_str_arr==NULL || history_str_arr->length<=0)
	{
		printf("\n no words \n");fflush(stdout);
		return;
	}

	char * curWord;
	//Word* word;
	if(curlistSprite->children ==NULL || curlistSprite->children->length<=0)
	{// first word
		int index = getIndexByValue(history_str_arr,input->value);
		if(index<0)
		{
			if(strlen(input->value)>0)
			{
				history_str_arr = get_more_list(input->value,1,1);
				index = getIndexByValue(history_str_arr,input->value);
			}
		}
		if(index>=0)
		{
			curWord = Array_getByIndex(history_str_arr,index);
		}else{
			curWord = Array_getByIndex(history_str_arr,0);
		}
		if(curWord==NULL)
			return;
		//printf("\r\n curWord:%s\r\n",curWord);

		open_dict();
		Word * word = _getWord(curWord);
		if(word==NULL)
			return;
		appendWordBtn(word,0);
	}
	//fflush(stdout); return;
	Sprite * lastSprite = NULL;
	if(curlistSprite->children && curlistSprite->children->length>0)
	{
		lastSprite = Sprite_getChildByIndex(curlistSprite,0);
		while(lastSprite->y + curlistSprite->y>0)
		{// prepend
			Word * _word = lastSprite->obj;
			int index = getIndexByValue(history_str_arr,_word->word);
			if(index==0)
			{
				//printf ("index==0 history_str_arr length: %d ->",history_str_arr->length);
				history_str_arr = get_more_list(_word->word,0,0);
				index = getIndexByValue(history_str_arr,_word->word);
				//printf (" history_str_arr ---->%d (%d)\r\n",history_str_arr->length,index);
				fflush(stdout);
			}
			if(index<0)
			{
				printf("getIndexByValue--- %d\n",index);
				break;
			}
			curWord = Array_getByIndex(history_str_arr,index-1);
			if(curWord){
				Word * word = _getWord(curWord);
				//Word * word = Array_getByIndex(history_str_arr,index-1); 
				lastSprite = appendWordBtn(word,0);
			}else{
				//printf("getIndexByValue---%d\n",index);
				break;
			}
		}

		if(curlistSprite->children && curlistSprite->children->length>0)
		{
			lastSprite = Sprite_getChildByIndex(curlistSprite,curlistSprite->children->length-1);
			while(lastSprite->y + curlistSprite->y + lastSprite->h < stage->stage_h)
			{//append
				Word * _word = lastSprite->obj;
				int index = getIndexByValue(history_str_arr,_word->word);
				//printf("\n-------------%s:%d/%d\n",_word->word,index,history_str_arr->length);fflush(stdout);
				if(index==history_str_arr->length-1)
				{
					//printf ("history_str_arr length: %d =======>",history_str_arr->length);
					history_str_arr = get_more_list(_word->word,1,0);
					index = getIndexByValue(history_str_arr,_word->word);
					//printf ("%d (%d)\r\n",history_str_arr->length,index);
				}
				//printf("\n-------------%s:%d/%d\n",_word->word,index,history_str_arr->length);fflush(stdout);
				//int index = curlistSprite->children->length;
				if(index<0 || index>= history_str_arr->length-1)
				{
					printf("getIndexByValue--- %s %d\n",_word->word,index); fflush(stdout);
					break;
				}

				char * curWord = Array_getByIndex(history_str_arr,index+1);
				//printf("\r\n curWord:%s\r\n",curWord);
				//if(curWord==NULL) return;
				Word * word = _getWord(curWord);

				appendWordBtn(word,1);
				lastSprite = Sprite_getChildByIndex(curlistSprite,curlistSprite->children->length-1);
			}
		}
	}
	removeOuts();
}

static void showHistory()
{
	if(curlistSprite){
		Sprite_removeChildren(curlistSprite);
		curlistSprite->x = 0;
		curlistSprite->y = input->sprite->h;
	}
	curlistSprite->visible = 1;
	changeHistoryList();
	Redraw(NULL);
}


void show_history_list(SpriteEvent*e)
{
	Sprite*target = e->target;
	SDL_Event* event = e->e;


	//Array * data = NULL;
	if(strcmp(target->obj,"历史")==0){
		if(STATS!=HISTORY)
		{
			STATS = HISTORY;
		}
		SDL_SetWindowTitle(stage->window, "history");
	}else if(strcmp(target->obj,"熟词")==0){
		if(STATS!=REMEMBERED)
		{
			STATS = REMEMBERED;
		}
		SDL_SetWindowTitle(stage->window, "remembered");
	}else if(strcmp(target->obj,"生词")==0){
		if(STATS!=NEW)
		{
			STATS = NEW;
		}
		SDL_SetWindowTitle(stage->window, "new words");
	}else{
		return;
	}
	get_more_list(input->value,1,1);

	/*
	   if(data)
	   {
	   Array_clear(history_str_arr);
	   history_str_arr = NULL;

	   cJSON* pRoot = cJSON_Parse(data);
	   if(pRoot){
	   int nCount = cJSON_GetArraySize( pRoot); 

	   int i;
	   for(i=0;i<nCount; i++){
	   if(history_str_arr==NULL)
	   history_str_arr = Array_new();

	   cJSON *child =  cJSON_GetArrayItem(pRoot, i);
	   cJSON * w = cJSON_GetObjectItem(child,"word");
	   char * s = w->valuestring;
	   if(strlen(s)>1){
	//char * word = cJSON_Print(w);;
	char * word = append_str(NULL,s);
	//char * word = w->valuestring;
	history_str_arr = Array_push(history_str_arr,word);
	}
	//printf("\n---------->%s,%s\n",word,(char*)Array_getByIndex(history_str_arr,i));

	child = child->next;
	}
	cJSON_Delete(pRoot);
	pRoot = NULL;
	//printf("---------->%d,%s",nCount,cJSON_Print(pRoot));
	}

	}
	*/

	//printf("\n=====\n");fflush(stdout);
	//printf("\n---------->%d\n",history_str_arr->length); fflush(stdout);

	showHistory();


	UserEvent_new(SDL_USEREVENT,0,Stage_redraw,NULL);//Stage_redraw
}

void mouseMoves(SpriteEvent*e)
{
	Sprite*target = e->target;
	if(target==NULL)
		return;

	if(target!=curlistSprite)
		return;

	SDL_Event* event = e->e;
	if(event->type!= SDL_MOUSEMOTION)
		return;

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
			if(STATS == DICT)
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
	changeWordList();
	//Array_clear(wordlist);
	return curlistSprite;
}


void textChangFunc(Input * input){
	STATS=DICT;
	SDL_SetWindowTitle(stage->window, "dictionary");
	if(dictContainer->visible && strlen(input->value)>0)
	{
		SDL_Log("text input changed!");
		curlistSprite->visible = 1;
		makeWordlist(input->value);
		Redraw(NULL);
	}
}
void stopInput(SpriteEvent* e){
	stage->focus = NULL;
	SDL_StopTextInput();
}

static void show_list(SpriteEvent* e){
	if(input && strlen(input->value)>0)
	{
		//printf("%d,\r\n",STATS);fflush(stdout);
		if(STATS==DICT)
			textChangFunc(input);
		else// if(STATS== HISTORY)
		{
			showHistory();
		}
	}
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
			if(strlen(input->value)>1){
				int l= strlen(input->value);
				char v[l];
				memset(v,0,l);
				memcpy(v,input->value,l-1);
				while( v[strlen(v)-1]=='\r'|| v[strlen(v)-1]=='\n')
					v[strlen(v)-1]='\0';
				searchWord(v);
				//searchWord(input->value);
				//Input_setText(input,"");
			}else{
				//Input_setText(input,"输入单词，回车查询！");
			}
			break;
		default:
			break;
	}
	Redraw(NULL);
}

static Sprite * makeSideBtn(char * name,int y, void (*func)(SpriteEvent*))
{
	Sprite * btn = Sprite_newText(name,45,0x0,0xffffffff);
	btn->y = y;
	Sprite_addEventListener(btn,SDL_MOUSEBUTTONUP,func);
	//printf("btn:(%s)\n",btn->obj);
	btn->x = stage->stage_w - btn->w;
	Sprite_addChild(sideBtns,btn);
	return btn;
}
static Sprite * makeTopBtn(Sprite * contener,char * name,int gap, void (*func)(SpriteEvent*))
{
	Sprite * btn = Sprite_newText(name,30,0x0,0xffffffff);
	if(contener->children && contener->children->length>0)
	{
		Sprite * lastSprite = Sprite_getChildByIndex(contener,contener->children->length-1);
		btn->x = lastSprite->x + lastSprite->w + gap;
	}
	Sprite_addEventListener(btn,SDL_MOUSEBUTTONUP,func);
	//printf("btn:(%s)\n",btn->obj);
	Sprite_addChild(contener,btn);
	return btn;
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
		textfield->w = stage->stage_w;
		Sprite_addEventListener(textfield->sprite,SDL_MOUSEBUTTONDOWN,stopInput); 



		input = Input_new(stage->stage_w,min(stage->stage_h/10,50));
		input->textChangFunc = textChangFunc;
		Sprite_addEventListener(input->sprite,SDL_MOUSEBUTTONDOWN,show_list);//click to show a list
		Sprite_addChild(dictContainer,input->sprite);
		stage->focus = input->sprite;

		explainContainer = Sprite_new();
		Sprite_addChildAt(dictContainer,explainContainer,0);
		explainContainer->y = input->sprite->h;

		Sprite * enBtn;
		int gap=2;
		enBtn = makeTopBtn(explainContainer,"英音",gap,read_out);
		enBtn = makeTopBtn(explainContainer,"美音",gap,read_out);
		enBtn = makeTopBtn(explainContainer,"复制单词",gap,read_out);
		enBtn = makeTopBtn(explainContainer,"复制解释",gap,read_out);

		Sprite_addChildAt(explainContainer,textfield->sprite,0);
		textfield->y = enBtn->h;
		textfield->h = stage->stage_h - explainContainer->y - textfield->y;


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

		sideBtns = Sprite_new();
		Sprite_addChild(dictContainer,sideBtns);


		gap=12;
		int y = input->sprite->h;
		enBtn = makeSideBtn("清除",y,read_out);
		y = enBtn->y + enBtn->h + gap;
		enBtn = makeSideBtn("粘贴",enBtn->y + enBtn->h + gap,read_out);
		enBtn = makeSideBtn("历史",enBtn->y + enBtn->h + gap,show_history_list);
		enBtn = makeSideBtn("生词",enBtn->y + enBtn->h + gap,show_history_list);
		enBtn = makeSideBtn("熟词",enBtn->y + enBtn->h + gap,show_history_list);
		//enBtn = makeSideBtn("正则查询",enBtn->y + enBtn->h + gap,read_out);
		//enBtn = makeSideBtn("测试",enBtn->y + enBtn->h + gap,read_out);
		/*
		   enBtn = makeSideBtn("英汉",enBtn->y + enBtn->h + 5,read_out);
		   enBtn = makeSideBtn("测试",enBtn->y + enBtn->h + 5,read_out);
		   */

	}
	if(history_db==NULL)
		init_db();
	//pthread_exit(NULL);  
	return NULL;
}

int startx;
int starty;
void stageMouseEvent(SpriteEvent* e){
	if(dictContainer->visible==SDL_FALSE)
		return;
	Sprite*target = e->target;
	SDL_Event* event = e->e;
	switch(event->type)
	{
		case SDL_MOUSEBUTTONDOWN:
			startx = event->button.x;
			starty = event->button.y;
			Sprite_addEventListener(stage->sprite,SDL_MOUSEMOTION,stageMouseEvent); 
			break;
		case SDL_MOUSEBUTTONUP:
			Sprite_removeEventListener(stage->sprite,SDL_MOUSEMOTION,stageMouseEvent); 
			break;
		case SDL_MOUSEMOTION:
			if(startx<stage->stage_w/10){
				if(event->button.x>stage->stage_w*2/10)
				{
					sideBtns->visible = SDL_FALSE;
					//printf("\r\nfrom left"); fflush(stdout);
					Redraw(NULL);
					Sprite_removeEventListener(stage->sprite,SDL_MOUSEMOTION,stageMouseEvent); 
				}
			}else if(startx>stage->stage_w*9/10){
				if(event->button.x<stage->stage_w*8/10)
				{
					sideBtns->visible = SDL_TRUE;
					//printf("\r\nfrom right"); fflush(stdout);
					Redraw(NULL);
					Sprite_removeEventListener(stage->sprite,SDL_MOUSEMOTION,stageMouseEvent); 
				}
			}else if(starty<stage->stage_h/10){
				if(event->button.y>stage->stage_h*2/10)
				{
					printf("\r\nfrom top\r\n"); fflush(stdout);
					sideBtns->visible = SDL_FALSE;
					Redraw(NULL);
					Sprite_removeEventListener(stage->sprite,SDL_MOUSEMOTION,stageMouseEvent); 
				}
			}else if(starty>stage->stage_h*9/10){
				if(event->button.y<stage->stage_h*8/10)
				{
					sideBtns->visible = SDL_TRUE;
					Redraw(NULL);
					printf("\r\nfrom bottom\r\n"); fflush(stdout);
					Sprite_removeEventListener(stage->sprite,SDL_MOUSEMOTION,stageMouseEvent); 
				}
			}
			break;
	}

}

void showSearchDict(int b)
{
	uiThread(NULL);
	STATS=DICT;
	SDL_SetWindowTitle(stage->window, "dictionary");
	dictContainer->visible = b;
	if(b){
		Sprite_addChild(stage->sprite,dictContainer);
		stage->focus = input->sprite;
	}
	Sprite_addEventListener(stage->sprite,SDL_MOUSEBUTTONDOWN,stageMouseEvent); 
	//Sprite_roundRect2D(0,0,100,100,30,30);
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
