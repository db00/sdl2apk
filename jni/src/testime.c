/**
 *
 gcc -g -Wall -D test_ime -Wall -lpthread utf8.c mysurface.c doc.c ole.c bytearray.c array.c ease.c tween.c kodi.c sqlite.c base64.c urlcode.c filetypes.c httpserver.c httploader.c readbaidu.c ipstring.c testime.c dict.c update.c myregex.c files.c mystring.c sprite.c matrix.c textfield.c -lcrypto -lssl -ldl -lm -I"../SDL2_mixer/" -I"../SDL2_image/" -I"../SDL2/include/" -I"../libxml/include/" -I"../SDL2_ttf/" -lsqlite3 -lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_image && ./a.out
 gcc -Wall -D test_ime -Wall -lpthread mysurface.c array.c ease.c tween.c kodi.c base64.c urlcode.c filetypes.c httpserver.c httploader.c readbaidu.c ipstring.c testime.c dict.c update.c myregex.c files.c mystring.c sprite.c matrix.c textfield.c regex.c -DSTDC_HEADERS -lpthread -lwsock32 lib/libeay32.dll.a lib/libssl32.dll.a -L"lib" -lcrypto -lssl -lgdi32 -lm -I"../SDL2_mixer/" -I"../SDL2_image/" -I"../SDL2/include/" -I"include/" -I"../SDL2_ttf/" -lSDL2_ttf -lSDL2_mixer -lSDL2_image -lmingw32 -lSDL2_test -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image && a
 gcc -Wall -D test_ime -Wall -lpthread ease.c tween.c kodi.c sqlite.c base64.c urlcode.c filetypes.c httpserver.c httploader.c readbaidu.c ipstring.c testime.c regex.c dict.c update.c myregex.c files.c mystring.c sprite.c matrix.c textfield.c -lcrypto -lssl -ldl -lm -I"../SDL2_mixer/" -I"../SDL2_image/" -I"../SDL2/include/" -I"../libxml/include/" -I"../SDL2_ttf/" -lsqlite3 -lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_image && ./a.out
 gcc testime.c -I"../SDL2/include/" -I"../SDL2_ttf/"  -lmingw32 -lSDL2_test -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image && a

http://g.hn165.com/hnck/Home/Index
http://ozzmaker.com/2014/06/30/virtual-keyboard-for-the-raspberry-pi/

Copyright (C) 1997-2014 Sam Lantinga <slouken@libsdl.org>

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely.
*/
/* A simple program to test the Input Method support in the SDL library (2.0+) */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "textfield.h"
#include "sprite.h"
#include "mystring.h"
#include "dict.h"
#include "files.h"
#include "myregex.h"
#include "regex.h"
#include "httploader.h"
#include "update.h"
#include "readbaidu.h"
#include "httpserver.h"
#include "mysurface.h"
#include "kodi.h"

#include "doc.h"
#include "ole.h"
#include "utf8.h"


#define MAX_TEXT_LENGTH 40

pthread_mutex_t mutex_lock= PTHREAD_MUTEX_INITIALIZER;  
TextField * textfield;
static SDL_Rect markedRect;
static char text[MAX_TEXT_LENGTH];
Sprite * curlistSprite = NULL;
Dict * dict = NULL;


char *utf8_next(char *p)
{
    int len = UTF8_numByte(p);
    size_t i = 0;
    if (!len)
        return 0;

    for (; i < len; ++i)
    {
        ++p;
        if (!*p)
            return 0;
    }
    return p;
}

char *utf8_advance(char *p, size_t distance)
{
    size_t i = 0;
    for (; i < distance && p; ++i)
    {
        p = utf8_next(p);
    }
    return p;
}



