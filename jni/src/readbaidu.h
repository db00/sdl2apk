#ifndef readbaidu_h__
#define readbaidu_h__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef unix
#include <unistd.h>
#endif

#include "SDL.h"
#include "SDL_mixer.h"
#include "urlcode.h"
#include "httploader.h"
#include "files.h"

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif
int Sound_init();
void READ_loadSound(char *word,int type);
int Sound_playFile(char * fileName);
#endif
