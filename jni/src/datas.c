/***
 *
 gcc cJSON.c sqlite.c datas.c files.c array.c mystring.c myregex.c -lm -lsqlite3 -D debug_datas -I"../SDL2/include/" && ./a.out
 */
#include "datas.h"

int add_new_word(char * word)
{
	int id=get_word_id(word);
	if(id>0)
	{
		printf("\r\nin list\r\n");
	}else{
		printf("\r\nnot in list\r\n");
		char * s ="insert into list(word,date,remembered,numAccess,numTest) values (\"%s\",%d,0,1,0);";
		char sql[256];
		memset(sql,0,256);
		sprintf(sql,s,word,time(NULL));
		int rc = DataBase_exec(history_db,sql);
		//if(!rc)printf("\n insert sql_result_str:%s",history_db->result_str);
		id = get_word_id(word);
	}
	return id;
}

/**
 *
 * 把word加入熟词（1）/生词（0）
 *
 */
int add_remembered_word(char * word,int remembered)
{
	int id = add_new_word(word);
	if(id>0)
	{
		char * s ="update list set remembered=%d where wordid=%d;";
		char sql[100];
		memset(sql,0,100);
		sprintf(sql,s,remembered,id);
		int rc = DataBase_exec(history_db,sql);
		if(!rc)printf("\n update sql_result_str:%s",history_db->result_str);
	}
	return id;
}

int get_word_id(char * word)
{
	char * s = "select wordid from list where word=\"%s\";";
	char sql[200];
	memset(sql,0,200);
	sprintf(sql,s,word);
	int rc = DataBase_exec(history_db,sql);
	if(regex_match(history_db->result_str,"/:\"[0-9]+\"/"))
	{
		//if(!rc)printf("\nsql_result_str:%s",history_db->result_str);
		int len=0;
		char * r =regex_replace(history_db->result_str,"/^.*:\"([0-9]+)\"\\}]$/i","$1",&len);
		//printf("\r\n-------------------%s\r\n",r);
		return atoi(r);
	}
	return 0;
}

int add_to_history(int wordid)
{

	char * s ="insert into history(wordid,status,date) values (%d,\"-\",%d);";
	char sql[200];
	memset(sql,0,200);
	sprintf(sql,s,wordid,time(NULL));
	int rc = DataBase_exec(history_db,sql);
	return rc;
}

void clear_result_str()
{
	if(history_db->result_str){
		free(history_db->result_str);
		history_db->result_str=NULL;
	}
}

char * datas_query(char * sql)
{
	clear_result_str();
	int rc;
	rc = DataBase_exec(history_db,sql);
	if(!rc){
		//printf("\n history :\n%s",history_db->result_str);
		return history_db->result_str;
	}
	return NULL;
}

Array * datas_query2(char * sql)
{
	clear_result_str();
	int rc = DataBase_exec2array(history_db,sql);
	if(!rc){
		DataBase_result_print(history_db);
		return history_db->result_arr;
	}
	return NULL;
}


char * get_history()
{
	return datas_query("select * from list group by wordid ORDER BY date desc;");
	/*
	   clear_result_str();
	   int rc;
	   rc = DataBase_exec(history_db,"select * from list group by wordid ORDER BY date desc;");
	   if(!rc){
	   printf("\n history :\n%s",history_db->result_str);

	   return history_db->result_str;
	   }
	   return NULL;
	   */
}
char * get_remembered_history(int remembered)
{
	char sql[200];
	memset(sql,0,200);
	char * s = ("select * from list where remembered=%d group by wordid ORDER BY date desc;");
	sprintf(sql,s,remembered);
	//printf("\n%s\n",sql);fflush(stdout);
	return datas_query(sql);
}

