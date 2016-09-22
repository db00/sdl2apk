/**
 * @file myttf.c
 gcc -Wall -I"../SDL2/include/" -I"../SDL2_ttf/" -I"../SDL2_image" matrix.c -lm myttf.c tween.c ease.c array.c sprite.c mystring.c -lSDL2_ttf -lSDL2 -Ddebug && ./a.out
 gcc -Wall -I"../SDL2/include/" -I"../SDL2_image/" -I"../SDL2_ttf/"  myttf.c sprite.c matrix.c -lSDL2_ttf -lmingw32 -lSDL2_test -lSDL2main -lSDL2 -Ddebug && a
 gcc -Wall -I"../SDL2/include/" -I"../SDL2_image/" -I"../SDL2_ttf/"  myttf.c sprite.c -lSDL2_ttf -lm -lSDL2 -Ddebug && ./a.out
 *  
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2015-07-21
 */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <SDL.h>
#include <unistd.h>
#include <dirent.h> 
#include <sys/stat.h> 
#include "SDL_ttf.h"
#include "SDL_revision.h"
#include "myttf.h"
#include "sprite.h"


SDL_Color WHITE = { 0xff, 0xff, 0xff, 0 };
SDL_Color BLACK = { 0x0, 0x00, 0x00, 0 };
SDL_Color RED = { 0xff, 0x00, 0x00, 0 };
SDL_Color GREEN = { 0x0, 0xff, 0x00, 0 };
SDL_Color BLUE = { 0x0, 0x0, 0xff, 0 };


int listDir2(const char *path,char **fileList,int * numFile,char**suffixs) 
{ 
	DIR              *pDir ; 
	struct dirent    *ent  ; 
	struct stat sb; 

	pDir=opendir(path); 
	while((ent=readdir(pDir))!=NULL) 
	{ 
		int flen = strlen(path) + strlen(ent->d_name) + 2;
		char * curFile = (char*)malloc(flen);
		if(curFile == NULL)return -1;
		sprintf(curFile,"%s/%s",path,ent->d_name);
		//printf("%s\n",curFile);

		if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0) 
		{
			if(curFile)free(curFile);
			curFile = NULL;
			continue; 
		}

		int willPush=0;
		if(stat(curFile, &sb) >= 0 && S_ISDIR(sb.st_mode))//directory
		{ 
			//printf("dir :\n"); 
			listDir(curFile,fileList,numFile); 
		} else if(suffixs){
			int i = 0;
			char *fileSuffix = strrchr(curFile,'.');
			char *suffix = suffixs[i];
			while(suffix){
				if(strcasecmp(fileSuffix,suffix)==0) {//suffix matched
					willPush = 1;
					break;
				}
				suffix = suffixs[++i];
			}
		}else{
			willPush = 1;//add all file to list
		}
		if(willPush){
			fileList[*numFile] = curFile;
			*numFile += 1;
		}else{
			printf("%s suffix not match!\n",curFile);
			free(curFile);
			curFile = NULL;
		}
		//if(curFile)free(curFile); curFile = NULL;
	} 
	closedir(pDir);
	return 0;
} 

int listDir(const char *path,char **fileList,int * numFile) 
{ 
	return listDir2(path,fileList,numFile,NULL);
} 

SDL_Surface * render2surface(SDL_Renderer*renderer)
{
	SDL_Surface *surface=NULL;
	SDL_Rect viewport;

	if (!renderer) {
		return NULL;
	}

	SDL_RenderGetViewport(renderer, &viewport);
	surface = SDL_CreateRGBSurface(0, viewport.w, viewport.h, 24,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
			0x00FF0000, 0x0000FF00, 0x000000FF,
#else
			0x000000FF, 0x0000FF00, 0x00FF0000,
#endif
			0x00000000);
	if (!surface) {
		fprintf(stderr, "Couldn't create surface: %s\n", SDL_GetError());
		return NULL;
	}

	if (SDL_RenderReadPixels(renderer, NULL, surface->format->format,
				surface->pixels, surface->pitch) < 0) {
		fprintf(stderr, "Couldn't read screen: %s\n", SDL_GetError());
		SDL_free(surface);
		return NULL;
	}
	return surface;
}

