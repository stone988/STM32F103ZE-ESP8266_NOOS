#include "mqtt.h"
#include "esp8266.h"
#include "delay.h"
#include <stdlib.h>
#include "cJSON.h"
#include "usart.h"

volatile u8 MQTTClosedFlag = 0;
volatile u8 MQTTReceiveFlag = 0;

extern struct USART_DataFram  USART2_DataFram;

void creat_json_msg(char* str,MQTT_MSG* msg)
{
	cJSON* cjson_test = NULL;

    char* temp;

    cjson_test = cJSON_CreateObject();

   
    //cJSON_AddStringToObject(cjson_test, "woniu", "hello");
	cJSON_AddNumberToObject(cjson_test,"temp",msg->temperature);
	cJSON_AddNumberToObject(cjson_test,"hum",msg->humidity);
	cJSON_AddNumberToObject(cjson_test,"light",msg->light);	
	
	
    temp = cJSON_Print(cjson_test);
	
	strcpy(str,temp);
	
	cJSON_Delete(cjson_test);

}

u8 parse_json_msg(char* str , MQTT_MSG* msg)
{
	
	cJSON* cjson_test = NULL;
    cJSON* cjson_temp = NULL;
    cJSON* cjson_hum = NULL;
    cJSON* cjson_light = NULL;

	

    cjson_test = cJSON_Parse(str);
    if(cjson_test == NULL)
    {
        printf("parse fail.\n");
		
        return -1;
    }
	
	cjson_temp = cJSON_GetObjectItem(cjson_test, "temp");
    cjson_hum  =   cJSON_GetObjectItem(cjson_test, "hum");
    cjson_light = cJSON_GetObjectItem(cjson_test, "light");

    msg->temperature =     cjson_temp->valueint;
    msg->humidity   = cjson_hum->valueint;
    msg->light  =    cjson_light->valueint;
		
    cJSON_Delete(cjson_test);
	return 0;

}

char* getMqttMsg(char* input)
{
	char* sb = strchr(input,'{');
	if(sb != NULL){
	
			return sb;
		}
	
	return NULL;
	
		
}

void addBackslashBeforeQuotes(char *input) 
{
	
    int len = strlen(input);
    char *result = (char *)malloc(2 * len + 1); // 为了确保有足够的空间存储新字符串


    int j = 0; // 用于追踪新字符串的索引

    for (int i = 0; i < len; ++i) 
	{
        if (input[i] == '"' || input[i] == ',' ) 
		{
            result[j++] = '\\'; 
			result[j++] = input[i];
			
        }
		else if(input[i] == '\t' )
		{
			
		}else if (input[i] == '\n')
		{
			
		}else result[j++] = input[i];
		
    }

    result[j] = '\0'; // 为新字符串添加结束符

    strcpy (input,result);

    free(result); // 释放动态分配的内存
}

void addBackslashBeforeChar(char* input)
{
	int len = strlen(input);
	char *result = (char*)malloc(2*len+1);
	
	int j = 0;
	
    for (int i = 0; i < len; ++i) 
	{	
        if (input[i] == '"' || input[i] == ',' ) 
		{
            result[j++] = '\\'; 
			
			
        }
		result[j++] = input[i];
	
	}

    result[j] = '\0'; // 为新字符串添加结束符

    strcpy (input,result);

    free(result); // 释放动态分配的内存	
	
	
}

void ESP8266_STA_MQTTClient_Connect(void)
{
	printf("正在配置 ESP8266 MQTT ...\r\n");
	
    ESP8266_AT_Test();						//恢复出厂默认
    ESP8266_Net_Mode_Choose(STA);			//配置模式
    while(!ESP8266_JoinAP(User_ESP8266_SSID, User_ESP8266_PWD));//配置热点信息
	ESP8266_MQTTUSERCFG(User_ESP8266_client_id,User_ESP8266_username,User_ESP8266_password);
	ESP8266_MQTTCONN( User_ESP8266_MQTTServer_IP, User_ESP8266_MQTTServer_PORT);
	


	ESP8266_MQTTSUB(User_ESP8266_MQTTServer_Topic_Property_Post);	
	
	printf("ESP8266 MQTT 配置完成 ...\r\n");
	
	USART2_DataFram.InfAll = 0;		
	
	
}



void ESP8266_STA_MQTTClient_Test(void)
{

	char str[128] = {0};
	u8 cnt = 0;
	int flat = 1;
	printf("正在配置 ESP8266 MQTT ...\r\n");
	
    ESP8266_AT_Test();						//恢复出厂默认
    ESP8266_Net_Mode_Choose(STA);			//配置模式
    while(!ESP8266_JoinAP(User_ESP8266_SSID, User_ESP8266_PWD));//配置热点信息
	ESP8266_MQTTUSERCFG(User_ESP8266_client_id,User_ESP8266_username,User_ESP8266_password);
	ESP8266_MQTTCONN( User_ESP8266_MQTTServer_IP, User_ESP8266_MQTTServer_PORT);
	
	ESP8266_MQTTSUB( User_ESP8266_MQTTServer_Topic_Property_Post_Reply);

	ESP8266_MQTTSUB(User_ESP8266_MQTTServer_Topic_Property_Post);
	
	
	printf("ESP8266 MQTT 配置完成 ...\r\n");
	
	USART2_DataFram.InfAll = 0;	
	
	
	
	while(1)
	{
		//每32s发送一次
		if(cnt % 32 == 0)
		{
			memset(str, 0,sizeof(str));
			sprintf(str,"I AM ESP8266");//格式化发送字符串到MQTT服务器
			MQTT_SendString (User_ESP8266_MQTTServer_Topic,str);//发送数据到MQTT服务器
			
			printf("mqtt_message %s",str);
			MQTT_SendString (User_ESP8266_MQTTServer_Topic_Property_Post,str);//发送数据到MQTT服务器			
				
		}

		
		if(USART2_DataFram.InfBit.FramFinishFlag == 1)		//判断是否接收到TCP网络数据
		{
				
			MQTTReceiveFlag = strstr(USART2_DataFram .Data_RX_BUF, "+MQTTSUBRECV" ) ? 1 : 0;
			if(MQTTReceiveFlag == 1)
			{
				printf("Message From ESP8266 : %s\r\n",USART2_DataFram .Data_RX_BUF);
				printf("Length Of Message    : %d\r\n",USART2_DataFram.InfBit.FramLength);			
				USART2_DataFram.InfAll = 0;		//清空ESP8266_Fram_Record_Struct接收标志，准备接收下一条消息
				MQTTReceiveFlag = 0;
			}
		}
				
		Delay_ms(1000);
		cnt++;
		
	}
}



