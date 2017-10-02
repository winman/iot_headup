#include "sys.h"
#include "delay.h"
#include "stm32f10x.h"
#include "vuart.h"
#include "stdio.h"

/**
*软件串口的实现(IO模拟串口)
* 波特率：9600    1-8-N
* TXD : PB7
* RXD : PB8
* 使用外部中断对RXD的下降沿进行触发，使用定时器4按照9600波特率进行定时数据接收。
* Demo功能: 接收11个数据，然后把接收到的数据发送出去
* 8-N-1 is a common shorthand notation for a serial port parameter setting or 
* configuration in asynchronous mode, in which there are eight (8) data bits,
* no (N) parity bit, and one (1) stop bit.
*/

#define OI_TXD    PBout(7)
#define OI_RXD    PBin(8)

#define BuadRate_9600    100

u8 vusart_cur_len = 0;    //接收计数
u8 USART_buf[USART_BUF_MAX_LEN];  //接收缓冲区

enum{
    COM_START_BIT,
    COM_D0_BIT,
    COM_D1_BIT,
    COM_D2_BIT,
    COM_D3_BIT,
    COM_D4_BIT,
    COM_D5_BIT,
    COM_D6_BIT,
    COM_D7_BIT,
    COM_STOP_BIT,
};

u8 recvStat = COM_STOP_BIT;
u8 recvData = 0;

void IO_TXD(u8 Data)
{
    u8 i = 0;
    OI_TXD = 0;  
    delay_us(BuadRate_9600);
    for(i = 0; i < 8; i++)
    {
        if(Data&0x01)
            OI_TXD = 1;  
        else
            OI_TXD = 0;     
        
        delay_us(BuadRate_9600);
        Data = Data>>1;
    }
    OI_TXD = 1;
    delay_us(BuadRate_9600);
}
    
void VUART_Send(u8 *buf, u8 len)
{
    u8 t;
    for(t = 0; t < len; t++)
    {
        IO_TXD(buf[t]);
    }
}
void serialprintf(const char *str)
{
	VUART_Send((u8 *)str, strlen(str));
}

void VUART_IOConfig(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB, ENABLE);     //使能PB端口时钟 
     
     //SoftWare Serial TXD
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;        
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;          //推挽输出
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         //IO口速度为50MHz     
    GPIO_Init(GPIOB, &GPIO_InitStructure);                      
    GPIO_SetBits(GPIOB,GPIO_Pin_7);                         
     
     
    //SoftWare Serial RXD
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        
    GPIO_Init(GPIOB, &GPIO_InitStructure);     

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource8);
    EXTI_InitStruct.EXTI_Line = EXTI_Line8;
    EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling; //下降沿触发中断
    EXTI_InitStruct.EXTI_LineCmd=ENABLE;
    EXTI_Init(&EXTI_InitStruct);


    NVIC_InitStructure.NVIC_IRQChannel= EXTI9_5_IRQn; //EXTI9_5_IRQHandler
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;  
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;  
    NVIC_Init(&NVIC_InitStructure);  
    
}
 
void VUART_TIM4_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能
    
    //定时器TIM4初始化
    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值    
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
    TIM_ClearITPendingBit(TIM4, TIM_FLAG_Update);
    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

    //中断优先级NVIC设置
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级1级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级1级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器             
}

void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetFlagStatus(EXTI_Line8) != RESET)
    {
        if(OI_RXD == 0) 
        {
            if(recvStat == COM_STOP_BIT)
            {
                recvStat = COM_START_BIT;
                TIM_Cmd(TIM4, ENABLE);
            }
        }
        EXTI_ClearITPendingBit(EXTI_Line8);
    }
}

void TIM4_IRQHandler(void)
{  
    if(TIM_GetFlagStatus(TIM4, TIM_FLAG_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM4, TIM_FLAG_Update);    
        recvStat++;
        if(recvStat == COM_STOP_BIT)
        {
            TIM_Cmd(TIM4, DISABLE);
            USART_buf[vusart_cur_len++] = recvData;
			isr_cli_process(); // cli module continue processing
            return;
        }
        if(OI_RXD)
        {
            recvData |= (1 << (recvStat - 1));
        }else{
            recvData &= ~(1 << (recvStat - 1));
        }    
  }        
}

//override printf, default to virtual uart
int fputc(int ch,FILE *f)
{
    u8 cur_char = ch;
    VUART_Send(&cur_char, 1);
    //返回ch
    return(ch);
}