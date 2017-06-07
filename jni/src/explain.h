#ifndef explain__h
#define explain__h

#include "pictures.h"
#include "dict.h"
#include "searhdict.h"
#include "sdlstring.h"

void Explain_hide();
Sprite * Explain_show(Sprite * parent,int y,Dict * dict,Word * word);

#endif
