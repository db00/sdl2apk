/**
 * @file loading.c
 gcc -g -Wall -I"../SDL2/include/"  files.c mystring.c myregex.c update.c httploader.c bytearray.c loading.c tween.c ease.c sprite.c matrix.c array.c zip.c ipstring.c urlcode.c base64.c -lz -lcrypto -lssl -lSDL2 -lm -D debug_loading && ./a.out
 *  
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2017-05-12
 */
#include "loading.h"

extern Data3d * data2D;
static Tween * tween = NULL;
static Sprite * contener=NULL;
static Sprite * _sprite=NULL;

void Loading_show(int boolean,char * s)
{
	if(s){
		printf("%s\r\n",s);
	}
	if(_sprite==NULL)
	{
		char * path = decodePath("~/sound/1.bmp");
		if(!fileExists("~/sound/1.bmp"))
		{
			loadAndunzip("https://git.oschina.net/db0/kodi/raw/master/earth.zip","~/sound/");
		}
		_sprite = Sprite_new();
		_sprite->is3D = 1;
		_sprite->surface = (SDL_LoadBMP(path));
		SDL_Log("surface size:%dx%d",_sprite->surface->w,_sprite->surface->h);
		Data3d*_data3D = _sprite->data3d;
		if(_data3D==NULL){
			_data3D = Data3D_new();
			if(_data3D->programObject==0){
				Data3D_init();
				Data3d_set(_data3D,data2D);
			}
			_sprite->data3d = _data3D;
			_data3D->numIndices = esGenSphere ( 20, .75f, &_data3D->vertices, &_data3D->normals, &_data3D->texCoords, &_data3D->indices );
		}
		//_sprite->alpha = 0.5;
	}
	if(contener == NULL)
	{
		contener = Sprite_new();
		Sprite_addChild(contener,_sprite);
		contener->x = stage->stage_w/2;
		contener->y = stage->stage_h/2;
		contener->w = stage->stage_w;
		contener->h = stage->stage_h;
	}

	if(boolean)
	{
		char * path = decodePath("~/sound/1.bmp");
		_sprite->surface = (SDL_LoadBMP(path));
		Sprite_addChild(stage->sprite,contener);
		contener->visible = SDL_TRUE;
		if(tween == NULL)
		{
			_sprite->rotationY = 0;
			TweenObj * tweenObj = (TweenObj*)TweenObj_new(_sprite);
			//tweenObj->end->scaleX = 8.0;
			//tweenObj->end->scaleY = 4.0;
			//tweenObj->end->x=240-_sprite->w;
			//tweenObj->end->y=320-_sprite->h;
			//tweenObj->end->alpha=1.0;
			//tweenObj->end->rotationX=(double)360.0*4;//度数
			//tweenObj->end->rotationY=(double)360.0*4;//度数
			tweenObj->end->rotationY=(double)360.0;//度数
			tween = tween_to(_sprite,3000 ,tweenObj);
			//tween->ease = easeInOut_bounce;
			tween->loop = 1;
		}
	}else{
		if(contener)
		contener->visible = SDL_FALSE;
		//Stage_redraw();
		//if(Sprite_contains(stage->sprite,contener)) Sprite_removeChild(stage->sprite,contener);
		if(tween)
		{
			Tween_kill(tween,1);
			tween = NULL;
		}
	}
}


#ifdef debug_loading




int main(int argc, char *argv[])
{
	Stage_init(1);


	Loading_show(1,"show");
	Loading_show(0,"hide");
	Loading_show(1,"show");
	//Loading_show(0,"hide");



	Stage_loopEvents();
	return 0;
}
#endif //debug_loading
