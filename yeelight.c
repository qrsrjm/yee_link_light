#include "yeelight.h"

/**
����ָ���YEEBOX
**/
static char* send_order_to_yeebox(const char* order);

/**
���Ϳ���ָ���YEEBOX id:�����
**/
static char* send_open_to_yeebox(int id);

/**
���͹ص�ָ���YEEBOX �����
**/
static char* send_close_to_yeebox(int id);

extern int yeebox_port;
extern char yeebox_ip[];
struct switch_info* get_switch_info(int devid,int senid){
	char* web_result=show_datapoint(devid,senid,NULL);
	char* c_timestamp;
	struct switch_info *sw_in=NULL;
	
	if(NULL==web_result){
		return NULL;
	}
	char* p=strstr(web_result,"timestamp");
	if(p!=NULL){
		c_timestamp=(char*)calloc(16,sizeof(char));
		p+=12;
		strncpy(c_timestamp,p,19);	//��ȡʱ���
		sw_in=(struct switch_info*)malloc(sizeof(struct switch_info));
		sw_in->timestamp=c_timestamp;
		sw_in->state=*(p+29)-'0';	//����״̬
	}
	free(web_result);
	return sw_in;
}

char* change_switch_state(int devid,int senid,int state){
	char* json_str=(char*)malloc(sizeof(char)*16);
	int stat;
	if(OPENED==state){
		stat=1;
	}
	else{
		stat=0;
	}
	sprintf(json_str,"{\"value\": %d}",stat);	//�������״̬�ı俪��״̬
	char *web_result=edit_datapoints(devid,senid," ",json_str);
	free(json_str);
	//free(web_result);
	return web_result;
}

static char* send_order_to_yeebox(const char* order){
	char* result=(char*)malloc(sizeof(char)*64);
	memset(result,0,64);
	struct hostent *host=NULL;
	int sockfd;
	struct sockaddr_in sockaddr;
	
	host=gethostbyname(yeebox_ip);
	if(NULL==host){
		return NULL;
	}
	
	if(-1==(sockfd=socket(AF_INET,SOCK_STREAM,0))){
		perror("socket");
		free(result);
		return NULL;
	}
	
	char buff[30];
	sockaddr.sin_family=AF_INET;
	sockaddr.sin_port=htons(yeebox_port);
	sockaddr.sin_addr.s_addr=inet_addr(inet_ntop(host->h_addrtype,host->h_addr,buff,sizeof(buff)));
	memset(sockaddr.sin_zero,0,sizeof(sockaddr.sin_zero));
	
	if(-1==(connect(sockfd,(struct sockaddr*)(&sockaddr),sizeof(struct sockaddr)))){
		perror("connect");
		free(result);
		return NULL;
	}
	
	if(-1==(send(sockfd,order,strlen(order),0))){
		perror("send");
		free(result);
		return NULL;
	}
	
	//���ó�ʱ
	struct timeval timeout = {0,50000};
	//���ý��ճ�ʱ
	setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(struct timeval));
	if(-1==(recv(sockfd,result,64,MSG_WAITALL))){
		perror("receive");
		free(result);
		return NULL;
	}
	
	close(sockfd);
	return result;
}

static char* send_open_to_yeebox(int id){
	char* order=(char*)malloc(sizeof(char)*64);
	char* result;
	sprintf(order,"C %04d,255,255,255,100,,\n",id);
	result=send_order_to_yeebox(order);
	free(order);
	return result;
}

static char* send_close_to_yeebox(int id){
	char* order=(char*)malloc(sizeof(char)*64);
	char* result;
	sprintf(order,"C %04d,,,,0,,\n",id);
	result=send_order_to_yeebox(order);
	free(order);
	return result;
}

void change_light_state(struct switch_info *s_info,int light){
	if(s_info->state==CLOSED){	//�ص�
				free(send_close_to_yeebox(light));
			}else{//����
				free(send_open_to_yeebox(light));
			}
}

//qmd test led state
int led_stat;
void send_close(int light){
	led_stat = CLOSED;
	printf("close led. *^*\n");
}
void send_open(int light){
	led_stat = OPENED;
	printf("open led. *~*\n");
}
void change_light_state_1(struct switch_info *s_info,int light){
	if(s_info->state==CLOSED){	//�ص�
		send_close(light);
	}else{//����
		send_open(light);
	}
}

int check_light_state_1(int id){
	return led_stat;
}


int check_light_state(int id){
	char* order=(char*)malloc(sizeof(char)*64);
	char* result;
	char* p=NULL;
	char* c_id=(char*)malloc(sizeof(char)*8);
	int state;

	sprintf(order,"GL\n");
	result=send_order_to_yeebox(order);
	
	sprintf(c_id,"%04d",id);//	idת�����,1-->0001

	p=strstr(result,c_id);

	if(p==NULL){
		free(order);
		free(result);
		free(c_id);
		return ERROR;
	}
	
	int i=0;
	while(p++){
		if(*p!=','){
			continue;
		}
		else{
			i++;
		}
		if(2==i){
			break;
		}
	}
	p++;	//pָ��״̬λ��0��ʾ���ߣ�1��ʾ����
	if('0'==*p){
		//state=OFFLINE;
		state=CLOSED;
	}else{
		while(p++){
			if(*p!=','){
			continue;
			}
			else{
				i++;
			}
			if(7==i){
				break;
			}
		}
		p++;	//pָ������λ
		if('0'==*p){	//�򵥵�ͨ�������жϵƿ���
			state=CLOSED;
		}else{
			state=OPENED;
		}
	}
	free(order);
	free(result);
	free(c_id);
	return state;
}
