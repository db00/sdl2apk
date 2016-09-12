/***
 *
 gcc sqlite.c -lsqlite3 -D debug_sqlite && ./a.out
 */
#include "sqlite.h"

void DataBase_clear(DataBase*database)
{
	if(database){
		sqlite3_close(database->db);
		if(database->name)
			free(database->name);
		if(database->result_str)
			free(database->result_str);
		free(database);
		database=NULL;
	}
}

DataBase * DataBase_new(char*name)
{
	DataBase *database = (DataBase*)malloc(sizeof(*database));
	memset(database,0,sizeof(*database));
	database->name = (char*)malloc(strlen(name)+1);
	memset(database->name,0,strlen(name)+1);
	sprintf(database->name,"%s",name);
	int rc = sqlite3_open(database->name, &(database->db));
	if( rc ){
		const char *errmsg = sqlite3_errmsg(database->db);
		fprintf(stderr, "Can't open database: %s\n", errmsg);
		DataBase_clear(database);
		return NULL;
	}
	return database;
}

static char * string_append(char * old,int *old_size,char * s)
{
	if(old == NULL){
		*old_size = 1024;
		old = malloc(*old_size);
		memset(old,0,1024);
	}
	if(s == NULL){
		return old;
	}
	while(strlen(old)+strlen(s)>= *old_size)
	{
		*old_size += 1024;
		old = realloc(old,*old_size);
	}
	return strcat(old,s);
}

static int callback(void *database, int argc, char **argv, char **azColName){
	DataBase*db = (DataBase*)database;
	int i;
	if(db->result_str && strlen(db->result_str)>1){
		db->result_str[strlen(db->result_str)-1]=',';
	}else{
		db->result_str = string_append(db->result_str,&(db->result_space),"[");
	}
	db->result_str = string_append(db->result_str,&(db->result_space),"{");
	for(i=0; i<argc; i++){
		db->result_str = string_append(db->result_str,&(db->result_space),"\"");
		db->result_str = string_append(db->result_str,&(db->result_space),azColName[i]);
		db->result_str = string_append(db->result_str,&(db->result_space),"\":\"");
		if(argv[i]) db->result_str = string_append(db->result_str,&(db->result_space),argv[i]);
		db->result_str = string_append(db->result_str,&(db->result_space),"\"");
		db->result_str = string_append(db->result_str,&(db->result_space),",");
	}
	db->result_str[strlen(db->result_str)-1]='\0';
	db->result_str = string_append(db->result_str,&(db->result_space),"}]");
	return 0;
}

int DataBase_exec(DataBase*db,const char * sql)
{
	if(db == NULL || sql == NULL)
	{
		return -1;
	}

	if(db->result_str){
		free(db->result_str);
		db->result_str = NULL;
		db->result_space= 0;
	}
	char *zErrMsg = 0;
	int rc = sqlite3_exec(db->db, sql ,callback, db, &zErrMsg);
	if(rc != SQLITE_OK){
		db->result_str = string_append(db->result_str,&(db->result_space),zErrMsg);
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return -2;
	}
	if(db->result_str==NULL)
	{
		db->result_str = string_append(db->result_str,&(db->result_space),"(ok)\n");
		db->result_str = string_append(db->result_str,&(db->result_space),(char*)sql);
	}
	return 0;
}

DataBase *exec_sql(DataBase*db,char*Path)
{
	char *sql=strstr(Path,":");
	char *sqlite=Path;
	if(sql!=NULL){
		sql=sql+1;
		if(strlen(sql)>0){
			if(strncmp("/sql:",sqlite,4)==0){//访问数据库
				if(db==NULL)
					db = DataBase_new("test.db");
				if(db){
					DataBase_exec(db,sql);
				}else{
					//DataBase_clear(db);
					//db = NULL;
				}
				return db;
			}
		}
	}
	return NULL;
}

#ifdef debug_sqlite
int main()
{
	DataBase *db = DataBase_new("test.db");
	if(db){
		int rc=0;
		rc = DataBase_exec(db,"create table if not exists member(id INTEGER primary key asc,time INTEGER,name,fromschool,age,sex INTEGER ,idcard,policies,familyaddress,parents_tel,tel,workplace,studyfee,workfee,classname,classno,photo);");
		if(!rc)printf("\nsql_result_str:%s",db->result_str);
		rc = DataBase_exec(db,"insert into member(name,sex,fromschool,age,idcard,policies,familyaddress,parents_tel,tel,workplace,studyfee,workfee,classname,classno,photo) values (\"name\",1,\"fromschool\",\"age\",\"430422000000000000\",\"qunzhong\",\"familyaddress\",\"parents_tel\",\"tel\",\"workplace\",\"studyfee\",\"workfee\",\"classname\",\"classno\",\"photo\");");
		if(!rc)printf("\nsql_result_str:%s",db->result_str);
		rc = DataBase_exec(db,"select * from member;");
		if(!rc)printf("\nsql_result_str:%s",db->result_str);
		rc = DataBase_exec(db,"select * from sqlite_master;");
		if(!rc)printf("\nsql_result_str:%s",db->result_str);
		DataBase_clear(db);
		db = NULL;
	}
	return 0;
}
#endif

//create table if not exists member(id INTEGER primary key asc,time INTEGER,name,fromschool,age,sex INTEGER,idcard,policies,familyaddress,parents_tel,tel,workplace,studyfee,workfee,classname,classno,photo);
//insert into member(name,fromschool,age,sex,idcard,policies,familyaddress,parents_tel,tel,workplace,studyfee,workfee,classname,classno,photo) values ("name","fromschool","age",1,"430422000000000000","qunzhong","familyaddress","parents_tel","tel","workplace","studyfee","workfee","classname","classno","photo");
//select * from sqlite_master
//"update 'table_name' 'column_name' = 'value'"
//"DELETE FROM FAVORITE_TABLE WHERE EntryIndex = %d"
//"delete from 'table_name'"
//"drop table 'table_name'"

//adb push gcc /mnt/sdcard/Android/data/com.n0n3m4.droidc/files/gcc/
/*ln -s /mnt/sdcard/Android/data/com.n0n3m4.droidc/files/gcc/bin/arm-linux-androideabi-gcc /system/bin/gcc*/
/*ln -s /mnt/sdcard/Android/data/com.n0n3m4.droidc/files/gcc/bin/arm-linux-androideabi-g++ /system/bin/g++*/
/*ln -s /mnt/sdcard/Android/data/com.n0n3m4.droidc/files/gcc/bin/make /system/bin/make*/
/**/
/*lib = /mnt/sdcard/Android/data/com.n0n3m4.droidc/files/gcc/arm-linux-androideabi/lib/ */
/*include = /mnt/sdcard/Android/data/com.n0n3m4.droidc/files/gcc/arm-linux-androideabi/include/ */
