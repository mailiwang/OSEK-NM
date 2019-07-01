#include "stm32f10x.h"
#include "PRINTF.h"

//加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
	int handle;
};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
	x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
	while((USART1->SR&0X40)==0){}//循环发送,直到发送完毕
    USART1->DR = (u8) ch;
	return ch;
}
#endif

//初始化printf函数
void Printf_Init(u32 baud)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	//使能USART1,GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA,ENABLE);

	//复位串口1
	USART_DeInit(USART1);

	//初始化PA9
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;//输出速度50MHz
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出
    GPIO_Init(GPIOA,&GPIO_InitStructure);

	//初始化USART1
	USART_InitStructure.USART_BaudRate=baud;//波特率=baud
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;//数据位长度8
	USART_InitStructure.USART_StopBits=USART_StopBits_1;//1位停止位
	USART_InitStructure.USART_Parity=USART_Parity_No;//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//无硬件流控制
	USART_InitStructure.USART_Mode=USART_Mode_Tx;//发送模式
    USART_Init(USART1,&USART_InitStructure);

	//使能串口1
    USART_Cmd(USART1,ENABLE);
}
