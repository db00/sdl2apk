/**
 * @file font.c
 gcc -g -Wall -I"../SDL2/include/"  -I"../SDL2_ttf/" myfont.c array.c utf8.c files.c myregex.c sdlstring.c textfield.c matrix.c sprite.c mystring.c  -lSDL2_ttf -lm -lSDL2 -D debugtext  && ./a.out
 *  
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2017-05-11
 */
#include "myfont.h"

#ifndef DEFAULT_TTF_FILE
#ifdef __ANDROID__
//#define	DEFAULT_TTF_FILE "/system/fonts/DroidSansFallback.ttf"
#define	DEFAULT_TTF_FILE "~/sound/DroidSansFallback.ttf"
#elif __MACOSX__
#define	DEFAULT_TTF_FILE "/System/Library/Fonts/STHeiti Light.ttc"
//#elif __IPHONEOS__
//#define	DEFAULT_TTF_FILE "/System/Library/Fonts/"
//#elif defined(linux)
//#define	DEFAULT_TTF_FILE "/usr/share/fonts/adobe-source-han-sans-cn/SourceHanSansCN-Normal.otf"
#else
#define	DEFAULT_TTF_FILE "~/sound/DroidSansFallback.ttf"
#endif
#endif

typedef struct TextFormat
{
	//TTF_Font * font;
	char * fontpath;
	char * fontfamilyname;
	char * fontfacestyle;

	/*
	   int align;//表示段落的对齐方式。
	   int blockIndent ;// 表示块缩进，以像素为单位。
	   int bold ;// 指定文本是否为粗体字。
	   int bullet;// 表示文本为带项目符号的列表的一部分。
	   SDL_Color *color;// 表示文本的颜色。
	   int indent; // 表示从左边距到段落中第一个字符的缩进。
	   int italic;// 表示使用此文本格式的文本是否为斜体。
	   int kerning;// 一个布尔值，表示是启用 (true) 还是禁用 (false) 字距调整。
	   int leading;// 一个整数，表示行与行之间的垂直间距（称为前导）量。
	   int leftMargin;// 段落的左边距，以像素为单位。
	   int letterSpacing;// 一个数字，表示在所有字符之间均匀分配的空间量。
	   int rightMargin;// 段落的右边距，以像素为单位。
	   int size;// 使用此文本格式的文本的大小（以像素为单位）。
	   int tabStops;// 将自定义 Tab 停靠位指定为一个非负整数的数组。
	   int target;// 表示显示超链接的目标窗口。
	   int underline;// 表示使用此文本格式的文本是带下划线 (true) 还是不带下划线 (false)。
	   char*url;//表示使用此文本格式的文本的目标 URL
	   */
}TextFormat;



TextFormat * Font_new();
void TextFormat_clear(TextFormat * font);

static Array * fontList = NULL;

TextFormat * Font_new()
{
	TextFormat * font = malloc(sizeof(TextFormat));
	memset(font,0,sizeof(TextFormat));
	return font;
}

void TextFormat_clear(TextFormat * font)
{
	if(font)
	{
		if(font->fontpath)
			free(font->fontpath);
		free(font);
	}
}

void Font_clearList()
{
	if(fontList)
	{
		int i = 0;
		while(i<fontList->length)
		{
			TextFormat * font = Array_getByIndex(fontList,i);
			TextFormat_clear(font);
			++i;
		}
		Array_clear(fontList);
		fontList = NULL;
	}
}

TextFormat * Font_push(char *path,int fontSize)
{
	//SDL_Log("%s",path);
	TTF_Font * ttf = TTF_OpenFont(path, fontSize);
	if(ttf){
		char *fontfamilyname = TTF_FontFaceFamilyName(ttf);
		char* fontfacestyle = TTF_FontFaceStyleName(ttf);
		TextFormat * font = Font_new();
		font->fontpath = path;
		font->fontfamilyname = fontfamilyname;
		font->fontfacestyle = fontfacestyle;
		fontList = Array_push(fontList,font);
		TTF_CloseFont(ttf);
		return font;
	}
	return NULL;
}

