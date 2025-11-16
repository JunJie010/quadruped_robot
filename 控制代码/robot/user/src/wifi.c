#include "main.h"

//配置USART2
/*
函数功能：USART2初始化
形参：u32 bps    波特率
返回值：void
函数说明：
USART2_TX --- PA2  --- 复用推挽输出
USART2_RX --- PA3 --- 浮空输入模式
*/

void Usart2_Init(u32 bps)
{
	//1、打开GPIOA时钟和USART2时钟  RCC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	//2、初始化PA2和PA3  ---  GPIO配置
	GPIO_InitTypeDef GPIO_InitStruct={0};//定义结构体变量名
	//PA2    USART2_TX
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//PA2配置为复用推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;//配置第2号管脚
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//输出速度为50Mhz
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	//PA3   USART2_RX    浮空输入模式
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;//PA3配置为浮空输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;//配置第3号管脚
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//3、USART2初始化
	USART_InitTypeDef USART_InitStruct={0};
	USART_InitStruct.USART_BaudRate = bps;//设置波特率
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//不使用硬件流传输
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//接收器和发送器使能
	USART_InitStruct.USART_Parity = USART_Parity_No;//不使用奇偶校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//停止位为1bit
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//数据长度为8bit
	USART_Init(USART2,&USART_InitStruct);
	
	//4、使能USART2接收中断和空闲中断    XXX_ITConfig();
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//接收中断
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);//空闲中断
	
	//5、配置NVIC
	NVIC_InitTypeDef NVIC_InitStruct={0};
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;//选择中断源
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//使能中断源通道
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;//占先优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;//次级优先级
	NVIC_Init(&NVIC_InitStruct);
	
	//6、使能USART2   
	USART_Cmd(USART2,ENABLE);
}

/*
USART2中断服务函数
*/
U2_STRUCT u2={0};
void USART2_IRQHandler(void)
{
	/*写USART2中断服务函数时，需要判断进入的是哪一个中断*/
	//此处判断进入接收中断
	if(USART_GetITStatus(USART2,USART_IT_RXNE))//此标志位硬件置一，需要软件清零
	{
		//清除标志位
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
		//接收数据部分    定义字符串数值    定义变量（记录接收数据长度）
		/*len变量到底加到什么时候才停止，才可以清零*/
		u2.buff[u2.len++] = USART_ReceiveData(USART2);
		
		
	}
	//此处判断进入空闲中断   表示数据接收完成
	if(USART_GetITStatus(USART2,USART_IT_IDLE))
	{
		//清除标志位   IDLE空闲标志位清除方法在中文参考手册寄存器部分
		/*先读USART2_SR寄存器，再读USART2_DR*/
		USART2->SR;
		USART2->DR;
		
		//数据接收完成
		u2.buff[u2.len] = '\0';//在字符串最后一字节补充结束符
		u2.len = 0;//清零数据长度，准备接收下一个数据
		u2.flag = 1;//代表数据接收完成
		printf("%s",u2.buff);
	}	
}



/*********************功能函数********************/

/*
函数功能：USART2发送1字节数据
形参：u8 data   需要发送的1byte数据
返回值：void
函数说明：
1、等待数据可以发送（等待发送标志位置一）
2、调用发送数据函数发送数据
*/

void usart2_send_byte(u8 data)
{
	//1、等待数据可以发送（等待发送标志位置一）
	while(!USART_GetFlagStatus(USART2,USART_FLAG_TXE))
	{
		
	}
	//2、调用发送数据函数发送数据
	USART_SendData(USART2,data);
}

/*
函数功能：USART2发送字符串函数
形参：u8 *data   需要发送的字符串
返回值：void
函数说明："hello world\0"
字符串特征：字符串结尾'\0'
*/

void usart2_send_str(u8 *data)
{
	while(*data != '\0')//当*data的值为'\0'时结束循环
	{
		usart2_send_byte(*data);
		data++;
	}	
}

/*
函数功能：Esp8266_SendCmd   ESP8266发送命令且判断是否成功执行
形参：
u8 *cmd       发送AT指令
u8 *respond   ESP8266需要回应的结果   "OK"
返回值：u8    0：成功     1：失败
*/

u8 Esp8266_SendCmd(u8 *cmd,u8 *respond)
{
	usart2_send_str(cmd);//发送AT指令
	delay_ms(300);
	//判断此指令ESP8266是否成功执行
	while(1)
	{
		//判断数据是否结束完成
		if(u2.flag == 1)
		{
			u2.flag = 0;
			if(strstr((const char *)u2.buff,(const char *)respond) != NULL)
				return 0;
			else
				return 1;
		}
	}

}



/*
函数功能：ESP8266_Init   WIFI初始化
形参：void
返回值：u8
*/

