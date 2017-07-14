#ifndef __serial_port_h__
#define __serial_port_h__

#define UART2_RX_BUF_LEN 1024
extern char *uart2_rx_buffer;
extern int uart2_rx_buffer_index;

void init_uart2_buffer(void);
void clear_uart2_buffer(void);
#endif

