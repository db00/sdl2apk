#ifndef input_h
#define input_h

#include "textfield.h"
typedef struct Input
{
    TextField * textfield;
    Sprite *sprite;
	SDL_Rect *rect;
    char * value;
}Input;
Input * Input_new(int w,int h);
void Input_clear(Input * input);
char * Input_setText(Input * input,char * s);
#endif