void Redraw() {
    //SDL_SetTextInputRect(&markedRect);
    //SDL_StopTextInput();
    if(text!=NULL){
        //if(text[0]=='\0')text[0]=' ';
        //SDL_Log("text==========:%s\n",text);
        textfield = TextField_setText(textfield,text);
        Sprite_addChild(stage->sprite,textfield->sprite);
        TextField_setScrollV(textfield,TextField_getMaxScrollV(textfield));
    }
    //Stage_redraw();
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

TextField * showExplain(TextField*textfield,char *explain)
{
    if(explain){
        char *tmp = regex_replace_all(explain,"([^a-zA-Z])( [\\*0-9]+ )","$1\n$2");
        free(explain);
        explain = tmp;
        //SDL_Log("%s",explain);
        textfield = TextField_setText(textfield,explain);
        free(explain);
        Sprite_addChild(stage->sprite,textfield->sprite);
        //if(dict)Dict_free(dict); dict = NULL;

    }

    if(curlistSprite){
        Sprite_destroy(curlistSprite);
        curlistSprite = NULL;
        //Stage_redraw();
        UserEvent_new(SDL_USEREVENT,0,Stage_redraw,NULL);//Stage_redraw
    }

    return textfield;
}


TextField* getMean(TextField*textfield,Word*word)
{
    open_dict();
    char * explain = NULL;
    explain = Dict_getMean(dict,word);
    TextField * ret = showExplain(textfield,explain);
    if(word->word)
        READ_loadSound(word->word,2);
    return ret;
}

TextField * searchWord(TextField*textfield ,char* _word)
{
    TextField * ret = NULL;
    if(_word && strlen(_word)){
        open_dict();
        char * explain = NULL;
        Word *word = Dict_getWord(dict,_word);
        if(word)
            READ_loadSound(word->word,1);
        explain = Dict_getMean(dict,word);
        ret = showExplain(textfield,explain);
    }
    return ret;
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
    switch(e->type)
    {
        case SDL_MOUSEBUTTONDOWN:
            stageMouseY = event->button.y;
            sprite->parent->other = NULL;
            return;
            break;
        case SDL_MOUSEBUTTONUP:
            if(sprite->parent->other)
                return;
            break;
    }

    Sprite_removeEventListener(sprite,e->type,selectedEvent);
    Sprite_removeEventListener(sprite,SDL_MOUSEBUTTONDOWN,selectedEvent);

    memset(text,0,MAX_TEXT_LENGTH);
    getMean(textfield,sprite->obj);
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
            target->other = e;
        }
        Sprite_limitPosion(target,target->dragRect);
        //Stage_redraw();
        UserEvent_new(SDL_USEREVENT,0,Stage_redraw,NULL);//Stage_redraw
    }
}

Sprite * makeWordlist(char * curWord)
{
    if(curlistSprite){
        Sprite_destroy(curlistSprite);
        curlistSprite = NULL;
    }

    if(curWord && strlen(curWord))
    {
    }else{
        return NULL;
    }

    int fontSize = 20*stage->stage_h/320;
    open_dict();
    int numWords = 10;
    int _i=0;
    Word**wordlist = Dict_getWordList(dict,curWord,&numWords);
    while(_i<numWords)
    {
        Word*word = wordlist[_i];
        if(word){
            if(curlistSprite==NULL){
                curlistSprite = Sprite_new();
                curlistSprite->x = 0;
                curlistSprite->y = 12*1.5*stage->stage_h/320;
            }
            printf("%s\n",word->word);
            Sprite * sprite = Sprite_newText(word->word,fontSize,0x0,0xffffffff);
            Sprite_addChild(curlistSprite,sprite);
            sprite->obj = word;
            //sprite->filter = 0;
            free(sprite->name);
            sprite->name = malloc(strlen(word->word)+1);
            memset(sprite->name,0,strlen(word->word)+1);
            sprintf(sprite->name,"%s",word->word);
            Sprite_addEventListener(sprite,SDL_MOUSEBUTTONDOWN,selectedEvent);
            Sprite_addEventListener(sprite,SDL_MOUSEBUTTONUP,selectedEvent);
            sprite->y = _i* fontSize*1.5;
        }
        ++_i;
    }
    if(curlistSprite){
        SDL_Rect * rect = malloc(sizeof(*rect));
        rect->x = curlistSprite->x;
        rect->y = curlistSprite->y-fontSize*1.5*(_i-1);
        rect->w = 0;
        rect->h = fontSize*1.5*(_i-1);
        curlistSprite->dragRect = rect;
        curlistSprite->surface = Surface_new(stage->stage_w,_i*fontSize*1.5);
        Sprite_addEventListener(curlistSprite,SDL_MOUSEMOTION,mouseMoves);
    }
    free(wordlist);
    return curlistSprite;
}



