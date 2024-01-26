#include "stm32f10x.h"
#include "esp8266.h"
#include "tcp.h"
#include "usart.h"
#include "delay.h"
#include  "mqtt.h"

int main()
{
	
	USART2_Init(115200);
	USART1_Init(115200);
	
	while(1)
	{
		
		//ESP8266_STA_TCPClient_Test();
	     ESP8266_STA_MQTTClient_Test();
		
	}
	
	
	
	
}