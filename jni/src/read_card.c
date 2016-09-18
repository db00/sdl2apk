/**
 * @file read_card.c
 gcc -g -Wall -D test_readcard -Wall -lpthread utf8.c mysurface.c input.c array.c ease.c tween.c kodi.c base64.c urlcode.c filetypes.c httpserver.c httploader.c readbaidu.c ipstring.c read_card.c dict.c myregex.c files.c mystring.c sprite.c matrix.c textfield.c -lcrypto -lssl -ldl -lm -I"../SDL2_mixer/" -I"../SDL2_image/" -I"../SDL2/include/" -I"../libxml/include/" -I"../SDL2_ttf/" -lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_image && ./a.out
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2016-09-18
 */
#include "mysurface.h"

#ifdef test_readcard

typedef struct Card
{
	Sprite * sprite;
	Sprite * img;
	Sprite * ch;//->obj
	Sprite * en;//->obj
}Card;

Card * Card_new(char * ch,char*en,char * url)
{
	Card * card = malloc(sizeof(Card));
	memset(card,0,sizeof(Card));

	card->sprite = Sprite_new();
	card->sprite->obj = card;
	card->ch = Sprite_newText(ch,20,0xff0000ff,0xffff00ff);
	card->en = Sprite_newText(en,20,0xff0000ff,0xffff00ff);
	card->img = Sprite_newImg(url);


	int w = 100;
	int h = 100;
	Sprite_fullcenter(card->img,0,0,w,h);
	Sprite_center(card->ch,0,0,w/2,card->ch->h);
	Sprite_center(card->en,w/2,0,w/2,card->en->h);
	card->sprite->w = w;
	card->sprite->h = h;

	Sprite_addChild(card->sprite,card->img);
	Sprite_addChild(card->sprite,card->ch);
	Sprite_addChild(card->sprite,card->en);

	return card;
}
void Card_free(Card*card)
{
	if(card)
	{
		free(card);
	}
}
void clicked(SpriteEvent* e)
{
	Sprite * sprite = e->target;
	Card * card = sprite->obj;
	char ch = 1;
	if(ch)
	{
		SDL_Log(card->ch->obj);
	}else{
		SDL_Log(card->en->obj);
	}
}

int main()
{
	Stage_init(1);
	if(stage==NULL)return 1;

	Card * card = Card_new("地球","earth","1.bmp");

	Sprite_center(card->sprite,0,0,stage->stage_w,stage->stage_h);

	Sprite_addChild(stage->sprite,card->sprite);
	Sprite_addEventListener(card->sprite,SDL_MOUSEBUTTONUP,clicked);
	Stage_loopEvents();
	return 0;
}
#endif
