#ifndef _USART_H
#define _USART_H


//函数声明和宏定义
#include "stm32f10x.h"                  // Device header


typedef struct{

	u8 data[256];
	u16 len;
	u8 flag;

}USART1STRUCT;


void usart1_init(u32 bps);
void usart1_send_byte(u8 data);

#endif