Array * get_history_list(int numWords,char * word,char * compare)
{
	char sql[300];
	memset(sql,0,300);
	char *s;
	if(word && compare)
	{
		//s = ("select * from list where group by wordid ORDER BY date desc;");
		if(compare[0]=='<')
			s = "select *  from list where wordid %s (select wordid from list where word==\"%s\") order by wordid desc limit 0,%d";
		else
			s = "select *  from list where wordid %s (select wordid from list where word==\"%s\") order by wordid limit 0,%d";
		sprintf(sql,s,compare,word,numWords);
	}else{
		s = "select *  from list order by wordid desc limit 0,%d";
		sprintf(sql,s,numWords);
	}
	printf("\n%s\n",sql);fflush(stdout);
	return datas_query2(sql);
}
Array * get_remembered_list(int remembered,int numWords,char * word,char * compare)
{
	char sql[300];
	memset(sql,0,300);
	char *s;
	if(word && compare)
	{
		//s = ("select * from list where remembered=%d group by wordid ORDER BY date desc;");
		if(compare[0]=='<')
			s = "select *  from list where wordid %s (select wordid from list where word==\"%s\") and remembered==%d order by wordid desc limit 0,%d";
		else
			s = "select *  from list where wordid %s (select wordid from list where word==\"%s\") and remembered==%d order by wordid limit 0,%d";
		sprintf(sql,s,compare,word,remembered,numWords);
	}else{
		s = "select *  from list where remembered==%d order by wordid desc limit 0,%d";
		sprintf(sql,s,remembered,numWords);
	}

	//select *  from list where wordid <= (select wordid from list where word=="good") order by wordid desc limit 0,20;
	//char * s = ("select * from list where remembered=%d group by wordid ORDER BY date desc;");
	//sprintf(sql,s,remembered);
	printf("\n%s\n",sql);fflush(stdout);
	return datas_query2(sql);
}


int add_to_test(int wordid,int result)
{
	char * s ="insert into history(wordid,status,date) values (%d,%d,%d);";
	char sql[128];
	memset(sql,0,128);
	sprintf(sql,s,wordid,result,time(NULL));
	int rc = DataBase_exec(history_db,sql);
	return rc;
}

int init_db()
{
	history_db = DataBase_new(decodePath("~/sound/test.db"));
	int rc=0;
	rc = DataBase_exec(history_db,"create table if not exists list(wordid INTEGER primary key asc,word varchar(50), date real, remembered char(1), numAccess INTEGER, numTest INTEGER);");
	//if(!rc)printf("\nsql_result_str:%s",history_db->result_str);
	rc = DataBase_exec(history_db,"create table if not exists history(id INTEGER primary key asc, wordid INTEGER, status varchar(1), date real);");
	//if(!rc)printf("\nsql_result_str:%s",history_db->result_str);
	return 0;
}


#ifdef debug_datas
int main()
{
	unsigned int i = -1;
	unsigned int j = -1;
	//printf("%d\r\n",-(((unsigned int)-1)/4));
	printf("%d\r\n",((unsigned int)-1)/4);
	init_db();
	if(history_db){
		int rc=0;
		/*
		   rc = add_new_word("test");
		   printf("\r\n-------------------id:%d\r\n",rc);
		   rc = add_to_history(rc);
		   if(!rc)printf("\nsql_result_str:%s",history_db->result_str);
		//printf("\r\n-------------------id:%d\r\n",rc);
		rc = DataBase_exec(history_db,"select * from sqlite_master;");
		if(!rc)printf("\nsql_result_str:%s",history_db->result_str);
		free(history_db->result_str);
		history_db->result_str=NULL;
		rc = DataBase_exec2array(history_db,"select * from list;");
		if(!rc)DataBase_result_print(history_db);
		*/
		//printf("%s",get_history());
		//add_remembered_word("test",1);
		//printf("%s",get_remembered_history(1));
		Array * arr = get_remembered_list(0,20,"good",">");
		DataBase_clear(history_db);
		history_db = NULL;
	}
	return 0;
}
#endif
