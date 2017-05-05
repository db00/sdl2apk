/**
 * @file sqldict.c
 gcc -ldl -lpthread -lsqlite3 array.c myregex.c mystring.c files.c -I"../SDL2/include/" sqldict.c sqlite.c dict.c  && ./a.out
 gcc regex.c array.c sqlite3.c myregex.c mystring.c files.c -I"../SDL2/include/" sqldict.c sqlite.c dict.c  && a
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2017-05-03
 */

#include "sqlite.h"
#include "dict.h"

int inserts(sqlite3*  conn,sqlite3_stmt * stmt3,char * word,char * explain)
{
	//在绑定时，最左面的变量索引值是1。
	//sqlite3_bind_int(stmt3,1,i);
	//sqlite3_bind_double(stmt3,2,i * 1.0);
	sqlite3_bind_text(stmt3,1,word,strlen(word),SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt3,2,explain,strlen(explain),SQLITE_TRANSIENT);
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
	char s[] = "%s\r\nversion=%s\r\nwordcount=%d\r\nidxfilesize=%d\r\nbookname=%s\r\nauthor=xx\r\ndescription=xxxxx。\r\ndate=2003.08.26\r\nsametypesequence=m";
	memset(str,0,len);
	sprintf(str,s,name,version,wordcount,idxfilesize,bookname);
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
			//if(i%1000==0)
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

	char * ifo_s = ifo(dict_name,"1.0",wordsArr->length,idxfilesize,dict_name);
	char filename[128];
	memset(filename,0,sizeof(filename));
	sprintf(filename,"%s.ifo",dict_name);
	writefile(filename,ifo_s,strlen(ifo_s));

	memset(filename,0,sizeof(filename));
	sprintf(filename,"%s.idx",dict_name);
	writefile(filename,idx,idxfilesize);

	memset(filename,0,sizeof(filename));
	sprintf(filename,"%s.dict",dict_name);
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

int update_word(char * db_path,char * word,char * explain)
{
	if(word==NULL || explain==NULL || strlen(word)==0 || strlen(explain)==0)
		return 1;
	DataBase *db = DataBase_new(db_path);
	if(db){
		int rc=0;
		rc = DataBase_exec(db,"create table if not exists dict(id INTEGER primary key asc,word varchar(200) UNIQUE,explain text);");
		if(!rc)printf("\nsql_result_str:%s",db->result_str);
		else return 1;

		const char* insertSQL = "replace into dict(word,explain) values(?,?);";
		sqlite3_stmt* stmt3 = NULL;
		sqlite3 * conn = db->db;
		if (sqlite3_prepare_v2(conn,insertSQL,strlen(insertSQL),&stmt3,NULL) != SQLITE_OK) {
			if (stmt3)
				sqlite3_finalize(stmt3);
			sqlite3_close(conn);
			return 1;
		}
		rc = inserts(db->db,stmt3,word,explain);
		if(!rc){
			printf("\r\n %s",word);fflush(stdout);
			//printf("\nsql_result_str:%s",db->result_str);
		}else{
			printf("\n ERROR:%s,%s\r\n",explain,word);
			return 1;
		}
		sqlite3_finalize(stmt3);
		DataBase_clear(db);
		db = NULL;
	}
	return 0;
}


int make_db(char * dict_name,char * db_path)
{
	DataBase *db = DataBase_new(db_path);
	if(db){
		int rc=0;
		rc = DataBase_exec(db,"create table if not exists dict(id INTEGER primary key asc,word varchar(200) UNIQUE,explain text);");
		if(!rc)printf("\nsql_result_str:%s",db->result_str);
		else return 1;
		//rc = DataBase_exec(db,"CREATE UNIQUE INDEX unique_index_word ON dict(word);");
		//if(!rc)printf("\nsql_result_str:%s",db->result_str);
		//else return 2;

		//char sql[30000];
		//memset(sql,0,sizeof(sql));


		//const char* insertSQL = "insert into dict(word,explain) values(?,?);";
		const char* insertSQL = "replace into dict(word,explain) values(?,?);";
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
			if(word)
			{
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

int main()
{
	//return update_word("/home/libiao/dict.db","hello","has long mean");
	//return make_dict("/home/libiao/dict.db","oxford");// from sqlite3 to stardict
	return make_db("oxford-gb","/home/libiao/dict.db");//from stardict to sqlite3
	return 0;
}
