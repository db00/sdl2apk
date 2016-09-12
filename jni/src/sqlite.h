#ifndef sqlite_h
#define sqlite_h
#include "sqlite3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct DataBase
{
	sqlite3 *db;
	char * name;
	char * result_str;
	int result_space;
}DataBase;

void DataBase_clear(DataBase*database);
DataBase * DataBase_new(char*name);
int DataBase_exec(DataBase*db,const char * sql);
DataBase *exec_sql(DataBase*db,char*Path);


#endif
