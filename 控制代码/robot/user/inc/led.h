#ifndef _LED_H
#define _LED_H


//函数声明和宏定义
#include "stm32f10x.h"                  // Device header



#define LED1_ON GPIO_ResetBits(GPIOA,GPIO_Pin_8); //低电平点亮LED1
#define LED1_OFF GPIO_SetBits(GPIOA,GPIO_Pin_8);  //高电平熄灭LED1




void led_init(void);




#endif