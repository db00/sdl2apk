/**
 * @file filter.c
 gcc -g -Wall -I"../SDL2/include/" -L"/usr/local/lib/" filter.c array.c tween.c ease.c sprite.c mystring.c matrix.c -lGLESv2 -lm -lSDL2 -D debug_filter && ./a.out
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2016-09-23
 */
#include "array.h"
#include "sprite.h"
typedef struct Filter
{
	int matrixX;//矩阵的 x 维度（矩阵中列的数目）。默认值为 0。 
	int matrixY;//矩阵的 y 维度（矩阵中行的数目）。默认值为 0。 
	Array * matrix;/*用于矩阵转换的值的数组。数组中的项数必须等于 matrixX * matrixY。

					 矩阵盘绕基于一个 n x m 矩阵，该矩阵说明输入图像中的给定像素值如何与其相邻的像素值合并以生成最终的像素值。每个结果像素通过将矩阵应用到相应的源像素及其相邻像素来确定。

					 对于 3 x 3 矩阵卷积，将以下公式用于每个独立的颜色通道：


					 dst (x, y) = ((src (x-1, y-1) * a0 + src(x, y-1) * a1....
					 src(x, y+1) * a7 + src (x+1,y+1) * a8) / divisor) + bias


					 某些规格的滤镜在由提供 SSE（SIMD 流扩展）的处理器运行时执行速度更快。以下是更快的卷积操作的条件：

					 滤镜必须是 3x3 滤镜。
					 所有滤镜项必须是介于 -127 和 +127 之间的整数。
					 所有滤镜项的总和不能包含大于 127 的绝对值。
					 如果任何滤镜项为负，则除数必须介于 2.00001 和 256 之间。
					 如果所有滤镜项都为正，则除数必须介于 1.1 和 256 之间。
					 偏差必须是整数。*/
	int divisor;//矩阵转换中使用的除数。默认值为 1。如果除数是所有矩阵值的总和，则可调平结果的总体色彩强度。忽略 0 值，此时使用默认值。 
	int bias;//要添加到矩阵转换结果中的偏差量。偏差可增加每个通道的颜色值，以便暗色变得较明亮。默认值为 0。
	int preserveAlpha;//表示是否已保留 Alpha 通道并且不使用滤镜效果，或是否对 Alpha 通道以及颜色通道应用卷积滤镜。值为 false 表示卷积应用于所有通道，包括 Alpha 通道。值为 true 表示只对颜色通道应用卷积。默认值为 true。 
	int clamp;//表示是否应锁定图像。对于源图像之外的像素，如果值为 true，则表明通过复制输入图像每个相应的边缘处的颜色值，沿着输入图像的每个边框按需要扩展输入图像。如果值为 false，则表明应按照 color 和 alpha 属性中的指定使用其他颜色。默认值为 true。 
	int color;//要替换源图像之外的像素的十六进制颜色。它是一个没有 Alpha 成分的 RGB 值。默认值为 0。 
	float alpha;//替换颜色的 Alpha 透明度值。有效值为 0 到 1.0。默认值为 0。例如，0.25 设置透明度值为 25%。
}Filter;

//Filter * ConvolutionFilter(int matrixX,int matrixY,Array* matrix,int divisor,int bias,int preserveAlpha,int clamp,unsigned int color,float alpha)
Filter * Filter_new()
{
	Filter * filter = NULL;
	filter = malloc(sizeof(*filter));
	memset(filter,0,sizeof(*filter));
	filter->clamp = 1;
	filter->preserveAlpha = 1;
	filter->divisor = 1;
	return filter;
}

char * getPixel(int x, int y,char * pixels,int w,int h)
{
	if(x<w && y<h)
		return pixels + x + y*w;
	return NULL;
}
char ConvolutionFilter(Filter * filter,int rgba,int x,int y,char * pixels,int w,int h)
{
	int pixel = 0;
	int _x = 0;
	while(_x<filter->matrixX)
	{
		int _y = 0;
		while(_y<filter->matrixY)
		{
			char * p = getPixel(x+_x,y+_y,pixels,w,h)+rgba;
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
			if(p==NULL)
				p = (char*)(&(filter->color)) + 3 - rgba;
#else
			if(p==NULL)
				p = (&(filter->color)) + rgba;
#endif
# if __WORDSIZE == 64
			int k = (long)(Array_getByIndex(filter->matrix,_x+_y*filter->matrixX));
#else
			int k = (int)(Array_getByIndex(filter->matrix,_x+_y*filter->matrixX));
#endif
			pixel += (*p) * k;
			++_y;
		}
		++_x;
	}
	int r = pixel/(filter->divisor);
	if(r>0xff)
		r=0xff;
	return (char)r;

}

