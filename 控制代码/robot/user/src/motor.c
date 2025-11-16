#include "main.h"


/***************************************************
*函数名    ：MOTOR_Power
*函数功能  ：舵机控制电源电路
*函数参数  ：u32 bound    波特率
*函数返回值：无
*函数描述  ：	MOTOR_POWER --- PA15 --- 通用推挽输出
							PA15输出高电平舵机通电
							PA15输出低电平舵机断电
****************************************************/
//舵机控制电源电路 PA15---重映射 低电平失能
void MOTOR_Power(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	//开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); 
	//重映射
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	//PA15配置
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct); 
	
	MOTOR_POWER_OFF;
	
//	//1、打开GPIOA时钟
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
//	
//	//2、PA15初始化   通用推挽输出
//	GPIOA->CRH &=~ (0xfu << 28);//配置位清零
//	GPIOA->CRH |= 3 << 28;//将PA15配置为输出模式，输出速度为50Mhz
//	GPIOA->CRH &=~ (3u << 30);//配置PA15配置为通用推挽输出
//	
//	//3、默认初始化完成后，舵机断电
//	MOTOR_POWER_OFF;
	
}


/****************************************************************
*函数名    ：ArmCtrl_Config
*函数功能  ：舵机初始化配置
*函数参数  ：无
*函数返回值：无
*函数描述  ：	机械臂舵机初始化函数   通用定时器 输出PWM  占空比
							arm0---PB8---TIM4_CH3-----右前下
							arm1---PB9---TIM4_CH4-----右前上
							arm2---PB6---TIM4_CH1-----右后上
							arm3---PB7---TIM4_CH2-----右后下
							arm4---PA6---TIM3_CH1-----左前下
							arm5---PA7---TIM3_CH2-----无反应
							arm6---PA0---TIM5_CH1-----左后上
							arm7---PA1---TIM5_CH2-----左后下
****************************************************************/
Arm_Countinfo armflag;
void ArmCtrl_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	//开时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	//GPIO配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//定时器基本配置
	TIM_TimeBaseStructure.TIM_Period = 20000;                				//自动重装载周期值(1+TIM_Period)
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;             				//预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;				  					//分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 		//向上计数
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	//定时器输出PWM配置
		/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;              	//输出PWM波形
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;      	//高电平有效还是低电平有效
	TIM_OC1Init(TIM5, &TIM_OCInitStructure);
	TIM_OC2Init(TIM5, &TIM_OCInitStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;              	//输出PWM波形
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;      	//高电平有效还是低电平有效
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;              	//输出PWM波形
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;      	//高电平有效还是低电平有效
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM5, ENABLE);
	
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	
	//定时器使能
	TIM_Cmd(TIM5, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	delay_ms(500);
}



//舵机转向控制
#define PWM_MIX (500)
#define PWM_MAX (2500)
#define ANGLE_MAX (180.0f)
#define ANGLE_PWM ((PWM_MAX - PWM_MIX)/ANGLE_MAX)

/**
  * @brief  输入角度值控制对应的舵机角度
  * @param  舵机号
  * @param  角度 0~179
  * @retval None
  */
void ArmCtrl_Angle (ArmDef ArmNum, float Angle)
{
	uint16_t Compare;
	Compare = PWM_MIX + ANGLE_PWM*Angle - 1;
	switch (ArmNum)
	{
		case ARM_RIGHT_BEFORE_L:			
			TIM_SetCompare3(TIM4, Compare);		//*
			break;
		case ARM_RIGHT_BEFORE_H:
			TIM_SetCompare4(TIM4, Compare);		//*
			break;
		case ARM_RIGHT_AFTER_H:
			TIM_SetCompare1(TIM4, Compare);		//*
			break;
		case ARM_RIGHT_AFTER_L:
			TIM_SetCompare2(TIM4, Compare);		//*
			break;
		case ARM_LEFT_BEFORE_L:
			TIM_SetCompare1(TIM3, Compare);		//*
			break;
		case ARM_LEFT_BEFORE_H:					
			TIM_SetCompare2(TIM3, Compare);		//*
			break;
		case ARM_LEFT_AFTER_H:
			TIM_SetCompare1(TIM5, Compare);		//*
			break;
		case ARM_LEFT_AFTER_L:
			TIM_SetCompare2(TIM5, Compare);		//*
			break;
		default:
			break;
	}
}


