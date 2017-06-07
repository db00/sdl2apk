
#include "explain.h"
#include "readbaidu.h"

static Sprite * explainContainer = NULL;
static TextField * textfield = NULL;
static Word * curWord = NULL;

static void show_copied(char * word,int r)
{
	if(r==0)
	{
		//printf("\n%d,",r);fflush(stdout);
		int success = SDL_ShowSimpleMessageBox(
				SDL_MESSAGEBOX_ERROR,
				word,
				//"copied",
				"已复制",
				NULL);
		if (success == -1) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error Presenting MessageBox: %s\n", SDL_GetError());
		}
	}
}

void stopInput(SpriteEvent* e){
	stage->focus = NULL;
	SDL_StopTextInput();
}

static void read_out(SpriteEvent*e)
{
	if(Sprite_getVisible(explainContainer)==0)
		return;
	Sprite*target = e->target;
	if(target==NULL || target->obj==NULL)
		return;
	SDL_Event* event = e->e;

	if(curWord==NULL)
		return;
	char * word = curWord->word;
	if(word==NULL || strlen(word)<=0)
		return;

	//printf("read %s\n",word);
	if(strcmp(target->obj,"发音")==0){
		if(isCE(word)){

		}else{
			Sound_playEng(word,1);
		}
	}else if(strcmp(target->obj,"美音")==0){
		if(isCE(word)){

		}else{
			Sound_playEng(word,2);
		}
	}else if(strcmp(target->obj,"搜图片")==0){
		if(isPictureMode){
			removePictures();
		}else{
			search_pic(explainContainer,word);
			Sprite_addChild(explainContainer,target);
		}
	}else if(strcmp(target->obj,"复制单词")==0){
		int r= setClipboardText(word);
		show_copied(word,r);
	}else if(strcmp(target->obj,"复制解释")==0){
		int r= setClipboardText(textfield->text);
		show_copied(textfield->text,r);
	}
	//Stage_redraw();
}

static Sprite * makeTopBtn(Sprite * contener,char * name,int gap, void (*func)(SpriteEvent*))
{
	Sprite * btn = Sprite_newText(name,30,0x0,0xffffffff);
	if(contener->children && contener->children->length>0)
	{
		Sprite * lastSprite = Sprite_getChildByIndex(contener,contener->children->length-1);
		btn->x = lastSprite->x + lastSprite->w + gap;
	}
	Sprite_addEventListener(btn,SDL_MOUSEBUTTONUP,func);
	//printf("btn:(%s)\n",btn->obj);
	Sprite_addChild(contener,btn);
	return btn;
}

Sprite * Explain_show(Sprite * parent,int _y,Dict * dict,Word * word)
{
	if(explainContainer==NULL)
	{
		explainContainer = Sprite_new();
		Sprite_addChildAt(parent,explainContainer,0);
		explainContainer->y = _y;//input->sprite->h;

		Sprite * enBtn;
		int gap=2;
		enBtn = makeTopBtn(explainContainer,"发音",gap,read_out);
		//enBtn = makeTopBtn(explainContainer,"美音",gap,read_out);
		enBtn = makeTopBtn(explainContainer,"搜图片",gap,read_out);
		enBtn = makeTopBtn(explainContainer,"复制单词",gap,read_out);
		enBtn = makeTopBtn(explainContainer,"复制解释",gap,read_out);

		textfield = TextField_new();
		textfield->sprite->canDrag = 1;
		textfield->w = stage->stage_w;
		Sprite_addEventListener(textfield->sprite,SDL_MOUSEBUTTONDOWN,stopInput); 
		Sprite_addChildAt(explainContainer,textfield->sprite,0);
		textfield->y = enBtn->h;
		textfield->h = stage->stage_h - explainContainer->y - textfield->y;
		explainContainer->visible = SDL_FALSE;
	}
	if(dict && word){
		char * explain = NULL;
		explain = Dict_getMean(dict,word);
		curWord = word;

		//char *tmp = regex_replace_all(explain,"([^a-zA-Z,;])( [\\*0-9]+ )","$1\n$2");
		//free(explain);
		//explain = regex_replace_all(tmp,"([:?!\\.])( )","$1\n$2");
		//free(tmp);
		//char *tmp = regex_replace_all(explain,"([^a-zA-Z0-9,;'>\\(\\)\\/ ~]|[:?!\\.])([ \\*0-9]{,2} )","$1\n$2");
		char *tmp = regex_replace_all(explain,"([^\r\n][:?!\\.\\*]) ","$1\n");
		free(explain);
		explain = regex_replace_all(tmp,"([^a-zA-Z,;\r\n])( [\\*0-9]{1,2} )","$1\n$2");
		//explain = tmp;
		TextField_setText(textfield,explain);
		explainContainer->visible = SDL_TRUE;
		free(explain);
	}
	return explainContainer;
}
void Explain_hide()
{
	if(explainContainer)
		explainContainer->visible = SDL_FALSE;
	curWord = NULL;
}
