/**
 * @file myttf.c
 gcc -Wall -I"../SDL2/include/"  -I"../SDL2_ttf/" myfont.c array.c utf8.c update.c httploader.c ipstring.c urlcode.c base64.c  bytearray.c zip.c files.c myregex.c sdlstring.c myttf.c matrix.c sprite.c mystring.c  -lSDL2_ttf -lz -lssl -lcrypto -lpthread -lm -lSDL2 -D debugtext  && ./a.out
 gcc -g -Wall -I"../SDL2/include/" -I"../SDL2_image/" -I"../SDL2_ttf/" array.c tween.c ease.c myttf.c matrix.c sprite.c sdlstring.c mystring.c files.c -lSDL2_ttf -lmingw32 -lSDL2main -lSDL2 -D debugtext && a
 *  
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2015-07-21
 */
#include "myttf.h"

static void mouseWheels(SpriteEvent*e);

static void TextWord_clear(TextWord*textword)
{
	if(textword == NULL)
		return;
	free(textword);
}

static void TextLine_clear(TextLine*line)
{
	if(line== NULL)
		return;

	if(line->words)
	{
		int i = 0;
		while(i<line->words->length)
		{
			TextWord * word = Array_getByIndex(line->words,i);
			TextWord_clear(word);
			++i;
		}
	}

	if(line->text) {
		free(line->text);
		line->text = NULL;
	}
	free(line);
	line = NULL;
}


static void TextField_clearLines(TextField*textfield)
{
	if(textfield->lines)
	{
		int i = 0;
		while(i<textfield->lines->length){
			TextLine * line = Array_getByIndex(textfield->lines,i);
			TextLine_clear(line);
			++i;
		}
		Array_clear(textfield->lines);
		textfield->lines = NULL;
	}
}
void TextField_clear(TextField*textfield)
{
	if(textfield == NULL)
		return;

	if(textfield->font){
	}

	TextField_clearLines(textfield);

	if(textfield->textColor){
		free(textfield->textColor);
	}

	if(textfield->backgroundColor){
		free(textfield->backgroundColor);
	}

	if(textfield->text) {
		free(textfield->text);
	}
	if(textfield->sprite) {
		Sprite_removeChildren(textfield->sprite);
		Sprite_destroy(textfield->sprite);
	}
	free(textfield);
}


static TextWord * Textword_new(TTF_Font * font, Uint8 * curChar)
{
	TextWord* textword = (TextWord*)malloc(sizeof(TextWord));
	memset(textword,0,sizeof(TextWord));

	textword->numbyte = UTF8_numByte((char*)curChar);
	if(('A'<=curChar[0] && curChar[0]<='Z') || ('a'<=curChar[0] && curChar[0]<='z'))
	{//单词不分段
		while(
				('a'<=curChar[textword->numbyte] && curChar[textword->numbyte]<='z')
				|| ('A'<=curChar[textword->numbyte] && curChar[textword->numbyte]<='Z')
			 )
			textword->numbyte++;
	}

	char word[textword->numbyte+1];
	memset(word,0,textword->numbyte + 1);
	memcpy(word,curChar,textword->numbyte);

	TTF_SizeUTF8(font,word,&(textword->w),&(textword->h));
	return textword;
}

static TextLine * TextLine_new()
{
	TextLine * textline = (TextLine*)malloc(sizeof(TextLine));
	memset(textline,0,sizeof(TextLine));
	memset(&(textline->rect) ,0 ,sizeof(SDL_Rect));
	return textline;
}

static TextLine * TextField_getLastLine(TextField * textfield)
{
	if(textfield==NULL){
		SDL_Log("TextField_getLastLine Error: textfield is NULL!\n");
		return NULL;
	}
	if(textfield->lines == NULL){
		TextLine * line = TextLine_new();
		textfield->lines = Array_new();
		Array_push(textfield->lines,line);
	}
	return Array_getByIndex(textfield->lines,textfield->lines->length-1);
}

static TextLine * TextField_appenNewLine(TextField * textfield)
{
	TextLine * line = TextLine_new();
	line->lineId = textfield->lines->length;
	Array_push(textfield->lines,line);
	line->rect.y = textfield->textHeight;
	textfield->textHeight = line->rect.y + line->rect.h;
	return line;
}

static SDL_bool TextField_lineFull(TextField * textfield,TextLine * line,TextWord*textword)
{
	if(textfield && line && textword){
		if((textword->w) + (line->rect.w) > (textfield->w)) {
			return SDL_TRUE;
		}

		if(line->text && strlen(line->text)) {
			char * lastWord = line->text + line->numbyte -1;
			if(*lastWord == '\r' || *lastWord == '\n' )
				return SDL_TRUE;
		}
	}
	return SDL_FALSE;
}


