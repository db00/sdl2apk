/**
 * @file sprite.c
 gcc -Wall -I"../SDL2/include/" -I"../SDL2_ttf/" -DSTDC_HEADERS -I"../SDL2_image/" array.c tween.c ease.c utf8.c btn.c myregex.c dict.c sprite.c mystring.c textfield.c files.c matrix.c -lSDL2_image -lSDL2_ttf -LGLESv2 -lm -lSDL2 && ./a.out
 gcc -Wall -I"../SDL2/include/" -I"../SDL2_ttf/" -DSTDC_HEADERS -I"../SDL2_image/" btn.c myregex.c regex.c dict.c sprite.c mystring.c textfield.c files.c matrix.c -lSDL2_image -lSDL2_ttf -lmingw32 -lSDL2main -lSDL2 && a
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2015-07-21
 */
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "sprite.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "matrix.h"
#include "textfield.h"
#include "dict.h"
#include "myregex.h"

typedef struct Button{
	Sprite * sprite;
}Button;

void mouseDown(SpriteEvent*e)
{
	Sprite * sprite1 = Sprite_getChildByName(stage->sprite,"sprite1");
	Point3d *p = Sprite_localToGlobal(sprite1,NULL);
	//Sprite * sprite2 = Sprite_getChildByName(stage->sprite,"sprite2");
	//Point3d *p = Sprite_localToGlobal(Sprite_getChildByName(sprite2,"sprite3"),NULL);
	//Point3d *p = Sprite_localToGlobal(sprite2,NULL);
	printf("mouseDown:-----------------------------%s,%d,%d,,,%d,%d\n"
			,e->target->name
			,e->target->mouse->x
			,e->target->mouse->y
			,p->x
			,p->y
		  );
	if(e->target->parent)
		Sprite_addChild(e->target->parent,e->target);
	if(e->target!= stage->sprite)e->target->y++;
}

