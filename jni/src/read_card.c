/**
 * @file read_card.c
 gcc -g -Wall -D test_readcard -Wall -lpthread music.c utf8.c mysurface.c pinyin.c input.c array.c ease.c tween.c kodi.c base64.c urlcode.c filetypes.c httpserver.c httploader.c readbaidu.c ipstring.c read_card.c dict.c myregex.c files.c mystring.c sprite.c matrix.c textfield.c -lcrypto -lssl -ldl -lm -I"../SDL2_mixer/" -I"../SDL2_image/" -I"../SDL2/include/" -I"../libxml/include/" -I"../SDL2_ttf/" -lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_image && ./a.out
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2016-09-18
 */
#include "read_card.h"
static Array * askArr = NULL;
static int curAskIndex = 0;
static int curKeyIndex=0;

//
//playPinyins("(hao3 a1)");
int playPinyins(char * s)
{
	if(s==NULL)
		return 1;
	char * pinyins = getStrBtw(s,"[","]",0);
	if(pinyins==NULL)
		return 2;
	Array * arr = string_split(pinyins," ");
	int i = 0;
	while(i<arr->length)
	{
		char * pinyin = Array_getByIndex(arr,i);
		if(pinyin && strlen(pinyin))
			playEasyPinyin(pinyin);
		++i;
	}
	return 0;
}

/*
   int playYinbiaos(char * s)
   {
   if(s==NULL)
   return 1;
   char * pinyins = getStrBtw(s,"[","]",0);
   if(pinyins==NULL)
   return 2;
//Array * arr = string_split(pinyins," ");
Array * arr = UTF8_each(pinyins);
int i = 0;
while(i<arr->length)
{
char * pinyin = Array_getByIndex(arr,i);
if(pinyin && strlen(pinyin))
playYinbiao(pinyin);
++i;
}
free(pinyins);
Array_clear(arr);
return 0;
}
*/

int getNumInLine()
{
	Array * curAsk= Array_getByIndex(askArr,curAskIndex);
	int len = curAsk->length;
	int i = sqrt(len);
	while(i*i<len)
		i=i+1;
	return i;
}

typedef struct Card
{
	int index;
	Sprite * sprite;
	Sprite * img;
	Sprite * ch;//->obj
	Sprite * en;//->obj
	int isRight;//1:right,2:wrong
	void(*complete)(struct Card*);
}Card;

void *readAsk(void*k)
{
	Array * curAsk = Array_getByIndex(askArr,curAskIndex);
	Array * curLine = Array_getByIndex(curAsk,curKeyIndex);
	char * item = Array_getByIndex(curLine,0);
	int num = atoi(item);
	char * zhuyin = Array_getByIndex(curLine,3);
	if(zhuyin){
		//if(zhuyin[0]=='(')
		playPinyins(zhuyin);
		//else playYinbiaos(zhuyin);
	}else if(num){
		SDL_Log("read %d",num);
		char * hzNum = num2hzs(num);
		SDL_Log("read %d -> %s",num,hzNum);
		if(hzNum){
			playHzsPinyin(hzNum);
			free(hzNum);
		}
	}else if((((unsigned char)item[0] >= 'A') && ((unsigned char)item[0] <= 'Z'))
			||
			(((unsigned char)item[0] >= 'a') && ((unsigned char)item[0] <= 'z'))
			)
	{
		Sound_playEng(item,1);
	}
	else
		playHzsPinyin(item);
	playHzsPinyin("在哪");
	return NULL;
}

