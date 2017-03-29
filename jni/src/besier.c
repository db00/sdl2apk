/**
 * @file besier.c
 gcc -g -Wall -I"include" -I"../SDL2/include/" besier.c -D DEBUG_BESIER  array.c  sprite.c mystring.c   matrix.c  -lm -lSDL2  && ./a.out

 * @author db0@qq.com
 * @version 1.0.1
 * @date 2016-06-17
 */

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

Point * Point_new(float x,float y,float z)
{
	Point * p = malloc(sizeof(Point));
	p->x = x;
	p->y = y;
	p->z = z;
	return p;
}

//阶乘
int factorial(int n)
{
	int i = 0;
	int end = 1;
	while(i<n)
	{
		end *= ++i;
	}
	return end;
}
//排列
int A_n_m(int n,int m)//(n-m+1)*...*(n-1)*(n)
{
	//return factorial(n)/factorial(n-m);
	int i = n-m;
	int end = 1;
	while(i<n)
	{
		end *= ++i;
	}
	return end;
}
//组合
int C_n_m(int n,int m)
{
	return A_n_m(n,m)/factorial(m);
}

//四个控制点的贝塞尔曲线 即三次Bezier曲线  
Point* drawBezier3(Point * A, Point * B, Point * C, Point * D,double t)     
{    
	double a1 = pow((1-t),3);  
	double a2 = pow((1-t),2)*3*t;  
	double a3 = 3*t*t*(1-t);  
	double a4 = t*t*t;  

	Point* P = Point_new(
			a1*A->x+a2*B->x+a3*C->x+a4*D->x,
			a1*A->y+a2*B->y+a3*C->y+a4*D->y,
			0.0
			);
	return P;    
}    
//三个控制点的贝塞尔曲线 即二次Bezier曲线  
Point* drawBezier2(Point * A, Point * B, Point * C,float t)     
{    
	float a1 = pow((1.0-t),2); 
	float a2 = 2*(1.0-t)*t;  
	float a3 = t*t;  
	//printf("%f,%f,%f\n",a1,a2,a3);

	Point* P = Point_new(
			a1*(A->x)+a2*(B->x)+a3*(C->x),
			a1*(A->y)+a2*(B->y)+a3*(C->y), 
			0.0
			);
	return P;    
}
//n个控制点的贝塞尔曲线 即n-1次Bezier曲线  
Point * drawBezierN(Point ** PA,int n,double t)     
{    
	Point* P = malloc(sizeof(Point));  
	P->x = 0;
	P->y = 0;

	int i = 0;
	double a; 
	while(i<n)
	{
		a = pow((1-t),n-i)*pow(t,i)*C_n_m(n,i);
		Point * point = PA[i];
		P->x += a*point->x;  
		P->y += a*point->y;  
		P->z = 0.0;
		++i;
	}
	return P;    
}
// 
Point * drawLine(Point * A, Point * B,double t)     
{    
	double a1 = 1-t; 
	double a2 = t;  

	Point* P = Point_new(
			a1*A->x+a2*B->x,
			a1*A->y+a2*B->y,  
			0.0
			);
	return P;    
}    

Array * moveto(Array*array,Point* p)
{
	return Array_push(array,p);
}
Array * lineto(Array*array,Point* p)
{
	Array * a = Array_push(array,p);
	//printf("\r\nlength:%d\r\n",a->length);
	return a;
}

Array * curveto(Array*array,Point* p,Point * endPoint)
{
	Point * startPoint = NULL;
	if(array->length==0)
	{
		startPoint = Point_new(0.0,0.0,0.0);
		array = Array_push(array,startPoint);
	}else{
		startPoint = Array_getByIndex(array,array->length - 1);
	}
	//printf("startPoint===%f,%f,%f\n",startPoint->x,startPoint->y,startPoint->z);

	//float numPoint = 20.0;//(abs(p->x - startPoint->x) + abs(p->y - startPoint->y) + abs(p->x - endPoint->x) + abs(p->y - endPoint->y)*100);
	float numPoint = (abs(p->x - startPoint->x) + abs(p->y - startPoint->y) + abs(p->x - endPoint->x) + abs(p->y - endPoint->y))*20.0;
	if(numPoint<=1) numPoint = 4.0;
	else if(numPoint>=100) numPoint = 100.0;
	float t = (float)1.0/numPoint;
	int i = 0;
	while(i<=numPoint)
	{
		//printf("===%f\n",t*i);
		Point * point = drawBezier2(startPoint,p,endPoint,t*i);
		//printf("===%f,%f,%f\n",point->x,point->y,point->z);
		array = Array_push(array,point);
		++i;
	}
	return array;
}

