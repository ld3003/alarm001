#ifndef __AT_CMD__
#define __AT_CMD__

#define UART_RECV_BUFFER_LEN	128

/**
AT指令处理使用方法

通过状态机检查来获取当前缓冲区的状态
当缓冲区的状态为 STATUS_RECV_FINISH 的时候代表缓冲区已经有了一个合法帧
针对合法帧做相应处理，处理结束后充值状态机

atcmd_recv_reset 调用此函数后，缓冲区继续接收新的帧

while

*/


enum {
	STATUS_RECV_INIT,				/// 接收初始化
	STATUS_RECV_PLUS,				/// 收到 +
	STATUS_RECV_A,					
	STATUS_RECV_T,
	STATUS_RECV_0D,					/// 收到0X0D
	STATUS_RECV_0A,					/// 收到0X0A
	STATUS_RECV_FINISH,			/// 收到有效数据
	STATUS_RECV_ERROR,			/// 接收错误
};
extern int recvbuffer_length;
extern unsigned char recvbuf[UART_RECV_BUFFER_LEN];
extern unsigned char status_recv;

void uart_recv_handle(unsigned char data);		/// 串口中断调用此函数，用于处理接收到的串口数据
void atcmd_recv_reset(void);									/// 缓冲区复位函数
void process_cmddata(void);

#endif
