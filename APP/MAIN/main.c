#include "stm32f10x.h"
#include "PRINTF.h"
#include "Delay.h"
#include "LED.h"

#include "OsekNM.h"
#include "Driver_Common.h" //平台相关的配置
#include "OsekNMServer.h"


//SYSCLK_Frequency = 72000000
//HCLK_Frequency = 72000000
//PCLK1_Frequency = 36000000
//PCLK2_Frequency = 72000000
//ADCCLK_Frequency = 36000000


int main(void)
{
    //中断优先级分组2
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init(72);
    Printf_Init(115200);
    LED_Init();

	printf("at main\r\n");

    StartNM();
}



/*
RCC_ClocksTypeDef get_rcc_clock;
RCC_GetClocksFreq(&get_rcc_clock);
printf("SYSCLK_Frequency = %d\r\n", get_rcc_clock.SYSCLK_Frequency);
printf("HCLK_Frequency = %d\r\n", get_rcc_clock.HCLK_Frequency);
printf("PCLK1_Frequency = %d\r\n", get_rcc_clock.PCLK1_Frequency);
printf("PCLK2_Frequency = %d\r\n", get_rcc_clock.PCLK2_Frequency);
printf("ADCCLK_Frequency = %d\r\n\r\n", get_rcc_clock.ADCCLK_Frequency);
Delay_Xms(1000);
*/
