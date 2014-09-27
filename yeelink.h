#ifndef __YEELINK
#define __YEELINK

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<fcntl.h>
#include<netdb.h>
#include <pthread.h>

#define HOST_NAME "api.yeelink.net"
#define PORT 80
#define MAXBUFF 1024


/**
 * �豸���ݽṹ
 * */
struct device{
	char *title;
	char *about;
	char *tags;
	char *local;
	float latitude;
	float longitude;
};

/**
 * ͨ�ô�����
 * */
struct sensors{
	char *type;
	char *title;
	char *about;
	char *tags;
};

/**
 * ��ֵ������
 * */
struct number_sensors{
	struct sensors *sensors;
	char *name;
	char *symbol;
};



/**
 * �����豸
 * */
char * create_device(struct device *dev);

/**
 * �༭�豸
 * */
char * edit_device(int devid,struct device *dev);

/**
 * �����豸
 * */
char *list_devices();

/**
 * �鿴�豸
 * */
char *show_device(int devid);

/**
 * ɾ���豸
 * */
char *delete_device(int devid);

/**
 * ������ͨ������
 * */
char *create_sensors(struct sensors *sen,int devid);

/**
 * ������ֵ�ʹ�����
 * */
char *create_numbersensors(struct number_sensors *sen,int devid);

/**
 * �༭��ͨ������
 * */
char *edit_sensors(int devid,int senid,struct sensors *sen);
/**
 * �༭��ֵ������
 * */
char *edit_number_sensors(int devid,int senid,struct number_sensors *sen);

/**
 * ���д�����
 * */
char *list_sensors(int devid);

/**
 * �鿴������
 * */
char *show_sensor(int devid,int senid);

/**
 * ɾ��������
 * */
char *delete_sensors(int devid,int senid);

/**
 * �������ݵ�
 * */
char *create_datapoints(int devid,int senid,char *file);

/**
 * �༭���ݵ�
 * */
char *edit_datapoints(int devid,int senid,char *key,char *file);

/**
 * �鿴���ݵ�
 * */
char *show_datapoint(int devid,int senid,char *key);

/**
 * ɾ�����ݵ�
 * */
char *delete_datapoint(int devid,int senid,char *key);

/**
 * ����ͼƬ   In test.
 * */
//char *create_photo(int devid,int senid,char *photo);

/**
 * ��ȡͼ����Ϣ
 * */
char *get_imageinfo(int devid,int senid,char *key);

/**
 * ��ȡͼ������  In test.
 * */
//char *get_image(int devid,int senid,char *key,char *file);

#endif
