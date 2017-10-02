#ifndef _CLI_H_
#define _CLI_H_

#include "stdio.h"
//#include "stm32l1xx.h"


extern int is_debug_onenet(void);
extern int is_debug_adc(void);
extern int is_debug_gps(void);

/*
extern uint8_t uart_buff[255];
extern uint8_t buff_cnt;
extern uint8_t data_flag;
*/
void isr_cli_process(void);

void help_process(void);
void led1_process(void);
void led2on_process(void);
void led2off_process(void);
void reset_process(void);
void set_debug_option(void);

#endif