//舵机动作函数
//初始动作
void Arm_Zero(void)
{
	for(int i = 0;i < 8;i++)
	{
		ArmCtrl_Angle(i,90);
	}
}
//四足立起
void Arm_Li(void)
{
	int ap = 30;
	int hi = 0;
	int position[] = { 90 + ap, 90 + hi, 90 - hi, 90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap};		//四足立起
	for(int i = 0;i < 8;i++)
	{
		ArmCtrl_Angle(i,position[i]);
		delay_ms(500);
	}
}
//跳跃-----OK
void Arm_Jump(void) 
{
	int ap = 25;
	int hi = 25;
	int position[] = { 90 + ap, 90 + hi, 90 - hi, 90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap};		//四足平行
	//舵机使能
	if(ARM_JUMP)			//判定是当前动作
	{
		MOTOR_POWER_ON;
		for (int i = 0; i < 8; i++) 
		{
		  ArmCtrl_Angle(i, position[i]);
		}
		//升
		armflag.action_push_up = true;
		armflag.action_jump_down = false;
		armflag.arm_time5 = 0;
		while(armflag.action_push_up == true)
		{
			ArmCtrl_Angle(ARM_RIGHT_BEFORE_L,90 + armflag.arm_time5);
			ArmCtrl_Angle(ARM_RIGHT_AFTER_L,90 + armflag.arm_time5);
			ArmCtrl_Angle(ARM_LEFT_BEFORE_L,90 - armflag.arm_time5);
			ArmCtrl_Angle(ARM_LEFT_AFTER_L,90 - armflag.arm_time5);
			if(armflag.arm_time5 == 40)
			{
				armflag.arm_time5 = 0;
				armflag.action_push_up = false;
				armflag.action_jump_down = true;
			}
		}
		//降
		while(armflag.action_jump_down == true)
		{
			ArmCtrl_Angle(ARM_RIGHT_BEFORE_L,130 - armflag.arm_time5);
			ArmCtrl_Angle(ARM_RIGHT_AFTER_L,130 - armflag.arm_time5);
			ArmCtrl_Angle(ARM_LEFT_BEFORE_L,50 + armflag.arm_time5);
			ArmCtrl_Angle(ARM_LEFT_AFTER_L,50 + armflag.arm_time5);
			if(armflag.arm_time5 == 40)
			{
				armflag.arm_time5 = 0;
				armflag.action_push_up = true;
				armflag.action_jump_down = false;
			}
		}		
	}
}

//俯卧撑-----OK
void Arm_Push(void)
{
	int ap = 30;
	int hi = 50;
	int position[] = { 90 + ap, 90, 90 + hi, 90, 90 - ap, 90, 90 - hi, 90};		//后足并拢
	if((ARM_PUSHUP))			//判定是当前动作
	{
		//舵机使能
		MOTOR_POWER_ON;
		armflag.action_push_up = true;
		armflag.action_push_down = false;
		//舵机使能
		MOTOR_POWER_ON;
		for (int i = 0; i < 8; i++) 
		{
			ArmCtrl_Angle(i, position[i]);
		}
		//运行时间清零
		armflag.arm_time10 = 0;
		//降
		while(armflag.action_push_up == true)
		{
			ArmCtrl_Angle(ARM_RIGHT_BEFORE_L,150 - armflag.arm_time10);
			ArmCtrl_Angle(ARM_LEFT_BEFORE_L,30 + armflag.arm_time10);
//			printf("temp1 = %d\r\n", armflag.arm_time10);
			if(armflag.arm_time10 == 60)
			{
				armflag.arm_time10 = 0;
				armflag.action_push_up = false;
				armflag.action_push_down = true;
				break;
			}
		}
		//升
		while(armflag.action_push_down == true)
		{
//			printf("temp2 = %d\r\n", armflag.arm_time10);
			ArmCtrl_Angle(ARM_RIGHT_BEFORE_L,90 + armflag.arm_time10);
			ArmCtrl_Angle(ARM_LEFT_BEFORE_L,90 - armflag.arm_time10);	
			if(armflag.arm_time10 == 60)
			{
				armflag.arm_time10 = 0;
				armflag.action_push_up =  true;
				armflag.action_push_down = false;
				break;
			}
		}
	}	
}

