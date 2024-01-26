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
    ESP8266_AT_Test();				//�ָ���������
//	ESP8266_Enable_ATE(DISABLE);	//�ر�ָ�����
	
	printf("��ʼ����ESP8266 ...\r\n");
	
    ESP8266_Net_Mode_Choose(STA);																				//���ù���ģʽ
    while(!ESP8266_JoinAP(User_ESP8266_SSID, User_ESP8266_PWD));												//�����ȵ���Ϣ
    ESP8266_Enable_MultipleId ( DISABLE );																		//���õ�����ģʽ
    while(!ESP8266_Link_Server(enumTCP, User_ESP8266_TCPServer_IP, User_ESP8266_TCPServer_PORT, Single_ID_0));	//���÷�������Ϣ
	
	printf("ESP8266������� ... \r\n");
	
	USART2_DataFram .InfAll = 0;			//���ESP8266_Fram_Record_Struct���ձ�־��׼����ʼ�������Է���������Ϣ
	
    while ( 1 )
    {
		//ÿ16s����һ����Ϣ
		if(cnt % 16 == 0)
		{
			memset(str,sizeof(str),0);
			sprintf (str,"HELLO , I AM STM32.\r\n" );//��ʽ�������ַ�����TCP������		
			ESP8266_SendString ( DISABLE, str, strlen(str) , Single_ID_0 );
			USART2_DataFram .InfAll = 0;
		}
		//��͸��ģʽ�£��ж��Ƿ�Ͽ�TCP���ӡ��ܹ���ⱻ�������߳�ȥ���������ⲻ���������رշ�������
        if(TcpClosedFlag) 		
        {
            do
            {
                res = ESP8266_Get_LinkStatus();     //��ȡ����״̬
            }while(!res);

            if(res == 4)                     		//ȷ��ʧȥ���ӣ�������������
            {               
				printf("ʧȥ���ӣ��������� ...\r\n");
                while (!ESP8266_JoinAP(User_ESP8266_SSID, User_ESP8266_PWD ) );
                while (!ESP8266_Link_Server(enumTCP, User_ESP8266_TCPServer_IP, User_ESP8266_TCPServer_PORT, Single_ID_0 ) );        
            } 
			TcpClosedFlag = 0;	//���TCP�Ͽ����ӱ�־
        }
		
	
		
		if(USART2_DataFram.InfBit.FramFinishFlag == 1)		//�ж��Ƿ���յ�TCP��������
		{
			TcpReceiveFlag = strstr ( USART2_DataFram .Data_RX_BUF, "+IPD" )? 1 : 0;
			if(TcpReceiveFlag == 1){

				printf("Message From ESP8266 : %s\r\n",USART2_DataFram .Data_RX_BUF);
				printf("Length Of Message    : %d\r\n",USART2_DataFram.InfBit.FramLength);
			
				USART2_DataFram.InfAll = 0;		//����ձ�־��׼��������һ����Ϣ
				TcpReceiveFlag = 0;
			}

		}
		cnt++;
		Delay_ms(1000);
		
    }   
}



