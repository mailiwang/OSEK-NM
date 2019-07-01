#include "stm32f10x.h"
#include "DELAY.h"

#if DELAY_SUPPORT_UCOSII
#include "includes.h"
#endif

static u8  fac_us=0;
static u16 fac_ms=0;

#if DELAY_SUPPORT_UCOSII

//SysTick�жϷ�����
void SysTick_Handler(void)
{
	OSIntEnter();
	OSTimeTick();
	OSIntExit();
}
//��������ʱ��
void StartHeartbeatClock(void)
{
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;//�رռ�����
	SysTick->VAL=0x00;//��ռ�����
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;//����SYSTICK�ж�
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;//����������,��������ʱ��
}

#endif

//��ʱ��ʼ��
void Delay_Init(u8 SYSCLK)
{

#if DELAY_SUPPORT_UCOSII
	u32 reload;
#endif

	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	fac_us=SYSCLK/8;
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;//�ȹرռ�����

#if DELAY_SUPPORT_UCOSII
	reload=SYSCLK/8;
	reload*=1000000/OS_TICKS_PER_SEC;

	fac_ms=1000/OS_TICKS_PER_SEC;
	SysTick->LOAD=reload;//��װ��ֵ
	SysTick->VAL=0x00;//��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;//��ʼ����
#else
	fac_ms=(u16)fac_us*1000;
#endif

}

//��UCOSII�µ���ʱ����
#if DELAY_SUPPORT_UCOSII

//��ʱXus
//Xus:0~204522252
void Delay_Xus(u32 Xus)
{
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;

	if(Xus==0)return;
	ticks=Xus*fac_us;
	OSSchedLock();//��ֹOS����
	told=SysTick->VAL;
	while(1)
	{
		tnow=SysTick->VAL;
		if(tnow!=told)
		{
			if(tnow<told)tcnt+=told-tnow;
			else tcnt+=reload-tnow+told;
			told=tnow;
			if(tcnt>=ticks)break;//��ʱʱ�䵽
		}
	}
	OSSchedUnlock();//�ָ�OS����
}
//��ʱXms
//Xms:0~65535
void Delay_Xms(u16 Xms)
{
	if(Xms==0)return;
	//UCOSII��ʼ����&&�Ҳ����ж���
	if(OSRunning&&OSIntNesting==0)
	{
		if(Xms>=fac_ms)
		{
			OSTimeDly(Xms/fac_ms);//UCOSII��ʱ
		}
		Xms%=fac_ms;
	}
	Delay_Xus((u32)(Xms*1000));//��ͨ��ʽ��ʱ
}

//����µ���ʱ����
#else

//��ʱXus
//Xus:0~798915us
void Delay_Xus(u32 Xus)
{
	u32 temp;

	if(Xus==0)return;
	SysTick->LOAD=Xus*fac_us;//ʱ�����
	SysTick->VAL=0x00;//��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;//��ʼ����
	do
	{
		temp=SysTick->CTRL;
	}
	while((temp&0x01)&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��

	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;//�رռ�����
	SysTick->VAL=0X00;//��ռ�����
}
//��ʱxms
void delay_xms(u16 xms)
{
	u32 temp;

	SysTick->LOAD=(u32)xms*fac_ms;//������ֵ
	SysTick->VAL=0x00;//��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;//��ʼ����
	do
	{
		temp=SysTick->CTRL;
	}
	while((temp&0x01)&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��

	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;//�رռ�����
	SysTick->VAL=0X00;//��ռ�����
}
//��ʱXms
//Xms:0~65535
void Delay_Xms(u16 Xms)
{
	u8 repeat=Xms/540;
	u16 remain=Xms%540;

	if(Xms==0)return;
	while(repeat)
	{
		delay_xms(540);
		repeat--;
	}
	if(remain)delay_xms(remain);
}

#endif
