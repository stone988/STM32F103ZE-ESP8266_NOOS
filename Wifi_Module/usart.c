#include "usart.h"
#include "esp8266.h"
#include "tcp.h"
#include "mqtt.h"


#pragma import(__use_no_semihosting)             


struct __FILE 
{
	int handle;
};

FILE __stdout;       

void _sys_exit(int x) 
{ 
	x = x; 
}

//�ض���fputc����
int fputc(int ch, FILE *f)
{

	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}

	USART_SendData(USART1, (uint8_t) ch);		

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}

	return ch;
}



void _ttywrch (int ch)
{
	ch = ch;
}


struct USART_DataFram  USART1_DataFram;
struct USART_DataFram  USART2_DataFram;


void USART2_Init(u32 bound)//����2  ����ΪPA2--U2_TX  PA3--U2_RX
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	    //ʹ��ָ���˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//��ʼ��GPIOA3
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//��ʼ��GPIOA2
	
	//USART2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;      
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         
	NVIC_Init(&NVIC_InitStructure); 

	
	//USART2 ���� ����
	USART_InitStructure.USART_BaudRate = bound;										//���ô��ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//�ֳ�Ϊ8
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								//����żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); 										//����USART����
						
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);									//�����˽����ж�
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);									//�������߿����ж�

	USART_Cmd(USART2, ENABLE);            			//ʹ��USART2
}




void USART1_Init(u32 bound)//����3  ����ΪPB10--U3_TX  PB11--U3__RX
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 				//PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		//�������
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				//PB11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);					//��ʼ��GPIO

    //USART3 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//��ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;      //��Ӧ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //USART_IRQnͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure); 						//��ʼ��NVIC

    //USART3 ���� ����
    USART_InitStructure.USART_BaudRate = bound;						//������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//���ݳ���
    USART_InitStructure.USART_StopBits = USART_StopBits_1;			//ֹͣλ1
    USART_InitStructure.USART_Parity = USART_Parity_No;				//У��λ��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//Ӳ����������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //ʹ�ܴ��ڵĽ��պͷ��͹���
    USART_Init(USART1, &USART_InitStructure); 						//��ʼ������


    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);			//���ý����ж�
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);			//�������߿����ж�
	
    USART_Cmd(USART1, ENABLE);  	//ʹ��USART3    
}


void USART2_IRQHandler( void )
{
	u8 ucCh;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
		USART_ClearITPendingBit(USART2, USART_IT_RXNE); 
		ucCh = USART_ReceiveData(USART2);
		/*δ����1024���ֽڵ��ַ���*/
        if(USART2_DataFram.InfBit.FramLength < ( RX_BUF_MAX_LEN - 1 ) ) 		// -1��Ŀ�ģ����ٱ������һλ������λ����'\0'
        {
            USART2_DataFram.Data_RX_BUF[ USART2_DataFram.InfBit.FramLength ++ ]  = ucCh;   
        }
		/*�Գ���1024���ֽڵ��ַ������ݴ���*/
		
    }
    else if(USART_GetFlagStatus(USART2, USART_FLAG_IDLE) != RESET)
    {
        USART2->SR;
        USART2->DR;
        USART2_DataFram.InfBit.FramFinishFlag = 1;	
		USART2_DataFram.Data_RX_BUF[USART2_DataFram.InfBit.FramLength] = '\0';
		
		
    }
}


void USART1_IRQHandler( void )
{
	u8 ucCh;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
		USART_ClearITPendingBit(USART1, USART_IT_RXNE); 
		ucCh = USART_ReceiveData(USART1);
        if(USART1_DataFram.InfBit.FramLength < ( RX_BUF_MAX_LEN - 1 ) ) 		
        {
            USART1_DataFram.Data_RX_BUF[ USART1_DataFram.InfBit.FramLength ++ ]  = ucCh;   
        }  
    }
    else if(USART_GetFlagStatus(USART1, USART_FLAG_IDLE) != RESET)
    {
        USART1->SR;
        USART1->DR;
        USART1_DataFram.InfBit.FramFinishFlag = 1;	
		USART1_DataFram.Data_RX_BUF[USART1_DataFram.InfBit.FramLength] = '\0';		
    }
}



