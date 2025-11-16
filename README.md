# 基于STM32的四足机器人

## 项目预览
![项目展示](制作过程纪念/成品展示.jpg)
## 项目职责
* 编写ESP8266**WIFI**模块代码
* 编写JQ8900-16P**语音播报**模块代码
* 编写SG90**舵机**驱动代码
***
## 关于舵机驱动控制
四足机器人有四个夹爪，其中每个夹爪采用两个SG90舵机，分别负责夹爪的水平和垂直移动，四个夹爪共8个SG90舵机

舵机的驱动原理是发送PWM信号，通过改变占空比驱动舵机的转动角度

PWM(Pulse Width Modulation)脉冲宽度调制，在具有惯性的系统中，可以通过一系列脉冲的宽度进行调制，来等效的获得所需模拟参量，常应用于电机控速领域

常用调制方法为通过定时器输出比较功能，通过设置PSC，ARR，CCR寄存器的值来调制一个频率可调，占空比受CCR控制的PWM波形

此处设计舵机控制电源电路，编写ADC驱动，用于测量电池电量情况，防止电池电压在小于5V情况下舵机运作出现异常

电路具体为PA15接通用推挽输出，当输出高电平时舵机通电可以运行，当输出低电平时舵机断电
### 舵机控制电路
![舵机驱动](制作过程纪念/舵机电源控制电路.png)
![舵机驱动](制作过程纪念/舵机电路.png)
![舵机驱动](制作过程纪念/舵机编号及对应引脚.png)

编写定时器驱动，定时器输出比较配置PWM，舵机初始化配置如下：
- arm0---PB8---TIM4_CH3-----右前下
- arm1---PB9---TIM4_CH4-----右前上
- arm2---PB6---TIM4_CH1-----右后上
- arm3---PB7---TIM4_CH2-----右后下
- arm4---PA6---TIM3_CH1-----左前下
- arm5---PA7---TIM3_CH2-----无反应
- arm6---PA0---TIM5_CH1-----左后上
- arm7---PA1---TIM5_CH2-----左后下

在完成舵机初始化函数后，只需再编写舵机驱动函数即可编写机器人动作函数
```
#define PWM_MIX (500)
#define PWM_MAX (2500)
#define ANGLE_MAX (180.0f)
#define ANGLE_PWM ((PWM_MAX - PWM_MIX)/ANGLE_MAX)       //宏定义SG90舵机相关参数

void ArmCtrl_Angle (ArmDef ArmNum, float Angle)        //输入角度值控制对应的舵机角度
{
  uint16_t Compare;
	Compare = PWM_MIX + ANGLE_PWM*Angle - 1;          //舵机角度换算公式
	switch (ArmNum)                                   //选择对应舵机号控制角度
	{
		case ARM_RIGHT_BEFORE_L:			
			TIM_SetCompare3(TIM4, Compare);		
			break;
		case ARM_RIGHT_BEFORE_H:
			TIM_SetCompare4(TIM4, Compare);		
			break;
		case ARM_RIGHT_AFTER_H:
			TIM_SetCompare1(TIM4, Compare);		
			break;
		case ARM_RIGHT_AFTER_L:
			TIM_SetCompare2(TIM4, Compare);		
			break;
		case ARM_LEFT_BEFORE_L:
			TIM_SetCompare1(TIM3, Compare);	
			break;
		case ARM_LEFT_BEFORE_H:					
			TIM_SetCompare2(TIM3, Compare);		
			break;
		case ARM_LEFT_AFTER_H:
			TIM_SetCompare1(TIM5, Compare);		
			break;
		case ARM_LEFT_AFTER_L:
			TIM_SetCompare2(TIM5, Compare);	
			break;
		default:
			break;
	}
}
```
紧接着，完成功能函数四足机器人动作函数：
```
void Arm_Zero();         //初始动作
void Arm_Li();           //四足立起
void Arm_Jump();         //跳跃
void Arm_Push();         //俯卧撑
void Arm_Hello();        //打招呼
void Arm_Twisting();     //扭动
void Arm_Peristalsis();  //蠕动
void Arm_Left();         //左转
void Arm_Right();        //右转
void Arm_ss();           //四足立起
void Arm_Forward();      //前进
//注意，此处只做简单展示，详细代码请看控制代码
```
### 动作
![舵机驱动](制作过程纪念/动作展示.png)
***

***
后面会继续更新，请等待
