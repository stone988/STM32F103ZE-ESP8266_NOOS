#ifndef __TCP_H
#define __TCP_H 			   
#include "stm32f10x.h"


/*
*���²�����Ҫ�û������޸Ĳ��ܲ����ù�
*/


#define User_ESP8266_SSID     "123"          	//wifi����
#define User_ESP8266_PWD      "123"      	//wifi����

#define User_ESP8266_TCPServer_IP     "10.0.0.9"     //������IP��������
#define User_ESP8266_TCPServer_PORT   "8888"      		//�������˿ں�


extern volatile uint8_t TcpClosedFlag;  	//����״̬��־
extern volatile uint8_t TcpReceiveFlag; 	//�������ݱ�־

void ESP8266_STA_TCPClient_Test(void);


#endif
