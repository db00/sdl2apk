/**
 *
 gcc -g -Wall -D test_ime -Wall -lpthread music.c utf8.c searhdict.c mysurface.c pinyin.c input.c array.c ease.c tween.c kodi.c sqlite.c base64.c urlcode.c filetypes.c httpserver.c httploader.c readbaidu.c ipstring.c testime.c dict.c myregex.c files.c mystring.c sprite.c read_card.c matrix.c textfield.c -lcrypto -lssl -ldl -lm -I"../SDL2_mixer/" -I"../SDL2_image/" -I"../SDL2/include/" -I"../libxml/include/" -I"../SDL2_ttf/" -lsqlite3 -lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_image && ./a.out
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
#include "searhdict.h"
#include "input.h"
#include "read_card.h"

static enum STATS {
    DICT,
    KODI,
    CARD,
    END
} stats;

void showCardTest(int b)
{
    if(cardContainer)
        cardContainer->visible = b;
    if(b){
        makeNewAsk(-1,-1);
    }
}

void changeStats()
{
    stats++;
    if(stats==END)
        stats=0;

    showSearchDict(0);
    Kodi_initBtns(0);
    showCardTest(0);

    switch(stats)
    {
        case KODI:
            Kodi_initBtns(1);
            SDL_SetWindowTitle(stage->window, "kodi");
            break;
        case DICT:
            showSearchDict(1);
            SDL_SetWindowTitle(stage->window, "dict");
            break;
        case CARD:
            showCardTest(1);
            SDL_SetWindowTitle(stage->window, "card");
            break;
        default:
            break;
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

        //textfield = TextField_setText(textfield,dropped_file);
        //textfield->sprite->canDrag = 1;
        SDL_free(dropped_file);    // Free dropped_file memory
    }
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


static void keyupEvent(SpriteEvent* e){
    SDL_Event *event = e->e;
    const char * kname = SDL_GetKeyName(event->key.keysym.sym);
    if(!strcmp(kname,"Menu"))
    {
        changeStats();
    }else{
        switch (event->key.keysym.sym)
        {
            case SDLK_MENU:
                changeStats();
                break;
            default:
                break;
        }
    }
    //Redraw(NULL);
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

    SDL_SetWindowTitle(stage->window, "card");
    showCardTest(1);
    //showSearchDict(1);

    Sprite_addEventListener(stage->sprite,SDL_KEYUP,keyupEvent); 
    Sprite_addEventListener(stage->sprite,SDL_DROPFILE,droppedFile);
    Stage_loopEvents();
    exit(0);
    return 0;
}


#endif
/* vi: set ts=4 sw=4 expandtab: */
