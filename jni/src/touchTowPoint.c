/**
 *
 gcc -Wall -I"../SDL2/include/" -I"../SDL2_ttf/" -I"../SDL2_image/"  touchTowPoint.c myttf.c -lSDL2_image -lSDL2_ttf -lmingw32 -lSDL2_test -lSDL2main -lSDL2 -Ddebug_touchTowPoint && a

*/
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "myttf.h"
#include "sprite.h"
#include "SDL_image.h"


#ifdef debug_touchTowPoint
int main(int argc, char *argv[])
{
	SDL_Renderer *renderer;
	renderer = init_();

	TweenObj tweenObj;
	memset(&tweenObj,0,sizeof(tweenObj));
	SDL_Texture *texture = IMG_LoadTexture(renderer,"../../a/icon.bmp");
	int w,h;
	SDL_QueryTexture(texture, NULL, NULL, &w, &h);//初始宽高


	SDL_RenderPresent(renderer);

	SDL_Delay(20000);
	return 0;
}
#endif
