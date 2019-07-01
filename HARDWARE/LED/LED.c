#include "stm32f10x.h"
#include "LED.h"

void LED_Init(void)//��ʼ��LED
{
	GPIO_InitTypeDef GPIO_Config_Struct;

	//ʹ��IO��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	//��ʼ��PC13
	GPIO_Config_Struct.GPIO_Mode=GPIO_Mode_Out_PP;//ͨ���������
	GPIO_Config_Struct.GPIO_Speed=GPIO_Speed_50MHz;//����ٶ�50Mhz
	GPIO_Config_Struct.GPIO_Pin=GPIO_Pin_14;
	GPIO_Init(GPIOC,&GPIO_Config_Struct);

	//LEDĬ�ϲ���
	LED = 1;
}
