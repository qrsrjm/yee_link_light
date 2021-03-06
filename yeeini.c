/*
 * yeeini.c
 *
 *  Created on: 2013年7月13日
 *      Author: root
 */
#include "yeeini.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int find_key(const char *buf, const char *section, const char *key,
	int *sectionStart, int *valueStart, unsigned long *valueSize);
static void get_line(const char *buf, int inStart, int *lineStart, int *lineEnd);
static void getPath();
#ifndef strnicmp
#define strnicmp strncasecmp
#endif
#define NOT_COMMENT(c)	(c!=';' && c!='#')	/* 不是注释行 */

static char* CFG_NAME="yeelight.conf";
char CFG_FILE[64]="";

int device_id;
int switch_id;
char yeebox_ip[16]="";
int yeebox_port;
int yeelight;
char yeelink_apikey[33]="";

/*
** 先获取当前目录绝对路径，并去掉程序名
** 加上配置文件名
*/
static void getPath()
{
	int cnt = readlink("/proc/self/exe", CFG_FILE, sizeof(CFG_FILE));
	if (cnt < 0 || cnt >= sizeof(CFG_FILE))
	{
		printf("***Error***\n");
		exit(-1);
	}
	//获取当前目录绝对路径，即去掉程序名
	int i;
	for (i = cnt; i >=0; --i)
	{
		if (CFG_FILE[i] == '/')
		{
			CFG_FILE[i+1] = '\0';
			break;
		}
	}
	strcat(CFG_FILE,CFG_NAME);
	return ;
}

/*
**	初始化配置
*/
void init_config(){

	//获取配置文件绝对路径
	getPath();
	//加载配置文件
	char* buf=load_file(CFG_FILE);
	//读取device id
	device_id=get_int(buf,"Yeelight","DEVICE_ID",0);
	//读取传感器ID
	switch_id=get_int(buf,"Yeelight","SWITCH_ID",0);
	
	get_string(buf,"Yeelight","YEEBOX_IP","",yeebox_ip,sizeof(yeebox_ip));
	yeebox_port=get_int(buf,"Yeelight","YEEBOX_PORT",0);

	//想要控制的灯的网络号
	yeelight=get_int(buf,"Yeelight","YEELIGHT",0);

	//Yeelink平台的APIKEY
	get_string(buf,"Yeelight","YEELINK_APIKEY","",yeelink_apikey,sizeof(yeelink_apikey));

	free(buf);
}

/*
** 加载配置文件
*/
char *load_file(const char *fileName)
{
	FILE *fp = NULL;
	long size = 0;
	char *buf = NULL;
	if ((fp=fopen(fileName, "rb")) == NULL)
		return NULL;
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	rewind(fp);
	buf = (char *)malloc(size+1);
	buf[size] = '\0';
	if (fread(buf, size, 1, fp) < 1)
	{
		free(buf);
		buf = NULL;
	}
	fclose(fp);
	return buf;
}

int get_string(const char *buf, const char *section, const char *key,
	const char *defaultValue, char *value, unsigned long size)
{
	int sectionStart, valueStart;
	unsigned long valueSize;

	if (find_key(buf, section, key, &sectionStart, &valueStart, &valueSize)!=0 || valueSize==0)	/* 未找到？ */
	{
		strncpy(value, defaultValue, size);
		return -1;
	}
	if (size-1 < valueSize)		/* 找到但太长？ */
		valueSize = size - 1;
	memset(value, 0, size);
	strncpy(value, buf+valueStart, valueSize);
	return 0;
}

int get_int(const char *buf, const char *section, const char *key, int defaultValue)
{
	char value[21] = {0};
	get_string(buf, section, key, "", value, sizeof(value));
	if (value[0] == '\0')	/* 找不到或找到但为空？ */
		return defaultValue;
	return atoi(value);
}

/*
**  获取一行的行首和行尾的位置
*/
static void get_line(const char *buf, int inStart, int *lineStart, int *lineEnd)
{
	int start, end;
	for (start=inStart; buf[start]==' ' || buf[start]=='\t' || buf[start]=='\r' || buf[start]=='\n'; start++);
	for (end=start; buf[end]!='\r' && buf[end]!='\n' && buf[end]!='\0'; end++);
	*lineStart = start;		//行首
	*lineEnd = end;			//行尾
}

/*
** 查找一行里的字段和值
*/
static int find_key(const char *buf, const char *section, const char *key,
	int *sectionStart, int *valueStart, unsigned long *valueSize)
{
	int lineStart, lineEnd, i;
	for (*sectionStart=-1, lineEnd=0; buf[lineEnd]!='\0'; )
	{
		get_line(buf, lineEnd, &lineStart, &lineEnd);	//获取一行
		if (buf[lineStart] == '[')
		{
			for (i=++lineStart; i<lineEnd && buf[i]!=']'; i++);
			if (i<lineEnd && strnicmp(buf+lineStart, section, i-lineStart)==0)	/* 找到Section？ */
			{
				*sectionStart = lineStart-1;
				if (key == NULL)
					return -1;
			}
			else if (*sectionStart != -1)	/* 找到Section但未找到Key */
				return -1;
		}
		else if (*sectionStart!=-1 && NOT_COMMENT(buf[lineStart]))	/* 找到Section且该行不是注释 */
		{
			for (i=lineStart+1; i<lineEnd && buf[i]!='='; i++);
			if (i<lineEnd && strnicmp(buf+lineStart, key, i-lineStart)==0)	/* 找到Key？ */
			{
				*valueStart = i + 1;
				*valueSize = lineEnd - *valueStart;
				return 0;
			}
		}
	}
	return -1;
}