SDL_Surface * texture2surface(SDL_Texture * texture)//uncompleted.............
{
	//SDL_Window*window;
	SDL_Surface *surface;
	SDL_Renderer *renderer = NULL;
	int w,h,access;
	Uint32 format;
	SDL_QueryTexture(texture, &format, &access, &w, &h);
	printf("%dx%d\n",w,h);
	//w = 240; h = 320;

	Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	surface = SDL_CreateRGBSurface(0, w, h, 32, rmask, gmask, bmask, amask);
	/*
	*/

	//window = SDL_CreateWindow("Chess Board", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 240, 320, SDL_WINDOW_SHOWN);
	//surface = SDL_GetWindowSurface(window);
	//printf("%dx%d\n",surface->w,surface->h);
	//SDL_SaveBMP(surface,"surface.bmp");
	//return NULL;
	renderer = SDL_CreateSoftwareRenderer(surface);
	//renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	//SDL_RenderPresent(renderer);
	//SDL_FreeSurface(surface);
	//SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
	//SDL_RenderClear(renderer);
	//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
	//SDL_CreateWindowAndRenderer( w, h, format, &window, &renderer);
	SDL_SetRenderDrawColor(renderer, 0xf, 0xf, 0xf, 0xf);
	SDL_RenderClear(renderer);
	//SDL_Rect rect; rect.x = 0; rect.y = 0; rect.w = w; rect.h = h;
	//SDL_UpdateWindowSurface(window);


	Uint8 alpha = 0x3f;
	Uint8 r= 0xff;
	Uint8 g= 0xff;
	Uint8 b= 0xff;
	//SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
	//SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;
	SDL_BlendMode blendMode = SDL_BLENDMODE_ADD;
	//SDL_BlendMode blendMode = SDL_BLENDMODE_NONE;
	//SDL_BlendMode blendMode = SDL_BLENDMODE_MOD;
	SDL_SetTextureAlphaMod(texture, alpha);
	SDL_SetTextureBlendMode(texture, blendMode);
	SDL_SetTextureColorMod(texture, r, g, b); 


	SDL_RendererInfo info;
	SDL_GetRendererInfo(renderer, &info);
	printf("name:%s\n",info.name);
	printf("flags:%d\n",info.flags);
	printf("num_texture_formats:%d\n",info.num_texture_formats);
	printf("max_texture_width:%d\n",info.max_texture_width);
	printf("max_texture_height:%d\n",info.max_texture_height);

	SDL_SetRenderTarget(renderer,texture);
	SDL_RenderCopy(renderer,texture,NULL,NULL);
	SDL_RenderPresent(renderer);
	//SDL_UpdateWindowSurface(window);
	return render2surface(renderer);
}



TTF_Font * getFontByName(const char * fontName,int fontSize)
{
	TTF_Font * font = NULL;
	const char *ttfDir = "c:/WINDOWS/Fonts";
#if defined(__ANDROID__)
	ttfDir ="/system/fonts";	
#elif defined(linux)
	ttfDir ="/usr/share/fonts";	
#endif
	char* fileList[1024];
	int numFile=0;
	listDir(ttfDir,fileList,&numFile);
	int i = 0;
	while(i<numFile)
	{
		char *fontFile = fileList[i];
		//printf("%s,%s\n",fontFile,fontName);
		if(fontFile && strlen(fontFile)>4 && strcasecmp(fontFile+strlen(fontFile)-4,".ttf")==0)
		{
			font = TTF_OpenFont(fontFile, fontSize);
			if(!font) {
				//printf("TTF_OpenFont: %s\n", TTF_GetError());
				++i;
				continue;
			}
			char *familyname = TTF_FontFaceFamilyName(font);
			if(familyname && strncasecmp(familyname,fontName,strlen(fontName))==0)
			{
				printf("The family name of the face in the font is: %s\n", familyname);
				freeArr(fileList,&numFile);
				return font;
			}
			TTF_CloseFont(font);
		}
		++i;
	}
	char*ttf_file = decodePath(DEFAULT_TTF_FILE);
	font = TTF_OpenFont(ttf_file, fontSize);
	free(ttf_file);
	if(!font) {
		printf("TTF_OpenFont:Error: %s\n", TTF_GetError());
		// handle error
	}
	freeArr(fileList,&numFile);
	return font;
}


