#ifndef YEEINI_H_
#define YEEINI_H_
char *load_file(const char *fileName);	//��ȡ�ļ�
int get_string(const char *buf, const char *section, const char *key,
	const char *defaultValue, char *value, unsigned long size);	/* ��ȡ�ַ��� */
int get_int(const char *buf, const char *section, const char *key, int defaultValue);	/* ��ȡ���� */

void init_config();	//��ʼ�����ò���

#endif
