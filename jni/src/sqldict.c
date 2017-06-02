/**
 * @file sqldict.c
 gcc -ldl -lpthread -lsqlite3 -lSDL2 date.c array.c myregex.c mystring.c files.c -I"../SDL2/include/" sqldict.c sqlite.c dict.c  && ./a.out
 gcc regex.c array.c sqlite3.c myregex.c mystring.c files.c -I"../SDL2/include/" sqldict.c sqlite.c dict.c  && a
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2017-05-03
 */

#include "sqlite.h"
#include "date.h"
#include "dict.h"

int inserts(sqlite3*  conn,sqlite3_stmt * stmt3,char * word,char * explain)
{
	//在绑定时，最左面的变量索引值是1。
	//sqlite3_bind_int(stmt3,1,i);
	//sqlite3_bind_double(stmt3,2,i * 1.0);
	sqlite3_bind_text(stmt3,1,explain,strlen(explain),SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt3,2,word,strlen(word),SQLITE_TRANSIENT);
	if (sqlite3_step(stmt3) != SQLITE_DONE) {
		sqlite3_finalize(stmt3);
		sqlite3_close(conn);
		return 1;
	}
	//重新初始化该sqlite3_stmt对象绑定的变量。
	sqlite3_reset(stmt3);
	//printf("Insert Succeed.\n");
	return 0;

}


char * ifo(char * name,char * version,int wordcount,int idxfilesize,char * bookname)
{
	int len = 1024;
	char * str = malloc(len);
	Date * date = Date_new(NULL);
	char s[] = "%s\r\nversion=%s\r\nwordcount=%d\r\nidxfilesize=%d\r\nbookname=%s\r\nauthor=xx\r\ndescription=db0@qq.com modified\r\ndate=%d.%d.%d\r\nsametypesequence=m";
	memset(str,0,len);
	sprintf(str,s,name,version,wordcount,idxfilesize,bookname,date->tm_year+1900,date->tm_mon,date->tm_mday);
	return str;
}
char * idx_item(char * p,char * word,int offset,int len)
{
	int l = strlen(word);
	sprintf(p,"%s",word);
	*(p+l)='\0';
	*(p+l+1)=*((char*)&offset+3);
	*(p+l+2)=*((char*)&offset+2);
	*(p+l+3)=*((char*)&offset+1);
	*(p+l+4)=*((char*)&offset+0);
	*(p+l+5)=*((char*)&len+3);
	*(p+l+6)=*((char*)&len+2);
	*(p+l+7)=*((char*)&len+1);
	*(p+l+8)=*((char*)&len+0);
	return p+l+9;
}
char * word_item(char*idx_p,char * dict_p,int * dictlen,char * word,char * explain)
{
	int len = strlen(explain);
	idx_p = idx_item(idx_p,word,strlen(dict_p),strlen(explain));
	sprintf(dict_p+(*dictlen),"%s",explain);
	*dictlen += len;
	return idx_p;
}

