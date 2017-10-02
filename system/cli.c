#include "stm32f10x.h"
#include "vuart.h"
#include "string.h"
//#include "led.h"
/*
uint8_t uart_buff[255];
uint8_t buff_cnt=0;
*/
typedef void (*process_cmd)(void);
typedef struct _CMD
{
	char *str;
	char *help;
	process_cmd  spectical_cmd_process;
}PC_CMD;

PC_CMD PC_CMD_TAB[]=
{
	{"help",    "List all of the valid commands.",help_process},
//	{"led1",    "Led1 state.",led1_process},
//	{"led2on",  "Enable led2.",led2on_process},
//	{"led2off", "Disable led2.",led2off_process},
	{"reset",   "Software reset mcu.",reset_process},
	{"debug",   "set debug option.",set_debug_option},
};

int debug_options=0xFF;
#define DEBUG_ONENET 1<<0
#define DEBUG_ADC    1<<3
#define DEBUG_GPS    1<<5

int is_debug_onenet(){
	return debug_options&DEBUG_ONENET;
}
int is_debug_adc(){
	return debug_options&DEBUG_ADC;
}
int is_debug_gps(){
	return debug_options&DEBUG_GPS;
}

void find_and_exec_cmd(uint8_t *cmd)
{	
	uint8_t  cnt=0;
	serialprintf(">>");
	VUART_Send(cmd, strlen(cmd));
	for(cnt=0;cnt<sizeof(PC_CMD_TAB)/sizeof(PC_CMD_TAB[0]);cnt++)
	{
		if(strstr((char*)cmd, PC_CMD_TAB[cnt].str)!=NULL)
		{
			vusart_cur_len=0;
			PC_CMD_TAB[cnt].spectical_cmd_process();
			return;
		}
	}
	vusart_cur_len=0;
	serialprintf("\r\nCmd error.\r\n");
}


void isr_cli_process()
{
	uint8_t data=USART_buf[vusart_cur_len-1];
	if(data==0x0d || data==0x0a || vusart_cur_len==USART_BUF_MAX_LEN-1)
	{
		USART_buf[vusart_cur_len]='\0';
		find_and_exec_cmd(USART_buf);
	}
}

void help_process()
{
	uint8_t cnt;
	
	serialprintf("\r\n");
	for(cnt=0;cnt<sizeof(PC_CMD_TAB)/sizeof(PC_CMD_TAB[0]);cnt++)
	{
		//"%s: %s \r\n", ,PC_CMD_TAB[cnt].str,PC_CMD_TAB[cnt].help
		serialprintf(PC_CMD_TAB[cnt].str);
		serialprintf(": ");
		serialprintf(PC_CMD_TAB[cnt].help);
		serialprintf(" \r\n");
	}
	return ;
}
#if 0
void led1_process()
{
	if(GPIO_ReadOutputDataBit(GPIOA, LED1))
	{
		serialprintf("\r\nled1 on\r\n");
	}
	else
	{
		serialprintf("\r\nled1 off\r\n");
	}
	return;
}
void led2on_process()
{
	GPIO_SetBits(GPIOA, LED2);
	serialprintf("\r\nenable led2\r\n");
	return;
}
void led2off_process(void)
{
	GPIO_ResetBits(GPIOA, LED2);
	serialprintf("\r\ndisable led2\r\n");
	return;
}
int fputc(int ch, FILE *f)
{
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    USART_SendData(USART2, (uint8_t) ch);
	return ch;
}

#endif
void reset_process()
{
	serialprintf("\r\nsoftware reset\r\n");
	__set_FAULTMASK(1);     
	NVIC_SystemReset();
}

void set_debug_option()
{
	serialprintf("\r\nset debug options\r\n");
	if (strstr(USART_buf, "onenet")) {
		debug_options^=DEBUG_ONENET;
	}
	if (strstr(USART_buf, "adc")) {
		debug_options^=DEBUG_ADC;
	}
	if (strstr(USART_buf, "gps")) {
		debug_options^=DEBUG_GPS;
	}
	return;
}
