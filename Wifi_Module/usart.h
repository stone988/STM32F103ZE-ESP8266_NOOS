#ifndef __USART_H
#define __USART_H 			   

#include "stm32f10x.h"
#include "stdio.h"

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#define RX_BUF_MAX_LEN 1024       

struct USART_DataFram   //数据帧结构体
{
    char Data_RX_BUF[RX_BUF_MAX_LEN];
    union 
    {
        __IO u16 InfAll;						//所有信息
        struct 
        {
            __IO u16 FramLength       :15;     	// 接收字节长度  14:0 
            __IO u16 FramFinishFlag   :1;      	// 接收完成标志  15 
        }InfBit;								//按位信息
    }; 
	
};

void USART2_Init(u32 bound);
void USART1_Init(u32 bound);


#endif
