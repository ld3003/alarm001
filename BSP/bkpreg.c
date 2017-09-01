#include "bkpreg.h"
#include <stm32f10x.h>
#include <stm32f10x_bkp.h>
#include <stm32f10x_rtc.h>

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
	*b = t2 & 0x0000FFFF;
	
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

void __set_next_wakeup_time(unsigned int time)
{
	unsigned short reg1,reg2;
	bkp32bit_write(time,&reg1,&reg2);
	
	BKP_WriteBackupRegister(BKP_DR6,reg1);
	BKP_WriteBackupRegister(BKP_DR7,reg2);
	//
}
unsigned int __get_next_wakeup_time(void)
{
	return bkp32bit_read(BKP_ReadBackupRegister(BKP_DR6),BKP_ReadBackupRegister(BKP_DR7));
}

void read_bkp_buffer(unsigned char*buf)
{
	unsigned char i=0;
	unsigned short *p;
	p = (unsigned short *)buf;
	
	p[i++] = BKP_ReadBackupRegister(BKP_DR1);
	p[i++] = BKP_ReadBackupRegister(BKP_DR2);
	p[i++] = BKP_ReadBackupRegister(BKP_DR3);
	p[i++] = BKP_ReadBackupRegister(BKP_DR4);
	p[i++] = BKP_ReadBackupRegister(BKP_DR5);
	p[i++] = BKP_ReadBackupRegister(BKP_DR6);
	p[i++] = BKP_ReadBackupRegister(BKP_DR7);
	p[i++] = BKP_ReadBackupRegister(BKP_DR8);
	p[i++] = BKP_ReadBackupRegister(BKP_DR9);
	p[i++] = BKP_ReadBackupRegister(BKP_DR10);
	p[i++] = BKP_ReadBackupRegister(BKP_DR11);
	p[i++] = BKP_ReadBackupRegister(BKP_DR12);
	p[i++] = BKP_ReadBackupRegister(BKP_DR13);
	p[i++] = BKP_ReadBackupRegister(BKP_DR14);
	p[i++] = BKP_ReadBackupRegister(BKP_DR15);
	p[i++] = BKP_ReadBackupRegister(BKP_DR16);
	p[i++] = BKP_ReadBackupRegister(BKP_DR17);
	p[i++] = BKP_ReadBackupRegister(BKP_DR18);
	p[i++] = BKP_ReadBackupRegister(BKP_DR10);
	p[i++] = BKP_ReadBackupRegister(BKP_DR20);
	//
}
void write_bkp_buffer(unsigned char*buf)
{
	unsigned char i=0;
	unsigned short *p;
	p = (unsigned short *)buf;
	
	BKP_WriteBackupRegister(BKP_DR1,p[i++]);
	BKP_WriteBackupRegister(BKP_DR2,p[i++]);
	BKP_WriteBackupRegister(BKP_DR3,p[i++]);
	BKP_WriteBackupRegister(BKP_DR4,p[i++]);
	BKP_WriteBackupRegister(BKP_DR5,p[i++]);
	BKP_WriteBackupRegister(BKP_DR6,p[i++]);
	BKP_WriteBackupRegister(BKP_DR7,p[i++]);
	BKP_WriteBackupRegister(BKP_DR8,p[i++]);
	BKP_WriteBackupRegister(BKP_DR9,p[i++]);
	BKP_WriteBackupRegister(BKP_DR10,p[i++]);
	BKP_WriteBackupRegister(BKP_DR11,p[i++]);
	BKP_WriteBackupRegister(BKP_DR12,p[i++]);
	BKP_WriteBackupRegister(BKP_DR13,p[i++]);
	BKP_WriteBackupRegister(BKP_DR14,p[i++]);
	BKP_WriteBackupRegister(BKP_DR15,p[i++]);
	BKP_WriteBackupRegister(BKP_DR16,p[i++]);
	BKP_WriteBackupRegister(BKP_DR17,p[i++]);
	BKP_WriteBackupRegister(BKP_DR18,p[i++]);
	BKP_WriteBackupRegister(BKP_DR19,p[i++]);
	BKP_WriteBackupRegister(BKP_DR20,p[i++]);
	
}