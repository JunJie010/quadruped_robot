#include "main.h"

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置优先级分组
	
	led_init();            //LED1初始化
	usart1_init(115200);   //USART1初始化
	printf("串口初始化成功！！！！！\r\n");
	voice_init(9600);      //语音模块波特率默认9600
	ArmCtrl_Config();      //舵机初始化配置
	MOTOR_Power();				 //舵机所用电源初始化配置
	Adc_Voltage_Config();  //ADC获取电池电压初始化配置
	Set_Song_Volume(30);   //设置音量大小：0-30级别
	delay_ms(500);         ////语音模块上电时间
	
	
	//机器人初始化动作
	Voice_Play((u8 *)"00016");
	MOTOR_POWER_ON;//舵机供电
	Arm_Li();//机器人初始动作（站立）
	//ESP8266初始化    2.4Ghz
	if(ESP8266_Init()==0)
		if(ESP8266_ConnectWifi((u8 *)"JunJie",(u8 *)"JJ13145230")==0)                 //热点不要设置连接wife设备个数   2.4G频段
			if(Esp8266_ConnectServer((u8 *)" 192.168.92.215",(u8 *)"8888")==0)
				Voice_Play((u8 *)"00012");
	
	printf("欢迎使用,四足机器人\r\n");
	
	tim2_init();			     //定时器2初始化
			
			
	u8 action_flag = 0;
	//程序运行部分   一直运行
	while(1)
	{
			if(strcmp((const char *)u2.buff,"Stop")==0)
		{
			action_flag = 0;
			Voice_Play((u8 *)"00001");//播放停止语音
			memset(u2.buff,0,sizeof(u2.buff));//清空数组
		}
		if(strcmp((const char *)u2.buff,"Pushup")==0)
		{
			action_flag = 1;
			Voice_Play((u8 *)"00010");//播放俯卧撑语音
			memset(u2.buff,0,sizeof(u2.buff));//清空数组
		}
		if(strcmp((const char *)u2.buff,"Dance")==0)
		{
			action_flag = 2;
			Voice_Play((u8 *)"00015");//播放音乐
			memset(u2.buff,0,sizeof(u2.buff));//清空数组
		}
		if(strcmp((const char *)u2.buff,"SayHello")==0)
		{
			action_flag = 3;
			Voice_Play((u8 *)"00009");//播放你好
			memset(u2.buff,0,sizeof(u2.buff));//清空数组
		}
		if(strcmp((const char *)u2.buff,"Forward")==0)
		{
			action_flag = 4;
			Voice_Play((u8 *)"00002");//播放直行
			memset(u2.buff,0,sizeof(u2.buff));//清空数组
		}
		if(strcmp((const char *)u2.buff,"TurnRight")==0)
		{
			action_flag = 5;
			Voice_Play((u8 *)"00005");//播放右转语音
			memset(u2.buff,0,sizeof(u2.buff));//清空数组
		}
		if(strcmp((const char *)u2.buff,"TurnLeft")==0)
		{
			action_flag = 6;
			Voice_Play((u8 *)"00004");//播放左转语音
			memset(u2.buff,0,sizeof(u2.buff));//清空数组
		}
		if(strcmp((const char *)u2.buff,"Twist")==0)
		{
			action_flag = 7;
			Voice_Play((u8 *)"00006");//播放左转语音
			memset(u2.buff,0,sizeof(u2.buff));//清空数组
		}
		switch(action_flag)
		{
			case 0:
				Arm_Li();//执行站立动作
			break;
			case 1:
				Arm_Push();//执行俯卧撑动作
			break;
			case 2:
				Arm_Jump();//执行跳舞动作
			break;
			case 3:
				Arm_Hello();//执行打招呼动作
			break;
			case 4:
				Arm_Forward();//执行前进动作
			break;
			case 5:
				Arm_Right();//执行右转动作
			break;
			case 6:
				Arm_Left();//执行左转动作
			break;
			case 7:
				Arm_Twisting();//执行扭动动作
			break;
		}
		
	}
	
}


