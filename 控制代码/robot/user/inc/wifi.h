#ifndef _WIFI_H
#define _WIFI_H

//函数声明和宏定义
#include "stm32f10x.h"                  // Device header

typedef struct{
	
	u8 buff[512];   //保存字符串
	u16 len;        //记录接收数据长度
	u8 flag;        //接收完成标志位

}U2_STRUCT;

extern U2_STRUCT u2;

u8 ESP8266_Init(void);
void usart2_send_str(u8 *data);
u8 ESP8266_ConnectWifi(u8 *ssid,u8 *pwd);
u8 ESP8266_ConnectServer(u8 *ip,u8 *port);
u8 Esp8266_ConnectServer(u8 *ip,u8 *port);
#endif