int make_dict(char * db_path,char * dict_name)
{
	DataBase *db = DataBase_new(db_path);
	if(db==NULL){
		printf("can not open DataBase %s ! \r\n",db_path);
		return 5;
	}

	int i = 0;
	char * dict = malloc(0x8000000);
	memset(dict,0,0x8000000);//2^27=64M
	char * idx = malloc(0x4000000);
	memset(idx,0,0x2000000);//2^25=16M
	char * idx_p = idx;
	int dictlen=0;



	Array * wordsArr = NULL;
	Array * explainsArr = NULL;
	char sql[128];
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select * from dict ORDER BY word COLLATE NOCASE limit 0,-1;");
	int rc = DataBase_exec2array(db,sql);
	if(rc){
		//printf("\nsql_result_str:%s\r\n",db->result_str);
		return 6;
	}else if(db->result_arr){
		Array * data = db->result_arr;
		Array * names = Array_getByIndex(data,0);
		if(names==NULL){
			printf("no names Array");
			return 4;
		}
		int nCount = names->length;
		int i = 0;
		while(i<nCount)
		{
			char * curName =Array_getByIndex(names,i);
			if(strcmp(curName,"word")==0)
			{
				wordsArr = Array_getByIndex(data,i+1);
				if(wordsArr == NULL || wordsArr->length==0){
					printf("%d:no word\r\n",i);
					return 1;
					break;
				}
			}else if(strcmp(curName,"explain")==0){
				explainsArr = Array_getByIndex(data,i+1);
				if(explainsArr == NULL || explainsArr->length==0){
					printf("%d:no explain\r\n",i);
					return 2;
					break;
				}
			}
			++i;
		}
	}

	if(wordsArr && explainsArr && wordsArr->length>0 && wordsArr->length == explainsArr->length)
	{
		int i = 0;
		while(i<wordsArr->length){
			char * word = NULL;
			char * explain = NULL;
			word = Array_getByIndex(wordsArr,i);
			explain = Array_getByIndex(explainsArr,i);
			if(word == NULL || explain == NULL)
				break;
			if(i%1000==0)
			{
				printf("\r\n %d,%s,",i,word);
			}
			idx_p = word_item(idx_p,dict,&dictlen,word,explain);
			++i;
		}
	}else{
		return 7;
	}

	printf("\r\n total word: %d,",wordsArr->length);

	int idxfilesize = idx_p - idx;

	char * ifo_s = ifo(dict_name,"2.4.5",wordsArr->length,idxfilesize,dict_name);
	char filename[128];
	memset(filename,0,sizeof(filename));

	char * dict_dir = decodePath(contact_str("~/sound/",dict_name));
	sprintf(filename,"%s/%s.ifo",dict_dir,dict_name);
	writefile(filename,ifo_s,strlen(ifo_s));

	memset(filename,0,sizeof(filename));
	sprintf(filename,"%s/%s.idx",dict_dir,dict_name);
	writefile(filename,idx,idxfilesize);

	memset(filename,0,sizeof(filename));
	sprintf(filename,"%s/%s.dict",dict_dir,dict_name);
	writefile(filename,dict,dictlen);

	//DataBase_result_print(db);
	DataBase_clear(db);
	db = NULL;

	free(ifo_s);
	free(idx);
	free(dict);

	//int writefile(char * path,char *data,size_t data_length)

	return 0;
}

int update_one_word(DataBase * db,char * word,char * explain)
{
	if(word==NULL || explain==NULL || strlen(word)==0 || strlen(explain)==0)
		return 1;
	int rc=0;
	if(db==NULL){
		rc = DataBase_exec(db,"create table if not exists dict(id INTEGER primary key asc,word text UNIQUE,explain text);");
		if(!rc){
			//printf("\nsql_result_str:%s",db->result_str);
		}else{
			return 1;
		}
	}

	//const char* insertSQL = "replace into dict(explain,word) values(?,?);";
	//const char* insertSQL = "insert into dict(explain,word) values(?,?);";
	const char* insertSQL = "replace into dict(explain,word) values(?,?);";
	//const char* insertSQL = "update dict set explain=? where word=?;";
	sqlite3_stmt* stmt3 = NULL;
	sqlite3 * conn = db->db;
	if (sqlite3_prepare_v2(conn,insertSQL,strlen(insertSQL),&stmt3,NULL) != SQLITE_OK) {
		if (stmt3)
			sqlite3_finalize(stmt3);
		sqlite3_close(conn);
		printf("\n ERROR 0:%s,%s\r\n",explain,word);
		return 1;
	}
	rc = inserts(db->db,stmt3,word,explain);
	if(!rc){
		//printf("\r\n %s",word);fflush(stdout);
		//printf("\nsql_result_str:%s",db->result_str);
	}else{
		printf("\n ERROR:%s,%s\r\n",explain,word);
		return 1;
	}
	sqlite3_finalize(stmt3);
	//DataBase_clear(db); db = NULL;
	return 0;
}
int update_word(char * db_path,char * word,char * explain)
{
	if(word==NULL || explain==NULL || strlen(word)==0 || strlen(explain)==0)
		return 1;
	DataBase *db = DataBase_new(db_path);
	if(db){
		return update_one_word(db,word,explain);
	}
	return -1;
}


