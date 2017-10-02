

//ͷ�ļ�
#include "stm32f10x.h"
#include "GPIOLIKE51.h"
#include "usart1.h"
#include "delay.h"
#include "string.h"

//��������
void GPIO_Configuration(void);

//=============================================================================
//�ļ����ƣ�Delay
//���ܸ�Ҫ����ʱ
//����˵����nCount����ʱ����
//�������أ���
//=============================================================================

void Delay(uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}


//=============================================================================
//�ļ����ƣ�main
//���ܸ�Ҫ��������
//����˵������
//�������أ�int
//=============================================================================
int main(void)
{
	char* msg="usart 1 sending...\r\n";
	  GPIO_Configuration();
	  delay_init();
	  USART1_Init(); //debug for the uvart only, remove after then
	// vuart init
    VUART_IOConfig();
    VUART_TIM4_Int_Init(107, 71);     //1M����Ƶ��
    while (1)
	{
		PCout(13)=1;
		Delay(0xfffff);
//		Delay(0xfffff);
//		Delay(0xfffff);
//		Delay(0xfffff);
		PCout(13)=0;
		usart1_write(USART1, msg, strlen(msg));
	    serialprintf("vuart is ready...\r\n");
		Delay(0xfffff);
//		Delay(0xfffff);
//		Delay(0xfffff);
//		Delay(0xfffff);		
    }
}

//=============================================================================
//�ļ����ƣ�GPIO_Configuration
//���ܸ�Ҫ��GPIO��ʼ��
//����˵������
//�������أ���
//=============================================================================
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE); 						 
//=============================================================================
//LED -> PC13
//=============================================================================			 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}