//打招呼-----OK
void Arm_Hello(void)
{
	int ap = 30;
	int hi = 50;
	int position[] = { 10, 120, 90 - hi, 90 + ap, 90 - ap, 90, 90 + hi, 100};			//右前抬起 左后下降
	if( ARM_SAYHELLO)			//判定是当前动作
	{
		//舵机使能
		MOTOR_POWER_ON;
		for (int i = 0; i < 8; i++) 
		{
			ArmCtrl_Angle(i,position[i]);
		}
		armflag.action_sayhello_up = true;
		armflag.action_sayhello_down = false;
		armflag.arm_time5 = 0;			//运行时间清零
		while(armflag.action_sayhello_up == true)
		{
			//右前向前挥动
			ArmCtrl_Angle(ARM_RIGHT_BEFORE_H,120 - armflag.arm_time5);
			//向下摆手
			ArmCtrl_Angle(ARM_RIGHT_BEFORE_L,10 + armflag.arm_time5);
			if(armflag.arm_time5 == 60)
			{
				armflag.arm_time5 = 0;
				armflag.action_sayhello_up = false;
				armflag.action_sayhello_down = true;
				break;
			}
		}
		while(armflag.action_sayhello_down == true)
		{
			//右前向后挥动
			ArmCtrl_Angle(ARM_RIGHT_BEFORE_H,60 + armflag.arm_time5);
			//向上摆手
			ArmCtrl_Angle(ARM_RIGHT_BEFORE_L,70 - armflag.arm_time5);
			if(armflag.arm_time5 == 60)
			{
				armflag.arm_time5 = 0;
				armflag.action_sayhello_up = true;
				armflag.action_sayhello_down = false;
				break;
			}
		}
	}
}