void clicked(SpriteEvent* e)
{
	Sprite * sprite = e->target;
	Card * card = sprite->parent->obj;
	char * word = sprite->obj;
	//SDL_Log(word);

	Array * curAsk = Array_getByIndex(askArr,curAskIndex);
	Array * curLine = Array_getByIndex(curAsk,card->index);
	char * zhuyin = Array_getByIndex(curLine,3);
	if(zhuyin){
		//if(zhuyin[0]=='(')
		playPinyins(zhuyin);
		//else playYinbiaos(zhuyin);
	}else if(
			( (unsigned char)(word[0])>='A' && (unsigned char)(word[0])<='Z')
			||
			( (unsigned char)(word[0])>='a' && (unsigned char)(word[0])<='z')
			)
	{
		Sound_playEng(word,1);//1:uk,2:us
	}else if(
			(unsigned char)(word[0])>='0'
			&&
			(unsigned char)(word[0])<='9'
			)
	{
		char *num = num2hzs(atoi(word));
		playHzsPinyin(num);
		free(num);
	}else{
		playHzsPinyin(word);
	}
	if(card->isRight==1)
	{
		playHzsPinyin("对");
		//Sprite_removeEvents(sprite);

		makeNewAsk(-1,-1);
	}else if(card->isRight==2){
		playHzsPinyin("错");
		//readAsk(NULL);
	}
	//if(card->complete) card->complete(card);
}


Card * Card_new(char * ch,char*en,char * url)
{
	Card * card = malloc(sizeof(Card));
	memset(card,0,sizeof(Card));

	card->sprite = Sprite_new();
	card->sprite->obj = card;
	en = contact_str(en,Array_joins(hzs2pinyin(en),""));
	card->ch = Sprite_newText(ch,stage->stage_h/320*18,0xff0000ff,0xffff00ff);
	card->en = Sprite_newText(en,stage->stage_h/320*18,0xff0000ff,0xffff00ff);
	card->img = Sprite_newImg(url);
	card->img->obj = ch;

	int len = getNumInLine();

	int w = stage->stage_w/len-1;
	int h = stage->stage_h/len-1;
	Sprite_fullcenter(card->img,0,0,w,h);
	//Sprite_center(card->ch,0,0,w/2,card->ch->h);
	//Sprite_center(card->en,w/2,0,w/2,card->en->h);
	card->en->y = h-card->en->h;
	card->ch->y = h-card->ch->h;
	card->en->x=card->ch->w;

	card->sprite->w = w;
	card->sprite->h = h;

	Sprite_addChild(card->sprite,card->img);
	Sprite_addChild(card->sprite,card->ch);
	Sprite_addChild(card->sprite,card->en);

	//Sprite_addEventListener(card->sprite,SDL_MOUSEBUTTONUP,clicked);
	Sprite_addEventListener(card->ch,SDL_MOUSEBUTTONUP,clicked);
	Sprite_addEventListener(card->en,SDL_MOUSEBUTTONUP,clicked);
	Sprite_addEventListener(card->img,SDL_MOUSEBUTTONUP,clicked);

	return card;
}
void Card_free(Card*card)
{
	if(card)
	{
		free(card);
	}
}
void removeCardContainer()
{
	if(cardContainer){
		/*
		   int i = cardContainer->children->length;
		   while(i>0)
		   {
		   --i;
		   Sprite * son = Sprite_getChildByIndex(cardContainer,i);
		   if(strcmp(son->name,"card")==0)
		   {
		   if(son->obj)
		   Card_free(son->obj);
		   son->obj=NULL;
		   }
		   }
		   */
		Sprite_removeChildren(cardContainer);
		Sprite_removeChild(stage->sprite,cardContainer);
	}
}