void keydownEvent(SpriteEvent* e){
    SDL_Event *event = e->e;
    const char * kname = SDL_GetKeyName(event->key.keysym.sym);
    if(!strcmp(kname,"Menu"))
    {
        textfield->sprite->visible = (0==Kodi_initBtns());
        return;
    }
    if(strncmp(text,"输入单词，回车查询",strlen("输入单词"))==0)
        memset(text,0,sizeof(text));

    SDL_Log("keydownEvent: %08X,%08X,%08X",event->key.keysym.sym,event->key.keysym.scancode,SDLK_MENU);
    switch (event->key.keysym.sym)
    {
        case SDLK_MENU:
            textfield->sprite->visible = (0==Kodi_initBtns());
            return;
            break;
        case SDLK_RETURN:
            if(text[0])
                searchWord(textfield,text);

            memset(text,0,sizeof(text));
            return;
            break;
        case SDLK_BACKSPACE:
            {
                int textlen=SDL_strlen(text);

                do {
                    if (textlen==0)
                    {
                        break;
                    }
                    if ((text[textlen-1] & 0x80) == 0x00)
                    {
                        /* One byte */
                        text[textlen-1]=0x00;
                        break;
                    }
                    if ((text[textlen-1] & 0xC0) == 0x80)
                    {
                        /* Byte from the multibyte sequence */
                        text[textlen-1]=0x00;
                        textlen--;
                    }
                    if ((text[textlen-1] & 0xC0) == 0xC0)
                    {
                        /* First byte of multibyte sequence */
                        text[textlen-1]=0x00;
                        break;
                    }
                } while(1);

            }
            break;
        default:
            if(strlen(text)==0)
            {
                SDL_StartTextInput();
            }
    }
    if(strlen(text)==0)
        sprintf(text,"输入单词，回车查询!");

    Redraw();
    return;

}
void textinputEvent(SpriteEvent*e)
{
    SDL_Event * event = (SDL_Event *)(e->e);
    if (event->text.text[0] == '\0' || event->text.text[0] == '\n' || markedRect.w < 0) return;

    if(strncmp(text,"输入单词，回车查询",strlen("输入单词"))==0)
        memset(text,0,sizeof(text));

    SDL_Log("Keyboard: text input \"%s\"\n", event->text.text);

#ifdef __ANDROID__
    if(!strcmp(event->text.text,"="))
    {
        textfield->sprite->visible = (0==Kodi_initBtns());
        Redraw();
        return;
    }
#endif

    if (SDL_strlen(text) + SDL_strlen(event->text.text) < sizeof(text))
        SDL_strlcat(text, event->text.text, sizeof(text));

    SDL_Log("text inputed: %s\n", text);

    if(strlen(text)>0){
        curlistSprite = (Sprite*)makeWordlist(text);
        Sprite_addChild(stage->sprite,curlistSprite);
    }
    /* is committed */
    Redraw();
}

void texteditingEvent(SpriteEvent*e){
    SDL_Event* event = (SDL_Event*)(e->e);
    SDL_Log("text editing \"%s\", selected range (%d, %d)\n",
            event->edit.text, event->edit.start, event->edit.length);

    Redraw();
}

