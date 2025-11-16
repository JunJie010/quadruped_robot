#include "main.h"

/***************************************************
*函数名    ：tim2_init
*函数功能  ：定时器2初始化函数
*函数参数  ：无
*函数返回值：无
*函数描述  ：	时间 = 计数周期/计数频率
	            时间(ms) = 计数周期 /10000  
							计数周期 = 10----10个数1ms 10000--1s
							计数频率= 时钟周期 /分频系数
							72000000/7200
****************************************************/
void tim2_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//开启TIM2时钟

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct; //定义结构体变量
	NVIC_InitTypeDef NVIC_InitStruct = {0}; 
	
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInitStruct.TIM_Period = 1000;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 71;  //分频值---
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //使能更新中断
	
	//NVIC控制器----内核---优先级：人为：抢占 响应  自然
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;        //选择指定的TIM2中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;          //在NVIC_IRQChannel中定义的中断通道被使能还是失能。
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;//设置抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;       //设置响应优先级      
	NVIC_Init(&NVIC_InitStruct);  
	
	
	TIM_Cmd(TIM2,ENABLE); //使能定时器
	TIM_ClearFlag(TIM2,TIM_FLAG_Update); //清更新标志
}



/***************************************************
*函数名    ：TIM2_IRQHandler
*函数功能  ：定时器2中断服务函数
*函数参数  ：无
*函数返回值：无
*函数描述  ：	
****************************************************/
uint32_t OStime = 0;
void TIM2_IRQHandler(void)
{
	if(TIM_GetFlagStatus(TIM2,TIM_FLAG_Update)==1)//获取定时器2是否为更新中断
	{
		
		TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);  //清除中断标志位，必须手动清除
		OStime++;
		if((OStime % 10 == 0))
		{
			armflag.arm_time10++;
		}
		if(OStime % 20 == 0)
		{
			armflag.arm_time20++;
		}
		if(OStime % 5 == 0)
		{
			armflag.arm_time5++;
		}
	}
}




