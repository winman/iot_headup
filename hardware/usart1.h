#ifndef USART1_H_H
#define USART1_H_H

#define MAX_RCV_LEN  128
#define MAX_CMD_LEN  64

extern void USART1_Init(void);
extern void usart1_write(USART_TypeDef* USARTx, uint8_t *Data,uint8_t len);
extern void SendCmd(char* cmd, char* result, int timeOut);

extern volatile unsigned char  rcv_cmd_start;
extern volatile unsigned char  rcv_cmd_flag;

extern unsigned char  usart1_rcv_buf[MAX_RCV_LEN];
extern volatile unsigned int   usart1_rcv_len;

extern unsigned char  usart1_cmd_buf[MAX_CMD_LEN];
extern volatile unsigned int   usart1_cmd_len;

#endif

