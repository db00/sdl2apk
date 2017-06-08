#ifndef mylist_h
#define mylist_h
#include "sprite.h"
#include "mysurface.h"
#include "dict.h"
Sprite * List_AddItem(Sprite * curlistSprite,Sprite * sprite,int end);
Sprite * List_removeItem(Sprite * curlistSprite,Sprite * sprite);
Sprite * makeWordBtn(Word * word,void (*selectedEvent)(SpriteEvent *));
void List_showOptical(Sprite * curlistSprite);
void List_removeOuts(Sprite * curlistSprite);
#endif
