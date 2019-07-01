#include "CAN.h"
#include "stm32f10x.h"
#include "LED.h"

/*��ʼ��CAN1ģ��*/
void STM32_CAN1_Init(void)
{
	//������������õĽṹ��
	GPIO_InitTypeDef GPIO_InitStructure;//GPIO
	CAN_InitTypeDef CAN_InitStructure;//CAN
  	CAN_FilterInitTypeDef CAN_FilterInitStructure;//CAN��ʶ��ɸѡ��
   	NVIC_InitTypeDef NVIC_InitStructure;

    //ʹ�����ʱ��
    /* ���ù��ܺ�GPIOB�˿�ʱ��ʹ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
    /* CAN1 ģ��ʱ��ʹ�� */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    //��ʼ��PB9, CAN1_TX
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//�����������
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //��ʼ��PB8, CAN1_RX
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //��������Remap, ����CAN�ڲ����շ��޷����ӵ�GPIO
    GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);

	/* ����CAN������ */
	CAN_InitStructure.CAN_TTCM=DISABLE;	//��ʱ�䴥��ͨ��ģʽ
  	CAN_InitStructure.CAN_ABOM=DISABLE;	//Ӳ���Զ����߹���
  	CAN_InitStructure.CAN_AWUM=DISABLE; //˯��ģʽͨ��Ӳ������(���CAN->MCR��SLEEPλ)
  	CAN_InitStructure.CAN_NART=DISABLE;	//��ֹ�����Զ�����
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ�
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//���ȼ��ɱ��ı�ʶ������
  	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;//CAN_Mode_LoopBack;//CAN_Mode_Normal;//ģʽ����

    /* ��ʼ��Ϊ500K�Ĳ����� CAN������=APB����Ƶ��/BRP��Ƶ��/(1+tBS1+tBS2) */
  	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;	//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=CAN_BS1_10tq; //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=CAN_BS2_7tq;  //Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=4;      //��Ƶϵ��(Fdiv)Ϊbrp+1
  	CAN_Init(CAN1, &CAN_InitStructure);

	//���ù�����
 	CAN_FilterInitStructure.CAN_FilterNumber=0;	  //������0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000; //32λID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//��������κα���
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;//���������0
  	CAN_FilterInit(&CAN_FilterInitStructure);

	/*ʹ���ж�*/
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0��Ϣ�Һ��ж�����	    
  	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // �����ȼ�Ϊ1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
}

/*���ͱ��ĵ�����
* �ɹ������� 1
*/
int STM32_TX_CAN_Transmit(NMPDU_t* NMPDU)
{
    int i = 0;
    NMTypeU8_t mbox;  //���ķ���������
    CanTxMsg TxMessage;

    //����ʹ��
    //printf("STM32_TX_CAN_Transmit\r\n");

    TxMessage.StdId=NMPDU->MsgID;
    //TxMessage.ExtId = NMPDU->MsgID;
    TxMessage.ExtId = 0;

    TxMessage.IDE = CAN_Id_Standard;    //ʹ�ñ�׼��ʶ��
    TxMessage.RTR = CAN_RTR_Data;       //��Ϣ����Ϊ����֡
    TxMessage.DLC = OSEKNM_DLC;

    TxMessage.Data[0] = NMPDU->MsgDA;
    TxMessage.Data[1] = NMPDU->MsgCtl;
    for(i=2; i<OSEKNM_DLC; i++)
        TxMessage.Data[i]=NMPDU->MsgData[i-2];//��һ֡��Ϣ
    mbox = CAN_Transmit(CAN1, &TxMessage);

    i=0;//�ȴ����ͽ���
    while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF)) i++;
    if(i >= 0XFFF) return 0;
    return 1;
}
//CAN�жϷ�����
void CAN1_RX0_IRQHandler(void)
{
    int i=0;
    CanRxMsg RxMessage;
    NMPDU_t NMPDU;
    static unsigned int j = 0;
    j++;

    //����ʹ��
    //printf("CAN1_RX0_IRQHandler\r\n");

    CAN_Receive(CAN1, 0, &RxMessage);

    /*��ʼ��NMPDU*/
    NMPDU.MsgCtl = RxMessage.Data[1];
    NMPDU.MsgDA = RxMessage.Data[0];
    NMPDU.MsgID = RxMessage.StdId;
    for(i=2;i<8;i++)
    {
        NMPDU.MsgData[i-2] = RxMessage.Data[i];
    }
    //�����ķ��뻺����
    Recv_EveryMessage(&NMPDU);

    /*LED�ƹ���*/
    GPIO_WriteBit(GPIOC, GPIO_Pin_14,(j%2)?Bit_SET:Bit_RESET);
}
