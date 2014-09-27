#include "yeelight.h"
#include "yeeini.h"

#include <pthread.h>

extern int device_id;				//device id
extern int switch_id;				//������ID
extern int yeelight;				//��Ҫ���ƵĵƵ������
struct switch_info *s_info=NULL;	//�������һ�λ�ȡ���Ŀ���״̬��Ϣ
pthread_mutex_t mutex;


/**
���ڼ��yeelink�ϵ�ָ�����ص�״̬���Ӷ����жԵƵĲ��ݡ�
**/
void yeelink_ser(){
	while(1){
		pthread_mutex_lock(&mutex);
		if(NULL==s_info){	//�������һ�μ��
			s_info=get_switch_info(device_id,switch_id);
			if(NULL==s_info){
				pthread_mutex_unlock(&mutex);
				continue;
			}else{
				change_light_state_1(s_info,yeelight);   //change_light_state(s_info,yeelight);
			}

		}
		else{	
			struct switch_info *s_i=get_switch_info(device_id,switch_id);	//��ȡ���µĿ�����Ϣ
			if(NULL==s_i){
				pthread_mutex_unlock(&mutex);
				continue;
			}else if(strcmp(s_i->timestamp,s_info->timestamp)!=0){	//����״̬�����仯
				change_light_state_1(s_i,yeelight);                 //change_light_state(s_i,yeelight);
			}
			
			free(s_info->timestamp);
			free(s_info);
			s_info=s_i;
			
		}
		pthread_mutex_unlock(&mutex);
		usleep(800000); 
	}
};

/*
���Ƶ�״̬�����Ƶ�״̬ӳ�䵽yeelink�Ŀ��ش�����
*/
void yeelight_ser(){
	int state;
	char* r;
	struct switch_info *s_i;
	
	while(1){
		sleep(1);
		pthread_mutex_lock(&mutex);
		state=check_light_state_1(yeelight);	//���Ƶ�״̬   check_light_state(yeelight);
		s_i=get_switch_info(device_id,switch_id);	//��ȡ���µĿ�����Ϣ
		
		/*
			���Ƶ�״̬�͵�ǰyeelink�Ŀ���״̬һ�µ�ʱ���޸�yeelink�ϵĿ��أ�
			Ŀ���Ǿ�������״̬��ͬ����������ֹ��ͻ��
		*/
		if(s_i==NULL||
			state==ERROR||
			s_i->state==state||
			strcmp(s_i->timestamp,s_info->timestamp))	//������һ��ִ�л�ͬ������ʱ����Ϳ��ص�ǰ��ʱ�����ͬ
		{
			if(NULL!=s_i){
				free(s_i->timestamp);
				free(s_i);
			}
			pthread_mutex_unlock(&mutex);
			continue;
		}
		
		r=change_switch_state(device_id,switch_id,state);	//ͬ��״̬
		free(r);
		free(s_i->timestamp);
		free(s_i);

		s_i=get_switch_info(device_id,switch_id);	//�ٴλ�ȡ����״̬
		if(NULL!=s_i){
			s_info=s_i;
		}

		pthread_mutex_unlock(&mutex);
	}
};


int main(int argc,char *argv[])
{
	init_config();
	
	pthread_mutex_init (&mutex,NULL);
	
	pthread_t t_yeelink_ser,t_yeelight_ser;
	
	pthread_create(&t_yeelink_ser,NULL,(void*)yeelink_ser,NULL);
	
	pthread_create(&t_yeelight_ser,NULL,(void*)yeelight_ser,NULL);
	
	for(;;);
	
	return 0;
}
