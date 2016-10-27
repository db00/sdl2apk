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

	Point* P = malloc(sizeof(Point));  
	P->x = a1*A->x+a2*B->x+a3*C->x+a4*D->x;  
	P->y = a1*A->y+a2*B->y+a3*C->y+a4*D->y;  
	P->z = 0.0;
	return P;    
}    
//三个控制点的贝塞尔曲线 即二次Bezier曲线  
Point* drawBezier2(Point * A, Point * B, Point * C,double t)     
{    
	double a1 = pow((1-t),2); 
	double a2 = pow((1-t),2)*2*t;  
	double a3 = t*t;  

	Point* P = malloc(sizeof(Point));  
	memset(P,0,sizeof(Point));
	P->x = a1*A->x+a2*B->x+a3*C->x;  
	P->y = a1*A->y+a2*B->y+a3*C->y;  
	P->z = 0.0;
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
	Point* P = malloc(sizeof(Point));  
	double a1 = 1-t; 
	double a2 = t;  

	P->x = a1*A->x+a2*B->x;
	P->y = a1*A->y+a2*B->y;  
	P->z = 0.0;
	return P;    
}    

Array * moveto(Array*array,Point* p)
{
	return Array_push(array,p);
}
Array * lineto(Array*array,Point* p)
{
	return Array_push(array,p);
}

Array * curveto(Array*array,Point* p,Point * endPoint)
{
	Point * startPoint = NULL;
	if(array->length==0)
	{
		startPoint = malloc(sizeof(Point));
		memset(startPoint,0,sizeof(Point));
		array = Array_push(array,startPoint);
	}else{
		startPoint = Array_getByIndex(array,array->length - 1);
	}

	//double numPoint = 20.0;//(abs(p->x - startPoint->x) + abs(p->y - startPoint->y) + abs(p->x - endPoint->x) + abs(p->y - endPoint->y)*100);
	double numPoint = (abs(p->x - startPoint->x) + abs(p->y - startPoint->y) + abs(p->x - endPoint->x) + abs(p->y - endPoint->y)*20);
	double t = (double)1.0/numPoint;
	int i = 0;
	while(i<=numPoint)
	{
		//printf("===%f\n",t*i);
		Point * point = drawBezier2(startPoint,p,endPoint,t*i);
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
	printf("numPoint=%d\n",numPoint);

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
			printf("x=%f,",point->x);
			printf("y=%f\n",point->y);
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


#ifdef DEBUG_BESIER
int main()
{
	int i = 0;
	while(i<=4)
	{
		//printf("%d! = %d\n",i,factorial(i));
		//printf("A_4_%d = %d\n",i, A_n_m(4,i));
		printf("C_4_%d = %d\n",i, C_n_m(4,i));
		++i;
	}



	Stage_init(1);
	//Sprite*sprite = Sprite_new();
	//SDL_Surface * surface = Surface_new(240,320);
	//sprite->surface = surface;


	UserData userData;
	memset(&userData,0,sizeof(UserData));
	//userData.points;
	Point p = {0.0,0.0,0.0};
	userData.points = moveto(userData.points,&p);
	Point p1 = {0.0,1.0,0.0};
	userData.points = lineto(userData.points,&p1);
	Point p2 = {0.2,-0.5,0.0};
	userData.points = lineto(userData.points,&p2);
	Point p3 = {.5,0.0,0.0};
	userData.points = lineto(userData.points,&p3);
	userData.points = curveto(userData.points,&p1,&p);
	//userData.points = curveto(userData.points,&p2,&p);

	Sprite*sprite = Sprite_new();
	sprite->x = 120;
	sprite->y = 180;
	sprite->data3d = &userData;
	sprite->showFunc = Data3d_show;
	sprite->destroyFunc = Data3d_destroy;
	Sprite_addChild(stage->sprite,sprite);


	Stage_loopEvents();
	return 0;
}
#endif
