#include "stm32f10x.h"
#include "delay.h"
#include "usart1.h"
#include "string.h"
/* usart1 is for M6312 */


volatile unsigned char  rcv_cmd_start=0;
volatile unsigned char  rcv_cmd_flag=0;

unsigned char  usart1_rcv_buf[MAX_RCV_LEN];
volatile unsigned int   usart1_rcv_len=0;

unsigned char  usart1_cmd_buf[MAX_CMD_LEN];
volatile unsigned int   usart1_cmd_len=0;


/**
  * @brief  USART1初始化函数
**/
void USART1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
    /* config USART1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    /* USART1 GPIO config */
    /* Configure USART1 Tx (PA.09) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);    
    /* Configure USART1 Rx (PA.10) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
      
    /* USART1 mode config */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure); 
    USART_Cmd(USART1, ENABLE);
		
	//Enable usart1 receive interrupt
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 
		
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);  
}


/**
  * @brief  USART1发送一个字符串
**/
void usart1_write(USART_TypeDef* USARTx, uint8_t *Data,uint8_t len)
{
    uint8_t i;

    for(i=0; i<len; i++)
    {          
	    USART1->SR; //stm32在复位时TC位被置1, 解决方法是在发送前先将TC为清零                              
        USART_SendData(USARTx, *Data++);
        while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
    }
}

/**
  * @brief  发送一条AT指令
**/
void SendCmd(char* cmd, char* result, int timeOut)
{
    while(1)
    {
        memset(usart1_rcv_buf,0,sizeof(usart1_rcv_buf)); //FIXME
		usart1_rcv_len=0;
        usart1_write(USART1, (unsigned char *)cmd, strlen((const char *)cmd));
        delay_ms(timeOut);
       
        if((NULL != strstr((const char *)usart1_rcv_buf, result)))	//判断是否有预期的结果
        {
            break;
        }
        else
        {
            delay_ms(100);
        }
    }
}

#if 0
/**
  * @brief  This function handles USART1 global interrupt request.
  * @param  None
  * @retval : None
  */
void USART1_IRQHandler(void)      //串口1 中断服务程序
{
  unsigned int i;
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)	   //判断读寄存器是否非空
  {	
    
    RxBuffer1[RxCounter1++] = USART_ReceiveData(USART1);   //将读寄存器的数据缓存到接收缓冲区里
	
    if(RxBuffer1[RxCounter1-2]==0x0d&&RxBuffer1[RxCounter1-1]==0x0a)     //判断结束标志是否是0x0d 0x0a
    {
	  for(i=0; i< RxCounter1; i++) TxBuffer1[i]	= RxBuffer1[i]; 	     //将接收缓冲器的数据转到发送缓冲区，准备转发
	  rec_f=1;															 //接收成功标志
	  TxBuffer1[RxCounter1]=0;		                                     //发送缓冲区结束符    
	  TxCounter1=RxCounter1;
	  RxCounter1=0;
    }
  }
  
  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)                   //这段是为了避免STM32 USART 第一个字节发不出去的BUG 
  { 
     USART_ITConfig(USART1, USART_IT_TXE, DISABLE);					     //禁止发缓冲器空中断， 
  }	
  
}
#endif