int main(int argc, char *argv[])
{
	Stage_init(1);
	Sprite_addEventListener(stage->sprite,SDL_MOUSEBUTTONDOWN,mouseDown);
	if(stage->GLEScontext == NULL){
		//return 0;
	}
	//else
	{
		Sprite*sprite = Sprite_new();
#ifdef __ANDROID__
		sprite->surface = IMG_Load("ic_launcher.png");
#else
		sprite->surface = IMG_Load("../../res/drawable-hdpi/ic_launcher.png");
#endif
		//sprite->surface = IMG_Load("1.bmp");
		//sprite->texture = SDL_CreateTextureFromSurface(stage->renderer,sprite->surface);
		sprite->x = 30;
		sprite->y = 30;
		sprite->w = 100.0;
		sprite->h = 120.0;
		//sprite->rotation =120;
		//sprite->alpha =0.4;
		sprite->canDrag = SDL_TRUE;
		Sprite_addChild(stage->sprite,sprite);
		Sprite_addEventListener(sprite,SDL_MOUSEBUTTONDOWN,mouseDown);
		//Point3d *gp = Sprite_localToGlobal(sprite,NULL);
		//printf("local:%d,%d---------global:%f,%f\n",sprite->x,sprite->y,gp->x,gp->y);

		/*
		   Sprite*sprite2 = Sprite_new();
#ifdef __ANDROID__
sprite2->surface = IMG_Load("ic_launcher.png");
#else
sprite2->surface = IMG_Load("../../res/drawable-hdpi/ic_launcher.png");
#endif
		//sprite2->texture = SDL_CreateTextureFromSurface(stage->renderer,sprite2->surface);
		sprite2->rotation =0;
		sprite2->x =0;
		sprite2->y =0;
		//sprite2->scaleX =2.0;
		//sprite2->scaleY =2.0;
		sprite2->mouseChildren = SDL_FALSE;
		sprite2->canDrag = SDL_TRUE;
		SDL_Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.w = 0;
		rect.h = 320-72;
		sprite2->dragRect = &rect;
		Sprite_addChild(stage->sprite,sprite2);

		Sprite*sprite3 = Sprite_new();
#ifdef __ANDROID__
sprite3->surface = IMG_Load("ic_launcher.png");
#else
sprite3->surface = IMG_Load("../../res/drawable-hdpi/ic_launcher.png");
#endif
		//sprite3->texture = SDL_CreateTextureFromSurface(stage->renderer,sprite2->surface);
		//sprite3->texture = IMG_LoadTexture(stage->renderer,"../../res/drawable-hdpi/ic_launcher.png");
		sprite3->y =20;
		sprite3->x =30;
		Sprite_addChild(sprite2,sprite3);
		Sprite_addEventListener(sprite2,SDL_MOUSEBUTTONDOWN,mouseDown);
		//Sprite_removeEventListener(sprite2,SDL_MOUSEBUTTONDOWN,mouseDown);
		*/


		/*
		   TextField* txt = TextField_new();
		//txt->backgroundColor = &WHITE;
		txt->sprite->w = stage->w/2;
		txt->sprite->h = stage->h/2;
		txt->x = txt->sprite->w/2;
		txt->y = txt->sprite->h/2;
		//txt->sprite->y = txt->sprite->h/2;
		txt->sprite->canDrag = 1;
		txt = TextField_appendText(txt,"ABCDEFGHIJKLMNOPQRSTUVWXYZ_:" );
		txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
		txt = TextField_appendText(txt,"一二三四五六七八九十一二三四五六七八九十end\n");
		txt = TextField_appendText(txt,"一二三四五六七八九十一二三四五六七八九十end\n");
		Sprite_addChild(stage->sprite,txt->sprite);
		*/
	}
	{	
		TextField* txt2 = TextField_new();
		txt2->w = stage->stage_w;
		txt2->h = stage->stage_h;
		txt2->sprite->canDrag = 1;
		txt2 = TextField_appendText(txt2,"search....\n一二三四五六七八九十3一二三四五六七八九十4一二三四五六七八九十5一二三四五六七八九十6一二三四五六七八九十7一二三四五六七八九十8一二三四五六七八九十9一二三四五六七八九十10一二三四五六七八九十\n一二三四五六七八九十3一二三四五六七八九十4一二三四五六七八九十5一二三四五六七八九十6一二三四五六七八九十7一二三四五六七八九十8一二三四五六七八九十9一二三四五六七八九十10一二三四五六七八九十\n一二三四五六七八九十3一二三四五六七八九十4一二三四五六七八九十5一二三四五六七八九十6一二三四五六七八九十7一二三四五六七八九十8一二三四五六七八九十9一二三四五六七八九十10一二三四五六七八九十\n一二三四五六七八九十3一二三四五六七八九十4一二三四五六七八九十5一二三四五六七八九十6一二三四五六七八九十7一二三四五六七八九十8");
		//txt2 = TextField_appendText(txt2,"search....\n一二三四五六七八九十3一二三四五六七八九十4一二三四五六七八九十5一二三四五六七八九十6一二三四五六七八九十7一二三四五六七八九十8一二三四五六七八九十9一二三四五六七八九十10一二三四五六七八九十\n一二三四五六七八九十3一二三四五六七八九十4一二三四五六七八九十5一二三四五六七八九十6一二三四五六七八九十7一二三四五六七八九十8一二三四五六七八九十9一二三四五六七八九十10一二三四五六七八九十\n一二三四五六七八九十3一二三四五六七八九十4一二三四五六七八九十5一二三四五六七八九十6一二三四五六七八九十7一二三四五六七八九十8一二三四五六七八九十9一二三四五六七八九十10一二三四五六七八九十\n一二三四五六七八九十3一二三四五六七八九十4一二三四五六七八九十5一二三四五六七八九十6一二三四五六七八九十7一二三四五六七八九十8");
		//txt2 = TextField_appendText(txt2,"search....\n一二三四五六七八九十3一二三四五六七八九十4一二三四五六七八九十5一二三四五六七八九十6一二三四五六七八九十7一二三四五六七八九十8一二三四五六七八九十9一二三四五六七八九十10一二三四五六七八九十\n一二三四五六七八九十3一二三四五六七八九十4一二三四五六七八九十5一二三四五六七八九十6一二三四五六七八九十7一二三四五六七八九十8一二三四五六七八九十9一二三四五六七八九十10一二三四五六七八九十\n一二三四五六七八九十3一二三四五六七八九十4一二三四五六七八九十5一二三四五六七八九十6一二三四五六七八九十7一二三四五六七八九十8一二三四五六七八九十9一二三四五六七八九十10一二三四五六七八九十\n一二三四五六七八九十3一二三四五六七八九十4一二三四五六七八九十5一二三四五六七八九十6一二三四五六七八九十7一二三四五六七八九十8");

		char * explain = NULL;
		Dict * dict = NULL;
		dict = Dict_new(); dict->name = "oxford-gb"; explain = Dict_explain(dict,"help"); if(dict)Dict_free(dict); dict = NULL;

		if(explain){
			char *tmp = regex_replace_all(explain,"([^a-zA-Z])( [\\*0-9]+ )","$1\r\n$2");
			free(explain);
			explain = tmp;
			//printf("%s",explain);
			txt2 = TextField_appendText(txt2,explain);
		}
		Sprite_addChild(stage->sprite,txt2->sprite);
	}

	printf("stage ----------- size:%dx%d\n",stage->stage_w,stage->stage_h);
	Stage_loopEvents();
	//SDL_Delay(20);
	return 0;
}