/**
 *
 * bmp: source bitmap
 * w: source bitmap width
 * h: source bitmap height
 * 
 * */
char * Filter_apply(Filter * filter,char * bmp,int w,int h,SDL_Rect * clip_rect)
{
	if(bmp==NULL)
		return NULL;
	int startx = 0;
	int starty = 0;
	//int endx = w;
	int endy = h;
	int targetw = w;
	int targeth = h;
	if(clip_rect)
	{
		startx = clip_rect->x;
		starty = clip_rect->y;
		targetw = clip_rect->w;
		targeth = clip_rect->h;
		if(startx+targetw>w) targetw = w-startx;
		if(starty+targeth>h) targeth = h-starty;
		//endx = startx+targetw;
		endy = starty+targeth;
	}
	if(targetw<=0 || targeth<=0)
		return NULL;
	char * bitmap = malloc(targetw*targeth*4);

	memset(bitmap,0,targetw*targeth*4);
	if(filter == NULL || filter->matrix==NULL || filter->matrix->length==0 || filter->matrixX*filter->matrixY!=filter->matrix->length)
	{
		int _y = starty;
		int cury = 0;
		while(_y<endy)
		{
			memcpy(bitmap+(cury*w)*4,bmp+(startx+_y*w)*4,targetw*4);//copy a line
			++_y;
			++cury;
		}
		return bitmap;
	}else{
		int _x = 0;
		while(_x<targetw-filter->matrixX)
		{
			int _y = 0;
			while(_y<targeth-filter->matrixY)
			{
				unsigned char r=0;
				unsigned char g=0;
				unsigned char b=0;
				unsigned char a=0;

				r = ConvolutionFilter(filter,0,startx+_x,starty+_y,bmp,w,h)+filter->bias;
				g = ConvolutionFilter(filter,1,startx+_x,starty+_y,bmp,w,h)+filter->bias;
				b = ConvolutionFilter(filter,2,startx+_x,starty+_y,bmp,w,h)+filter->bias;
				//a = ConvolutionFilter(filter,3,startx+_x,starty+_y,bmp,w,h);
				//if(!filter->preserveAlpha) a = filter->alpha;
				a=0xff;

				bitmap[(_x+_y*targetw)*4]=r;
				bitmap[(_x+_y*targetw)*4+1]=g;
				bitmap[(_x+_y*targetw)*4+2]=b;
				bitmap[(_x+_y*targetw)*4+3]=a;
				++_y;
			}
			++_x;
		}
	}
	return bitmap;
}

#ifdef debug_filter
int main()
{
	Stage_init(1);
	Sprite*sprite = Sprite_new();
	sprite->surface = SDL_LoadBMP("/home/db0/sound/1.bmp");
	Filter * filter = Filter_new();
	filter->matrixX = 3;
	filter->matrixY = 3;
	filter->matrix = Array_push(filter->matrix,(void*)1);
	filter->matrix = Array_push(filter->matrix,(void*)1);
	filter->matrix = Array_push(filter->matrix,(void*)1);
	filter->matrix = Array_push(filter->matrix,(void*)1);
	filter->matrix = Array_push(filter->matrix,(void*)1);
	filter->matrix = Array_push(filter->matrix,(void*)1);
	filter->matrix = Array_push(filter->matrix,(void*)1);
	filter->matrix = Array_push(filter->matrix,(void*)1);
	filter->matrix = Array_push(filter->matrix,(void*)1);
	filter->divisor = 9;
	sprite->surface->pixels = Filter_apply(filter,sprite->surface->pixels,sprite->surface->w,sprite->surface->h,NULL);
	Sprite_addChild(stage->sprite,sprite);
	Stage_loopEvents();
	return 0;
}
#endif
