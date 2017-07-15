#ifndef __BSP__
#define __BSP__

#include "rtc.h"

	
#define MODEM_POWER_RCC_TYPE	RCC_APB2PeriphClockCmd
#define MODEM_POEWR_RCC				RCC_APB2Periph_GPIOB
#define MODEM_POEWR_GPIO			GPIOB
#define MODEM_POEWR_PIN				GPIO_Pin_10

void init_uart1(void);
void init_uart2(void);

void uart1_putchar(unsigned char data);
void uart2_putchar(unsigned char data);

void modem_poweron(void);
void modem_poweroff(void);


#endif