int draw_line(SDL_Renderer*renderer,TxtLine * txtline,TextField*txt)
{
	//TTF_SetFontStyle(txtline->font, TTF_STYLE_NORMAL);
	SDL_Surface* text = NULL;
	char * txtStr = (char*)malloc(txtline->numByte+2);
	memset(txtStr,0,txtline->numByte+2);
	snprintf(txtStr,txtline->numByte+1,"%s",txtline->text);
	SDL_Log("\n%d::%s\n",txtline->lineIndex,txtStr);

	if(txt->backgroundColor)
		text = TTF_RenderUTF8_Shaded(txt->font, txtStr, *(txt->textColor),*(txt->backgroundColor));
	else
		text = TTF_RenderUTF8_Solid(txt->font, txtStr, *(txt->textColor));
	//text = TTF_RenderUNICODE_Shaded(txtline->font, txtline->text, *(txtline->textColor),*(txtline->backgroundColor));
	//text = TTF_RenderText_Shaded(txtline->font, txtline->text, *(txtline->textColor),*(txtline->backgroundColor));
	//text = TTF_RenderGlyph_Shaded(txtline->font, *txtline->text, *(txtline->textColor),*(txtline->backgroundColor));
	//TTF_CloseFont(txtline->font);
	//printf("draw_line:%s\n",txtline->text);
	if(text){
		SDL_Rect rect;
		rect.x = txtline->x + txt->x;
		rect.y = txtline->y + txt->y;
		rect.w = text->w;
		rect.h = text->h;

		Sprite*sprite = Sprite_new();
		sprite->surface = text;
		sprite->x= rect.x;
		sprite->y= rect.y;
		Sprite_addChild(stage->sprite,sprite);
		//if(strlen(txtStr)>1)SDL_SaveBMP(texture2surface(texture),"rendered.bmp");
		//if(strlen(txtStr)>1)SDL_SaveBMP(text,"rendered.bmp");
		if(
				txt->y + txt->h >= txtline->y + txtline->h/2
				&& txt->y <= txtline->y + txtline->h/2
		  )
		{
			if(renderer){
				SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, text);
				SDL_RenderCopy(renderer, texture, NULL, &rect);
			}
		}
		//if(strlen(txtStr)>1)SDL_SaveBMP(render2surface(renderer),"rendered.bmp");

		//SDL_SetRenderTarget(renderer,texture);
		if(renderer)SDL_FreeSurface(text);
	}
	if(txtStr){
		free(txtStr);
		txtStr = NULL;
	}
	return 0;
}

int freeText(TextField*txt)
{
	if(txt){
		if(txt->lines){
			int i=0;
			while(i<(txt->numLines)) {
				TxtLine *p = txt->lines[i];
				if(p){
					free(p);
					p = NULL;
					txt->lines[i]=NULL;
				}
				++i;
			}
			txt->numLines = 0;
			free(txt->lines);
			txt->lines = NULL;
		}
		if(txt->font)
		{
			TTF_CloseFont(txt->font);
			txt->font = NULL;
		}
	}
	return 0;
}

int txt2lines(TextField*txt)
{
	if(!TTF_WasInit())
	{
		if ( TTF_Init() < 0 ) {
			fprintf(stderr, "Couldn't initialize TTF: %s\n",SDL_GetError());
			SDL_Quit();
			return -1;
		}
	}


	if(txt->w<=0)txt->w = 100;
	if(txt->h<=0)txt->h = 100;
	if(txt->font==NULL)txt->font = getFontByName("microsoft yahei",16);
	//if(txt->font==NULL)txt->font = getFontByName("simhei",16);
	if(txt->textColor==NULL)txt->textColor = &RED;
	//if(txt->backgroundColor==NULL)txt->backgroundColor= &BLACK;

	int dealedlen = 0;
	int slen = strlen(txt->text);
	txt->numLines = 0;
	txt->textHeight= 0;
	//return 0;
	if(txt->font==NULL)return -2;
	int fontheight = TTF_FontHeight(txt->font);

	char *_text = (char*)malloc(slen+1);
	while(dealedlen<slen)
	{
		TxtLine * line = (TxtLine*)malloc(sizeof(TxtLine));
		memset(line,0,sizeof(TxtLine));
		line->lineIndex = txt->numLines;
		txt->lines[line->lineIndex] = line;

		line->numByte = 0;
		line->numChar = 0;
		line->x = 0;
		line->y = txt->textHeight;
		line->w = 0;
		line->h = 0;
		line->indexInText = dealedlen;
		line->text = txt->text+line->indexInText;

		while(line->w + fontheight/2 < txt->w && dealedlen < slen)
		{
			const char *curChar = (const char*)(txt->text + dealedlen);
			int numChar = 1;
			if(*curChar == 0xff || *curChar == 0xfe) {//非utf8字符

			}else if((*curChar & (63 << 2)) == (63<<2)) {//6位
				numChar = 6;
			}else if((*curChar & (31 << 3)) == (31<<3)) {//5位
				numChar = 5;
			}else if((*curChar & (15 << 4)) == (15<<4)) {//4位
				numChar = 4;
			}else if((*curChar & (7 << 5)) == (7<<5)) {//3位
				numChar = 3;
			}else if((*curChar & (3 << 6)) == (3<<6)) {//2位
				numChar = 2;
			}
			dealedlen += numChar;

			if(*curChar == '\r' || *curChar == '\n') {
				txt->length += 1;
				break;
			}

			line->numByte += numChar;
			(line->numChar) += 1;

			memset(_text,0,line->numByte+2);
			snprintf(_text,line->numByte+1,"%s",line->text);

			//TTF_SizeText(txt->font, _text, &(line->w), &(line->h));

			SDL_Surface*text = TTF_RenderUTF8_Shaded(txt->font, _text, BLACK,WHITE);
			line->w = text->w;
			line->h = text->h;
			if(line->w > txt->textWidth)
				txt->textWidth = line->w;
			txt->length += line->numChar;
		}
		//printf("line%d text:%s,(%d)\n",line->lineIndex,_text,line->indexInText);
		txt->numLines += 1;
		txt->textHeight += line->h;
	}
	if(_text){
		free(_text);
		_text = NULL;
	}
	return 0;
}