void fingerEvent(SpriteEvent*e){
    if(stage->sprite->mouse->y < 20)
    {
        SDL_StartTextInput();
    }else{
        SDL_StopTextInput();
    }
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

        /*
        FILE * _file = fopen(dropped_file,"rb");
        int fileLen = fseek(_file,0,SEEK_END);
        fileLen = ftell(_file);
        rewind(_file);
        //printf("%s,%d\n",dropped_file,fileLen);

        ByteArray * bytearray = ByteArray_new(fileLen);
        fread(bytearray->data,1,fileLen,_file);
        fclose(_file);

        DocFile * file = DocFile_parse(bytearray);
        if(file && file->text)
        {
            printf("=============--------------------============%s\n",file->text->data);
            file->text->position = 0;
            //ByteArray_prints(file->text,file->text->length);
            textfield = TextField_setText(textfield,file->text->data);
        }
        //printf("\n%x",(unsigned int)file);
        DocFile_free(file);
        ByteArray_free(bytearray);
        */


        textfield = TextField_setText(textfield,dropped_file);
        SDL_free(dropped_file);    // Free dropped_file memory
        //break;
    }
}

void *uiThread(void *ptr){
    pthread_mutex_lock(&mutex_lock);  
    //memset(text,0,sizeof(text));
    textfield = TextField_new();
    textfield->w = stage->stage_w;
    textfield->h = stage->stage_h;
    textfield->sprite->canDrag = SDL_TRUE;
    sprintf(text,"输入单词，回车查询");
    textfield = TextField_setText(textfield,text);
    Sprite_addChild(stage->sprite,textfield->sprite);

    SDL_StartTextInput();
    Sprite_addEventListener(stage->sprite,SDL_MOUSEBUTTONDOWN,fingerEvent); 
    //Sprite_addEventListener(stage->sprite,SDL_FINGERDOWN,fingerEvent); 
    Sprite_addEventListener(stage->sprite,SDL_KEYDOWN,keydownEvent); 
    Sprite_addEventListener(stage->sprite,SDL_TEXTINPUT,textinputEvent); 
    Sprite_addEventListener(stage->sprite,SDL_TEXTEDITING,texteditingEvent); 
    //searchWord(textfield,"hello");
    pthread_mutex_unlock(&mutex_lock);  
    //pthread_exit(NULL);  
    Redraw();
    SDL_Delay(100);
    return NULL;
}


static void *webThread(void *ptr){
    pthread_detach(pthread_self());
    //pthread_mutex_lock(&mutex_lock);  
    SDL_Log("webThread-----\n");
    Server*server = Server_new("/",8809);
    Server_recv(server);
    Server_clear(server);
    SDL_Log("webThread exit-----\n");
    //pthread_mutex_unlock(&mutex_lock);  
    pthread_exit(NULL);  
    return NULL;
}

static int TestThread(void *ptr)
{
    Sprite *bg = NULL;
    int cnt=0;
    bg = Sprite_new();
    bg->surface = Httploader_loadimg("https://git.oschina.net/db0/SDL2apk/raw/master/res/drawable-xxhdpi/ic_launcher.png");
    if(bg->surface){
        SDL_Log("load background png ok!");
        Sprite_center(bg,0,0,stage->stage_w,stage->stage_h);
        Sprite_addChildAt(stage->sprite,bg,0);
        UserEvent_new(SDL_USEREVENT,0,Stage_redraw,NULL);//Stage_redraw
        //Sprite_addChild(stage->sprite,bg);
    }else{
        SDL_Log("load background png ERROR!");
        if(bg){
            Sprite_destroy(bg);
            bg = NULL;
        }
    }
    SDL_Delay(100);

    //Update_init();
    return cnt;
}

