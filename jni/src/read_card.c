/**
 * @file read_card.c
 gcc -g -Wall -D test_readcard -Wall -lpthread music.c utf8.c mysurface.c pinyin.c input.c array.c ease.c tween.c kodi.c base64.c urlcode.c filetypes.c httpserver.c httploader.c readbaidu.c ipstring.c read_card.c dict.c myregex.c files.c mystring.c sprite.c matrix.c textfield.c -lcrypto -lssl -ldl -lm -I"../SDL2_mixer/" -I"../SDL2_image/" -I"../SDL2/include/" -I"../libxml/include/" -I"../SDL2_ttf/" -lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_image && ./a.out
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2016-09-18
 */
#include "read_card.h"

static const char *numArr[] = {
	"1","one","~/sound/img/1.jpg",
	"2","two","~/sound/img/2.jpg",
	"3","three","~/sound/img/3.jpg",
	"4","four","~/sound/img/4.jpg",
	"5","five","~/sound/img/5.jpg",
	"6","six","~/sound/img/6.jpg",
	"7","seven","~/sound/img/7.jpg",
	"8","eight","~/sound/img/8.jpg",
	"9","nine","~/sound/img/9.jpg",
	NULL,NULL,NULL
};
static const char *alphaArr[] = {
	"A","a","~/sound/img/a.jpg",
	"B","b","~/sound/img/b.jpg",
	"C","c","~/sound/img/c.jpg",
	"D","d","~/sound/img/d.jpg",
	"E","e","~/sound/img/e.jpg",
	"F","f","~/sound/img/f.jpg",
	"G","g","~/sound/img/g.jpg",
	"H","h","~/sound/img/h.jpg",
	"I","i","~/sound/img/i.jpg",
	"J","j","~/sound/img/j.jpg",
	"K","k","~/sound/img/k.jpg",
	"L","l","~/sound/img/l.jpg",
	"M","m","~/sound/img/m.jpg",
	"N","n","~/sound/img/n.jpg",
	"O","o","~/sound/img/o.jpg",
	"P","p","~/sound/img/p.jpg",
	"Q","q","~/sound/img/q.jpg",
	"R","r","~/sound/img/r.jpg",
	"S","s","~/sound/img/s.jpg",
	"T","t","~/sound/img/t.jpg",
	"U","u","~/sound/img/u.jpg",
	"V","v","~/sound/img/v.jpg",
	"W","w","~/sound/img/w.jpg",
	"X","x","~/sound/img/x.jpg",
	"Y","y","~/sound/img/y.jpg",
	"Z","z","~/sound/img/z.jpg",
	NULL,NULL,NULL
};
char ** getCurArr()
{
	return (char**)numArr;
	//return (char**)alphaArr;
}

int getArrLen(char ** arr)
{
	char **p = (char**)arr;
	int len=0;
	while(*p!=NULL)
	{
		p+=3;
		len++;
	}
	return len;
}

int getNumInLine()
{
	int len = getArrLen(getCurArr());
	int i = sqrt(len);
	//SDL_Log("arr sqrt len:%d",i);
	while(i*i<len)
		i=i+1;
	return i;
}

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
	char ** curArray = getCurArr();
	int num = atoi(curArray[cardskey*3]);
	if(num){
		SDL_Log("read %d",num);
		char * hzNum = readNum(num);
		SDL_Log("read %d -> %s",num,hzNum);
		if(hzNum){
			playHzPinyin(hzNum);
			free(hzNum);
		}
	}else if(
			*((unsigned char*)curArray[cardskey*3]) >= 'A'
			&&
			*((unsigned char*)curArray[cardskey*3]) <= 'Z'
			)
	{
		Sound_playEng(curArray[cardskey*3],1);
	}
	else
		playHzPinyin(curArray[cardskey*3]);
	playHzPinyin("在哪");
	return NULL;
}