typedef struct UserData
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
} UserData;

static void Data3d_destroy(Sprite * sprite)
{
	if(sprite == NULL || sprite->data3d==NULL)
		return;
	UserData * data3d = sprite->data3d;
	if(data3d){
		if(data3d->vertices)free(data3d->vertices);
		if(data3d->indices)free(data3d->indices);
		if(data3d->normals)free(data3d->normals);
		if(data3d->texCoords)free(data3d->texCoords);
		free(data3d);
	}
	sprite->data3d= NULL;
}

static void Data3d_show(Sprite*sprite)
{
	UserData * userData = sprite->data3d;
	if(userData==NULL)
		return ;
	if(userData->programObject == 0)
	{
		GLbyte vShaderStr[] =  
			"uniform mat4 u_mvpMatrix;    \n"
			"attribute vec4 vPosition;    \n"
			"attribute vec4 color;    \n"
			"varying vec4 vcolor;    \n"
			"void main()                  \n"
			"{                            \n"
			"   vcolor = color;  \n"
			"   gl_Position = u_mvpMatrix * vec4(vPosition);  \n"//
			"}                            \n";

		GLbyte fShaderStr[] =  
#ifndef HAVE_OPENGL
			"precision mediump float;\n"
#endif
			"varying vec4 vcolor;    \n"
			"void main()                                  \n"
			"{                                            \n"
			"  gl_FragColor = vec4 ( vcolor );\n"
			"}                                            \n";

		GLuint vertexShader;
		GLuint fragmentShader;
		GLuint programObject;
		GLint linked;
		// Load the vertex/fragment shaders
		vertexShader = LoadShader ( GL_VERTEX_SHADER, vShaderStr );
		fragmentShader = LoadShader ( GL_FRAGMENT_SHADER, fShaderStr );
		// Create the program object
		programObject = GL_CHECK(gles2.glCreateProgram ());
		if ( programObject == 0 ){
			SDL_Log( "programObject==0\n");
			return ;
		}

		GL_CHECK(gles2.glAttachShader ( programObject, vertexShader ));
		GL_CHECK(gles2.glAttachShader ( programObject, fragmentShader ));

		// Link the program
		GL_CHECK(gles2.glLinkProgram ( programObject ));

		// Check the link status
		GL_CHECK(gles2.glGetProgramiv ( programObject, GL_LINK_STATUS, &linked ));

		if ( !linked ) 
		{
			GLint infoLen = 0;
			GL_CHECK(gles2.glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen ));
			if ( infoLen > 1 )
			{
				char infoLog [(sizeof(char) * infoLen )];
				GL_CHECK(gles2.glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog ));
				SDL_Log( "Error linking program:\n%s\n", infoLog );
				//esLogMessage ( "Error linking program:\n%s\n", infoLog );            
			}
			GL_CHECK(gles2.glDeleteProgram ( programObject ));
			return ;
		}
		userData->programObject = programObject;
		if(programObject){
			userData->positionLoc = GL_CHECK(gles2.glGetAttribLocation ( programObject, "vPosition" ));
			SDL_Log("positionLoc:%d\n",userData->positionLoc);
			userData->colorLoc = GL_CHECK(gles2.glGetAttribLocation( programObject, "color"));
			SDL_Log("colorLoc:%d\n",userData->colorLoc);
			userData->mvpLoc = GL_CHECK(gles2.glGetUniformLocation( programObject, "u_mvpMatrix" ));
			SDL_Log("mvpLoc:%d\n",userData->mvpLoc);
		}
		SDL_Log( "linking program:\n%d\n", programObject);
		//GL_CHECK(gles2.glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f ));
	}

	if(userData->programObject)
	{
		GL_CHECK(gles2.glUseProgram ( userData->programObject));
	}else{
		return;
	}

	int numPoint = userData->points->length;
	//printf("numPoint=%d\n",numPoint);

	GLfloat *vVertices= malloc(sizeof(GLfloat)*numPoint*3);
	//GLushort *vVertices = malloc(sizeof(GLfloat)*numPoint*3);
	GLfloat *fillColor = malloc(sizeof(GLfloat)*numPoint*4);
	GLfloat *lineColor = malloc(sizeof(GLfloat)*numPoint*4);
	GLushort *indices = malloc(sizeof(GLushort)*numPoint*3);
	int index = 0;
	srand((unsigned)time(NULL));  
	Point * point;
	while(index<numPoint)
	{
		float r;
		//r = 1.0;
		//printf("r=%f\n",r);
		{
			point = Array_getByIndex(userData->points,index);
			vVertices[3*index] = point->x;
			vVertices[3*index+1] = point->y;
			vVertices[3*index+2] = 0.0f;
			//printf("x=%f,",point->x);
			//printf("y=%f\n",point->y);
		}
		{
			r = (rand()%1000)/1000.0f;
			fillColor[4*index] = 1.0 * r;
			lineColor[4*index] = 1.0 * r;
			r = (rand()%1000)/1000.0f;
			fillColor[4*index+1] = 1.0 * r;
			lineColor[4*index+1] = 1.0 * r;
			r = (rand()%1000)/1000.0f;
			fillColor[4*index+2] = 1.0 * r;
			lineColor[4*index+2] = 1.0 * r;

			fillColor[4*index+3] = 1.0;
			lineColor[4*index+3] = 1.0;
		}
		{
			indices[index] = index;
		}
		++index;
	}

	if(userData->mvpLoc>=0){
		Sprite_matrix(sprite);
		GL_CHECK(gles2.glUniformMatrix4fv( userData->mvpLoc, 1, GL_FALSE, (GLfloat*) &sprite->mvpMatrix.rawData[0][0]));
	}
	GL_CHECK(gles2.glDisable( GL_CULL_FACE ));
	GL_CHECK(gles2.glLineWidth(2.0));
	//GL_CHECK(gles2.glEnable(GL_LINE_SMOOTH));
	// Load the vertex data
	GL_CHECK(gles2.glVertexAttribPointer ( userData->positionLoc, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), vVertices ));
	GL_CHECK(gles2.glEnableVertexAttribArray ( userData->positionLoc ));
	/*
	   GL_CHECK(gles2.glVertexAttribPointer ( userData->colorLoc, 4, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), fillColor));
	   GL_CHECK(gles2.glEnableVertexAttribArray ( userData->colorLoc));
	   GL_CHECK(gles2.glDrawArrays ( GL_TRIANGLE_FAN, 0, numPoint));
	   */
	//GL_CHECK(gles2.glDrawArrays ( GL_TRIANGLES, 0, 3 ));
	//
	//GL_CHECK(gles2.glDrawArrays( GL_LINES,0, 8));//, GL_UNSIGNED_SHORT, indices);

	GL_CHECK(gles2.glVertexAttribPointer ( userData->colorLoc, 4, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), lineColor));
	GL_CHECK(gles2.glEnableVertexAttribArray ( userData->colorLoc));

	//GL_CHECK(gles2.glDrawElements ( GL_LINE_LOOP, numPoint, GL_UNSIGNED_SHORT, indices));
	//GL_CHECK(gles2.glDrawArrays ( GL_LINE_STRIP, 0, numPoint));
	GL_CHECK(gles2.glDrawElements ( GL_LINE_STRIP,numPoint, GL_UNSIGNED_SHORT, indices));
	//GL_CHECK(gles2.glDrawElements ( GL_TRIANGLES, numPoint, GL_UNSIGNED_SHORT, indices));
	//eglSwapBuffers ( esContext->eglDisplay, esContext->eglSurface );
	//
	free(indices);
	free(fillColor);
	free(lineColor);
	free(vVertices);
}

