#ifndef __DELAY_H__
#define __DELAY_H__

#include "stm32f10x.h"

//������ʱԴ�ļ��Ƿ�֧��UCOSII
//0,��֧��;1,֧��
#define DELAY_SUPPORT_UCOSII 0

void Delay_Init(u8 SYSCLK);//��ʱ��ʼ��
void Delay_Xus(u32 Xus);//΢�뼶��ʱ
void Delay_Xms(u16 Xms);//���뼶��ʱ
void StartHeartbeatClock(void);//��������ʱ��

#endif