void clicked(SpriteEvent* e)
{
	Sprite * sprite = e->target;
	Card * card = sprite->parent->obj;
	char * word = sprite->obj;

	char ** curArray = getCurArr();
	int array_len = getArrLen(curArray);
	//SDL_Log(word);
	if(
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
		int i= ((rand()%array_len));
		if(cardskey-i==0)
			cardskey = (i+1)%array_len;
		else
			cardskey = (i%array_len);
		SDL_Log("cardskey =  %d,%d",cardskey,i);

		pthread_t thread;//创建不同的子线程以区别不同的客户端  
		if(pthread_create(&thread, NULL, makeList, &cardskey)!=0)//创建子线程  
		{  
			perror("pthread_create");  
		}else
			pthread_detach(thread);

	}else if(card->isRight==2){
		playHzPinyin("错");
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
	card->ch = Sprite_newText(ch,stage->stage_h/320*18,0xff0000ff,0xffff00ff);
	card->en = Sprite_newText(en,stage->stage_h/320*18,0xff0000ff,0xffff00ff);
	card->img = Sprite_newImg(url);
	if(card->img->obj)
		free(card->img->obj);
	card->img->obj = contact_str(ch,"");

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
	char ** curArray = getCurArr();
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
	int sq = getNumInLine();
	cardContainer = Sprite_new();
	cardContainer->surface = Surface_new(1,1);
	char pixels[4] ={'\0','\0','\0','\xff'};
	memcpy(cardContainer->surface->pixels,(char*)pixels,sizeof(pixels));
	cardContainer->w = stage->stage_w;
	cardContainer->h = stage->stage_h;
	Sprite_addChild(stage->sprite,cardContainer);

	char * ask = contact_str((char*)curArray[k*3],"在哪？");
	Sprite * sprite = Sprite_newText(ask,18,0xff0000ff,0xffff00ff);
	free(ask);
	Sprite_addChild(cardContainer,sprite);
	int i = 0;
	int w = stage->stage_w/sq;
	int h = stage->stage_h/sq;
	int _y =  (sq*sq-getArrLen(getCurArr()))/sq*stage->stage_h/sq/2;
	while(1)
	{
		if((char*)(curArray[i*3])==NULL)
			break;
		Card * card = Card_new((char*)curArray[i*3],(char*)curArray[i*3+1],(char*)curArray[i*3+2]);
		if(i==k)
			card->isRight = 1;
		else
			card->isRight = 2;
		if(card->sprite->name)
			free(card->sprite->name);
		card->sprite->name = contact_str("","card");
		Sprite_addChild(cardContainer,card->sprite);
		Sprite_center(card->sprite,(i%sq)*w,(i/sq)*h+_y,w,h);
		//SDL_Log("cardskey:%d,%d",k,i);
		++i;
	}
	UserEvent_new(SDL_USEREVENT,0,Stage_redraw,NULL);//Stage_redraw

	//readAsk(NULL); return NULL;
	pthread_t thread;//创建不同的子线程以区别不同的客户端  
	if(pthread_create(&thread, NULL, readAsk, NULL)!=0)//创建子线程  
	{  
		perror("pthread_create");  
	}else
		pthread_detach(thread);
	return NULL;
}

#ifdef test_readcard
int main()
{
	Stage_init(1);
	if(stage==NULL)return 1;
	stage->sound = Sound_new(16000);

	SDL_Log("sizeof numArr : %ld\n",sizeof(numArr));
	SDL_Log("alphaArr len: %d\n",getArrLen((char**)alphaArr));
	/*
	   Card * card = Card_new("1","one","~/sound/img/1.jpg");
	//card->isRight = 1;
	Sprite_center(card->sprite,0,0,stage->stage_w,stage->stage_h);
	Sprite_addChild(stage->sprite,card->sprite);
	*/

	char ** curArray = getCurArr();
	srand((unsigned)time(NULL));  
	cardskey=(int)(rand()%getArrLen(curArray));
	makeList(&cardskey);

	Stage_loopEvents();
	return 0;
}
#endif
