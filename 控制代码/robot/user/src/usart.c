#include "main.h"


/***************************************************
*函数名    ：usart1_init
*函数功能  ：USART1初始化
*函数参数  ：u32 bps  波特率
*函数返回值：无
*函数描述  ：USART1_TXD --- PA9 --- 复用推挽输出
						 USART1_RXD --- PA10 --- 浮空输入模式
****************************************************/
void usart1_init(u32 bps)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//打开GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//打开USART1时钟
	
	GPIO_InitTypeDef GPIO_InitStruct={0};//定义结构体变量名
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//配置为复用推挽输出模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;//配置第9号管脚
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//速度为50Mhz
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;//配置为浮空输入模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;//配置管脚
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	USART_InitTypeDef USART_InitStruct={0};
	USART_InitStruct.USART_BaudRate = bps;//波特率
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流传输
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//使能发送器和接收器
	USART_InitStruct.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//停止位为1bit
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//数据长度为8bit
	USART_Init(USART1,&USART_InitStruct);
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//使能接收中断
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);//使能空闲中断
	//NVIC
	NVIC_InitTypeDef NVIC_InitStruct={0};
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;//配置中断源
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//使能中断源
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;//占先
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;//响应
	
	NVIC_Init(&NVIC_InitStruct);
	
	USART_Cmd(USART1,ENABLE);//使能USART1
}



/***************************************************
*函数名    ：USART1_IRQHandler
*函数功能  ：中断服务函数
*函数参数  ：无
*函数返回值：无
*函数描述  ：
****************************************************/
USART1STRUCT usart1_rec={0};
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE))
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		
		usart1_rec.data[usart1_rec.len++] = USART_ReceiveData(USART1);
		
	}
	if(USART_GetITStatus(USART1,USART_IT_IDLE))
	{
		USART1->DR;
		USART1->SR;
		
		usart1_rec.data[usart1_rec.len] = '\0';
		usart1_rec.len = 0;
		usart1_rec.flag = 1;
		
		printf("%s",usart1_rec.data);
	}
}



/***************************************************
*函数名    ：usart1_send_byte
*函数功能  ：USART1发送1字节函数
*函数参数  ：u8 data
*函数返回值：无
*函数描述  ：USART1_TXD --- PA9 --- 复用推挽输出
             USART1_RXD --- PA10 --- 浮空输入模式
****************************************************/
void usart1_send_byte(u8 data)
{
	while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE));
	USART_SendData(USART1,data);

}



/***********************
函数功能：printf重定向
形参：u8 data
返回值：void
************************/
int fputc(int c, FILE * stream)
{
	usart1_send_byte(c);
	return c;	
}