void makeList()
{
	Array * curAsk= Array_getByIndex(askArr,curAskIndex);
	removeCardContainer();
	if(cardContainer == NULL)
	{
		cardContainer = Sprite_new();
		cardContainer->surface = Surface_new(1,1);
		char pixels[4] ={'\0','\0','\0','\xff'};
		memcpy(cardContainer->surface->pixels,(char*)pixels,sizeof(pixels));
		cardContainer->w = stage->stage_w;
		cardContainer->h = stage->stage_h;
	}
	Sprite_addChild(stage->sprite,cardContainer);

	Array * curLine = Array_getByIndex(curAsk,curKeyIndex);
	char * ask = contact_str(Array_getByIndex(curLine,0),"在哪？");
	Sprite * sprite = Sprite_newText(ask,18,0xff0000ff,0xffff00ff);
	free(ask);
	free(sprite->name);
	Sprite_addChild(cardContainer,sprite);
	int i = 0;
	int sq = getNumInLine();
	int w = stage->stage_w/sq;
	int h = stage->stage_h/sq;
	int _y =  (sq*sq-curAsk->length)/sq*stage->stage_h/sq/2;
	while(i<curAsk->length)
	{
		Array * itemArr = Array_getByIndex(curAsk,i);
		char * s1 = Array_getByIndex(itemArr,0);
		char * s2 = Array_getByIndex(itemArr,1);
		char * s3 = Array_getByIndex(itemArr,2);
		if(s1 && s2 && s3 && strlen(s1) && strlen(s2) && strlen(s3))
		{
			printf("%s,%s,%s\n",s1,s2,s3);
			Card * card = Card_new(s1,s2,s3);
			if(i==curKeyIndex)
				card->isRight = 1;
			else
				card->isRight = 2;
			card->index = i;
			if(card->sprite->name)
				free(card->sprite->name);
			card->sprite->name = contact_str("","card");
			Sprite_addChild(cardContainer,card->sprite);
			Sprite_center(card->sprite,(i%sq)*w,(i/sq)*h+_y,w,h);
		}
		//SDL_Log("curKeyIndex:%d,%d",curKeyIndex,i);
		++i;
	}
	UserEvent_new(SDL_USEREVENT,0,Stage_redraw,NULL);//Stage_redraw

	//readAsk(NULL); return NULL;
	pthread_t thread;//创建不同的子线程以区别不同的客户端  
	if(pthread_create(&thread, NULL, readAsk, NULL)!=0)//创建子线程  
	{  
		perror("pthread_create");  
	}else{
		pthread_detach(thread);
	}
}

Array * getAskArr()
{
	if(askArr)
		return askArr;
	size_t data_len = 0;
	char * data = NULL;
	data = readfile("~/sound/test.txt",&data_len);
	Array * array0 = string_split(data,"\n\n");
	if(array0 == NULL || array0->length == 0)
	{
		if(array0)
			Array_clear(array0);
		if(data)
			free(data);
		return NULL;
	}

	askArr = Array_new();
	int i = 0;
	while(i<array0->length)
	{
		char * s = Array_getByIndex(array0,i);
		if(s==NULL || strlen(s)==0)
		{
			++i;
			continue;
		}
		Array * line = string_split(s,"\n");
		Array * lineArr = NULL;
		int l = 0;
		while(l<line->length)
		{
			char * sl = Array_getByIndex(line,l);
			if(sl && strlen(sl))
			{
				Array * arr = string_split(sl,",");
				if(arr && arr->length>=3)
				{
					if(lineArr==NULL){
						lineArr = Array_new();
						Array_push(askArr,lineArr);
					}
					Array_push(lineArr,arr);
				}
			}
			++l;
		}
		Array_clear(line);
		++i;
	}
	return askArr;
}

void makeNewAsk(int askIndex,int keyIndex)
{
	if(askArr == NULL)
		askArr = getAskArr();
	if(askArr == NULL)
		return;
	if(askIndex<0){
		srand((unsigned)time(NULL));  
		askIndex =(int)(rand()%askArr->length);
	}
	if(askIndex>=askArr->length)
		askIndex = askArr->length-1;
	curAskIndex = askIndex;
	Array * curAsk= Array_getByIndex(askArr,curAskIndex);

	if(keyIndex<0)
	{
		srand((unsigned)time(NULL));  
		keyIndex =(int)(rand()%curAsk->length);
	}
	if(keyIndex>=curAsk->length)
		keyIndex = curAsk->length-1;
	curKeyIndex = keyIndex;

	SDL_Log("curAskIndex: %d,curKeyIndex: %d\n",curAskIndex,curKeyIndex);

	makeList();
}

#ifdef test_readcard
int main()
{
	Stage_init(1);
	if(stage==NULL)return 1;
	stage->sound = Sound_new(16000);

	/*
	   Card * card = Card_new("1","one","~/sound/img/1.jpg");
	//card->isRight = 1;
	Sprite_center(card->sprite,0,0,stage->stage_w,stage->stage_h);
	Sprite_addChild(stage->sprite,card->sprite);
	*/
	//playPinyins("(hao3 a1)");

	//makeNewAsk(-1,-1);
	makeNewAsk(0,-1);

	Stage_loopEvents();
	return 0;
}
#endif