void drawRoundRect(UserData * userData,float _x, float _y,float _w,float _h,float r_x,float r_y)
{
	//if(r_x*2>_w) r_x = _w/2;
	//if(r_y*2>_h) r_y = _h/2;
	Point * p0 = Point_new(_x,_y,0.0);//left top
	Point * p1 = Point_new(_x+r_x,_y,0.0);
	Point * p2 = Point_new(_x+_w-r_x,_y,0.0);
	Point * p3 = Point_new(_x+_w,_y,0.0);//rig_ht top
	Point * p4 = Point_new(_x+_w,_y+r_y,0.0);
	Point * p5 = Point_new(_x+_w,_y+_h-r_y,0.0);
	Point * p6 = Point_new(_x+_w,_y+_h,0.0);//rig_ht bottom
	Point * p7 = Point_new(_x+_w-r_x,_y+_h,0.0);
	Point * p8 = Point_new(_x+r_x,_y+_h,0.0);
	Point * p9 = Point_new(_x,_y+_h,0.0);//left bottom
	Point * p10 = Point_new(_x,_y+_h-r_y,0.0);
	Point * p11 = Point_new(_x,_y+r_y,0.0);

	userData->points = moveto(userData->points,p1);
	userData->points = lineto(userData->points,p2);
	userData->points = curveto(userData->points,p3,p4);
	//userData->points = moveto(userData->points,p4);
	userData->points = lineto(userData->points,p5);
	userData->points = curveto(userData->points,p6,p7);
	//userData->points = moveto(userData->points,p7);
	userData->points = lineto(userData->points,p8);
	userData->points = curveto(userData->points,p9,p10);
	//userData->points = moveto(userData->points,p10);
	userData->points = lineto(userData->points,p11);
	userData->points = curveto(userData->points,p0,p1);
}

