#ifndef __DELAY_H__
#define __DELAY_H__

#include "stm32f10x.h"

//定义延时源文件是否支持UCOSII
//0,不支持;1,支持
#define DELAY_SUPPORT_UCOSII 0

void Delay_Init(u8 SYSCLK);//延时初始化
void Delay_Xus(u32 Xus);//微秒级延时
void Delay_Xms(u16 Xms);//毫秒级延时
void StartHeartbeatClock(void);//开启心跳时钟

#endif
