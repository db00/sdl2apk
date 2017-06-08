#include "mylist.h"
Sprite * List_AddItem(Sprite * curlistSprite,Sprite * sprite,int end)
{
	if(curlistSprite->children && curlistSprite->children->length>0)
	{
		Sprite * lastSprite = NULL;
		if(end){
			lastSprite = Sprite_getChildByIndex(curlistSprite,curlistSprite->children->length-1);
		}else{
			lastSprite = Sprite_getChildByIndex(curlistSprite,0);
		}
		if(lastSprite)
		{
			if(end){
				sprite->y = lastSprite->y + lastSprite->h + 1;
			}else{
				sprite->y = lastSprite->y - sprite->h - 1;
			}
		}
	}
	if(end){
		Sprite_addChild(curlistSprite,sprite);
	}else{
		Sprite_addChildAt(curlistSprite,sprite,0);
	}
	return sprite;
}

Sprite * makeWordBtn(Word * word,void (*selectedEvent)(SpriteEvent *))
{
	int fontSize = 20*stage->stage_h/320;
	//printf("fontSize: (%d===%d)", fontSize,h);
	Sprite * sprite = Sprite_newText(word->word,fontSize,0x0,0xffffffff);
	sprite->obj= word;
	//sprite->filter = 0;
	if(sprite->name)
		free(sprite->name);
	sprite->name = contact_str("",word->word);
	Sprite_addEventListener(sprite,SDL_MOUSEBUTTONDOWN,selectedEvent);
	Sprite_addEventListener(sprite,SDL_MOUSEBUTTONUP,selectedEvent);
	return sprite;
}

Sprite * List_removeItemByIndex(Sprite * curlistSprite,int i)
{
	int j = curlistSprite->children->length-1;
	if(i<0 || i>j)
		return NULL;
	Sprite * cur = NULL;

	while(j>i)
	{
		if(cur==NULL)
			cur = Sprite_getChildByIndex(curlistSprite,j);
		Sprite * pre = Sprite_getChildByIndex(curlistSprite,j-1);
		//SDL_Log("(%d,%d)(%d,%d)\r\n",cur->x,cur->y,pre->x,pre->y);
		cur->y = pre->y;
		cur = pre;
		--j;
	}

	Sprite * sprite = Sprite_removeChildAt(curlistSprite,i);
	Stage_redraw();
	return sprite;
}

Sprite * List_removeItem(Sprite * curlistSprite,Sprite * sprite)
{
	int i = Sprite_getChildIndex(curlistSprite,sprite);
	return List_removeItemByIndex(curlistSprite,i);
}

void List_showOptical(Sprite * curlistSprite)
{
	if(curlistSprite->children && curlistSprite->children->length>0)
	{
		int i = 0;
		while(i<curlistSprite->children->length)
		{
			Sprite * lastSprite = Sprite_getChildByIndex(curlistSprite,i);
			if(lastSprite->y + lastSprite->h + curlistSprite->y<0)
			{
				lastSprite->visible = SDL_FALSE;
			}else if(lastSprite->y + curlistSprite->y > stage->stage_h){
				lastSprite->visible = SDL_FALSE;
			}else{
				lastSprite->visible = SDL_TRUE;
			}
			++i;
		}
	}
	Stage_redraw();
}
void List_removeOuts(Sprite * curlistSprite)
{
	if(curlistSprite->children && curlistSprite->children->length>0)
	{
		Sprite * lastSprite = NULL;
		lastSprite = Sprite_getChildByIndex(curlistSprite,0);
		while(lastSprite->y + lastSprite->h + curlistSprite->y<0)
		{
			Word * _word = lastSprite->obj;
			Sprite_removeChildAt(curlistSprite,0);
			Sprite_destroy(lastSprite);
			lastSprite = Sprite_getChildByIndex(curlistSprite,0);
		}

		lastSprite = Sprite_getChildByIndex(curlistSprite,curlistSprite->children->length-1);
		while(lastSprite->y + curlistSprite->y > stage->stage_h)
		{
			Word * _word = lastSprite->obj;
			Sprite_removeChild(curlistSprite,lastSprite);
			Sprite_destroy(lastSprite);
			lastSprite = Sprite_getChildByIndex(curlistSprite,curlistSprite->children->length-1);
		}
	}
	Stage_redraw();
}
