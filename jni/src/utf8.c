/**
 * @file utf8.c
 gcc -D debug_utf8 mystring.c utf8.c array.c && ./a.out 
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2016-09-07
 */
#include "utf8.h"

// one charactor size
int UTF8_numByte(char * utf8)
{
	unsigned char c = *utf8;
	if(c==0)
		return 0;
	if(c<0x80)
		return 1;
	if(c<0xe0)
		return 2;
	if(c<0xf0)
		return 3;
	if(c<0xf8)
		return 4;
	if(c<0xfe)
		return 5;
	if(c<0xff)
		return 6;
	return 0;
}

//num charactor of a utf8 string 
size_t UTF8_length(char * utf8)
{
	size_t len = 0;
	char * p = (char*)utf8;
	if(utf8)
	{
		int l = UTF8_numByte(p);
		while(l>0)
		{
			len++;
			p += l;
			l = UTF8_numByte(p);
		}

	}
	return len;
}

//make a Array contains each charactor of the utf8 string
Array * UTF8_each(const char * utf8)
{
	Array * array = NULL;
	char * p = (char*)utf8;
	if(utf8)
	{
		int l = UTF8_numByte(p);
		while(l>0)
		{
			char * s = getSubStr(p,0,l);
			array = Array_push(array,s);

			p += l;
			l = UTF8_numByte(p);
		}

	}
	return array;
}

#ifndef __ANDROID__
char * UTF8_encodeTo(char* inbuf,size_t * outlen,char * coding){
	size_t inlen =strlen(inbuf);
	iconv_t cd=iconv_open(coding,"UTF-8");
	char*outbuf=(char*)malloc(inlen*4);
	memset(outbuf,0,inlen*4);
	char*in=inbuf;
	char*out=outbuf;
	*outlen=inlen*4;
	iconv(cd,&in,(size_t*)&inlen,&out,outlen);
	*outlen=strlen(outbuf);
	//printf("%s\n",outbuf);
	//free(outbuf);
	iconv_close(cd);
	return outbuf;
}

char * UTF8_readFrom(char* inbuf,size_t * outlen,char * coding){
	size_t inlen =strlen(inbuf);
	iconv_t cd=iconv_open("UTF-8",coding);
	char*outbuf=(char*)malloc(inlen*4);
	memset(outbuf,0,inlen*4);
	char*in=inbuf;
	char*out=outbuf;
	*outlen=inlen*4;
	iconv(cd,&in,(size_t*)&inlen,&out,outlen);
	*outlen=strlen(outbuf);
	//printf("%s\n",outbuf);
	//free(outbuf);
	iconv_close(cd);
	return outbuf;
}
#endif

#ifdef debug_utf8
int main()
{
	printf("%d\n",UTF8_length("help年可!\n"));
	Array * array = UTF8_each("和可了ijs！");
	if(array)
	{
		printf("array->length:%d\n",array->length);
		int i = 0;
		while(i<array->length)
		{
			char * s = Array_getByIndex(array,i);
			if(s)
				printf("%d:%s\n",i,s);
			++i;
		}
		Array_freeEach(array);
		array=NULL;
	}
	return 0;
}
#endif
