/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The USART application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-10-30
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "ov2640api.h"

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


static int gotoalarm(void);
static void test_timer(void *arg)
{
	feed_watchdog();
	printf("current RTC time %d \r\n",CURRENT_RTC_TIM);
}



/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/
int main(void)
{
	
	
	SysTick_Config(SystemCoreClock / 100);
	init_uart1();
	init_uart2();
	RTC_Init();
	
	
//	{
//		alloc_jpegbuffer();
//		ov_poweron();
//		_config_mco();
//		InitSCCB();
//		ov2640_init();
//		for(;;){ov2640_read();utimer_sleep(2000);};
//		free_jpegbuffer();
//		
//		for(;;){};
//	}
		
	
	WKUP_Pin_Init();
	watch_dog_config();
		
	init_utimer();
	init_task();
	init_mem();
	
	SET_SYSTEM_COUNTER;
	
//	printf("start delay ___ \r\n");
//	utimer_sleep(5000);
//	printf("end delay ___ \r\n");
//	RTC_SetAlarm(RTC_GetCounter() + 5);
//	RTC_WaitForLastTask();
//	Sys_Enter_Standby();
	
	
//	gotoalarm();

	printf("(当前时间[%d]最后一次报警时间[%d]\r\n",CURRENT_RTC_TIM,GET_LAST_ALARM_TIME);
	
	led0_on();

	for(;;)
	{
		switch(GET_SYSTEM_STATUS)
		{
			case SYSTEM_STATUS_INIT:
				IOI2C_Init();
				init_mma845x();
				SET_SYSTEM_STATUS(SYSTEM_STATUS_SLEEP);
				break;
			case SYSTEM_STATUS_SLEEP:
			
				//检测是否有入侵报警
				if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
				{
					SET_SYSTEM_STATUS(SYSTEM_STATUS_RUN);
					break;
				}
			
				printf("进入休眠...\r\n");
				Sys_Enter_Standby();
				break; 
			
			case SYSTEM_STATUS_RUN:
				printf("运行...\r\n");
			
				//如果当前的RTC小于60那么不进入报警规则
				if (CURRENT_RTC_TIM < 60)
				{
					//
					printf("当前开机时间小于60秒\r\n");
					SET_SYSTEM_STATUS(SYSTEM_STATUS_SLEEP);
					Sys_Enter_Standby();
					break;
				}
				
				//如果两次报警间隔不小于 120 则不进入报警
				if ((CURRENT_RTC_TIM - GET_LAST_ALARM_TIME) < 120)
				{
					printf("报警间隔小于120秒，当前时间 [%d] 最后一次报警时间 [%d]\r\n",CURRENT_RTC_TIM,GET_LAST_ALARM_TIME);
					
					//如果是因为距离上次报警时间不够，那么重新更新上次报警时间
					SET_LAST_ALARM_TIME;
					
					SET_SYSTEM_STATUS(SYSTEM_STATUS_SLEEP);
					Sys_Enter_Standby();
					break;
				}
				
				gotoalarm();
				break;
		}
	}
	
	return 0;
}


static int gotoalarm(void)
{
	init_uart2_buffer();
	
	//	utimer_sleep(5000);
	//	Sys_Enter_Standby();
	//	for(;;){};
		
	//	JpegBuffer = malloc(JPEG_BUFFER_LENGTH);
	//	ov_poweron();
	//	config_mco();
	//	InitSCCB();
	//	init_ov2640();
	//	for(;;){readimg();utimer_sleep(2000);};
	//	free(JpegBuffer);
	//	return 1;
	
	//
	//Data1 = *(unsigned short *)(0X1FFFF7BA);
	//printf("DATA1 DADA2 %X %X \r\n",Data1,Data2);
	
	//Adc_Init();
	//printf("XXXX %d \r\n",Get_val());
	
	
	mem = (struct MEM_DATA *)malloc(sizeof(struct MEM_DATA));
	read_config();
	
	
	
	//启动一个定时器，进行喂狗操作
	timerlist[0].func = test_timer;
	timerlist[0].arg = 0;
	timerlist[0].type = 0;
	timerlist[0].__dangqian = 0;
	timerlist[0].__zhouqi = 10;
	timerlist[0].enable = 1;
	
	for(;;)
	{
		run_task();
		mainloop();
	}
	
	return 0;
	
}



/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

