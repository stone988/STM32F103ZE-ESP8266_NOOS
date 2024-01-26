#include "tcp.h"
#include "usart.h"
#include "esp8266.h"
#include "delay.h"
#include "stdio.h"
#include "string.h"
#include "stm32f10x.h"

extern struct USART_DataFram  USART2_DataFram;
volatile u8 TcpClosedFlag = 0;
volatile u8 TcpReceiveFlag = 0;

void ESP8266_STA_TCPClient_Test(void)
{

    u8 res = 0;
	u8 cnt = 0;
    char str[128]={0};
	printf("*************  ESP8266 STA TCP Client Test  *************\r\n");
    ESP8266_AT_Test();				//恢复出厂设置
//	ESP8266_Enable_ATE(DISABLE);	//关闭指令回显
	
	printf("开始配置ESP8266 ...\r\n");
	
    ESP8266_Net_Mode_Choose(STA);																				//配置工作模式
    while(!ESP8266_JoinAP(User_ESP8266_SSID, User_ESP8266_PWD));												//配置热点信息
    ESP8266_Enable_MultipleId ( DISABLE );																		//配置单连接模式
    while(!ESP8266_Link_Server(enumTCP, User_ESP8266_TCPServer_IP, User_ESP8266_TCPServer_PORT, Single_ID_0));	//配置服务器信息
	
	printf("ESP8266配置完成 ... \r\n");
	
	USART2_DataFram .InfAll = 0;			//清空ESP8266_Fram_Record_Struct接收标志，准备开始接收来自服务器的消息
	
    while ( 1 )
    {
		//每16s发送一次消息
		if(cnt % 16 == 0)
		{
			memset(str,sizeof(str),0);
			sprintf (str,"HELLO , I AM STM32.\r\n" );//格式化发送字符串到TCP服务器		
			ESP8266_SendString ( DISABLE, str, strlen(str) , Single_ID_0 );
			USART2_DataFram .InfAll = 0;
		}
		//非透传模式下，判断是否断开TCP连接。能够检测被服务器踢出去的情况，检测不出服务器关闭服务的情况
        if(TcpClosedFlag) 		
        {
            do
            {
                res = ESP8266_Get_LinkStatus();     //获取连接状态
            }while(!res);

            if(res == 4)                     		//确认失去连接，尝试重新连接
            {               
				printf("失去连接，正在重连 ...\r\n");
                while (!ESP8266_JoinAP(User_ESP8266_SSID, User_ESP8266_PWD ) );
                while (!ESP8266_Link_Server(enumTCP, User_ESP8266_TCPServer_IP, User_ESP8266_TCPServer_PORT, Single_ID_0 ) );        
            } 
			TcpClosedFlag = 0;	//清空TCP断开连接标志
        }
		
	
		
		if(USART2_DataFram.InfBit.FramFinishFlag == 1)		//判断是否接收到TCP网络数据
		{
			TcpReceiveFlag = strstr ( USART2_DataFram .Data_RX_BUF, "+IPD" )? 1 : 0;
			if(TcpReceiveFlag == 1){

				printf("Message From ESP8266 : %s\r\n",USART2_DataFram .Data_RX_BUF);
				printf("Length Of Message    : %d\r\n",USART2_DataFram.InfBit.FramLength);
			
				USART2_DataFram.InfAll = 0;		//清接收标志，准备接收下一条消息
				TcpReceiveFlag = 0;
			}

		}
		cnt++;
		Delay_ms(1000);
		
    }   
}