static void setLineTexture(TextField * textfield,TextLine * line)
{
	//printf(" Surface_new ERROR!\n");fflush(stdout);
	SDL_Surface * surface = NULL;
	if(textfield->backgroundColor)
	{
		surface= TTF_RenderUTF8_Shaded(textfield->font, line->text, *(textfield->textColor),*(textfield->backgroundColor));
	}
	else
	{
		//surface= TTF_RenderUTF8_Blended(textfield->font, line->text, *(textfield->textColor));
		surface= TTF_RenderUTF8_Solid(textfield->font, line->text, *(textfield->textColor));
	}

	if(surface){
		//line->rect.w = surface->w;
		//line->rect.h = surface->h;//初始宽高
		//SDL_BlendMode saved_mode;
		SDL_Rect dstrect;
		dstrect.x = line->rect.x;
		dstrect.y = line->rect.y;
		dstrect.w = line->rect.w;
		dstrect.h = line->rect.h;
		//SDL_GetSurfaceBlendMode(textfield->sprite->surface, &saved_mode);
		//SDL_SetSurfaceBlendMode(textfield->sprite->surface, SDL_BLENDMODE_NONE);
		SDL_BlitSurface(surface, NULL, textfield->sprite->surface, &dstrect);
		//SDL_SetSurfaceBlendMode(textfield->sprite->surface, saved_mode);

		SDL_FreeSurface(surface); surface = NULL;
	}
}


static int drawLines(TextField * textfield)
{
	Sprite *sprite = textfield->sprite;


	SDL_Surface * surface = sprite->surface;
	if((surface && (surface->w != textfield->textWidth || surface->h != textfield->textHeight)))
	{
		Sprite_destroySurface(sprite);
		Sprite_destroyTexture(sprite);
	}

	int fontheight = TTF_FontHeight(textfield->font);

	if((stage->GLEScontext && sprite->surface==NULL && sprite->data3d==NULL))
	{
		sprite->surface = Surface_new(textfield->textWidth,textfield->textHeight);
		if(sprite->w != textfield->textWidth) sprite->w = textfield->textWidth;
		if(sprite->h != textfield->textHeight) sprite->h = textfield->textHeight;

		int i = 0;
		while(i<textfield->lines->length)
		{
			TextLine * line = Array_getByIndex(textfield->lines,i);
			if(sprite->surface)
				setLineTexture(textfield,line);
			++i;
		}
	}

	Sprite * target = sprite;
	SDL_Rect *rect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	rect->x = textfield->x;
	rect->y = textfield->y - textfield->textHeight + fontheight;
	rect->w = 0;
	rect->h = textfield->textHeight - fontheight;
	if(target->dragRect)
		free(target->dragRect);
	target->dragRect = rect;
	Sprite_limitPosion(target,target->dragRect);
	return 0;
}


static void mouseWheels(SpriteEvent*e)
{
	SDL_Event *event  = e->e;
	Sprite *sprite = e->target;
	TextField * textfield = (TextField*)(sprite->obj);
	if(textfield->textHeight<1)
		return;


	if(textfield && textfield->lines){
		if(event->type == SDL_MOUSEWHEEL){//
			int lineHeight = TTF_FontHeight(textfield->font);
			if(event->wheel.y > 0){
				sprite->y += lineHeight;
			}else if(event->wheel.y < 0){
				sprite->y -= lineHeight;
			}
		}else if(event->motion.state){
			sprite->y += event->motion.yrel;
		}
		if(sprite->y > textfield->y)//top
			sprite->y = textfield->y;
		else if(sprite->y+sprite->h < textfield->y+textfield->h)
			sprite->y = textfield->y+textfield->h - sprite->h;
		Stage_redraw();
	}

}

static char * setTextLineText(TextField*textfield,TextLine*line)
{
	if(line){
		if(line->text){
			free(line->text);
			line->text = NULL;
		}
		if(line->numbyte > 0){
			line->text = malloc(line->numbyte+1);
			memset(line->text,0,line->numbyte+1);
			memcpy(line->text,textfield->text+line->indexInText,line->numbyte);
		}
		return line->text;
	}
	return NULL;
}

TextField * TextField_appendText(TextField*textfield,char*s)
{
	if(s == NULL || strlen(s)==0)
		return textfield;
	if(textfield == NULL)
		textfield = TextField_new();

	int dealedlen = 0;
	if(textfield->text) {
		dealedlen = strlen(textfield->text);
	}

	textfield->text = contact_str(textfield->text,s);

	TextLine * line = TextField_getLastLine(textfield);//最后一行
	while(dealedlen < strlen(textfield->text))
	{
		TextWord * textword = Textword_new(textfield->font,(Uint8*)(textfield->text+dealedlen));

		if(TextField_lineFull(textfield,line,textword)){//行满,另起一行
			line = TextField_appenNewLine(textfield);
			line->indexInText = dealedlen;
		}

		Array_push(line->words,textword);

		line->numbyte += textword->numbyte;
		line->rect.w += textword->w;// line w
		if(line->rect.h < textword->h){
			line->rect.h = textword->h;//line h
		}
		if(textfield->textWidth < line->rect.w){
			textfield->textWidth = line->rect.w;
		}
		if(textfield->textHeight < line->rect.y + line->rect.h)
			textfield->textHeight = line->rect.y + line->rect.h;//textHeight

		dealedlen += textword->numbyte;
		setTextLineText(textfield,line);
	}

	//SDL_Log("textfield->textWidth:%d textfield->textHeight:%d \n",textfield->textWidth,textfield->textHeight);fflush(stdout);
	drawLines(textfield);

	return textfield;
}

