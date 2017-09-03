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
	watch_dog_config();
	init_uart1();
	init_uart2();
	RTC_Init();
	
	//设置APP已经进入启动
	SET_BOOTLOADER_STATUS(2);
	
	if (read_usb_status() == 0)
	{
		printf("USB 线缆已经插入 !\r\n");
		//
	}else{
		printf("USB 线未插入 !\r\n");
	}
	
	
	
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
	
	printf("(当前时间[%d]最后一次报警时间[%d]\r\n",CURRENT_RTC_TIM,GET_LAST_ALARM_TIME);
	
	led0_off();
	
	init_uart2_buffer();
	
	/*
	RUN_TEST;
	mem = (struct MEM_DATA *)malloc(sizeof(struct MEM_DATA));
	read_config();
	*/
	
	for(;;){feed_watchdog();mainloop();}
	
	return 0;
}




#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
	
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

