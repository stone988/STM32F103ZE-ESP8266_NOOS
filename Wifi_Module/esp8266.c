#include "esp8266.h"
#include "usart.h"
#include "Delay.h"
#include <stdarg.h>


extern struct USART_DataFram  USART2_DataFram;

void ESP8266_Init(u32 bound)
{
    USART2_Init(bound); 
}


//��ESP8266ģ�鷢��ATָ��
// cmd 		�������͵�ָ��
// ack1,ack2���ڴ�����Ӧ��ΪNULL������Ӧ������Ϊ���߼���ϵ
// time 	���ȴ���Ӧʱ��
//����1���ͳɹ��� 0ʧ��
bool ESP8266_Send_AT_Cmd(char *cmd,char *ack1,char *ack2,u32 time)
{
		
    USART2_DataFram.InfAll = 0; //���½����µ����ݰ�
    ESP8266_USART("%s\r\n", cmd);

    if(ack1==0 && ack2==0)     //����Ҫ��������
    {
		return true;
    }
	
	Delay_ms(time);   //��ʱ
	Delay_ms(1000);

	while(USART2_DataFram.InfBit.FramFinishFlag != 1);
	
	USART2_DataFram.InfAll = 0;
	printf("ESP8266_Send_AT_Cmd Receive Message :\r\n");		
	printf("%s\r\n",USART2_DataFram .Data_RX_BUF);


	
    if(ack1!=0 && ack2!=0)
    {
        return ( ( bool ) strstr ( USART2_DataFram .Data_RX_BUF, ack1 ) || 
                         ( bool ) strstr ( USART2_DataFram .Data_RX_BUF, ack2 ) );
    }
    else if( ack1 != 0 )  
        return ( ( bool ) strstr ( USART2_DataFram .Data_RX_BUF, ack1 ) );

    else
        return ( ( bool ) strstr ( USART2_DataFram .Data_RX_BUF, ack2 ) );

}




//ESP8266  ָ�����
//enumEnATE  
//���óɹ�����true����֮false
bool ESP8266_Enable_ATE (FunctionalState enumEnATE )
{
    char cStr [20];

    sprintf ( cStr, "ATE%d", ( enumEnATE ? 1 : 0 ) );

    return ESP8266_Send_AT_Cmd ( cStr, "OK", 0, 500 );

}

//���ͻָ�����Ĭ������ָ�ģ��ָ��ɳ�������
void ESP8266_AT_Test(void)
{
    char count=0;
    Delay_ms(1000); 
    while(count < 10)
    {
		printf("AT+RESTORE start to try %d times ...\r\n",count+1);
		
        if(ESP8266_Send_AT_Cmd("AT+RESTORE","OK",NULL,500)) 
        {
			printf("AT+RESTORE succeeded ...\r\n");
            return;
        }
        ++ count;
		
    }
	printf("AT+RESTORE failed ...\r\n");
}


//ѡ��ESP8266�Ĺ���ģʽ
// enumMode ģʽ����
//�ɹ�����true��ʧ�ܷ���false
bool ESP8266_Net_Mode_Choose(ENUM_Net_ModeTypeDef enumMode)
{
    switch ( enumMode )
    {
        case STA:
            return ESP8266_Send_AT_Cmd ( "AT+CWMODE=1", "OK", "no change", 2500 ); 

        case AP:
            return ESP8266_Send_AT_Cmd ( "AT+CWMODE=2", "OK", "no change", 2500 ); 

        case STA_AP:
            return ESP8266_Send_AT_Cmd ( "AT+CWMODE=3", "OK", "no change", 2500 ); 

        default:
          return false;
    }       
}


//ESP8266�����ⲿ��WIFI
//pSSID WiFi�ʺ�
//pPassWord WiFi����
//���óɹ�����true ��֮false
bool ESP8266_JoinAP( char * pSSID, char * pPassWord)
{
    char cCmd [120];
	
    sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );
    return ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 5000 );
}

//ESP8266 ͸��ʹ��
//enumEnUnvarnishTx  �Ƿ�����ӣ�bool����
//���óɹ�����true����֮false
bool ESP8266_Enable_MultipleId (FunctionalState enumEnUnvarnishTx )
{
    char cStr [20];

    sprintf ( cStr, "AT+CIPMUX=%d", ( enumEnUnvarnishTx ? 1 : 0 ) );

    return ESP8266_Send_AT_Cmd ( cStr, "OK", 0, 500 );

}


