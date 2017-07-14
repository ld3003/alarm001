#include "serialport.h"
#include "bsp.h"
#include "common.h"
#include "mem.h"

#include <string.h>

char *uart2_rx_buffer;
int uart2_rx_buffer_index;

void clear_uart2_buffer(void)
{
	memset(uart2_rx_buffer,0x0,UART2_RX_BUF_LEN);
	uart2_rx_buffer_index = 0;
}

void init_uart2_buffer(void)
{
	uart2_rx_buffer = (char*)alloc_mem(__FILE__,__LINE__,UART2_RX_BUF_LEN);
}

