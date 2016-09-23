#ifndef pinyin_h
#define pinyin_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL.h"
#include "SDL_mixer.h"
#include "utf8.h"
#include "files.h"
#include "myregex.h"
#include "httploader.h"
#include "readbaidu.h"
#include "music.h"
#include "sprite.h"

int playHzPinyin(char * s);
char * readNum(int num);

#endif
