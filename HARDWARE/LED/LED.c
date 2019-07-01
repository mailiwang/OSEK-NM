#include "stm32f10x.h"
#include "LED.h"

void LED_Init(void)//初始化LED
{
	GPIO_InitTypeDef GPIO_Config_Struct;

	//使能IO口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	//初始化PC13
	GPIO_Config_Struct.GPIO_Mode=GPIO_Mode_Out_PP;//通用推挽输出
	GPIO_Config_Struct.GPIO_Speed=GPIO_Speed_50MHz;//输出速度50Mhz
	GPIO_Config_Struct.GPIO_Pin=GPIO_Pin_14;
	GPIO_Init(GPIOC,&GPIO_Config_Struct);

	//LED默认不亮
	LED = 1;
}
