#include "stm32f10x.h"
#include "PRINTF.h"

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//��׼����Ҫ��֧�ֺ���
struct __FILE
{
	int handle;
};

FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
	x = x;
}
//�ض���fputc����
int fputc(int ch, FILE *f)
{
	while((USART1->SR&0X40)==0){}//ѭ������,ֱ���������
    USART1->DR = (u8) ch;
	return ch;
}
#endif

//��ʼ��printf����
void Printf_Init(u32 baud)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	//ʹ��USART1,GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA,ENABLE);

	//��λ����1
	USART_DeInit(USART1);

	//��ʼ��PA9
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;//����ٶ�50MHz
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//�����������
    GPIO_Init(GPIOA,&GPIO_InitStructure);

	//��ʼ��USART1
	USART_InitStructure.USART_BaudRate=baud;//������=baud
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;//����λ����8
	USART_InitStructure.USART_StopBits=USART_StopBits_1;//1λֹͣλ
	USART_InitStructure.USART_Parity=USART_Parity_No;//����żУ��
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//��Ӳ��������
	USART_InitStructure.USART_Mode=USART_Mode_Tx;//����ģʽ
    USART_Init(USART1,&USART_InitStructure);

	//ʹ�ܴ���1
    USART_Cmd(USART1,ENABLE);
}
