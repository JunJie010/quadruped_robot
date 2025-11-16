#ifndef __VOLTAGE__H__
#define __VOLTAGE__H__

//函数声明和宏定义
#include "stm32f10x.h"                  // Device header


void Adc_Voltage_Config(void);
u16 Adc1_GetValue(u8 channel);
void ADC_Get(void);


#endif