int make_db(char * dict_name,char * db_path)
{
	DataBase *db = DataBase_new(db_path);
	if(db){
		int rc=0;
		//rc = DataBase_exec(db,"create table if not exists dict(id INTEGER primary key asc,word varchar(200) UNIQUE,explain text);");
		rc = DataBase_exec(db,"create table if not exists dict(id INTEGER primary key asc,word text UNIQUE,explain text);");
		if(!rc)printf("\nsql_result_str:%s",db->result_str);
		else return 1;
		//rc = DataBase_exec(db,"CREATE UNIQUE INDEX unique_index_word ON dict(word);");
		//if(!rc)printf("\nsql_result_str:%s",db->result_str);
		//else return 2;

		//char sql[30000];
		//memset(sql,0,sizeof(sql));


		//const char* insertSQL = "insert into dict(word,explain) values(?,?);";
		const char* insertSQL = "replace into dict(explain,word) values(?,?);";
		sqlite3_stmt* stmt3 = NULL;
		sqlite3 * conn = db->db;
		if (sqlite3_prepare_v2(conn,insertSQL,strlen(insertSQL),&stmt3,NULL) != SQLITE_OK) {
			if (stmt3)
				sqlite3_finalize(stmt3);
			sqlite3_close(conn);
			return 1;
		}


		Dict * dict = Dict_new();
		dict->name = dict_name;
		if(dict->file==0){
			dict->file = Dict_open(dict);
		}
		int i = 0;
		/*while(i < dict->wordcount)*/
		while(i < dict->wordcount)
		{
			Word*word = Dict_getWordByIndex(dict,i);
			//if(word && regex_match(word->word,"\\(Brit\\)"))
			if(word)
			{
				//printf("%s\r\n",word->word);
				char * explain = Dict_getMean(dict,word);
				if(explain==NULL)
				{
					printf("\n ERROR:%s,%d,%s\r\n",explain,i,word->word);
					return 2;
				}
				int rc = inserts(db->db,stmt3,word->word,explain);
				if(!rc){
					if(i%100==0)
					{
						printf("\r\n %d,%s",i,word->word);fflush(stdout);
					}
					//printf("\nsql_result_str:%s",db->result_str);
				}else{
					printf("\n ERROR:%s,%d,%s\r\n",explain,i,word->word);
					return 1;
				}
				free(explain);
			}
			++i;
		}
		sqlite3_finalize(stmt3);



		//rc = DataBase_exec(db,"select * from dict;");
		//if(!rc)printf("\nsql_result_str:%s",db->result_str);
		DataBase_clear(db);
		db = NULL;
	}
	return 0;
}
//REPLACE INTO dict(word, explain) VALUES (?, ?);

