#ifndef __mainloop_h__
#define __mainloop_h__

#include "setting.h"

typedef enum {
	SYS_INIT,
	SYS_INIT2,
	SYS_INIT3,
	SYS_POWERON,
	SYS_POWEROFF,
	MODEM_POWEROFF,
	MODEM_RESET,
	MODEM_POWERACTIVE,
	MODEM_POWERACTIVE_TESTAT,
	
	MODEM_GET_IMEI,
	MODEM_ATD,
	MODEM_CHECK_MODEM_TYPE,
	
	MODEM_POWERON,
	
	MODEM_CPIN,
	
	MODEM_CHECK_CSQ,
	
	MODEM_GPRS_READY,								//GPRS已经准备好
	MODEM_GPRS_CGDCONT,
	MODEM_GPRS_CGACT,
	MODEM_GPRS_CGATT,
	
	MODEM_GPRS_MIPCALL_SUCCESS,			//进行网络注册
	MODEM_GPRS_MIPOPEN_SUCCESS,			//创建SOCKET连接
	
	PROTO_CHECK_1091,
	PROTO_SEND_ALARM,
	PROTO_RECV_2091,
	START_SEND_IMG,
	PROTO_SEND_IMG,
	PROTO_SEND_IMG_SUCCESS,
	PROTO_SEND_IMG_ERROR,
	
	PROTO_UPLOAD_DONE_GOTO_SLEEP,
	
	PROTO_ALARM,
	
	MDATA_STATUS_NULL,
	
} SYSTEM_STATUS;

enum {
	DOING_10A0 = 0,
	DOING_10A1,
};

struct MAINLOOP_DATA {
	SYSTEM_STATUS status;								//状态机
	unsigned char doing;								//
	
	unsigned char _10a0type;
	
	//计数器
	unsigned char modem_reset_cnt;			//模块重启计数器
	unsigned char test_at_cnt;
	
	unsigned char send_10a0_cnt;
	
	unsigned char pictype;							//图片类型，测试？实际?
	
	unsigned int time1;
	unsigned int time2;
	
	unsigned int paizhao_time;
	
	unsigned char upload_index;					//上传索引
	unsigned char need_uload_cnt;				//需要上传几张
	
	unsigned short voltage;
	
};


extern struct MAINLOOP_DATA mdata;
void mainloop_init(void);							//程序初始化
void mainloop(void);									//程序循环

int push_data_A6(unsigned char *data , int length , unsigned char *outdata , int timeout);

#endif
