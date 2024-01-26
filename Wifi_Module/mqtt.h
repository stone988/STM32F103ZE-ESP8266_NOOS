#ifndef __MQTT_H
#define __MQTT_H 			   
#include "stm32f10x.h"


/*
*以下参数需要用户自行修改才能测试用过
*/




#define User_ESP8266_SSID     "123"          	//wifi名字
#define User_ESP8266_PWD      "123"      	//wifi密码

#define User_ESP8266_client_id    	"w1"   	//MQTTclientID 用于标志client身份  最长256字节
#define User_ESP8266_username     	"ww"										//用于登录 MQTT 服务器 的 username, 最长 64 字节	
#define User_ESP8266_password		"1234"          			//用于登录 MQTT 服务器 的 password, 最长 64 字节
#define User_ESP8266_MQTTServer_IP    "10.0.0.10" 		//MQTT本地服务器IP或者域名
#define User_ESP8266_MQTTServer_PORT   1883   //服务器端口号

//自定义Topic
#define User_ESP8266_MQTTServer_Topic  						"/8266/user/test"  	

//属性上报
#define User_ESP8266_MQTTServer_Topic_Property_Post			"/8266/event/property/post"
#define User_ESP8266_MQTTServer_Topic_Property_Post_Reply	"/8266/event/property/post_reply"


extern volatile uint8_t MQTTClosedFlag;  	//连接状态标志
extern volatile uint8_t MQTTReceiveFlag; 	//接收数据标志

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
