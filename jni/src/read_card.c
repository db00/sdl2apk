/**
 * @file read_card.c
 gcc -g -Wall -D test_readcard -Wall -lpthread utf8.c mysurface.c pinyin.c input.c array.c ease.c tween.c kodi.c base64.c urlcode.c filetypes.c httpserver.c httploader.c readbaidu.c ipstring.c read_card.c dict.c myregex.c files.c mystring.c sprite.c matrix.c textfield.c -lcrypto -lssl -ldl -lm -I"../SDL2_mixer/" -I"../SDL2_image/" -I"../SDL2/include/" -I"../libxml/include/" -I"../SDL2_ttf/" -lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_image && ./a.out
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2016-09-18
 */
#include "read_card.h"

static const char * numArr[][3] = {
	{"1","one","~/sound/img/1.jpg"},
	{"2","two","~/sound/img/2.jpg"},
	{"3","three","~/sound/img/3.jpg"},
	{"4","four","~/sound/img/4.jpg"},
	{"5","five","~/sound/img/5.jpg"},
	{"6","six","~/sound/img/6.jpg"},
	{"7","seven","~/sound/img/7.jpg"},
	{"8","eight","~/sound/img/8.jpg"},
	{"9","nine","~/sound/img/9.jpg"},
	{NULL,NULL,NULL}
};

typedef struct Card
{
	Sprite * sprite;
	Sprite * img;
	Sprite * ch;//->obj
	Sprite * en;//->obj
	int isRight;//1:right,2:wrong
	void(*complete)(struct Card*);
}Card;

void *readAsk(void*k)
{
	playHzPinyin(readNum(atoi(numArr[cardskey][0])));
	playHzPinyin("在哪？");
	return NULL;
}

void clicked(SpriteEvent* e)
{
	Sprite * sprite = e->target;
	Card * card = sprite->parent->obj;
	char * word = sprite->obj;
	//SDL_Log(word);
	if(
			(unsigned char)(word[0])>='a'
			&&
			(unsigned char)(word[0])<='z'
	  )
	{
		Sound_playEng(word,1);//1:uk,2:us
	}else if(
			(unsigned char)(word[0])>='0'
			&&
			(unsigned char)(word[0])<='9'
			)
	{
		char *num = readNum(atoi(word));
		playHzPinyin(num);
		free(num);
	}else{
		playHzPinyin(word);
	}
	if(card->isRight==1)
	{
		playHzPinyin("对");
		//Sprite_removeChildren(stage->sprite);
		srand(time(NULL));  
		int i= ((rand())%9);
		if(cardskey-i==0)
			cardskey = (i+1)%9;
		else
			cardskey = (i%9);
		SDL_Log("cardskey =  %d,%d",cardskey,i);

		pthread_t thread;//创建不同的子线程以区别不同的客户端  
		if(pthread_create(&thread, NULL, makeList, &cardskey)!=0)//创建子线程  
		{  
			perror("pthread_create");  
		}
		pthread_detach(thread);

	}else if(card->isRight==2){
		playHzPinyin("错");
		readAsk(NULL);
	}
	//if(card->complete) card->complete(card);
}


Card * Card_new(char * ch,char*en,char * url)
{
	Card * card = malloc(sizeof(Card));
	memset(card,0,sizeof(Card));

	card->sprite = Sprite_new();
	card->sprite->obj = card;
	card->ch = Sprite_newText(ch,20,0xff0000ff,0xffff00ff);
	card->en = Sprite_newText(en,20,0xff0000ff,0xffff00ff);
	card->img = Sprite_newImg(url);
	if(card->img->obj)
		free(card->img->obj);
	card->img->obj = contact_str(ch,"");


	int w = stage->stage_w/3-1;
	int h = stage->stage_h/3-1;
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
		if(card->ch)
			free(card->ch->obj);
		if(card->en)
			free(card->en->obj);
		if(card->img)
			free(card->img->obj);
		if(card->sprite)
		{
			Sprite_removeChildren(card->sprite);
			Sprite_destroy(card->sprite);
		}
		free(card);
	}
}

void * makeList(void *_k)
{
	int k = *(int*)_k;
	cardskey = k;
	if(cardContainer){
		int i = cardContainer->children->length;
		while(i>0)
		{
			--i;
			Sprite * card = Sprite_getChildByIndex(cardContainer,i);
			if(strcmp(card->name,"card")==0)
				Card_free(card->obj);
		}
		Sprite_removeChildren(cardContainer);
		Sprite_removeChild(stage->sprite,cardContainer);
	}
	cardContainer = Sprite_new();
	cardContainer->surface = Surface_new(1,1);
	char pixels[4] ={'\0','\0','\0','\xff'};
	memcpy(cardContainer->surface->pixels,(char*)pixels,sizeof(pixels));
	cardContainer->w = stage->stage_w;
	cardContainer->h = stage->stage_h;
	Sprite_addChild(stage->sprite,cardContainer);


	char * ask = contact_str((char*)numArr[k][0],"在哪？");
	Sprite * sprite = Sprite_newText(ask,18,0xff0000ff,0xffff00ff);
	free(ask);
	Sprite_addChild(cardContainer,sprite);
	int i = 0;
	int w = stage->stage_w/3;
	int h = stage->stage_h/3;
	while(1)
	{
		if((char*)(numArr[i][0])==NULL)
			break;
		Card * card = Card_new((char*)numArr[i][0],(char*)numArr[i][1],(char*)numArr[i][2]);
		if(i==k)
			card->isRight = 1;
		else
			card->isRight = 2;
		if(card->sprite->name)
			free(card->sprite->name);
		card->sprite->name = contact_str("","card");
		Sprite_addChild(cardContainer,card->sprite);
		Sprite_center(card->sprite,(i%3)*w,(i/3)*h,w,h);
		//SDL_Log("cardskey:%d,%d",k,i);
		++i;
	}
	UserEvent_new(SDL_USEREVENT,0,Stage_redraw,NULL);//Stage_redraw

	pthread_t thread;//创建不同的子线程以区别不同的客户端  
	if(pthread_create(&thread, NULL, readAsk, NULL)!=0)//创建子线程  
	{  
		perror("pthread_create");  
	}
	pthread_detach(thread);
	return NULL;
}

#ifdef test_readcard
int main()
{
	Stage_init(1);
	if(stage==NULL)return 1;

	/*
	   Card * card = Card_new("1","one","~/sound/img/1.jpg");
	//card->isRight = 1;
	Sprite_center(card->sprite,0,0,stage->stage_w,stage->stage_h);
	Sprite_addChild(stage->sprite,card->sprite);
	*/

	srand((unsigned)time(NULL));  
	cardskey=(int)(rand()%9);
	makeList(&cardskey);

	Stage_loopEvents();
	return 0;
}
#endif