//扭动
void Arm_Twisting(void)
{
	int ap = 30;
	int hi = 0;
	int position[] = { 90 + ap, 90 + hi, 90 - hi, 90 + ap, 90 + ap, 90 - hi, 90 + hi, 90 + ap};			//右前抬起 左后下降
	if(ARM_TEIST)			//判定是当前动作
	{
		//舵机使能
		MOTOR_POWER_ON;
		//起始动作
		for (int i = 0; i < 8; i++) 			
		{
			ArmCtrl_Angle(i,position[i]);
		}
		armflag.action_twisting_up = true;
		armflag.action_twisting_down = false;
		armflag.arm_time10 = 0;			//运行时间清零
		while(armflag.action_twisting_up == true)
		{
			ArmCtrl_Angle(ARM_RIGHT_AFTER_L,120 - 2 * armflag.arm_time10);
			ArmCtrl_Angle(ARM_LEFT_BEFORE_L,120 - 2 * armflag.arm_time10);
			if(armflag.arm_time10 == 20)
			{
				armflag.arm_time10 = 0;
				armflag.action_twisting_up = false;
				armflag.action_twisting_down = true;
			}
		}
		while(armflag.action_twisting_down == true)
		{
			ArmCtrl_Angle(ARM_RIGHT_BEFORE_L,120 - 2 * armflag.arm_time10);
			ArmCtrl_Angle(ARM_LEFT_AFTER_L,120 - 2 * armflag.arm_time10);
			if(armflag.arm_time10 == 20)
			{
				armflag.arm_time10 = 0;
				armflag.action_twisting_up = true;
				armflag.action_twisting_down = false;
			}
		}
		while(armflag.action_twisting_up == true)
		{
			ArmCtrl_Angle(ARM_RIGHT_AFTER_L,90 + 2 * armflag.arm_time10);
			ArmCtrl_Angle(ARM_LEFT_BEFORE_L,90 + armflag.arm_time10);
			if(armflag.arm_time10 == 20)
			{
				armflag.arm_time10 = 0;
				armflag.action_twisting_up = false;
				armflag.action_twisting_down = true;
			}
		}
		while(armflag.action_twisting_down == true)
		{
			ArmCtrl_Angle(ARM_RIGHT_BEFORE_L,90 + 2 * armflag.arm_time10);
			ArmCtrl_Angle(ARM_LEFT_AFTER_L,90 + armflag.arm_time10);
			if(armflag.arm_time10 == 20)
			{
				armflag.arm_time10 = 0;
				armflag.action_twisting_up = true;
				armflag.action_twisting_down = false;
			}
		}
	}
}
//蠕动------OK
void Arm_Peristalsis(void)
{
	int ap = 25;
	int hi = 0;
	int position[] = { 90 + ap, 90 + hi, 90 - hi, 90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap};			//右前抬起 左后下降
	if(ARM_PERISTALSIS)			//判定是当前动作
	{
		//舵机使能
		MOTOR_POWER_ON;
		for (int i = 0; i < 8; i++) 
		{
			ArmCtrl_Angle(i,position[i]);
		}
		armflag.action_Peristalsis_up = true;
		armflag.action_Peristalsis_down = false;
		armflag.arm_time10 = 0;			//运行时间清零
		while(armflag.action_Peristalsis_up == true)
		{
			//上部
			ArmCtrl_Angle(ARM_RIGHT_BEFORE_H,90 - armflag.arm_time10);
			ArmCtrl_Angle(ARM_RIGHT_AFTER_H,90 - armflag.arm_time10);
			ArmCtrl_Angle(ARM_LEFT_BEFORE_H,90 + armflag.arm_time10);
			ArmCtrl_Angle(ARM_LEFT_AFTER_H,90 + armflag.arm_time10);
			delay_ms(5);
			//下部
			ArmCtrl_Angle(ARM_RIGHT_BEFORE_L,125 - armflag.arm_time10);
			ArmCtrl_Angle(ARM_RIGHT_AFTER_L,125 - armflag.arm_time10);
			ArmCtrl_Angle(ARM_LEFT_BEFORE_L,55 + armflag.arm_time10);
			ArmCtrl_Angle(ARM_LEFT_AFTER_L,55 + armflag.arm_time10);
			if(armflag.arm_time10 == 35)
			{
				armflag.arm_time10 = 0;			//运行时间清零
				armflag.action_Peristalsis_up = false;
				armflag.action_Peristalsis_down = true;
			}
		}
		while(armflag.action_Peristalsis_down == true)
		{
			//上部
			ArmCtrl_Angle(ARM_RIGHT_BEFORE_H,55 + armflag.arm_time10);
			ArmCtrl_Angle(ARM_RIGHT_AFTER_H,55 + armflag.arm_time10);
			ArmCtrl_Angle(ARM_LEFT_BEFORE_H,125 - armflag.arm_time10);
			ArmCtrl_Angle(ARM_LEFT_AFTER_H,125 - armflag.arm_time10);
			delay_ms(5);
			//下部
			ArmCtrl_Angle(ARM_RIGHT_BEFORE_L,90 + armflag.arm_time10);
			ArmCtrl_Angle(ARM_RIGHT_AFTER_L,90 + armflag.arm_time10);
			ArmCtrl_Angle(ARM_LEFT_BEFORE_L,90 - armflag.arm_time10);
			ArmCtrl_Angle(ARM_LEFT_AFTER_L,90 - armflag.arm_time10);
			if(armflag.arm_time10 == 35)
			{
				armflag.arm_time10 = 0;			//运行时间清零
				armflag.action_Peristalsis_up = true;
				armflag.action_Peristalsis_down = false;
			}
		}
	}
}
//左转
void Arm_Left(void)
{
	int ap = 30;
	int hi = 0;
	int position[] = { 90 + ap, 90 + hi, 90 - hi, 90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap};			
	if(ARM_TURNLEFT)			//判定是当前动作
	{
		//舵机使能
		MOTOR_POWER_ON;
		for (int i = 0; i < 8; i++) 
		{
			ArmCtrl_Angle(i,position[i]);
		}
		armflag.action_left_up = true;
		armflag.action_left_down = false;
		armflag.arm_time5 = 0;			//运行时间清零
		//0 7抬起
		while(armflag.action_left_up == true)
		{
			ArmCtrl_Angle(ARM_RIGHT_BEFORE_L,120 - armflag.arm_time5);
			ArmCtrl_Angle(ARM_LEFT_AFTER_L,60 + armflag.arm_time5);
			if(armflag.arm_time5 == 30)
			{
				armflag.action_left_up = false;
				armflag.action_left_down = true;
				armflag.arm_time5 = 0;			//运行时间清零
			}
		}
		//1 6顺时针
		while(armflag.action_left_down == true)
		{
			ArmCtrl_Angle(ARM_RIGHT_BEFORE_H,90 - armflag.arm_time5);
			ArmCtrl_Angle(ARM_LEFT_AFTER_H,90 - armflag.arm_time5);
			if(armflag.arm_time5 == 30)
			{
				armflag.action_left_up = true;
				armflag.action_left_down = false;
				armflag.arm_time5 = 0;			//运行时间清零
			}
		}
		//0 7放下
		while(armflag.action_left_up == true)
		{
			ArmCtrl_Angle(ARM_RIGHT_BEFORE_L,90 + armflag.arm_time5);
			ArmCtrl_Angle(ARM_LEFT_AFTER_L,90 - armflag.arm_time5);
			if(armflag.arm_time5 == 30)
			{
				armflag.action_left_up = false;
				armflag.action_left_down = true;
				armflag.arm_time5 = 0;			//运行时间清零
			}
		}
		//3 4抬起
		while(armflag.action_left_down == true)
		{
			ArmCtrl_Angle(ARM_RIGHT_AFTER_L,120 - armflag.arm_time5);
			ArmCtrl_Angle(ARM_LEFT_BEFORE_L,60 + armflag.arm_time5);
			if(armflag.arm_time5 == 30)
			{
				armflag.action_left_up = true;
				armflag.action_left_down = false;
				armflag.arm_time5 = 0;			//运行时间清零
			}
		}
		//2 5逆时针
		while(armflag.action_left_up == true)
		{
			ArmCtrl_Angle(ARM_RIGHT_AFTER_H,90 - armflag.arm_time5);
			ArmCtrl_Angle(ARM_LEFT_BEFORE_H,90 - armflag.arm_time5);
			if(armflag.arm_time5 == 20)
			{
				armflag.action_left_up = false;
				armflag.action_left_down = true;
				armflag.arm_time5 = 0;			//运行时间清零
			}
		}
		//1 6逆时针
		while(armflag.action_left_down == true)
		{
			ArmCtrl_Angle(ARM_RIGHT_BEFORE_H,60 + armflag.arm_time5);
			ArmCtrl_Angle(ARM_LEFT_AFTER_H,60 + armflag.arm_time5);
			if(armflag.arm_time5 == 30)
			{
				armflag.action_left_up = true;
				armflag.action_left_down = false;
				armflag.arm_time5 = 0;			//运行时间清零
			}
		}
		//3 4放下
		while(armflag.action_left_up == true)
		{
			ArmCtrl_Angle(ARM_RIGHT_AFTER_L,90 + armflag.arm_time5);
			ArmCtrl_Angle(ARM_LEFT_BEFORE_L,90 - armflag.arm_time5);
			if(armflag.arm_time5 == 30)
			{
				armflag.action_left_up = false;
				armflag.action_left_down = true;
				armflag.arm_time5 = 0;			//运行时间清零
			}
		}
		//2 5顺时针
		while(armflag.action_left_down == true)
		{
			ArmCtrl_Angle(ARM_RIGHT_AFTER_H,70 + armflag.arm_time5);
			ArmCtrl_Angle(ARM_LEFT_BEFORE_H,70 + armflag.arm_time5);
			if(armflag.arm_time5 == 20)
			{
				armflag.action_left_up = true;
				armflag.action_left_down = false;
				armflag.arm_time5 = 0;			//运行时间清零
			}
		}
	}	
}

