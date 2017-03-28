#ifndef datas_h
#define datas_h

#include "sqlite.h"
#include "files.h"
#include "cJSON.h"


/**
 * oxford word max length: 21
 * langdao word max length: 48 35
 *
 * word max explain length go:23445
 *

 CREATE TABLE list(
 wordid INTEGER primary key asc,
 word varchar(50),
 date real,//加入时间
 remembered char,//0:new word,1:remembered
 numAccess INTEGER,//访问次数
 numTest INTEGER;//测试次数
 );

 CREATE TABLE history(
 id INTEGER primary key asc,
 wordid INTEGER;
 status varchar(10),//-:no data,0:test fail,1:test pass,
 date real,//访问时间
 );
 *
 */
#include <time.h>
DataBase *history_db;
int init_db();
int get_word_id(char * word);
int add_to_history(int wordid);
int add_to_test(int wordid,int result);
int add_new_word(char * word);

#endif