//ESP8266 ���ӷ�����
//enumE  ��������
//ip ��������IP
//ComNum  �������˿�
//id�����Ӻţ�ȷ��ͨ�Ų���������
//���óɹ�����true����֮fasle
bool ESP8266_Link_Server(ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
{
    char cStr [100] = { 0 }, cCmd [120];

    switch (  enumE )
    {
        case enumTCP:
          sprintf ( cStr, "\"%s\",\"%s\",%s", "TCP", ip, ComNum );
          break;

        case enumUDP:
          sprintf ( cStr, "\"%s\",\"%s\",%s", "UDP", ip, ComNum );
          break;

        default:
            break;
    }

    if ( id < 5 )    //������
        sprintf ( cCmd, "AT+CIPSTART=%d,%s", id, cStr);

    else    		//������
        sprintf ( cCmd, "AT+CIPSTART=%s", cStr );

    return ESP8266_Send_AT_Cmd ( cCmd, "OK", "ALREAY CONNECT", 4000 );

}


//͸��ʹ��
//���óɹ�����true�� ��֮false
bool ESP8266_UnvarnishSend ( void )
{
    if (!ESP8266_Send_AT_Cmd ( "AT+CIPMODE=1", "OK", 0, 500 ))		//����͸��ģʽ
        return false;

    return 
        ESP8266_Send_AT_Cmd( "AT+CIPSEND", "OK", ">", 500 );		//��ʼ��������

}


//ESP8266�����ַ���
//enumEnUnvarnishTx�Ƿ�ʹ��͸��ģʽ
//pStr�ַ���
//ulStrLength�ַ�������
//ucId ���Ӻ�
//���óɹ�����true�� ��֮false
bool ESP8266_SendString(FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId )
{
    char cStr [32];
    bool bRet = false;


    if ( enumEnUnvarnishTx )    //͸��ģʽ��ֱ�ӷ���
    {
        ESP8266_USART ( "%s", pStr );

        bRet = true;

    }

    else    //��͸��ģʽ
    {
        if ( ucId < 5 )    //������
//            sprintf ( cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength + 2 );
            sprintf ( cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength );
        else    //������
//            sprintf ( cStr, "AT+CIPSEND=%d", ulStrLength + 2 );
            sprintf ( cStr, "AT+CIPSEND=%d", ulStrLength );
        ESP8266_Send_AT_Cmd ( cStr, "> ", 0, 1000 );

        bRet = ESP8266_Send_AT_Cmd ( pStr, "SEND OK", 0, 1000 );
  }

    return bRet;

}


//ESP8266�˳�͸��ģʽ
void ESP8266_ExitUnvarnishSend ( void )
{
    Delay_ms(1000);
    ESP8266_USART( "+++" );
    Delay_ms( 500 );    
}


//ESP8266 �������״̬
//����0����ȡ״̬ʧ��
//����2�����ip
//����3���������� 
//����4��ʧȥ���� 
u8 ESP8266_Get_LinkStatus ( void )
{
    if (ESP8266_Send_AT_Cmd( "AT+CIPSTATUS", "OK", 0, 500 ) )
    {
        if ( strstr ( USART2_DataFram.Data_RX_BUF, "STATUS:2\r\n" ) )
            return 2;

        else if ( strstr ( USART2_DataFram .Data_RX_BUF, "STATUS:3\r\n" ) )
            return 3;

        else if ( strstr ( USART2_DataFram .Data_RX_BUF, "STATUS:4\r\n" ) )
            return 4;       

    }

    return 0;
}


static char *itoa( int value, char *string, int radix )
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers.(ʮ����) */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value(����) insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;

    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */


void USART_printf ( USART_TypeDef * USARTx, char * Data, ... )
{
    const char *s;
    int d;   
    char buf[16];


    va_list ap;
    va_start(ap, Data);

    while ( * Data != 0 )     // �ж������Ƿ񵽴������
    {                                         
        if ( * Data == 0x5c )  //'\'
        {                                     
            switch ( *++Data )
            {
                case 'r':                                     //�س���
                USART_SendData(USARTx, 0x0d);
                Data ++;
                break;

                case 'n':                                     //���з�
                USART_SendData(USARTx, 0x0a);   
                Data ++;
                break;

                default:
                Data ++;
                break;
            }            
        }

        else if ( * Data == '%')
        {                                     
            switch ( *++Data )
            {               
                case 's':                                         //�ַ���
                s = va_arg(ap, const char *);
                for ( ; *s; s++) 
                {
                    USART_SendData(USARTx,*s);
                    while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
                }
                Data++;
                break;

                case 'd':           
                    //ʮ����
                d = va_arg(ap, int);
                itoa(d, buf, 10);
                for (s = buf; *s; s++) 
                {
                    USART_SendData(USARTx,*s);
                    while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
                }
                     Data++;
                     break;
                default:
                     Data++;
                     break;
            }        
        }
        else USART_SendData(USARTx, *Data++);
        while ( USART_GetFlagStatus ( USARTx, USART_FLAG_TXE ) == RESET );

    }
}

//����ΪESP8266MQTT����ָ��

/*
*MQTT�����û�����
*LinkID ����ID,Ŀǰֻ֧��0
*scheme ���ӷ�ʽ������ѡ��MQTT over TCP,��������Ϊ1
*client_id MQTTclientID ���ڱ�־client���
*username ���ڵ�¼ MQTT ������ �� username
*password ���ڵ�¼ MQTT ������ �� password
*cert_key_ID ֤�� ID, Ŀǰ֧��һ�� cert ֤��, ����Ϊ 0
*CA_ID Ŀǰ֧��һ�� CA ֤��, ����Ϊ 0
*path ��Դ·������������Ϊ""
*���óɹ�����true ��֮false
*/
bool ESP8266_MQTTUSERCFG( char * pClient_Id, char * pUserName,char * PassWord)
{
//    char cCmd [120];
	char cCmd[300];
    sprintf ( cCmd, "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"", pClient_Id,pUserName,PassWord );
    return ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 500 );
}


/*
*����ָ����MQTT������
*LinkID ����ID,Ŀǰֻ֧��0
*IP��MQTT�������϶�Ӧ��IP��ַ
*ComNum MQTT�������϶�Ӧ�Ķ˿ںţ�һ��Ϊ1883
*���óɹ�����true ��֮false
*/
bool ESP8266_MQTTCONN( char * Ip, int  Num)
{
    char cCmd [120];
    sprintf ( cCmd,"AT+MQTTCONN=0,\"%s\",%d,0", Ip,Num);
    return ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 500 );
}

