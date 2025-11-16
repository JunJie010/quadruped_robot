#include "main.h"


/***************************************************
*函数名    ：Adc_Voltage_Config
*函数功能  ：ADC获取电池电压初始化
*函数参数  ：无
*函数返回值：无
*函数描述  ：电源电压检测 PC0 ----- ADC1_IN10
****************************************************/
void Adc_Voltage_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	ADC_InitTypeDef ADC_InitStruct = {0};
	//开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1,ENABLE); 
	//PC0配置
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOC,&GPIO_InitStruct); 
	//ADC分频 //6分频 12MHZ
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	//ADC1配置
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;		//单次模式
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;		 //ADC 数据右对齐
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;			// ADC1 和 ADC2 工作在独立模式
	ADC_InitStruct.ADC_NbrOfChannel = 1;					//一个通道
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;				//非扫描
	ADC_Init(ADC1,&ADC_InitStruct);
	//使能ADC1
	ADC_Cmd(ADC1,ENABLE);
	
	//ADC校准
	ADC_ResetCalibration(ADC1);
	//获取ADC校准起存器状态
	while(ADC_GetResetCalibrationStatus(ADC1));
	//开启校准
	ADC_StartCalibration(ADC1);
	//等待校准完成
	while(ADC_GetCalibrationStatus(ADC1));
}


/***************************************************
*函数名    ：Adc1_GetValue
*函数功能  ：ADC获取电池电压转换数字量数据
*函数参数  ：u8 channel
*函数返回值：返回转换的数字量数据
*函数描述  ：
****************************************************/
u16 Adc1_GetValue(u8 channel)
{
	int i = 0;
	int j = 0;
	u16 temp = 0;
	u16 buff[10];
	//选择通道和采样周期
	ADC_RegularChannelConfig(ADC1,channel,1,ADC_SampleTime_239Cycles5);
	
	for(i = 0;i < 10;i++)
	{
		//软件触发
		ADC_SoftwareStartConvCmd(ADC1,ENABLE);
		//等待转换完成
		while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
		buff[i] =  ADC_GetConversionValue(ADC1);
//		temp =  ADC_GetConversionValue(ADC1);
	}
	//软件滤波  去掉最大值 去掉最小值
	//1.蒙逼定理-----冒泡排序
	for(i = 0;i < 10 - 1;i++)
	{
		for(j = 0;j < 10 - i - 1;j++)
		{
			if(buff[j] > buff[j + 1])
			{
				temp = buff[j];
				buff[j] = buff[j + 1];
				buff[j + 1] = temp;
			}
		}
	}
	temp = 0;
	for(i = 1;i < 9;i++)
	{
		temp += buff[i];
	}
	temp /= 8;
	return temp;
	
}

/***************************************************
*函数名    ：Adc1_GetValue
*函数功能  ：ADC电池电压计算结果
*函数参数  ：无
*函数返回值：无
*函数描述  ：计算出转换后的电池电压大小
****************************************************/
void ADC_Get(void)
{
	u16 temp = 0;
	float V_Arm = 0;
	//获取GZ数字量并打印
	temp = Adc1_GetValue(10);
//	V_Arm = (1.4 / 4096) * temp * 6 + 6;
//printf("Varm：%.1f\r\n",V_Arm);
//	printf("电压数字量：%d\r\n",temp);
// 	printf("V:%f\r\n",(1.0+ temp)/207);	
//	printf("V:%f\r\n",(1.4 / 4096) * temp * 6 + 4);
	printf("V:%f\r\n",(3.3*temp) /4096 *6);
	if(V_Arm < 6.7)
	{
		MOTOR_POWER_OFF;
	}
}