Array * Font_getlist()
{
	if(fontList)
		return fontList;
	if(!TTF_WasInit())
	{
		if ( TTF_Init() < 0 ) {
			fprintf(stderr, "Couldn't initialize TTF: %s\n",SDL_GetError());
			SDL_Quit();
			return NULL;
		}
	}
	char * file = decodePath(DEFAULT_TTF_FILE);
	Font_push(file,0);

	const char *ttfDir = "c:/WINDOWS/Fonts";
#if defined(__ANDROID__)
	//ttfDir ="/system/fonts";	
	ttfDir ="/system/fonts";	
#elif defined(linux)
	ttfDir ="/usr/share/fonts";	
#elif defined(__MACOSX__)
	ttfDir ="/Library/Fonts";	
#elif defined(__IPHONEOS__)
	return NULL;
	ttfDir ="/System/Library/Fonts/Cache";	
#endif
	int fontSize = 12;
	Array * suffixs = NULL;
	suffixs = Array_push(suffixs,".ttf");
	Array * fontFileList = listDir2(ttfDir,suffixs);
	if(fontFileList){
		int i = 0;
		while(i<fontFileList->length)
		{
			char * path = Array_getByIndex(fontFileList,i);
			//printf("===>%s,",path);
			if(path){
				Font_push(path,fontSize);
			}
			++i;
		}
		Array_clear(fontFileList);
	}
	Array_clear(suffixs);
	return fontList;
}

TTF_Font * getFontByPath(char * path,int fontSize)
{
	fontList = Font_getlist();
	if(fontList){
		int i=0;
		while(i<fontList->length)
		{
			TextFormat * font = Array_getByIndex(fontList,i);
			if(font && strcmp(font->fontpath,path)==0){
				SDL_Log("%s in fontList\n",font->fontpath);
				return TTF_OpenFont(font->fontpath, fontSize);
			}
			++i;
		}
	}
	{
		TextFormat * font = Font_push(path,fontSize);
		if(font){
			SDL_Log("%s push",font->fontpath);
			return TTF_OpenFont(font->fontpath, fontSize);
		}
	}
	//return NULL;
	return getDefaultFont(fontSize);
}

TTF_Font * getDefaultFont(int fontSize)
{
	TTF_Font * default_font = NULL;
	if(!TTF_WasInit())
	{
		if ( TTF_Init() < 0 ) {
			fprintf(stderr, "Couldn't initialize TTF: %s\n",SDL_GetError());
			SDL_Quit();
			return NULL;
		}
	}
	//if(!fileExists(DEFAULT_TTF_FILE))
	//default_font = getFontByContainString("历əʊs1",fontSize);
	default_font = getFontByContainString("史",fontSize);
	if(default_font==NULL)
	{
		fprintf(stderr, "Couldn't initialize TTF: %s\n",DEFAULT_TTF_FILE);
	}
	return default_font;
}


TTF_Font * getFontByName(const char * fontName,int fontSize)
{
	fontList = Font_getlist();
	if(fontList)
	{
		int i = 0;
		while(i<fontList->length)
		{
			TextFormat * font = Array_getByIndex(fontList,i);
			char *fontfamilyname = font->fontfamilyname;
			if(fontfamilyname && strncasecmp(fontfamilyname,fontName,strlen(fontName))==0)
			{
				SDL_Log("%s",font->fontpath);
				return TTF_OpenFont(font->fontpath, fontSize);
			}
			++i;
		}
	}
	return getDefaultFont(fontSize);
}

int isWordInFont(TTF_Font * font,char * utf8)
{
	Array * wordsArr =  UTF8_each(utf8);
	if(wordsArr==NULL)
		return 1;
	int i = 0;
	while(i<wordsArr->length)
	{
		char * s = Array_getByIndex(wordsArr,i);
		Uint16 w = utf8_to_u16(s);
		int index= 1;
		index = TTF_GlyphIsProvided(font,w);
		if(!index){
			//SDL_Log("There is no %s in the loaded font!\n",s);
			Array_clear(wordsArr);
			return 0;
		}
		++i;
	}

	Array_clear(wordsArr);
	return 1;
}

TTF_Font * getFontByContainString(char * s,int fontSize)
{
	fontList = Font_getlist();
	if(fontList){
		int i=0;
		while(i<fontList->length)
		{
			TextFormat * font = Array_getByIndex(fontList,i);
			TTF_Font * _font = TTF_OpenFont(font->fontpath,fontSize);
			if(font && _font && isWordInFont(_font,s)){
				SDL_Log("%s in fontList\n",font->fontpath);
				return _font;
			}
			TTF_CloseFont(_font);
			++i;
		}
	}
	return NULL;
}

