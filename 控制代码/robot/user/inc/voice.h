#ifndef _VOICE_H
#define _VOICE_H

//函数声明和宏定义
#include "stm32f10x.h"                  // Device header


extern u8 Play[4];
extern u8 Stop[4];
extern u8 Next[4];
extern u8 Vol[4];
extern u8 Vol_Up[4];


void voice_init(u32 bound);
void Voice_SendData(u8 *cmd,u8 cmd_len);
void Voice_Play(u8 *name);



void Set_Song_Volume(unsigned char volume_num);


#endif
