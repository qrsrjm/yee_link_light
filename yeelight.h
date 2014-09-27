#ifndef _YEELIGHT_H
#define _YEELIGHT_H

#include "yeelink.h"



#define CLOSED 0
#define OPENED 1
#define ERROR -1
#define OFFLINE -2

//�����������أ���Ϣ
struct switch_info{
	char* timestamp;
	int state;
};


/**
��ȡ����״̬ CLOSED:�ر� OPENED���� ERROR��δ֪����
**/
struct switch_info* get_switch_info(int devid,int senid);	

/**
�ı俪��״̬
**/
char* change_switch_state(int devid,int senid,int state);

/**
��ѯ�Ƶ�״̬�� ����|�ر�|����
**/
int check_light_state(int id);

/**
����switch_info������Ϣ�ı�Ƶ�״̬
**/
void change_light_state(struct switch_info*,int);
#endif
