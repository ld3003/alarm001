#include "atcmd.h"
#include <stdio.h>
#include <BSP_Uart.h>

unsigned char recvbuf[UART_RECV_BUFFER_LEN];
int recvbuffer_length = 0;
unsigned char status_recv = STATUS_RECV_INIT;

void atcmd_recv_reset(void)
{
	recvbuffer_length = 0;
	status_recv = STATUS_RECV_INIT;
}

void uart_recv_handle(unsigned char data)
{
	
	switch(data)
	{
		case '+':
		{
			recvbuffer_length = 0;
			status_recv = STATUS_RECV_PLUS;
			break;
		}
		
		case 0x0a:
		{
			status_recv = STATUS_RECV_FINISH;
			break;
		}
		
	}

	switch(status_recv)
	{
		
		case STATUS_RECV_PLUS:
		{
			if (recvbuffer_length < UART_RECV_BUFFER_LEN)
				recvbuf[recvbuffer_length++] = data;
			break;
		}
		
		case STATUS_RECV_FINISH:
		{
			int i=0;
			printf("数据长度 %d\r\n",recvbuffer_length);
			printf("数据内容 %s\r\n",recvbuf);
			for(i=0;i<recvbuffer_length;i++)
			{
				EnQueue(&UartRingQueue,recvbuf[i]);
			}
			
			break;
		}
		
	}
	//
}