int draw_str(SDL_Renderer * renderer, TextField* txt)
{
	char* text = txt->text;
	if(text ==NULL && *text == '\0')return -1;
	int slen = strlen(text);

	txt->lines = (TxtLine**)malloc(slen*sizeof(TxtLine**));
	memset(txt->lines,0,slen*sizeof(TxtLine**));
	txt2lines(txt);
	//return 0;

	TxtLine* _line;
	printf("numLines:%d\n",txt->numLines);
	int line_no = 0;//从第line_no行开始
	while (line_no < txt->numLines) {
		_line = txt->lines[line_no];
		if(_line){
			draw_line(renderer,_line,txt);
		}else{
			printf("line is NULL !\n");
			//break;
		}
		++line_no;
	}
	if(renderer && txt->borderColor){
		SDL_Rect rect;
		rect.x = txt->x;
		rect.y = txt->y;
		rect.w = txt->w;
		rect.h = txt->h;
		SDL_SetRenderDrawColor(renderer, 
				txt->borderColor->r, 
				txt->borderColor->g, 
				txt->borderColor->b, 
				txt->borderColor->a
				);
		SDL_RenderDrawRect(renderer,&rect);
		SDL_RenderPresent(renderer);
	}
	return 0;
}

char*get_text()
{
	char*show_str=NULL;

#if SDL_VERSION_ATLEAST(2, 0, 0)
	show_str=append_str(show_str,
			"Compiled with SDL 2.0 or newer\n");
#else
	show_str=append_str(show_str,
			"Compiled with SDL older than 2.0\n");
#endif
	SDL_version compiled;
	SDL_VERSION(&compiled);
	show_str=append_str(show_str,
			"Compiled version: %d.%d.%d.%d (%s)\n",
			compiled.major,
			compiled.minor,
			compiled.patch,
			SDL_REVISION_NUMBER,
			SDL_REVISION);
	SDL_version linked;
	SDL_GetVersion(&linked);
	show_str=append_str(show_str,
			"Linked version: %d.%d.%d.%d (%s)\n",
			linked.major,
			linked.minor,
			linked.patch,
			SDL_GetRevisionNumber(),
			SDL_GetRevision());
	return show_str;
}

#ifdef debug
int main(int argc, char *argv[])
{
	Stage_init(1);

	//char*show_str="0123456789\nABCDEFG\nHIJKLMN\nOPQRST\nUVWXYZ\nabcdefg\nhijklmn\nopqrst\nuvwxyz\n0123456789\n\n-\n=ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789=ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789\n0123456789\nABCDEFG\nHIJKLMN\nOPQRST\nUVWXYZ\nabcdefg\nhijklmn\nopqrst\nuvwxyz\n0123456789";

	char * show_str = get_text();
	show_str = (char*)append_str(show_str, "base一二三四五六七八九十一二三四五六七八九十path: '%s'\n", SDL_GetBasePath());
	show_str = (char*)append_str(show_str, "pref path: '%s'", SDL_GetPrefPath("test", "subsystem2"));

	printf("%s\n",show_str);

	TextField txt;
	memset(&txt,0,sizeof(txt));
	txt.text = show_str;
	txt.x = 1;
	txt.y = 1;
	txt.w = 240-2;
	txt.h = 320-2;
	//txt.wordWrap = 1;
	//txt.borderColor = &WHITE;
	draw_str(stage->renderer,&txt);
	freeText(&txt);

	free(show_str);show_str = NULL;
	SDL_RenderPresent(stage->renderer);

	//SDL_SaveBMP(render2surface(renderer),"screencap.bmp");

	Stage_loopEvents();
	return 0;
}
#endif