//右转
void Arm_Right(void)
{
	int ap = 30;
	int hi = 0;
	int position[] = { 90 + ap, 90 + hi, 90 - hi, 90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap};			//右前抬起 左后下降
	if(ARM_TURNRIGHT)			//判定是当前动作
	{
		//舵机使能
		MOTOR_POWER_ON;
		for (int i = 0; i < 8; i++) 
		{
			ArmCtrl_Angle(i,position[i]);
		}
		armflag.action_right_up = true;
		armflag.action_right_down = false;
		armflag.arm_time5 = 0;			//运行时间清零
		//0 7抬起
		while(armflag.action_right_up == true)
		{
			ArmCtrl_Angle(ARM_RIGHT_BEFORE_L,120 - armflag.arm_time5);
			ArmCtrl_Angle(ARM_LEFT_AFTER_L,60 + armflag.arm_time5);
			if(armflag.arm_time5 == 30)
			{
				armflag.action_right_up = false;
				armflag.action_right_down = true;
				armflag.arm_time5 = 0;			//运行时间清零
			}
		}
		//1 6逆时针
		while(armflag.action_right_down == true)
		{
			ArmCtrl_Angle(ARM_RIGHT_BEFORE_H,90 + armflag.arm_time5);
			ArmCtrl_Angle(ARM_LEFT_AFTER_H,90 + armflag.arm_time5);
			if(armflag.arm_time5 == 30)
			{
				armflag.action_right_up = true;
				armflag.action_right_down = false;
				armflag.arm_time5 = 0;			//运行时间清零
			}
		}
		//0 7放下
		while(armflag.action_right_up == true)
		{
			ArmCtrl_Angle(ARM_RIGHT_BEFORE_L,90 + armflag.arm_time5);
			ArmCtrl_Angle(ARM_LEFT_AFTER_L,90 - armflag.arm_time5);
			if(armflag.arm_time5 == 30)
			{
				armflag.action_right_up = false;
				armflag.action_right_down = true;
				armflag.arm_time5 = 0;			//运行时间清零
			}
		}
		//3 4抬起
		while(armflag.action_right_down == true)
		{
			ArmCtrl_Angle(ARM_RIGHT_AFTER_L,120 - armflag.arm_time5);
			ArmCtrl_Angle(ARM_LEFT_BEFORE_L,60 + armflag.arm_time5);
			if(armflag.arm_time5 == 30)
			{
				armflag.action_right_up =true;
				armflag.action_right_down = false;
				armflag.arm_time5 = 0;			//运行时间清零
			}
		}
		//2 5顺时针
		while(armflag.action_right_up == true)
		{
			ArmCtrl_Angle(ARM_RIGHT_AFTER_H,90 + armflag.arm_time5);
			ArmCtrl_Angle(ARM_LEFT_BEFORE_H,90 + armflag.arm_time5);
			if(armflag.arm_time5 == 30)
			{
				armflag.action_right_up = false;
				armflag.action_right_down = true;
				armflag.arm_time5 = 0;			//运行时间清零
			}
		}
		//1 6顺时针
		while(armflag.action_right_down == true)
		{
			ArmCtrl_Angle(ARM_RIGHT_BEFORE_H,120 - armflag.arm_time5);
			ArmCtrl_Angle(ARM_LEFT_AFTER_H,120 - armflag.arm_time5);
			if(armflag.arm_time5 == 30)
			{
				armflag.action_right_up = true;
				armflag.action_right_down = false;
				armflag.arm_time5 = 0;			//运行时间清零
			}
		}
		//3 4放下
		while(armflag.action_right_up == true)
		{
			ArmCtrl_Angle(ARM_RIGHT_AFTER_L,90 + armflag.arm_time5);
			ArmCtrl_Angle(ARM_LEFT_BEFORE_L,90 - armflag.arm_time5);
			if(armflag.arm_time5 == 30)
			{
				armflag.action_right_up = false;
				armflag.action_right_down = true;
				armflag.arm_time5 = 0;			//运行时间清零
			}
		}
		//2 5逆时针
		while(armflag.action_right_down == true)
		{
			ArmCtrl_Angle(ARM_RIGHT_AFTER_H,120 - armflag.arm_time5);
			ArmCtrl_Angle(ARM_LEFT_BEFORE_H,120 - armflag.arm_time5);
			if(armflag.arm_time5 == 30)
			{
				armflag.action_right_up = true;
				armflag.action_right_down = false;
				armflag.arm_time5 = 0;			//运行时间清零
			}
		}
	}
}






