#ifndef update_h__
#define update_h__
#include "mystring.h"
#include "sprite.h"
#include "files.h"
#include "sdlfiles.h"
#include "httploader.h"

typedef struct UPDATE
{
	char *data;
	char *version;
	char *path;
	char *modify;
}UPDATE;

UPDATE cur;
UPDATE net;

void Update_clear(UPDATE*update);
UPDATE * Update_decode(UPDATE*update,char *data);
void Update_init();
#endif
