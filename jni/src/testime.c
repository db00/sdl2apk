/**
 *
 gcc -g -Wall -D test_ime -Wall -lpthread utf8.c mysurface.c pinyin.c input.c array.c ease.c tween.c kodi.c sqlite.c base64.c urlcode.c filetypes.c httpserver.c httploader.c readbaidu.c ipstring.c testime.c dict.c myregex.c files.c mystring.c sprite.c read_card.c matrix.c textfield.c -lcrypto -lssl -ldl -lm -I"../SDL2_mixer/" -I"../SDL2_image/" -I"../SDL2/include/" -I"../libxml/include/" -I"../SDL2_ttf/" -lsqlite3 -lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_image && ./a.out
 gcc -Wall -D test_ime -Wall -lpthread mysurface.c array.c ease.c tween.c kodi.c base64.c urlcode.c filetypes.c httpserver.c httploader.c readbaidu.c ipstring.c testime.c dict.c update.c myregex.c files.c mystring.c sprite.c matrix.c textfield.c regex.c -DSTDC_HEADERS -lpthread -lwsock32 lib/libeay32.dll.a lib/libssl32.dll.a -L"lib" -lcrypto -lssl -lgdi32 -lm -I"../SDL2_mixer/" -I"../SDL2_image/" -I"../SDL2/include/" -I"include/" -I"../SDL2_ttf/" -lSDL2_ttf -lSDL2_mixer -lSDL2_image -lmingw32 -lSDL2_test -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image && a
 gcc -Wall -D test_ime -Wall -lpthread ease.c tween.c kodi.c sqlite.c base64.c urlcode.c filetypes.c httpserver.c httploader.c readbaidu.c ipstring.c testime.c regex.c dict.c update.c myregex.c files.c mystring.c sprite.c matrix.c textfield.c -lcrypto -lssl -ldl -lm -I"../SDL2_mixer/" -I"../SDL2_image/" -I"../SDL2/include/" -I"../libxml/include/" -I"../SDL2_ttf/" -lsqlite3 -lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_image && ./a.out
 gcc testime.c -I"../SDL2/include/" -I"../SDL2_ttf/"  -lmingw32 -lSDL2_test -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image && a

http://g.hn165.com/hnck/Home/Index
http://ozzmaker.com/2014/06/30/virtual-keyboard-for-the-raspberry-pi/

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "SDL_image.h"
#include "dict.h"
#include "httploader.h"
#include "readbaidu.h"
#include "httpserver.h"
#include "kodi.h"
#include "input.h"
#include "read_card.h"

static enum STATS {
    DICT,
    KODI,
    CARD,
    END
} stats;

Input * input = NULL;
TextField * textfield = NULL;
Sprite * curlistSprite = NULL;
Sprite * dictContainer= NULL;
Dict * dict = NULL;

void Redraw(char *text) {
    if(text){
        textfield = TextField_setText(textfield,text);
        //TextField_setScrollV(textfield,TextField_getMaxScrollV(textfield));
    }
    UserEvent_new(SDL_USEREVENT,0,Stage_redraw,NULL);//Stage_redraw
}

void open_dict()
{
    if(dict==NULL)
    {
        dict = Dict_new();
        dict->name = "oxford-gb";
    }
}

char * showExplain(char *explain)
{
    if(explain){
        char *tmp = regex_replace_all(explain,"([^a-zA-Z])( [\\*0-9]+ )","$1\n$2");
        free(explain);
        explain = tmp;
        TextField_setText(textfield,explain);
    }
    if(curlistSprite)
    {
        curlistSprite->visible = SDL_FALSE;
        UserEvent_new(SDL_USEREVENT,0,Stage_redraw,NULL);//Stage_redraw
    }

    return explain;
}

static void * readWordUs(void * _key)
{
    Word * word = _key;
    READ_loadSound(word->word,2);
    return NULL;
}
static void * readWordEn(void * _key)
{
    Word * word = _key;
    READ_loadSound(word->word,1);
    return NULL;
}

int getMean(Word*word)
{
    printf("selected Word: %s\n",word->word);
    //return 0;
    if(word==NULL)
        return 0;
    open_dict();
    char * explain = NULL;
    explain = Dict_getMean(dict,word);
    showExplain(explain);
    if(word->word)
    {
        pthread_t thread;//创建不同的子线程以区别不同的客户端  
        if(pthread_create(&thread, NULL, readWordUs, word)!=0)//创建子线程  
        {  
            perror("pthread_create");  
        }
        pthread_detach(thread);
        //READ_loadSound(word->word,2);
    }
    return 0;
}

int searchWord(char* _word)
{
    if(_word && strlen(_word)){
        open_dict();
        char * explain = NULL;
        Word *word = Dict_getWord(dict,_word);
        if(word)
        {
            pthread_t thread;//创建不同的子线程以区别不同的客户端  
            if(pthread_create(&thread, NULL, readWordEn, word)!=0)//创建子线程  
            {  
                perror("pthread_create");  
            }
            pthread_detach(thread);
            //READ_loadSound(word->word,1);
        }
        explain = Dict_getMean(dict,word);
        showExplain(explain);
    }
    return 0;
}

int stageMouseY =0;
void selectedEvent(SpriteEvent*e)
{
    if(e==NULL)
        return;

    Sprite*sprite = e->target;
    SDL_Event* event = e->e;

    if(sprite==NULL || sprite->parent==NULL)
        return;

    Word * word = sprite->obj;
    //printf("---------------------%s\n",word->word);
    switch(e->type)
    {
        case SDL_MOUSEBUTTONDOWN:
            stageMouseY = event->button.y;
            sprite->parent->obj= NULL;
            return;
            break;
        case SDL_MOUSEBUTTONUP:
            if(word==NULL)
            {
                return;
            }
            if(sprite->parent->obj)
                return;
            if(abs(stageMouseY - event->button.y)>2)
                return;
            break;
    }

    //return;
    Sprite_removeEventListener(sprite,e->type,selectedEvent);
    //Sprite_removeEventListener(sprite,SDL_MOUSEBUTTONDOWN,selectedEvent);

    getMean(word);
}

void mouseMoves(SpriteEvent*e)
{
    Sprite*target = e->target;
    SDL_Event* event = e->e;

    if(event->motion.state){
        //if(abs(event->motion.xrel)<20 && abs(event->motion.yrel)<20)
        {
            target->x += event->motion.xrel;
            target->y += event->motion.yrel;
        }
        int stageSize = stage->stage_w>stage->stage_h?stage->stage_w:stage->stage_h;
        if(abs(stageMouseY-event->motion.y)>stageSize/320 || abs(event->motion.xrel)>stageSize/320){
            target->obj= e;
        }
        Sprite_limitPosion(target,target->dragRect);
        Redraw(NULL);
    }
}

Sprite * makeWordlist(char * curWord)
{
    if(curWord==NULL || strlen(curWord)==0)
    {
        return NULL;
    }
    if(curlistSprite){
        Sprite_removeChildren(curlistSprite);
        curlistSprite->x = 0;
        curlistSprite->y = input->sprite->h;
    }

    int fontSize = 20*stage->stage_h/320;
    open_dict();
    int numWords = 10;
    int _i=0;
    Array *wordlist = Dict_getWordList(dict,curWord,&numWords);
    while(_i<numWords)
    {
        Word*word = Array_getByIndex(wordlist,_i);
        if(word){
            printf("%s\n",word->word);
            Sprite * sprite = Sprite_newText(word->word,fontSize,0x0,0xffffffff);
            sprite->obj= word;
            //sprite->filter = 0;
            if(sprite->name)
                free(sprite->name);
            sprite->name = contact_str("",word->word);
            Sprite_addEventListener(sprite,SDL_MOUSEBUTTONDOWN,selectedEvent);
            Sprite_addEventListener(sprite,SDL_MOUSEBUTTONUP,selectedEvent);
            sprite->y = _i* fontSize*1.5;
            Sprite_addChild(curlistSprite,sprite);
        }
        ++_i;
    }
    //Array_clear(wordlist);
    return curlistSprite;
}

void changeStats()
{
    stats++;
    if(stats==END)
        stats=0;
    if(dictContainer)
        dictContainer->visible = 0;
    if(cardContainer)
    {
        cardContainer->visible = 0;
        //Sprite_removeChild(stage->sprite,cardContainer);
    }
    Kodi_initBtns(0);
    switch(stats)
    {
        case KODI:
            Kodi_initBtns(1);
            break;
        case DICT:
            dictContainer->visible = 1;
            Sprite_addChild(stage->sprite,dictContainer);
            stage->focus = input->sprite;
            break;
        case CARD:
            srand((unsigned)time(NULL));  
            int i=(int)(rand()%9);
            if(cardskey==i)
                cardskey = (i+1)%9;
            else
                cardskey = i;
            makeList(&cardskey);
            break;
        default:
            break;
    }
}


void keyupEvent(SpriteEvent* e){
    SDL_Event *event = e->e;
    const char * kname = SDL_GetKeyName(event->key.keysym.sym);
    if(!strcmp(kname,"Menu"))
    {
        changeStats();
        //dictContainer->visible = (0==Kodi_initBtns());
    }else{
        switch (event->key.keysym.sym)
        {
            case SDLK_MENU:
                changeStats();
                //dictContainer->visible = (0==Kodi_initBtns());
                break;
            case SDLK_RETURN:
                if(strlen(input->value)>0){
                    searchWord(input->value);
                    Input_setText(input,"");
                }else{
                    Input_setText(input,"输入单词，回车查询！");
                }
                break;
            default:
                break;
        }
    }
    Redraw(NULL);
}

void textChangFunc(Input * input){
    if(dictContainer->visible && strlen(input->value)>0)
    {
        SDL_Log("text input changed!");
        makeWordlist(input->value);
        curlistSprite->visible = 1;
        Redraw(NULL);
    }
}
void stopInput(SpriteEvent* e){
    stage->focus = NULL;
    SDL_StopTextInput();
}


void droppedFile(SpriteEvent*e){
    SDL_Event* event = (SDL_Event*)(e->e);
    //case (SDL_DROPFILE):
    {      // In case if dropped file
        char * dropped_file = event->drop.file;
        // Shows directory of dropped file
        SDL_ShowSimpleMessageBox(
                SDL_MESSAGEBOX_INFORMATION,
                "File dropped on window",
                dropped_file,
                NULL	
                );

        textfield = TextField_setText(textfield,dropped_file);
        textfield->sprite->canDrag = 1;
        SDL_free(dropped_file);    // Free dropped_file memory
    }
}
void *uiThread(void *ptr){
    dictContainer = Sprite_new();
    dictContainer->surface = Surface_new(1,1);
    char pixels[4] ={'\0','\0','\0','\xff'};
    memcpy(dictContainer->surface->pixels,(char*)pixels,sizeof(pixels));
    Sprite_addChild(stage->sprite,dictContainer);
    dictContainer->w = stage->stage_w;
    dictContainer->h = stage->stage_h;

    textfield = TextField_new();
    textfield->sprite->canDrag = 1;
    Sprite_addChild(dictContainer,textfield->sprite);
    Sprite_addEventListener(textfield->sprite,SDL_MOUSEBUTTONDOWN,stopInput); 



    input = Input_new(stage->stage_w,stage->stage_h/10);
    input->textChangFunc = textChangFunc;
    Sprite_addChild(dictContainer,input->sprite);
    stage->focus = input->sprite;



    textfield->w = stage->stage_w;
    //textfield->h = stage->stage_h - input->sprite->h;
    textfield->y = input->sprite->h;


    if(curlistSprite==NULL){
        int numWords = 10;
        int fontSize = 16;
        curlistSprite = Sprite_new();
        SDL_Rect * rect = malloc(sizeof(*rect));
        rect->x = curlistSprite->x;
        rect->y = input->sprite->y+input->textfield->h;
        rect->w = 0;
        rect->h = fontSize*1.5*numWords;
        curlistSprite->dragRect = rect;
        curlistSprite->surface = Surface_new(stage->stage_w,numWords*fontSize*1.5);
        Sprite_addChild(dictContainer,curlistSprite);
        Sprite_addEventListener(curlistSprite,SDL_MOUSEMOTION,mouseMoves);
    }

    SDL_StartTextInput();
    //pthread_exit(NULL);  
    return NULL;
}


static void *webThread(void *ptr){
    pthread_detach(pthread_self());
    SDL_Log("webThread-----\n");
    Server*server = Server_new("/",8809);
    Server_recv(server);
    Server_clear(server);
    SDL_Log("webThread exit-----\n");
    pthread_exit(NULL);  
    return NULL;
}

#ifdef test_ime


int main(int argc, char *argv[]) {
    Stage_init(1);
    if(stage==NULL)return 0;

    pthread_t thread1;
    if(pthread_create(&thread1, NULL, webThread, NULL)!=0)//创建子线程  
    {  
        perror("pthread_create");  
    }else{
        pthread_detach(thread1);// do not know why uncommit this line , will occur an ERROR !
        //pthread_join(thread1,NULL);
    }

    uiThread(NULL);

    Sprite_addEventListener(stage->sprite,SDL_KEYUP,keyupEvent); 
    Sprite_addEventListener(stage->sprite,SDL_DROPFILE,droppedFile);
    Stage_loopEvents();
    exit(0);
    return 0;
}


#endif
/* vi: set ts=4 sw=4 expandtab: */
