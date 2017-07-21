#include "bkpreg.h"
#include <stm32f10x.h>

unsigned int bkp32bit_read(unsigned short a , unsigned short b)
{
	
	unsigned int ret;
	
	ret = a;
	
	ret = ret << 16;
	
	ret += b;
	
	return ret;
	//
}

void bkp32bit_write(unsigned int val , unsigned short *a , unsigned short *b)
{
	unsigned int t1,t2;
	t1 = val;
	t2 = val;
	*a = t1 >> 16;
	*b = t2 & 0x00FF;
	
}

void __set_last_alarm_time(void)
{
	
	unsigned short reg1,reg2;
	unsigned int current_time = RTC_GetCounter();
	bkp32bit_write(current_time,&reg1,&reg2);
	
	BKP_WriteBackupRegister(BKP_DR3,reg1);
	BKP_WriteBackupRegister(BKP_DR4,reg2);
	//
}
unsigned int __get_last_alarm_time(void)
{
	return bkp32bit_read(BKP_ReadBackupRegister(BKP_DR3),BKP_ReadBackupRegister(BKP_DR4));
	//
}