int addirregularverb()
{
	char *s = readfile("irregularVerbs.txt",NULL);
	Array * arr = string_split(s,"\n");
	int i = 0;
	Dict * dict = Dict_new();
	dict->name = "oxford-gb";
	while(i<arr->length)
	{
		char * s2 = (char*)Array_getByIndex(arr,i);
		//printf("%d:%s\r\n",i,s2);
		Array * arr2 = string_split(s2," ");
		//if(arr2) printf("%d:%s\r\n",arr2->length,s2);
		//pt pp
		if(arr2 && arr2->length==3)
		{
			//printf("%d:%s\r\n",i,s2);
			char * verb = (char*)Array_getByIndex(arr2,0);
			char * pt = (char*)Array_getByIndex(arr2,1);
			char * pp = (char*)Array_getByIndex(arr2,2);
			if(strcmp(pp,"-")==0){
				printf("%d:%s\r\n",i,s2);
			}else{

				Array * pts = string_split(pt,",");
				Array * pps = string_split(pp,",");

				if(pts && pts->length>0)
				{
					int j=0;
					while(j<pts->length)
					{
						char * _pt = (char*)Array_getByIndex(pts,j);
						int _index = Dict_getWordIndex(dict,_pt);
						if(_index<0)
						{
							int len = strlen(_pt)+16;
							char explain[len];
							memset(explain,0,len);
							sprintf(explain,"pt of %s",verb);
							update_word("/home/libiao/dict.db",_pt,explain);
							printf("%s:%s\r\n",_pt,explain);
						}
						++j;
					}
				}

				if(pps && pps->length>0)
				{
					int j=0;
					while(j<pps->length)
					{
						char * _pp = (char*)Array_getByIndex(pps,j);
						int _index = Dict_getWordIndex(dict,_pp);
						if(_index<0)
						{
							int len = strlen(_pp)+16;
							char explain[len];
							memset(explain,0,len);
							if(Array_getIndexByStringValue(pts,_pp)>=0){
								sprintf(explain,"pt,pp of %s",verb);
							}else{
								sprintf(explain,"pp of %s",verb);
							}
							update_word("/home/libiao/dict.db",_pp,explain);
							printf("%s:%s\r\n",_pp,explain);
						}
						++j;
					}
				}
			}
			//Array * arr2 = string_split(s2," ");

		}
		++i;
	}
	return 0;
}

