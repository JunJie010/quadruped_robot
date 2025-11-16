#include "main.h"


const  char *voice_buff[] = {
	"00001",		//停止
	"00002",		//直行
	"00003",		//后退
	"00004",		//左转
	"00005",		//右转
	"00006",		//扭动
	"00007",		//蠕动
	"00008",		//跳动
	"00009",		//打招呼
	"00010"			//俯卧撑
};



/***************************************************
*函数名    ：voice_init
*函数功能  ：语音串口5初始化函数
*函数参数  ：u32 bound    波特率
*函数返回值：无
*函数描述  ：	UART5_TX --- PC12  --- 复用推挽输出
						  UART5_RX --- PD2 	 --- 浮空输入模式
****************************************************/
void voice_init(u32 bound)
{
	
	//1、打开GPIOC时钟、GPIOD时钟和UART5时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
	
	//2、初始化PC12和PD2  ---  GPIO配置
	GPIO_InitTypeDef GPIO_InitStruct={0};//定义结构体变量名
	//PC12    UART5_TX
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//PC12配置为复用推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;//配置第12号管脚
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//输出速度为50Mhz
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	//PD2   UART5_RX    浮空输入模式
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;//PD2配置为浮空输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;//配置第2号管脚
	GPIO_Init(GPIOD,&GPIO_InitStruct);
	
	//3、UART5初始化
	USART_InitTypeDef USART_InitStruct={0};
	USART_InitStruct.USART_BaudRate = bound;//设置波特率
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//不使用硬件流传输
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//接收器和发送器使能
	USART_InitStruct.USART_Parity = USART_Parity_No;//不使用奇偶校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//停止位为1bit
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//数据长度为8bit
	USART_Init(UART5,&USART_InitStruct);
	
	//4、使能UART5   
	USART_Cmd(UART5,ENABLE);

}



/***************************************************
*函数名    ：uart5_send_byte
*函数功能  ：串口5发送一字节数据函数
*函数参数  ：u8 data
*函数返回值：无
*函数描述  ：	PC12--Uart5_Tx   PD2--Uart5_Rx
1、等待数据可以发送（等待发送标志位置一）
2、调用发送数据函数发送数据
****************************************************/
void uart5_send_byte(u8 data)
{
	//轮询检测发送数据寄存器是否为空
	while(USART_GetFlagStatus(UART5,USART_FLAG_TXE) == RESET);
	USART_SendData(UART5,data); //发送数据
}

//播放
u8 Play[4]={0xAA,0x02,0x00};

u8 Stop[4]={0xAA,0x03,0x00};

u8 Next[4]={0xAA,0x05,0x00};

u8 Vol_Up[4]={0xAA,0x14,0x00};

u8 Vol[4]={0xAA,0x13 ,0x01 ,0x1e};


/***************************************************
*函数名    ：Voice_SendData
*函数功能  ：语音串口5发送数据包函数
*函数参数  ：u8 *cmd      需要发送指令码
						 u8 cmd_len   决定指令码数据长度
*函数返回值：无
*函数描述  ：	PC12--Uart5_Tx   PD2--Uart5_Rx
****************************************************/
void Voice_SendData(u8 *cmd,u8 cmd_len)
{
	u8 cheak_sum = 0;
	for(u8 i=0;i<cmd_len;i++)
	{
		cheak_sum += *cmd;//计算校验和
		uart5_send_byte(*cmd++);
		
	}
	//发送校验和,取低八位
	uart5_send_byte((cheak_sum & 0xff));
}


/***************************************************
*函数名    ：Voice_Play
*函数功能  ：语音发送指令执行相应功能函数
*函数参数  ：u8 *name
*函数返回值：无
*函数描述  ：
"00001"  --  停止
"00002"  --  直行
"00003"  --  后退
"00004"  --  左转
"00005"  --  右转
"00006"  --  扭动
"00007"  --  蠕动
"00008"  --  跳动
"00009"  --  打招呼
"00010"  --  俯卧撑
"00011"  --  初始化网络
"00012"  --  初始化成功
"00013"  --  未知
"00014"  --  初始化成功
"00015"  --  只因你太美
"00016"  --  欢迎使用
****************************************************/
void Voice_Play(u8 *name)
{
	u8 mix_data[40]={0xAA,0x08,0x00,0x02};//指定盘符指定路径播放 0x02是盘符
	u8 temp[10];//暂存路径：将路径存放到temp数组中，temp[10] = "/name*???"
	sprintf((char *)temp,"/%s*???",name);
	//计算路径长度
	mix_data[2] = strlen((const char *)temp)+1;//数据长度=盘符长度+路径长度=1+路径长度
	//拼接路径
	strcpy((char *)(mix_data+4),(const char *)temp);//将路径加到mix_data中
	Voice_SendData(mix_data,strlen((const char *)mix_data));//将指令数据发送给从机
	
}


/***************************************************
*函数名    ：Set_Song_Volume
*函数功能  ：设置音量大小函数
*函数参数  ：unsigned char volume_num 音量大小 0---30			 
*函数返回值：无
*函数描述  ：
****************************************************/
void Set_Song_Volume(unsigned char volume_num)
{
	//设置音量数组
	char Play_song_volume[4]={0xAA,0x13,0x01,0x09};
	//音量值进行赋值
	Play_song_volume[3] = volume_num;
	Voice_SendData((u8*)Play_song_volume,sizeof(Play_song_volume));
}


