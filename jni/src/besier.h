
#ifndef besier_h
#define besier_h

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "array.h"
#include "time.h"
#include "sprite.h"
typedef struct Point{
	float x;
	float y;
	float z;
} Point;

typedef struct GraphicData
{
	GLuint programObject;
	GLint positionLoc;
	GLint colorLoc;
	GLint mvpLoc;
	GLfloat *vertices;
	GLfloat *normals;
	GLfloat *texCoords;
	GLuint  *indices;

	Array * points;
} GraphicData;

void Graphic_destroy(Sprite * sprite);
void Graphic_show(Sprite*sprite);

Point * Point_new(float x,float y,float z);
Array * Graphic_curveto(Array*array,Point* p,Point * endPoint);
Array * Graphic_moveto(Array*array,Point* p);
Array * Graphic_lineto(Array*array,Point* p);
void Graphic_drawRoundRect2D(GraphicData * userData,int _x, int _y,int _w,int _h,int r_x,int r_y);
void Graphic_drawRoundRect(GraphicData * userData,float _x, float _y,float _w,float _h,float r_x,float r_y);
#endif