void drawRoundRect2D(UserData * userData,int _x, int _y,int _w,int _h,int r_x,int r_y)
{
	drawRoundRect(userData,
			xto3d(_x),yto3d(_y),
			wto3d(_w),(-hto3d(_h)),
			wto3d(r_x),(-hto3d(r_y))
			);
}

#ifdef DEBUG_BESIER
int main()
{
	int i = 0;
	while(i<=4)
	{
		//printf("%d! = %d\n",i,factorial(i));
		//printf("A_4_%d = %d\n",i, A_n_m(4,i));
		//printf("C_4_%d = %d\n",i, C_n_m(4,i));
		++i;
	}



	Stage_init(1);
	//Sprite*sprite = Sprite_new();
	//SDL_Surface * surface = Surface_new(240,320);
	//sprite->surface = surface;


	UserData userData;
	memset(&userData,0,sizeof(UserData));
	drawRoundRect(&userData,-0.1,-0.1,.50,.50,0.05,0.05);


	//
	Sprite*sprite2 = Sprite_new();
	sprite2->surface = SDL_LoadBMP("1.bmp");
	Sprite_addChild(stage->sprite,sprite2);

	Sprite*sprite = Sprite_new();
	sprite->x = stage->stage_w/2;
	sprite->y = stage->stage_h/2;
	sprite->data3d = &userData;
	sprite->showFunc = Data3d_show;
	sprite->destroyFunc = Data3d_destroy;
	Sprite_addChild(stage->sprite,sprite);


	UserData userData2;
	memset(&userData2,0,sizeof(UserData));
	drawRoundRect2D(&userData2,0,0,200,300,40,40);
	Sprite*sprite3 = Sprite_new();
	sprite3->x = stage->stage_w/2;
	sprite3->y = stage->stage_h/2;
	sprite3->data3d = &userData2;
	sprite3->showFunc = Data3d_show;
	sprite3->destroyFunc = Data3d_destroy;
	Sprite_addChild(stage->sprite,sprite3);

	Stage_loopEvents();
	return 0;
}
#endif
