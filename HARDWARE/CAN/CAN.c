#include "CAN.h"
#include "stm32f10x.h"
#include "LED.h"

/*初始化CAN1模块*/
void STM32_CAN1_Init(void)
{
	//定义相关配置用的结构体
	GPIO_InitTypeDef GPIO_InitStructure;//GPIO
	CAN_InitTypeDef CAN_InitStructure;//CAN
  	CAN_FilterInitTypeDef CAN_FilterInitStructure;//CAN标识符筛选器
   	NVIC_InitTypeDef NVIC_InitStructure;

    //使能相关时钟
    /* 复用功能和GPIOB端口时钟使能 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
    /* CAN1 模块时钟使能 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    //初始化PB9, CAN1_TX
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //初始化PB8, CAN1_RX
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //必须加这个Remap, 否则CAN内部的收发无法连接到GPIO
    GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);

	/* 配置CAN控制器 */
	CAN_InitStructure.CAN_TTCM=DISABLE;	//非时间触发通信模式
  	CAN_InitStructure.CAN_ABOM=DISABLE;	//硬件自动离线管理
  	CAN_InitStructure.CAN_AWUM=DISABLE; //睡眠模式通过硬件唤醒(清除CAN->MCR的SLEEP位)
  	CAN_InitStructure.CAN_NART=DISABLE;	//禁止报文自动传送
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//报文不锁定,新的覆盖旧的
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//优先级由报文标识符决定
  	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;//CAN_Mode_LoopBack;//CAN_Mode_Normal;//模式设置

    /* 初始化为500K的波特率 CAN波特率=APB总线频率/BRP分频器/(1+tBS1+tBS2) */
  	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=CAN_BS1_10tq; //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=CAN_BS2_7tq;  //Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=4;      //分频系数(Fdiv)为brp+1
  	CAN_Init(CAN1, &CAN_InitStructure);

	//配置过滤器
 	CAN_FilterInitStructure.CAN_FilterNumber=0;	  //过滤器0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000; //32位ID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//允许接收任何报文
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;//激活过滤器0
  	CAN_FilterInit(&CAN_FilterInitStructure);

	/*使能中断*/
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许	    
  	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
}

/*发送报文到总线
* 成功：返回 1
*/
int STM32_TX_CAN_Transmit(NMPDU_t* NMPDU)
{
    int i = 0;
    NMTypeU8_t mbox;  //报文放入的邮箱号
    CanTxMsg TxMessage;

    //调试使用
    //printf("STM32_TX_CAN_Transmit\r\n");

    TxMessage.StdId=NMPDU->MsgID;
    //TxMessage.ExtId = NMPDU->MsgID;
    TxMessage.ExtId = 0;

    TxMessage.IDE = CAN_Id_Standard;    //使用标准标识符
    TxMessage.RTR = CAN_RTR_Data;       //消息类型为数据帧
    TxMessage.DLC = OSEKNM_DLC;

    TxMessage.Data[0] = NMPDU->MsgDA;
    TxMessage.Data[1] = NMPDU->MsgCtl;
    for(i=2; i<OSEKNM_DLC; i++)
        TxMessage.Data[i]=NMPDU->MsgData[i-2];//第一帧信息
    mbox = CAN_Transmit(CAN1, &TxMessage);

    i=0;//等待发送结束
    while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF)) i++;
    if(i >= 0XFFF) return 0;
    return 1;
}
//CAN中断服务函数
void CAN1_RX0_IRQHandler(void)
{
    int i=0;
    CanRxMsg RxMessage;
    NMPDU_t NMPDU;
    static unsigned int j = 0;
    j++;

    //调试使用
    //printf("CAN1_RX0_IRQHandler\r\n");

    CAN_Receive(CAN1, 0, &RxMessage);

    /*初始化NMPDU*/
    NMPDU.MsgCtl = RxMessage.Data[1];
    NMPDU.MsgDA = RxMessage.Data[0];
    NMPDU.MsgID = RxMessage.StdId;
    for(i=2;i<8;i++)
    {
        NMPDU.MsgData[i-2] = RxMessage.Data[i];
    }
    //将报文放入缓冲区
    Recv_EveryMessage(&NMPDU);

    /*LED灯光闪*/
    GPIO_WriteBit(GPIOC, GPIO_Pin_14,(j%2)?Bit_SET:Bit_RESET);
}