void remend()
{
	DataBase * db = DataBase_new(decodePath("~/dict.db"));
	//char * sql = "select * from dict where explain like \"%<i>US</i>%\";";
	//char * sql = "select * from dict where word like \"%,%\";";
	//char * sql = "select * from dict where word like \"%/ %/%\";";
	char * sql = "select * from dict where word like \"`%\";";
	int rc = DataBase_exec2array(db,sql);
	if(rc){
		printf("ERROR: %s\r\n",db->result_arr);
		return;
	}
	//DataBase_result_print(db);
	Array * wordsArr = NULL;
	Array * explainsArr = NULL;
	if(db->result_arr){
		Array * data = db->result_arr;
		Array * names = Array_getByIndex(data,0);
		if(names==NULL){
			printf("no names Array");
			return ;
		}
		int nCount = names->length;
		int i = 0;
		while(i<nCount)
		{
			char * curName =Array_getByIndex(names,i);
			if(strcmp(curName,"word")==0)
			{
				wordsArr = Array_getByIndex(data,i+1);
				if(wordsArr == NULL || wordsArr->length==0){
					printf("%d:no word\r\n",i);
					return ;
					break;
				}
			}else if(strcmp(curName,"explain")==0){
				explainsArr = Array_getByIndex(data,i+1);
				if(explainsArr == NULL || explainsArr->length==0){
					printf("%d:no explain\r\n",i);
					return ;
					break;
				}
			}
			++i;
		}
	}
	if(wordsArr && explainsArr && wordsArr->length>0 && wordsArr->length == explainsArr->length)
	{
		int i = 0;
		char _explain[160];
		Dict * dict = Dict_new();
		dict->name = "oxford-gb";
		while(i<wordsArr->length){
			//char * explain = NULL;
			char * word = Array_getByIndex(wordsArr,i);
			char * explain = Array_getByIndex(explainsArr,i);
			//if(regex_match(word,"/\\(esp /"))
			if(regex_match(word,"/^`/"))
			{
				//char * _word = regex_replace_all(word,"/ \\(Brit.*$/","");
				//char * _word = regex_replace_all(word,"/^also[ `]*/","");
				//char * _word = regex_replace_all(word,"/^[^,]*, /","");
				//char * _word = regex_replace_all(word,"/^([^ ]{1,}) .*$/","$1");
				char * _word = regex_replace_all(word,"/^`/","");
				//printf("%d:%s\r\n",i,_word);fflush(stdout);

				int _index = Dict_getWordIndex(dict,_word);
				if(_index<0)
				{
					printf("%s->\"%s\" \r\n",word,_word);fflush(stdout);
					//update_one_word(db,_word,explain);
				}

				char * __word = NULL;
				//__word = regex_replace_all(word,"/^.* US (.*)$/","$1");
				__word = regex_replace_all(word,"/^[^ ]{1,} /(.*)/.*$/","$1");
				memset(_explain,0,sizeof(_explain));
				sprintf(_explain,"=> %s",_word);
				_index = Dict_getWordIndex(dict,__word);
				if(_index<0)
				{
					//printf("\"%s\": %s\r\n",__word,_explain);
					//update_one_word(db,__word,_explain);
				}
				//
			}
			/*
			   if(regex_match(tmp,"/ <i>US</i> /"))
			   explain = regex_replace_all(tmp,"/ <i>US</i> /","");
			   if(regex_match(tmp,"/<i>US</i>/"))
			   explain = regex_replace_all(tmp,"/<i>US</i>/","");
			   if(word && explain){
			   update_one_word(db,word,explain);
			   }
			   */
			++i;
		}
	}
}
static void addChineseExplain()
{
	DataBase * db = DataBase_new(decodePath("~/dict.db"));
	Dict * dict = Dict_new();
	dict->name = "oxford-gb";
	Dict * dict2 = Dict_new();
	dict2->name = "langdao";
	//dict2->name = "stardict";
	int i = 0;
	int numWords = Dict_getNumWords(dict);
	int j=0;
	while(i<numWords)
	{
		Word * word = Dict_getWordByIndex(dict,i);
		char * explain = Dict_getMean(dict,word);
		//if(regex_match(explain,"/^[\x01-\x7fːæðŋǀɑɒɔəɜɪʃʊʌʒθ]{2,}$/i"))
		//if(regex_match(explain,"/^[\x01-\x7fːæðŋǀɑɒɔəɜɪʃʊʌʒθ]{2,}$/m"))
		//if(regex_match(explain,"/[\x2f/\\[\\]\x5b\x5d]/m")==0)
		if(strstr(explain,"[")==NULL && strstr(explain,"/")==NULL)
		{
			int _index = Dict_getWordIndex(dict2,word->word);
			if(_index>=0)
			{
				Word * word2 = Dict_getWordByIndex(dict2,_index);
				char * explain2 = Dict_getMean(dict2,word2);
				//printf("%s:(%s)\r\n",word->word,explain);

				if(strstr(explain2,"[")!=NULL)
				{
					int len = strlen(explain2)+strlen(explain)+4;
					char newExplain[len];
					memset(newExplain,0,len);
					sprintf(newExplain,"%s\n%s",explain,explain2);
					printf("====>%s\r\n",newExplain);
					if(strcmp(dict2->name,"strdict")==0)
					{
						//update_one_word(db,word->word,explain2);
					}else{
						update_one_word(db,word->word,newExplain);
					}
				}
				free(explain2);
			}
			free(explain);
			explain = NULL;
			++j;
			//if(j>300) break;
		}
		if(explain)
			free(explain);
		++i;
	}
	printf("a ok\r\n");
}

int main()
{
	/*
	   const char * dict_name = "strdict";
	   char * dict_dir = decodePath(contact_str("~/sound/",dict_name));
	   printf("%s/%s.ifo",dict_dir,dict_name);
	   */
	//remend();
	//addChineseExplain();
	//return update_word("/home/libiao/dict.db","hello","has long mean");
	//make_db("oxford-gb","/home/libiao/dict.db");//from stardict to sqlite3
	//addirregularverb();
	//select count(*) from dict where explain like "%<i>US</i>%";
	//return make_dict("/home/libiao/dict.db","oxford-gb");// from sqlite3 to stardict
	return 0;
}