/*
*����ָ�����ӵ� MQTT ����, ���ظ���ζ��Ĳ�ͬ topic
*LinkID ����ID,Ŀǰֻ֧��0
*Topic ���ĵ��������֣���������ΪTopic
*Qosֵ��һ��Ϊ0����������Ϊ1
*���óɹ�����true ��֮false
*/
bool ESP8266_MQTTSUB(char * Topic)
{
    char cCmd [256];
    sprintf ( cCmd, "AT+MQTTSUB=0,\"%s\",1",Topic );
    return ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 500 );
}

/*
*ȡ������ָ�����ӵ� MQTT ����
*LinkID ����ID,Ŀǰֻ֧��0
*Topic ���ĵ��������֣���������ΪTopic
*Qosֵ��һ��Ϊ0����������Ϊ1
*���óɹ�����true ��֮false
*/
bool ESP8266_MQTTUNSUB(char * Topic)
{
    char cCmd [256];
    sprintf ( cCmd, "AT+MQTTUNSUB=0,\"%s\"",Topic );
    return ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 500 );
}


/*
*��LinkID��ͨ�� topic �������� data, ���� data Ϊ�ַ�����Ϣ
*LinkID ����ID,Ŀǰֻ֧��0
*Topic ���ĵ��������֣���������ΪTopic
*data���ַ�����Ϣ
*���óɹ�����true ��֮false
*/
bool ESP8266_MQTTPUB( char * Topic,char *temp)
{
    char cCmd [256];
    sprintf (cCmd, "AT+MQTTPUB=0,\"%s\",\"%s\",1,0", Topic ,temp);
    return ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 1000 );
}

/*
*�ر� MQTT Client Ϊ LinkID ������, ���ͷ��ڲ�ռ�õ���Դ
*LinkID ����ID,Ŀǰֻ֧��0
*Topic ���ĵ��������֣���������ΪTopic
*data���ַ�����Ϣ
*���óɹ�����true ��֮false
*/
bool ESP8266_MQTTCLEAN(void)
{
    char cCmd [120];
    sprintf ( cCmd, "AT+MQTTCLEAN=0");
    return ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 500 );
}

//ESP8266�����ַ���
//enumEnUnvarnishTx�Ƿ�ʹ��͸��ģʽ
//pStr�ַ���
//ulStrLength�ַ�������
//ucId ���Ӻ�
//���óɹ�����true�� ��֮false
bool MQTT_SendString(char * pTopic,char *temp2)
{
	
    bool bRet = false;
    ESP8266_MQTTPUB(pTopic,temp2);
	Delay_ms(1000);
    bRet = true;
    return bRet;

}

