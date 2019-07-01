#include "stm32f10x.h"
#include "DELAY.h"

#if DELAY_SUPPORT_UCOSII
#include "includes.h"
#endif

static u8  fac_us=0;
static u16 fac_ms=0;

#if DELAY_SUPPORT_UCOSII

//SysTick中断服务函数
void SysTick_Handler(void)
{
	OSIntEnter();
	OSTimeTick();
	OSIntExit();
}
//开启心跳时钟
void StartHeartbeatClock(void)
{
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;//关闭计数器
	SysTick->VAL=0x00;//清空计数器
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;//开启SYSTICK中断
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;//开启计数器,产生心跳时钟
}

#endif

//延时初始化
void Delay_Init(u8 SYSCLK)
{

#if DELAY_SUPPORT_UCOSII
	u32 reload;
#endif

	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	fac_us=SYSCLK/8;
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;//先关闭计数器

#if DELAY_SUPPORT_UCOSII
	reload=SYSCLK/8;
	reload*=1000000/OS_TICKS_PER_SEC;

	fac_ms=1000/OS_TICKS_PER_SEC;
	SysTick->LOAD=reload;//重装载值
	SysTick->VAL=0x00;//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;//开始计数
#else
	fac_ms=(u16)fac_us*1000;
#endif

}

//有UCOSII下的延时函数
#if DELAY_SUPPORT_UCOSII

//延时Xus
//Xus:0~204522252
void Delay_Xus(u32 Xus)
{
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;

	if(Xus==0)return;
	ticks=Xus*fac_us;
	OSSchedLock();//禁止OS调度
	told=SysTick->VAL;
	while(1)
	{
		tnow=SysTick->VAL;
		if(tnow!=told)
		{
			if(tnow<told)tcnt+=told-tnow;
			else tcnt+=reload-tnow+told;
			told=tnow;
			if(tcnt>=ticks)break;//延时时间到
		}
	}
	OSSchedUnlock();//恢复OS调度
}
//延时Xms
//Xms:0~65535
void Delay_Xms(u16 Xms)
{
	if(Xms==0)return;
	//UCOSII开始运行&&且不在中断中
	if(OSRunning&&OSIntNesting==0)
	{
		if(Xms>=fac_ms)
		{
			OSTimeDly(Xms/fac_ms);//UCOSII延时
		}
		Xms%=fac_ms;
	}
	Delay_Xus((u32)(Xms*1000));//普通方式延时
}

//裸机下的延时函数
#else

//延时Xus
//Xus:0~798915us
void Delay_Xus(u32 Xus)
{
	u32 temp;

	if(Xus==0)return;
	SysTick->LOAD=Xus*fac_us;//时间加载
	SysTick->VAL=0x00;//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;//开始倒数
	do
	{
		temp=SysTick->CTRL;
	}
	while((temp&0x01)&&!(temp&(1<<16)));//等待时间到达

	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;//关闭计数器
	SysTick->VAL=0X00;//清空计数器
}
//延时xms
void delay_xms(u16 xms)
{
	u32 temp;

	SysTick->LOAD=(u32)xms*fac_ms;//加载数值
	SysTick->VAL=0x00;//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;//开始计数
	do
	{
		temp=SysTick->CTRL;
	}
	while((temp&0x01)&&!(temp&(1<<16)));//等待时间到达

	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;//关闭计数器
	SysTick->VAL=0X00;//清空计数器
}
//延时Xms
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
