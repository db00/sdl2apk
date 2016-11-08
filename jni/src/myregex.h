#ifndef myregex_h
#define myregex_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <regex.h>
#include "array.h"
#include "mystring.h"

#define SUBSLEN 99             

int regex_match(const char*s,const char*reg);
char *regex_search(char * s,char * reg , int callback,int *dealed_len);
char *regex_replace2(char * str,Array* str_arr );
char *regex_replace(char * s, const char * reg , const char * replace_str,int *dealed_len);
int regex_search_all(char * s,char * reg , Array*matched_arr);
char *regex_replace_all(char * s, const char * reg , const char * replace_str);
void regex_matchedarrClear(Array* matched_arr);

#endif