u8 ESP8266_Init(void)
{
	
	Voice_Play((u8 *)"00011");//初始化网络
	
	u8 ret = 0;
	Usart2_Init(115200);//115200   ESP8266默认波特率：115200
	
	//①退出透传
	usart2_send_str((u8 *)"+++");
	delay_ms(300);
	//②随便发送一个AT指令，跳过错误
	usart2_send_str((u8 *)"AT\r\n");
	delay_ms(300);
	//③恢复出厂设置
	usart2_send_str((u8 *)"AT+RESTORE\r\n");
	delay_ms(2000);
	//④设置ESP8266为客户端模式     此模式可以连接软路由
	ret = Esp8266_SendCmd((u8 *)"AT+CWMODE=1\r\n",(u8 *)"OK");
	if(ret != 0)
	{
		printf("ESP8266设置客户端模式失败\r\n");
		return 1;
	}
		printf("ESP8266设置客户端模式成功\r\n");
	//⑤设置ESP8266为透传模式
	ret = Esp8266_SendCmd((u8 *)"AT+CIPMODE=1\r\n",(u8 *)"OK");
	if(ret != 0)
	{
		printf("ESP8266设置透传模式失败\r\n");
		return 2;
	}
	printf("ESP8266设置透传模式成功\r\n");
	
	printf("ESP8266初始化成功\r\n");
	return 0;
	
}


/*
函数功能：ESP8266_ConnectWifi    连接软路由
形参：
u8 *ssid     填写WIFI账号名
u8 *pwd      填写WIFI密码
返回值：u8     0:WIFI连接成功      1:WIFI连接失败
函数说明：
连接软路由指令：
AT+CWJAP="WIFI账号名","WIFI密码"\r\n
*/

u8 ESP8266_ConnectWifi(u8 *ssid,u8 *pwd)
{
	u8 cnt = 0;
	//①字符串拼接    ②sprintf
	u8 wifi_buff[150] = {0};
	sprintf((char *)wifi_buff,"AT+CWJAP=\"%s\",\"%s\"\r\n",ssid,pwd);
	A:
	usart2_send_str((u8 *)wifi_buff);//发送拼接好的指令
	while(1)
	{
		while(!u2.flag);//等待数据接收完成
		u2.flag = 0;
		
		if(strstr((const char *)u2.buff,"OK") != NULL)//WIFI连接成功
		{
			printf("WIFI连接成功!!!!\r\n");
			return 0;
		}
		if(strstr((const char *)u2.buff,"FAIL") != NULL)//WIFI连接失败
		{
			printf("WIFI连接失败，正在第%d次重新连接....\r\n",++cnt);
			if(cnt == 3)
			{
				printf("WIFI连接失败!!!!!\r\n");
				return 1;
			}
			goto A;
		}	
	}	
}



/*
函数功能：ESP8266_ConnectServer    连接服务器
形参：
u8 *ip        填写IP地址
u8 *port      填写端口号
返回值：u8     0:WIFI连接成功      1:WIFI连接失败
函数说明：
连接服务器指令：
AT+CIPSTART="TCP","192.168.31.207",8080\r\n

电脑端：连接手机热点
查IP：①控制台 win + R 输入cmd，在控制台中输入ipconfig，回车
			②在上位机软件中查找，
*/

//u8 ESP8266_ConnectServer(u8 *ip,u8 *port)
//{
//	u8 cnt = 0;
//	//①字符串拼接    ②sprintf
//	u8 wifi_buff[200] = {0};
//	sprintf((char *)wifi_buff,"AT+CIPSTART=\"TCP\",\"%s\",%s\r\n",ip,port);
//	A:
//	usart2_send_str((u8 *)wifi_buff);//发送拼接好的指令
//	while(1)
//	{
//		while(!u2.flag);//等待数据接收完成
//		u2.flag = 0;
//		
//		if(strstr((const char *)u2.buff,"OK") != NULL)//连接服务器成功
//		{
//			printf("连接服务器成功!!!!\r\n");
//			usart2_send_str((u8 *)"AT+CIPSEND\r\n");//设置ESP8266为发送数据模式
//			return 0;
//		}
//		if(strstr((const char *)u2.buff,"ERROR") != NULL)//连接服务器失败
//		{
//			printf("连接服务器失败，正在第%d次重新连接....\r\n",++cnt);
//			if(cnt == 3)
//			{
//				printf("连接服务器失败!!!!!\r\n");
//				return 1;
//			}
//			goto A;
//		}	
//	}	
//}


u8 Esp8266_ConnectServer(u8 *ip,u8 *port)
{
	
	u8 cat[200]={0};
	u8 sever_cnt = 0;
	strcat((char *)cat,"AT+CIPSTART=\"TCP\",\"");
	strcat((char *)cat,(const char *)ip);
	strcat((char *)cat,"\",");
	strcat((char *)cat,(const char *)port);
	strcat((char *)cat,"\r\n");
	B:
	usart2_send_str(cat);//发送拼接完成的指令
	
	while(1)
	{
		while(!u2.flag);//等待数据接收完成
		u2.flag = 0;
		
		if(strstr((const char *)u2.buff,"OK")!=NULL)
		{
			printf("连接服务器成功\r\n");
			usart2_send_str((u8 *)"AT+CIPSEND\r\n");//进入发送模式命令    >
			return 0;
		}
		if(strstr((const char *)u2.buff,"ERROR")!=NULL)
		{
			if(sever_cnt == 3)
			{
				printf("连接服务器失败\r\n");
				return 1;
			}
			printf("连接服务器失败，正在尝试第%d次重试\r\n",++sever_cnt);
			goto B;
		}
		
	}		
}