/***
 *
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

int testxml()
{
//定义文档和节点指针
xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"plugins");

//设置根节点
xmlDocSetRootElement(doc,root_node);

//在根节点中直接创建节点
//xmlNewTextChild(root_node, NULL, BAD_CAST "plugin", BAD_CAST "newNode1 content");

//创建一个节点，设置其内容和属性，然后加入根结点
//xmlNodePtr node;
//node = xmlNewNode(NULL,BAD_CAST"plugin");
//xmlNodePtr content = xmlNewText(BAD_CAST"NODE CONTENT");
//xmlAddChild(root_node,node);
//xmlAddChild(node,content);
//xmlNewProp(node,BAD_CAST"name",BAD_CAST "adder");

//创建一个儿子和孙子节点
xmlNodePtr node;
node = xmlNewNode(NULL, BAD_CAST "plugin");
xmlAddChild(root_node,node);
xmlNewProp(node,BAD_CAST"name",BAD_CAST "adder");

xmlNodePtr lib = xmlNewNode(NULL, BAD_CAST "library");
xmlAddChild(node, lib);
xmlNewProp(lib,BAD_CAST"path",BAD_CAST "/home/libiao/adder.so");

xmlNodePtr entry = xmlNewNode(NULL, BAD_CAST "entry");
xmlAddChild(node, entry);
xmlNewProp(entry,BAD_CAST"name",BAD_CAST "add");

//xmlAddChild(lib, xmlNewText(BAD_CAST "This is a grandson node"));

//存储xml文档
#ifdef __ANDROID__
int nRel = xmlSaveFile("/sdcard/CreatedXml.xml",doc);
#else
int nRel = xmlSaveFile("CreatedXml.xml",doc);
#endif
if (nRel != -1)
{
//cout<<"一个xml文档被创建,写入"<<nRel<<"个字节"<<endl;
SDL_Log("一个xml文档被创建,写入 %d 个字节\n", nRel);
}else{
SDL_Log("xml write ERROR!\n");
}
fflush(stdout);

//释放文档内节点动态申请的内存
xmlFreeDoc(doc);

return 1;
}
 **/
#ifdef test_ime


int main(int argc, char *argv[]) {
    //testxml();
    Stage_init(1);
    if(stage==NULL)return 0;

    pthread_mutexattr_t attr;
    int ret;  
    if(( ret = pthread_mutexattr_init(&attr)) != 0){  
        fprintf(stderr, "create mutex attribute error. msg:%s", strerror(ret));  
        exit(1);  
    }  
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);  
    //pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);  
    pthread_mutex_init(&mutex_lock, &attr); 


    //char * hello_str = "_______hello_world_______\n";
    //writefile("test",hello_str,strlen(hello_str));
    //SDL_Log("readfile:%s",(char*)readfile("test",NULL));fflush(stdout);
    //SDL_Log("pwd:%s",(char*)mysystem("pwd",NULL));fflush(stdout);


    /***
      char atext[1024];
      memset(atext,0,1024);
      sprintf(atext,"p:%s,b:%s",(char*)(prefpath()),(char*)(basepath()));
      Sprite_alertText(atext);
      */

    pthread_t thread1;
    if(pthread_create(&thread1, NULL, webThread, NULL)!=0)//创建子线程  
    {  
        perror("pthread_create");  
    }else{
        pthread_detach(thread1);// do not know why uncommit this line , will occur an ERROR !
        //pthread_join(thread1,NULL);
    }

#ifndef __WIN32__
    SDL_Thread *thread = SDL_CreateThread(TestThread, NULL, NULL);
    if (NULL == thread) {
        printf("\nSDL_CreateThread failed: %s\n", SDL_GetError());
    } else {
        int threadReturnValue;
        SDL_WaitThread(thread, &threadReturnValue);
        SDL_Log("\nThread returned value: %d", threadReturnValue);
        //SDL_DetachThread(thread);
    }

#endif

#if 1
    uiThread(NULL);
#else
    while(textfield==NULL)
    {
        pthread_t uithread;
        if(pthread_create(&uithread, NULL, uiThread, NULL)!=0)//创建子线程  
        {  
            perror("pthread_create");  
        }else{
            //pthread_detach(uithread);
            pthread_join(uithread,NULL);
        }
    }
#endif
    //pthread_mutex_destroy(&mutex_lock);  

    //Kodi_initBtns();
    //printfStatus();
#ifdef __ANDROID__
    //startGps();
    //Sprite_alertText("ok");
#endif
    Sprite_addEventListener(stage->sprite,SDL_DROPFILE,droppedFile);
    Stage_loopEvents();
    exit(0);
    return 0;
}


#endif
/* vi: set ts=4 sw=4 expandtab: */
