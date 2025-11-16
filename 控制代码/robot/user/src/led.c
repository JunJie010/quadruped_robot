#include "main.h"



/*******************************************
函数名：led_init
函数功能：LED初始化
形参：void
返回值：void
函数说明：
LED1 --- PA8 --- 通用推挽输出
软件设计：
1、	打开时钟（GPIOA）
2、	初始化PA8
3、	LED默认灭状态 --- 低电平点亮
4、	点灯
5、延时函数
**********************************************/
void led_init(void)
{
	//开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};//定义结构体变量
	//GPIOA8配置
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 		//推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;				// 选中管脚 8
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;		// 最高输出速率 50MHz
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//默认灭
	LED1_OFF;
}





