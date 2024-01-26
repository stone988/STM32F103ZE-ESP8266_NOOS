#ifndef __MQTT_H
#define __MQTT_H 			   
#include "stm32f10x.h"


/*
*���²�����Ҫ�û������޸Ĳ��ܲ����ù�
*/




#define User_ESP8266_SSID     "123"          	//wifi����
#define User_ESP8266_PWD      "123"      	//wifi����

#define User_ESP8266_client_id    	"w1"   	//MQTTclientID ���ڱ�־client���  �256�ֽ�
#define User_ESP8266_username     	"ww"										//���ڵ�¼ MQTT ������ �� username, � 64 �ֽ�	
#define User_ESP8266_password		"1234"          			//���ڵ�¼ MQTT ������ �� password, � 64 �ֽ�
#define User_ESP8266_MQTTServer_IP    "10.0.0.10" 		//MQTT���ط�����IP��������
#define User_ESP8266_MQTTServer_PORT   1883   //�������˿ں�

//�Զ���Topic
#define User_ESP8266_MQTTServer_Topic  						"/8266/user/test"  	

//�����ϱ�
#define User_ESP8266_MQTTServer_Topic_Property_Post			"/8266/event/property/post"
#define User_ESP8266_MQTTServer_Topic_Property_Post_Reply	"/8266/event/property/post_reply"


extern volatile uint8_t MQTTClosedFlag;  	//����״̬��־
extern volatile uint8_t MQTTReceiveFlag; 	//�������ݱ�־

void addBackslashBeforeQuotes(char *input);

typedef struct message
{
	uint8_t temperature; 
	uint8_t humidity;   
	uint8_t light;

} MQTT_MSG;

typedef struct control
{
	uint8_t light; 
	uint8_t fan;
		
} MQTT_CTRL;

void ESP8266_STA_MQTTClient_Test(void);
u8 parse_json_msg(char* str , MQTT_MSG* msg);
void creat_json_msg(char* str,MQTT_MSG* msg);
char* getMqttMsg(char* input);
void addBackslashBeforeChar(char* input);
#endif
