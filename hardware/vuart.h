#ifndef _VUART_H  
#define _VUART_H  
//#include "sys.h"  
//#include "delay.h"  
#include "cli.h"
#include "Common.h"


#define USART_BUF_MAX_LEN    255

extern u8 vusart_cur_len;    //接收计数
extern u8 USART_buf[USART_BUF_MAX_LEN];  //接收缓冲区
/*
extern uint8_t uart_buff[255];
extern uint8_t buff_cnt;
extern uint8_t data_flag;
*/
void VUART_Send(u8 *buf, u8 len);
void VUART_IOConfig(void);
void VUART_TIM4_Int_Init(u16 arr,u16 psc);
void serialprintf(const char *str);

#endif 