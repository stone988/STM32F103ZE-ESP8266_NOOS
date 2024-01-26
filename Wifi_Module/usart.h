#ifndef __USART_H
#define __USART_H 			   

#include "stm32f10x.h"
#include "stdio.h"

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#define RX_BUF_MAX_LEN 1024       

struct USART_DataFram   //����֡�ṹ��
{
    char Data_RX_BUF[RX_BUF_MAX_LEN];
    union 
    {
        __IO u16 InfAll;						//������Ϣ
        struct 
        {
            __IO u16 FramLength       :15;     	// �����ֽڳ���  14:0 
            __IO u16 FramFinishFlag   :1;      	// ������ɱ�־  15 
        }InfBit;								//��λ��Ϣ
    }; 
	
};

void USART2_Init(u32 bound);
void USART1_Init(u32 bound);


#endif
