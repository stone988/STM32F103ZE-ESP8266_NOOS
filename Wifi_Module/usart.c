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

//重定义fputc函数
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


void USART2_Init(u32 bound)//串口2  引脚为PA2--U2_TX  PA3--U2_RX
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	    //使能指定端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//初始化GPIOA3
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//初始化GPIOA2
	
	//USART2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;      
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         
	NVIC_Init(&NVIC_InitStructure); 

	
	//USART2 参数 配置
	USART_InitStructure.USART_BaudRate = bound;										//设置串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//字长为8
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无流控
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式
	USART_Init(USART2, &USART_InitStructure); 										//配置USART参数
						
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);									//配置了接收中断
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);									//配置总线空闲中断

	USART_Cmd(USART2, ENABLE);            			//使能USART2
}




void USART1_Init(u32 bound)//串口3  引脚为PB10--U3_TX  PB11--U3__RX
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 				//PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		//推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				//PB11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);					//初始化GPIO

    //USART3 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;      //响应优先级0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //USART_IRQn通道使能
    NVIC_Init(&NVIC_InitStructure); 						//初始化NVIC

    //USART3 参数 配置
    USART_InitStructure.USART_BaudRate = bound;						//波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//数据长度
    USART_InitStructure.USART_StopBits = USART_StopBits_1;			//停止位1
    USART_InitStructure.USART_Parity = USART_Parity_No;				//校验位无
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//硬件流控制无
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //使能串口的接收和发送功能
    USART_Init(USART1, &USART_InitStructure); 						//初始化串口


    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);			//配置接收中断
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);			//配置总线空闲中断
	
    USART_Cmd(USART1, ENABLE);  	//使能USART3    
}


void USART2_IRQHandler( void )
{
	u8 ucCh;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
		USART_ClearITPendingBit(USART2, USART_IT_RXNE); 
		ucCh = USART_ReceiveData(USART2);
		/*未超出1024个字节的字符串*/
        if(USART2_DataFram.InfBit.FramLength < ( RX_BUF_MAX_LEN - 1 ) ) 		// -1的目的：至少保留最后一位做结束位，补'\0'
        {
            USART2_DataFram.Data_RX_BUF[ USART2_DataFram.InfBit.FramLength ++ ]  = ucCh;   
        }
		/*对超出1024个字节的字符串做容错处理*/
		
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