//四足立起
void Arm_ss(void)
{
	int ap = 60;
	int hi = 30;
	int position[] = { 90 + ap, 90 + hi, 90 - hi, 90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap};		//四足立起
	for(int i = 0;i < 8;i++)
	{
		ArmCtrl_Angle(i,position[i]);
		delay_ms(30);
	}
}




//前进
void Arm_Forward(void)
{
	//              0   1   2   3   4   5    6    7
	u8 position[]={120,120,60, 90, 90, 90 , 90 , 60};
	
	for(u8 i=0;i<8;i++)
	{
		ArmCtrl_Angle(i,position[i]);
	}
	armflag.arm_time5 = 0;
	armflag.action_forward_up = 1;
	armflag.action_forward_down = 0;
	//第一个动作    0号舵机抬起    3号舵机向下     4号舵机向下      7号舵机抬起
	while(armflag.action_forward_up == 1)
	{
		ArmCtrl_Angle(ARM_RIGHT_BEFORE_L,120 - armflag.arm_time5);
		ArmCtrl_Angle(ARM_RIGHT_AFTER_L,90 + armflag.arm_time5);
		ArmCtrl_Angle(ARM_LEFT_BEFORE_L,90 - armflag.arm_time5);
		ArmCtrl_Angle(ARM_LEFT_AFTER_L,60 + armflag.arm_time5);
		if(armflag.arm_time5 == 30)
		{
			armflag.arm_time5 = 0;
			armflag.action_forward_up = 0;
			armflag.action_forward_down = 1;
		}
	}
	//第二个动作  上臂部分     1号、5号逆时针    2号、6号顺时针
	while(armflag.action_forward_down == 1)
	{
		ArmCtrl_Angle(ARM_RIGHT_BEFORE_H,120 - armflag.arm_time5);
		ArmCtrl_Angle(ARM_RIGHT_AFTER_H,60 + armflag.arm_time5);
		ArmCtrl_Angle(ARM_LEFT_BEFORE_H,90 - armflag.arm_time5);
		ArmCtrl_Angle(ARM_LEFT_AFTER_H,90 + armflag.arm_time5);
		if(armflag.arm_time5 == 30)
		{
			armflag.arm_time5 = 0;
			armflag.action_forward_up = 1;
			armflag.action_forward_down = 0;
		}
	}
	//第三个动作    0号舵机向下    3号舵机抬起     4号舵机抬起      7号舵机向下
	while(armflag.action_forward_up == 1)
	{
		ArmCtrl_Angle(ARM_RIGHT_BEFORE_L,90 + armflag.arm_time5);
		ArmCtrl_Angle(ARM_RIGHT_AFTER_L,120 - armflag.arm_time5);
		ArmCtrl_Angle(ARM_LEFT_BEFORE_L,60 + armflag.arm_time5);
		ArmCtrl_Angle(ARM_LEFT_AFTER_L,90 - armflag.arm_time5);
		if(armflag.arm_time5 == 30)
		{
			armflag.arm_time5 = 0;
			armflag.action_forward_up = 0;
			armflag.action_forward_down = 1;
		}
	}
	//第四个动作  上臂部分     1号、5号顺时针    2号、6号逆时针
	while(armflag.action_forward_down == 1)
	{
		ArmCtrl_Angle(ARM_RIGHT_BEFORE_H,90 + armflag.arm_time5);
		ArmCtrl_Angle(ARM_RIGHT_AFTER_H,90 - armflag.arm_time5);
		ArmCtrl_Angle(ARM_LEFT_BEFORE_H,60 + armflag.arm_time5);
		ArmCtrl_Angle(ARM_LEFT_AFTER_H,120 - armflag.arm_time5);
		if(armflag.arm_time5 == 30)
		{
			armflag.arm_time5 = 0;
			armflag.action_forward_up = 1;
			armflag.action_forward_down = 0;
		}
	}	
}