TextField * TextField_new()
{
	if(!TTF_WasInit())
	{
		if ( TTF_Init() < 0 ) {
			fprintf(stderr, "Couldn't initialize TTF: %s\n",SDL_GetError());
			SDL_Quit();
			return NULL;
		}
	}

	TextField* textfield = (TextField*)malloc(sizeof(TextField));
	memset(textfield,0,sizeof(TextField));
	textfield->w = 100;
	textfield->h = 100;
	textfield->sprite = Sprite_new();
	textfield->sprite->mouseChildren = SDL_FALSE;
	textfield->sprite->obj = textfield;

	if(textfield->textColor==NULL)
		textfield->textColor = uintColor(0x00ff0000);

	if(textfield->font==NULL){
		int stageSize = max(stage->stage_w,stage->stage_h);
		int textSize = 6 * stageSize/320;
		if(textSize<12)
			textSize = 12;
		textfield->font = getDefaultFont(textSize);
	}

	return textfield;
}



TextField * TextField_setText(TextField*textfield,char *s)
{
	if(textfield==NULL)
		textfield = TextField_new();

	textfield->textWidth = 0;
	textfield->textHeight = 0;
	textfield->length = 0;

	TextField_clearLines(textfield);

	if(textfield->text) {
		free(textfield->text);
	}
	textfield->text = NULL;

	if(textfield->sprite) {
		Sprite_destroySurface(textfield->sprite);
		Sprite_destroyTexture(textfield->sprite);
		textfield->sprite->canDrag = 1;
	}
	if(textfield->sprite==NULL){
		textfield->sprite = Sprite_new();
		textfield->w = 100;
		textfield->h = 100;
		textfield->sprite->mouseChildren = SDL_FALSE;
		textfield->sprite->obj = textfield;
	}
	textfield->sprite->y = textfield->y;


	Sprite_addEventListener(textfield->sprite,SDL_MOUSEWHEEL,mouseWheels);
	if(textfield->sprite->canDrag){
		Sprite_addEventListener(textfield->sprite,SDL_MOUSEMOTION,mouseWheels);
	}

	if(s){
		TextField_appendText(textfield,s);
	}
	return textfield;
}


#ifdef debugtext
#include "sdlstring.h"
int main(int argc, char *argv[])
{
	Stage_init(1);
	TextField* txt = TextField_new();//txt = TextField_setText(txt,getLinkedVersionString());
	//txt->x = stage->stage_w/4;
	//txt->y = stage->stage_h/4;
	//txt->font = getFontByPath("DroidSansFallback.ttf",24);
	txt->font = getDefaultFont(24);
	txt->w = stage->stage_w;
	txt->h = stage->stage_h;
	txt->sprite->canDrag = 1;


	//char *s =NULL;
	//txt = TextField_setText(txt,SDL_GetBasePath());
	txt = TextField_setText(txt,"hello");
	txt=TextField_appendText(txt,"\n");
	txt = TextField_appendText(txt,SDL_GetBasePath());
	txt=TextField_appendText(txt,"\n");
	txt = TextField_setText(txt,"hello");
	//SDL_Log("basepath:%s\n",s);
	//TextField_setText(txt,"");
	//TextField_appendText(txt,"\n");
	//TextField_appendText(txt,"\n");
	//TextField_appendText(txt,"1234567890abcdefghijklmnopqrst:" );
	//txt = TextField_appendText(txt,"1234567890\nabcdefghijklmnopqrstuvwxwz\nABCDEFGHIJKLMNOPQRSTUVWXYZ:" );
	//txt = TextField_appendText(txt,"1234567890:" );
	//txt = TextField_setText(txt,getLinkedVersionString());
	//txt = TextField_appendText(txt,"abcdefghijklmnopqrstuvwxyz:" );
	txt = TextField_appendText(txt,"ABCDEFGHIJKLMNOPQRSTUVWXYZ_:" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"01234567890abcdefghijklmnopqrstuvwxyz,< >中间:ABCDEFGHIJKLMNOPQRSTUVWXYZ==" );
	txt = TextField_appendText(txt,"一二三四五六七八九十一二三四五六七八九十end\n");
	//txt = TextField_setText(txt,"一二三四五六七八九十一二三四五六七八九十end\n");
	txt = TextField_appendText(txt,"一二三四五六七八九十一二三四五六七八九十end\n");
	Sprite_addChild(stage->sprite,txt->sprite); 
	Stage_loopEvents(); return 0;
	//TextField_appendText(txt,"\npref path:\n" );
	//TextField_appendText(txt,SDL_GetPrefPath("test", "subsystem2"));

}
#endif


