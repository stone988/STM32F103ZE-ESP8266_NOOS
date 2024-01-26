#ifndef __TCP_H
#define __TCP_H 			   
#include "stm32f10x.h"


/*
*以下参数需要用户自行修改才能测试用过
*/


#define User_ESP8266_SSID     "123"          	//wifi名字
#define User_ESP8266_PWD      "123"      	//wifi密码

#define User_ESP8266_TCPServer_IP     "10.0.0.9"     //服务器IP或者域名
#define User_ESP8266_TCPServer_PORT   "8888"      		//服务器端口号


extern volatile uint8_t TcpClosedFlag;  	//连接状态标志
extern volatile uint8_t TcpReceiveFlag; 	//接收数据标志

void ESP8266_STA_TCPClient_Test(void);


#endif
