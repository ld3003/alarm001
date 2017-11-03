#include "mainloop.h"
#include "modem.h"
#include "serialport.h"
#include "task.h"
#include "at.h"
#include "utimer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "udp_proto.h"
#include "dcmi_ov2640.h"
#include "rtc.h"
#include "mem.h"
#include "setting.h"
#include "testpoint.h"
#include "bsp.h"
#include "main.h"
#include "ov2640api.h"

#define MAINLOOP_STATUS_DELAY __time_100ms_cnt[TIMER_100MS_MAINLOOP_DELAY]

#if 0
SYS_INIT = 0,
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
	MODEM_GPRS_READY,								//GPRSÒÑ¾­×¼±¸ºÃ
	MODEM_GPRS_CGDCONT,
	MODEM_GPRS_CGACT,
	MODEM_GPRS_CGATT,
	MODEM_GPRS_CGREG,
	MODEM_GPRS_MIPCALL_SUCCESS,			//½øÐÐÍøÂç×¢²á
	MODEM_GPRS_MIPOPEN_SUCCESS,			//´´½¨SOCKETÁ¬½Ó
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
#endif

const struct STATUS_STR_ITEM sstr_item[] = {
	{1,"XXX"},
};

static const char *get_status_str(unsigned char status)
{
	char i=0;
	for(i=0;i<(sizeof(sstr_item)/sizeof(struct STATUS_STR_ITEM));i++)
	{
		if (sstr_item[i].status == status)
		{
			return sstr_item[i].str;
		}
		//
	}
	
	return 0;
}

struct MAINLOOP_DATA mdata;

static int push_1091(void);
static int push_10A0(c_u16 alarmtype);
static int push_img_routing(void);
static void init_push_img(void);
static void set_img_data(void);
static int process_1091pkg(unsigned char *rbuffer , int recvlen , unsigned short seq);

static void status_master(unsigned char status , unsigned int time)
{
	static unsigned char __history_status = MDATA_STATUS_NULL;
	static unsigned int current_status_time_ms = 0;
	
	//Î¹¹·
	
	if (status != __history_status)
	{
		//printf("×´Ì¬Ìø×ª %d:%s ---> %d:%s \r\n",__history_status,get_status_str(__history_status),status,get_status_str(status));
		__history_status = status;
		mdata.status_running_cnt = 0;
		
		//
	}else{
		mdata.status_running_cnt ++;
		//printf("×´Ì¬ %d ±»Ö´ÐÐµÚ %d ´Î\r\n",status,mdata.status_running_cnt);
	}
	
	
	
}

void mainloop_init(void)
{
	memset(&mdata,0x0,sizeof(mdata));
	mdata.status = SYS_INIT;
}

static void SYSINIT(void)
{
	
	#ifdef __WAKEUP_DBUG
	DEBUG_BKP_REG;
	#endif
	
	switch(GET_SYSTEM_STATUS)
	{
		case SYSTEM_STATUS_INIT:
			
			//³õÊ¼»¯£¬³õÊ¼»¯Ö®ºóÐÝÃß
			IOI2C_Init();
			init_mma845x();
			SET_SYSTEM_STATUS(SYSTEM_STATUS_TAKEPHOTH); //ÉÏµçÅÄÕÕÒ»´Î
			Sys_Enter_Standby();
			
			break;
		case SYSTEM_STATUS_DEEPSLEEP:
			
			if (CURRENT_RTC_TIM > GET_NEXT_WAKEUP_TIME)
			{
				#ifdef __WAKEUP_DBUG
				printf("ÍË³öÉî¶ÈÐÝÃß£¬½øÈëÆÕÍ¨ÐÝÃß\r\n");
				#endif
				
				SET_SYSTEM_STATUS(SYSTEM_STATUS_WAIT_WAKEUP);
				Sys_Enter_Standby();
				
			}else{
				#ifdef __WAKEUP_DBUG
				printf("Éî¶ÈÐÝÃßÖÐ...\r\n");
				#endif
				Sys_Enter_DeepStandby();
			}
			break;
		case SYSTEM_STATUS_WAIT_WAKEUP:
		{
			int i=0;
			//±»»½ÐÑÁË
			
			if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
			{
				//±»Íâ²¿ÖÐ¶Ï»½ÐÑ
//				i=0;
//				while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
//				{
//					if (i > 0xFF)
//					{
//						//MMA8452¹ÒÁË£¬ÐèÒªÖØÐÂÆô¶¯
//						printf("ÖØÐÂÆô¶¯MMA8452\r\n");
//						IOI2C_Init();
//						init_mma845x();
//						break;
//					}
//					i++;
//				}

				
				//Èç¹ûÁ½´Î±¨¾¯¼ä¸ô²»Ð¡ÓÚ 120 Ôò²»½øÈë±¨¾¯
				if ((CURRENT_RTC_TIM - GET_LAST_ALARM_TIME) < ALARM_MIN_TIME)
				{
				
					#ifdef __WAKEUP_DBUG
					printf("±¨¾¯¼ä¸ôÐ¡ÓÚ%dÃë£¬µ±Ç°Ê±¼ä [%d] ×îºóÒ»´Î±¨¾¯Ê±¼ä [%d]\r\n",ALARM_MIN_TIME,CURRENT_RTC_TIM,GET_LAST_ALARM_TIME);
					#endif
					
					//Èç¹ûÊÇÒòÎª¾àÀëÉÏ´Î±¨¾¯Ê±¼ä²»¹»£¬ÄÇÃ´ÖØÐÂ¸üÐÂÉÏ´Î±¨¾¯Ê±¼ä
					
					SET_LAST_ALARM_TIME;
					
					SET_SYSTEM_STATUS(SYSTEM_STATUS_WAIT_WAKEUP);
					Sys_Enter_Standby();
					
					#ifdef __WAKEUP_DBUG
					printf("½øÈë³¤´ï %d ÃëµÄÉî¶ÈÐÝÃß\r\n",ALARM_MIN_TIME / 2); // ÁôÒ»°ëÊ±¼äµÄãÐÖµ
					#endif
					
					SET_NEXT_WAKEUP_TIME(CURRENT_RTC_TIM + ALARM_MIN_TIME);
					SET_SYSTEM_STATUS(SYSTEM_STATUS_DEEPSLEEP);
					Sys_Enter_DeepStandby();
					
				}else{
					
					#ifdef __WAKEUP_DBUG
					printf("½øÈë±¨¾¯×´Ì¬£¬Çå¿Õ¾²Ö¹±¨¾¯±ê¼ÇÎ»\r\n");
					#endif
					
					SET_MOTIONLESS_STATUS(0);
					
					mdata.doing = DOING_10A0;
					mdata._10a0type = 0;
					
					mdata.status = MODEM_POWEROFF;
					
					SET_LAST_ALARM_TIME;
					
					goto exit_standby;
				}
			}else{
				//¿´ÃÅ¹·»½ÐÑ / ¶¨ÆÚ»½ÐÑ
				
				//¼ì²éµ±Ç°ÊÇ·ñ´¦ÓÚ¾²Ö¹×´Ì¬£¬Èç¹ûÈç¹ûÊÇ¾²Ö¹×´Ì¬ÐèÒªÆÁ±Î»½ÐÑÔ´
				if (GET_MOTIONLESS_STATUS == 0)
				{
					
					if ((CURRENT_RTC_TIM - GET_LAST_ALARM_TIME) > MOTIONLESS_TIME)
					{
						mdata.doing = DOING_10A0;
						mdata._10a0type = 1;
						mdata.status = MODEM_POWEROFF;
						
						SET_MOTIONLESS_STATUS(1);
						
						goto exit_standby;
						
					}else{
						
						#ifdef __WAKEUP_DBUG
						printf("Èç¹û %d Ãëºó Éè±¸ÈÔÈ»¾²Ö¹£¬Ôò´¥·¢¾²Ö¹±¨¾¯ \r\n",(MOTIONLESS_TIME - (CURRENT_RTC_TIM - GET_LAST_ALARM_TIME)));
						#endif
					}
					
				}
				
				if (GET_AUTOWAKEUP_TIM <= RTC_GetCounter())
				{
					printf("¶¨ÆÚ»½ÐÑÊ±¼äµ½ÁË\r\n");
					
					//Ä¿Ç°¶¨ÆÚ»½ÐÑ£¬Ö»ÊÇÎªÁËÅÄÕÕ
					SET_SYSTEM_STATUS(SYSTEM_STATUS_TAKEPHOTH);
					
					//¹Ø»úÖØÆôÖØÐÂ½øÈëÁ÷³Ì
					sys_shutdown();
					//
				}else{
					#ifdef __WAKEUP_DBUG
					printf("¾àÀë¶¨ÆÚ»½ÐÑ»¹ÓÐ %dÃë\r\n",GET_AUTOWAKEUP_TIM - RTC_GetCounter());
					#endif
				}
				
				
				
				
				#ifdef __WAKEUP_DBUG
				printf("½øÈëÐÝÃß...\r\n");
				#endif
				Sys_Enter_Standby();
				
			}
			
			break; 
		}
		
		case SYSTEM_STATUS_TAKEPHOTH:	
		{
			//ÅÄÕÕ
			if (mdata.cam_ok == 1)
			{
				mdata.doing = DOING_UPLOADPHPTO;
				mdata.status = MODEM_POWEROFF;
				//»Ø¸´µÈ´ý»½ÐÑ×´Ì¬
				SET_SYSTEM_STATUS(SYSTEM_STATUS_WAIT_WAKEUP);
				goto exit_standby;
			}else{
				SET_AUTOWAKEUP_TIM(RTC_GetCounter() + SEND_IMGERROR_SLEEP_TIME)
				SET_SYSTEM_STATUS(SYSTEM_STATUS_WAIT_WAKEUP);
			}
			break;
		}
		case SYSTEM_STATUS_RUN:
			break;
	}
	
	exit_standby:
	printf("ÍË³öÐÝÃß\r\n");
	return;
}

void mainloop(void)
{
	
	#define SYNC_STATUS_2_REG //__WRITE_STANDBY_STATUS(mdata.status);
	#define SET_STATUS(X) //__WRITE_STANDBY_STATUS(X);
	
	switch(mdata.status)
	{
		case SYS_INIT:
			status_master(mdata.status,60*1000);
		
			#if 0
			IOI2C_Init();
			init_mma845x();
			mdata.doing = DOING_10A0;
			mdata._10a0type = 1;
			mdata.status = MODEM_POWEROFF;
			#else
			SYSINIT();
			#endif
			
		
			break;
		case SYS_POWEROFF:
			
			printf("ÈÎÎñ´¦ÀíÍê³É£¬¹Ø»ú,½øÈëÐÝÃß!\r\n");
		
			gprs_modem_power_off();
		
			printf("Delay 2000MS \r\n");
			utimer_sleep(2000);
		
		
		
			IOI2C_Init();
			init_mma845x();
		
			printf("Delay 2000MS \r\n");
			utimer_sleep(2000);
			
			//¹Ø»ú½øÈëÐÝÃß
			SET_SYSTEM_STATUS(SYSTEM_STATUS_WAIT_WAKEUP);
			Sys_Enter_Standby();
			break;
		
		case MODEM_RESET:
			status_master(mdata.status,60*1000);
		
			printf("ÖØÆôGPRSÄ£¿é,µ±Ç°µÚ %d ´ÎÖØÆô\r\n",mdata.modem_reset_cnt);
		
			//½«Ò»Ð©¹Ø¼ü¼ÆÊýÆ÷ÇåÁã
			mdata.modem_reset_cnt ++;
			mdata.status = MODEM_POWEROFF;
			break;
		
		case MODEM_POWEROFF:
			
			/*
					
			*/
		
			//ÔÚÆô¶¯GPRSÄ£¿éÖ®Ç°Íê³ÉµçÑ¹²É¼¯
			mdata.device_voltage = read_vdd_voltage() + 600;
			printf("µ±Ç°µç³ØµçÑ¹ %d\r\n",mdata.device_voltage);
		
		
			status_master(mdata.status,60*1000);
		
			printf("Æô¶¯GPRSÄ£¿é\r\n");
		
			gprs_modem_power_off();
			utimer_sleep(1000);
			gprs_modem_power_on();
			utimer_sleep(3000);
		
			mdata.status = MODEM_POWERACTIVE_TESTAT; //½øÈë²âÊÔATÖ¸ÁîµÄ»·½Ú
		
			break;

		case MODEM_POWERACTIVE:
			status_master(mdata.status,60*1000);
		
			if (MAINLOOP_STATUS_DELAY >= 50) //5Ãë
			{
				
				mdata.test_at_cnt = 0;
				mdata.status = MODEM_POWERACTIVE_TESTAT;
				
				printf("mdata.status = MODEM_POWERACTIVE_TESTAT;\r\n");
			}
			break;
		case MODEM_POWERACTIVE_TESTAT:
		{
			int ret;
			
			status_master(mdata.status,60*1000);
			
			//Í¨¹ýÏòÄ£¿é·¢ËÍATÖ¸ÁîÅÐ¶Ï·µ»Ø½á¹ûÊÇ·ñÕýÈ·À´ÅÐ¶¨Ä£¿éÊÇ·ñÒÑ¾­Õý³£Æô¶¯ÁË
			ret = at_cmd_wait("ATE0\r\n",AT_AT,0,2000);
			
			mdata.test_at_cnt ++;
			
			if (ret == AT_RESP_OK)
			{
				//ret = at_cmd_wait("ATE0\r\n",AT_AT,0,AT_WAIT_LEVEL_2);
				
				//Ä£¿éATÖ¸Áî·µ»ØÕýÈ·£¬ÈÏÎªÄ£¿éÒÑ¾­Õý³£Æô¶¯£¬½øÈëÏÂÒ»¸ö×´Ì¬
				printf("Ä£¿é¿ª»ú³É¹¦,ÐÝÃß5ÃëÈÃÄ£¿é³õÊ¼»¯Íê³É\r\n");
				utimer_sleep(5000);
				mdata.status = MODEM_GET_IMEI;
				
			}else{
				
				//Èç¹ûATÖ¸Áî·µ»Ø²»ÕýÈ·£¬ÔòÖØÐÂ·¢ËÍATÖ¸Áî£¬Èç¹ûN´Î¶¼Ã»ÓÐÊÕµ½£¬ÔòÖØÆôÄ£¿é
				if (mdata.test_at_cnt > 3)
				{
					mdata.test_at_cnt = 0;

					//Ä£¿éÖØÆôÁË3´Î¶¼Ã»ÓÐATÖ¸Áî·¢ËÍ³É¹¦£¬¿ÉÒÔÈÏÎªÊÇ´®¿ÚÒç³öÁË£¬²»ÄÜÊÕµ½ÐÅÏ¢ÁË£¬ÖØÆôÕû¸öÏµÍ³
					printf("******************************************************\r\n");
					printf("*               Ä£¿é¿ª»úÊ§°Ü                         *\r\n");
					printf("******************************************************\r\n");
					//30ÃëºóÔÙÆô¶¯
					//__SET_NEXT_WAKEUP_TIM(26);
					mdata.status = MODEM_GPRS_ERROR;
					
				}
			}
			break;
		}

		case MODEM_ATD:
			status_master(mdata.status,60*1000);
			break;
		case MODEM_GET_IMEI:
		{
			int ret;
			status_master(mdata.status,60*1000);
			ret = at_cmd_wait_str("AT+GSN\r\n",AT_GSN,"OK",100);
			if (AT_RESP_OK == ret)
				mdata.status = MODEM_CHECK_MODEM_TYPE;
			
			if (mdata.status_running_cnt > 3)
				mdata.status = MODEM_GPRS_ERROR;
			
			break;
		}
		case MODEM_CHECK_MODEM_TYPE:
		{
			
			status_master(mdata.status,60*1000);
			mdata.status = MODEM_POWERON;
			
			break;
		}
		
		case MODEM_POWERON:
		{
			int ret;
			
			status_master(mdata.status,60*1000);
			
			ret = at_cmd_wait_str("AT+CPIN?\r\n",AT_AT,"+CPIN: READY",300);
			
			if (ret == AT_RESP_OK)
			{
				//ÍøÂçÒÑ¾­×¢²á³É¹¦£¬½øÈë²éÑ¯ÐÅºÅÖÊÁ¿
				printf("¼ì²éSIM¿¨³É¹¦\r\n");
				mdata.status = MODEM_CHECK_CSQ;
				
			}else{
				printf("¼ì²éSIM¿¨Ê§°Ü,ÖØÆôÄ£×é\r\n");
				
				if (mdata.status_running_cnt > 5)
					mdata.status = MODEM_GPRS_ERROR;
				else
					utimer_sleep(1000); //µÈ´ý1sºóÔÙ´ÎÖ´ÐÐ
			}
			break;
		}
		
		case MODEM_CHECK_CSQ:
		{
			int ret;
			status_master(mdata.status,60*1000);
			ret = at_cmd_wait("AT+CSQ\r\n",AT_CSQ,0,500);
			if (AT_RESP_OK == ret)
			{
				printf("µ±Ç°ÐÅºÅÖÊÁ¿ : %d\r\n",gsm_signal);
				mdata.status = MODEM_GPRS_READY;
			}else{
				if (mdata.status_running_cnt > 3)
				{
					mdata.status = MODEM_GPRS_ERROR;
				}
			}
			
			break;
			//
		}
		
		case MODEM_GPRS_READY:
		{
			status_master(mdata.status,60*1000);
			mdata.status = MODEM_GPRS_CGDCONT;
			break;
		}
		
		case MODEM_GPRS_CGDCONT:
		{
			int ret;
			status_master(mdata.status,60*1000);
			ret = at_cmd_wait("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n",AT_CGDCONT,AT_WAIT,5000);
			if (ret == 0)
			{
				mdata.status = MODEM_GPRS_CGACT;
			}else{
				if (mdata.status_running_cnt > 2)
				{
					mdata.status = MODEM_GPRS_ERROR;
				}
			}
			
			break;
		}
		case MODEM_GPRS_CGACT:
		{
			int ret;
			status_master(mdata.status,60*1000);
			ret = at_cmd_wait("AT+CGACT=1,1\r\n",AT_CGACT,AT_WAIT,5000);
			if (ret == 0)
			{
				mdata.status = MODEM_GPRS_CGATT;
			}else{
				printf("¼¤»îGPRS CGACT Ê§°Ü\r\n");
				if (mdata.status_running_cnt > 5)
					mdata.status = MODEM_GPRS_ERROR;
				else
					utimer_sleep(1000); //µÈ´ý1sºóÔÙ´ÎÖ´ÐÐ
			}
			break;
		}
		
		case MODEM_GPRS_CGATT:
		{
			int ret;
			status_master(mdata.status,60*1000);
			ret = at_cmd_wait_str_str("AT+CGATT=1\r\n","OK","OK",5000);
			if (ret == 0)
			{
				mdata.status = MODEM_GPRS_CGREG;
			}else{
				printf("²éÑ¯¼¤»îGPRS CGATT Ê§°Ü\r\n");
				if (mdata.status_running_cnt > 5)
					mdata.status = MODEM_GPRS_ERROR;
				else
					utimer_sleep(1000); //µÈ´ý1sºóÔÙ´ÎÖ´ÐÐ
			}
			break;
		}
		
		case MODEM_GPRS_CGREG:
		{
			int ret=0;
			status_master(mdata.status,60*1000);
			at_cmd_wait_str_str("AT+CGREG=2\r\n","OK","OK",5000);
			ret += at_cmd_wait_str("AT+CGREG?\r\n",AT_CGREG,"OK",5000);
			if (ret != AT_RESP_CGREGOK)
			{
				if (mdata.status_running_cnt > 5)
					mdata.status = MODEM_GPRS_ERROR;
			}else{
				mdata.status = MODEM_GPRS_GETLOC;
			}
			break;
			
		}
		
		case MODEM_GPRS_GETLOC:
		{
			int ret;
			status_master(mdata.status,60*1000);
			ret = at_cmd_wait("AT+AMGSMLOC\r\n",AT_AMGSMLOC,AT_WAIT,5000);
			if (ret == AT_RESP_OK)
			{
				mdata.status = MODEM_GPRS_CIPMOD;
				//
			}else{
			}
			
			
			
			break;
		}
		
		case MODEM_GPRS_CIPMOD:
		{
			int ret;
			status_master(mdata.status,60*1000);
			ret = at_cmd_wait_str_str("AT+CIPMODE=1\r\n","OK","OK",2000);
			if (ret!=AT_RESP_OK)
			{
				if (mdata.status_running_cnt > 2)
					mdata.status = MODEM_GPRS_ERROR;
			}else{
				mdata.status = MODEM_GPRS_MIPCALL_SUCCESS;
			}
			break;
		}
		case MODEM_GPRS_MIPCALL_SUCCESS:
		{
			
			int ret;
			char *tmpstr;
			status_master(mdata.status,60*1000);
			
			tmpstr = (char*)alloc_mem(__FILE__,__LINE__,1024); 
			tmpstr[0] = 0x0;

			snprintf(tmpstr,1024,"AT+CIPSTART=\"UDP\",\"%s\",%d\r\n",SERVER_ADDR,SERVER_PORT);
			ret = at_cmd_wait(tmpstr,AT_IPSTART,AT_WAIT,10000);
			free_mem(__FILE__,__LINE__,(unsigned char *)tmpstr);

			if(ret == 0)
			{
				mdata.status = MODEM_GPRS_MIPOPEN_SUCCESS;
				printf("SERVERÁ´½Ó³É¹¦\r\n");
			}else{
				printf("SERVER Á´½ÓÊ§°Ü\r\n");
				if (mdata.status_running_cnt > 2)
				{
					mdata.status = MODEM_GPRS_ERROR;
				}
				else
				{
					utimer_sleep(1000); //µÈ´ý1sºóÔÙ´ÎÖ´Ð
				}
			}
			
			break;
		}
		case MODEM_GPRS_MIPOPEN_SUCCESS:
		{
			status_master(mdata.status,60*1000);
			mdata.status = PROTO_CHECK_1091;
			
			break;
		}
		
		
		//
		
		case MODEM_GPRS_ERROR:
		{
			
			mdata.modem_gprs_error_cnt ++;
			
			if(mdata.modem_gprs_error_cnt > 3)
			{
				printf("******************************************************\r\n");
				printf("           MODEM_GPRS_ERROR!                         *\r\n");
				printf("******************************************************\r\n");
				mdata.status = SYS_POWEROFF;
				
				//¸ù¾Ýµ±Ç°×´Ì¬
				switch(mdata.doing)
				{
					case DOING_UPLOADPHPTO:
						SET_NEXT_WAKEUP_TIME(CURRENT_RTC_TIM + SEND_IMGERROR_SLEEP_TIME);
						break;
					case DOING_10A0:
						break;
					default:
						break;
				}
				
			}else{
				mdata.status = MODEM_RESET;
			}
			break;
		}
		
		case PROTO_CHECK_1091:
		{
			int ret;
			status_master(mdata.status,60*1000);
			ret = push_1091();
			if (ret == 0)
			{
				if(mdata.doing == DOING_10A0)
				{
					mdata.status = PROTO_SEND_ALARM;
				}
				else if (mdata.doing == DOING_UPLOADPHPTO)
				{
					mdata.status = START_SEND_IMG;
				}
				
			}
			
			//3´ÎÃ»ÓÐ·´Ó¦ÔòÈÏÎª1091Ê§°Ü
			if (mdata.status_running_cnt > 3)
			{
				if (mdata.doing == DOING_UPLOADPHPTO)
				{
					mdata.status = PROTO_SEND_IMG_ERROR;
				}
				else if(mdata.doing == DOING_10A0)
				{
					mdata.status = SYS_POWEROFF;
				}
			}
			
			break;
		}
		
		case PROTO_SEND_ALARM:
		{
			
			status_master(mdata.status,10*60*1000);
		
			if (push_10A0(mdata._10a0type) < 0)
			{
				printf("·¢ËÍ10A0 Ê§°Ü\r\n");
				
			}else{
				printf("·¢ËÍ10a0Êý¾Ý³É¹¦\r\n");
				mdata.status = SYS_POWEROFF;
			}
			
			if (mdata.status_running_cnt >= 5)
			{
				mdata.status = SYS_POWEROFF;
			}
			
			break;
		}
		
		case START_SEND_IMG:
		{
			status_master(mdata.status,60*1000);
			init_push_img();
			set_img_data();
			update_message_id();
			mdata.status = PROTO_SEND_IMG;
			
			break;
			
		}
			
		case PROTO_SEND_IMG:
		{
			int ret;
			
			status_master(mdata.status,10*60*1000);
			
			ret = push_img_routing();

			switch(ret)
			{
				case 1:
					mdata.status = PROTO_SEND_IMG_SUCCESS;
					
					break;
				case 0:
					break;
				case -1:
					mdata.status = PROTO_SEND_IMG_ERROR;
					
					//
					break;
			}
			break;
		}
			//
		
		case PROTO_SEND_IMG_ERROR:
			
			status_master(mdata.status,60*1000);
			
			//Èç¹ûÍ¼Æ¬·¢ËÍÊ§°Ü£¬ÔòÉÏ´«PIC
			printf("******************************************************\r\n");
			printf("           UPLOAD PHOTO ERROR!                       *\r\n");
			printf("******************************************************\r\n");
		
			//ÐÝÃß
		
			//1¸öÐ¡Ê±Ö®ºóÖØ·¢
			printf("Ò»¸öÐ¡Ê±Ö®ºóÖØÐÂÅÄ\r\n");
			SET_AUTOWAKEUP_TIM(RTC_GetCounter() + SEND_IMGERROR_SLEEP_TIME);
			mdata.status = SYS_POWEROFF;
		
			break;
		case PROTO_SEND_IMG_SUCCESS:
			
			status_master(mdata.status,60*1000);
		
			printf("******************************************************\r\n");
			printf("           UPLOAD PHOTO SUCCESS                       \r\n");
			printf("******************************************************\r\n");
		
			//mdata.time1
			SET_AUTOWAKEUP_TIM(RTC_GetCounter() + mdata.time1);
			mdata.status = SYS_POWEROFF;
		
			break;
		
		case PROTO_ALARM:
			status_master(mdata.status,60*1000);
			break;
		default:
			status_master(mdata.status,60*1000);
			break;
	}
	


	//
}



static int push_1091(void)
{
	
	struct UDP_PROTO_2091_DATA *data2091p;
	struct UDP_PROTO_HDR *hdr;
	
	unsigned char __dd[] = {0x00,0x03,0x00,0x05,0x01};
	
	int ret;
	int ret1 = -1;
	int length,recvlen;
	
	
	unsigned char *sbuffer,*rbuffer;
	char *hexbuffer;
	char *atbuffer;
	
	sbuffer = alloc_mem(__FILE__,__LINE__,512);
	rbuffer = sbuffer;//alloc_mem(__FILE__,__LINE__,1024);
	
	
	length = make_0x1091(sbuffer,__dd,sizeof(__dd));
	
	
	recvlen = push_data_A6(sbuffer,length,rbuffer,WAIT_UDP_PKG_TIME);
	
	if (recvlen >= sizeof(struct UDP_PROTO_HDR))
	{
		char *hexbuffer = 0;
		
		data2091p = (struct UDP_PROTO_2091_DATA *)rbuffer;
	  hdr = (struct UDP_PROTO_HDR *)rbuffer;
		

		switch(hdr->cmdcode)
		{
			case 0x9120:
			{
				process_1091pkg(rbuffer,recvlen,0);
				ret1 = 0;
				break;
			}
			case 0x00:
				break;
			default:
				break;
		}
		//
	}else {
		printf("RECV 2091 ERROR \r\n");
	}
	
	free_mem(__FILE__,__LINE__,(unsigned char*)sbuffer);
	//free_mem((unsigned char*)rbuffer);
	
	return ret1;
	//
}


static int push_10A0(c_u16 alarmtype)
{
	
	struct UDP_PROTO_HDR *hdr;
	
	
	int ret;
	int ret1 = -1;
	int length,recvlen;
	
	
	unsigned char *sbuffer,*rbuffer;
	char *hexbuffer;
	char *atbuffer;
	
	sbuffer = alloc_mem(__FILE__,__LINE__,512);
	rbuffer = sbuffer;//alloc_mem(__FILE__,__LINE__,1024);
	
	length = make_0x10A0(sbuffer,alarmtype,1,0);
	
	recvlen = push_data_A6(sbuffer,length,rbuffer,WAIT_UDP_PKG_TIME);
	
	if (recvlen >= sizeof(struct UDP_PROTO_HDR))
	{
		
	  hdr = (struct UDP_PROTO_HDR *)rbuffer;

		switch(hdr->cmdcode)
		{
			case 0xA020:
				printf("·þÎñÆ÷³É¹¦ÊÕµ½ 10A0\r\n");
				ret1 = 0;
				break;
			case 0x00:
				break;
			default:
				break;
			
		}
		//
	}else{
		printf("RECV 2091 ERROR \r\n");
	}
	
	free_mem(__FILE__,__LINE__,(unsigned char*)sbuffer);
	//free_mem((unsigned char*)rbuffer);
	
	return ret1;
	//
}

static int push_10A1(void)
{
	
	struct UDP_PROTO_HDR *hdr;
	
	
	int ret;
	int ret1 = -1;
	int length,recvlen;
	
	
	unsigned char *sbuffer,*rbuffer;
	char *hexbuffer;
	char *atbuffer;
	
	sbuffer = alloc_mem(__FILE__,__LINE__,512);
	rbuffer = sbuffer;//alloc_mem(__FILE__,__LINE__,1024);
	
	length = make_0x10A0(sbuffer,1,1,0);
	
	recvlen = push_data_A6(sbuffer,length,rbuffer,WAIT_UDP_PKG_TIME);
	
	if (recvlen >= sizeof(struct UDP_PROTO_HDR))
	{
		
	  hdr = (struct UDP_PROTO_HDR *)rbuffer;

		switch(hdr->cmdcode)
		{
			case 0xA020:
				printf("·þÎñÆ÷³É¹¦ÊÕµ½ 10A0\r\n");
				ret1 = 0;
				break;
			case 0x00:
				break;
			default:
				break;
			
		}
		//
	}else{
		printf("RECV 2091 ERROR \r\n");
	}
	
	free_mem(__FILE__,__LINE__,(unsigned char*)sbuffer);
	//free_mem((unsigned char*)rbuffer);
	
	return ret1;
	//
}

static int push_10B0(char *json)
{
	//int make_0x10B0(unsigned char *data , char *json)
	
	struct UDP_PROTO_HDR *hdr;
	
	
	int ret;
	int ret1 = -1;
	int length,recvlen;
	
	
	unsigned char *sbuffer,*rbuffer;
	char *hexbuffer;
	char *atbuffer;
	
	sbuffer = alloc_mem(__FILE__,__LINE__,512);
	rbuffer = sbuffer;//alloc_mem(__FILE__,__LINE__,1024);
	
	length = make_0x10B0(sbuffer,json);
	
	recvlen = push_data_A6(sbuffer,length,rbuffer,WAIT_UDP_PKG_TIME);
	
	if (recvlen >= sizeof(struct UDP_PROTO_HDR))
	{
		
	  hdr = (struct UDP_PROTO_HDR *)rbuffer;

		switch(hdr->cmdcode)
		{
			case 0xB020:
				printf("RECV 20A0 SUCCESS \r\n");
				ret1 = 0;
				break;
			case 0x00:
				break;
			default:
				break;
			
		}
		//
	}else{
		printf("RECV 20B0 ERROR \r\n");
	}
	
	free_mem(__FILE__,__LINE__,(unsigned char*)sbuffer);
	//free_mem((unsigned char*)rbuffer);
	
	return ret1;
	//
}

//-----------------------------------

struct PUSH_IMG_DATA {
	unsigned char status;
	unsigned char *imgdata;
	int img_total_len;
	int img_send_index;
	int seq;
	unsigned short seq_cnt;
	unsigned char send_err_cnt;
};

struct PUSH_IMG_DATA push_img_dat;

static void set_img_data(void)
{
	//¸ù¾Ýµ±Ç°ÉÏ´«¹æÔò£¬Ð´Èë°üÊý¾Ý
	
	extern unsigned int ___paizhaoshijian;
	
	unsigned int paizhaotime;
	push_img_dat.imgdata = read_imgbuffer(0,&push_img_dat.img_total_len,&paizhaotime);
	mdata.paizhao_time = paizhaotime;
	DEBUG_VALUE(push_img_dat.img_total_len);
	
	
	if((push_img_dat.img_total_len % IMG_PKG_SIZE) == 0)
	{
		push_img_dat.seq_cnt = (push_img_dat.img_total_len / IMG_PKG_SIZE);
	}else{
		push_img_dat.seq_cnt = (push_img_dat.img_total_len / IMG_PKG_SIZE) + 1;
	}
	printf("push_img_dat.seq_cnt %d %d %d %d\r\n",push_img_dat.seq_cnt , ((int)push_img_dat.img_total_len / IMG_PKG_SIZE) , push_img_dat.img_total_len , IMG_PKG_SIZE);
}

static void init_push_img(void)
{
	memset(&push_img_dat,0x0,sizeof(struct PUSH_IMG_DATA));
}

static int push_1092(unsigned short imgl , unsigned char *imgdata ,unsigned short frq_count , unsigned char imgtype ,  unsigned short img_frq)
{
	
	struct UDP_PROTO_2092_DATA *data2092p;
	struct UDP_PROTO_HDR *hdr;
	
	int ret;
	int ret1 = -1;
	int length;
	int recvlen;
	
	unsigned char *sbuffer; //´æ´¢¶þ½øÖÆÂãÊý¾Ý
	unsigned char *rbuffer;
	
	sbuffer = alloc_mem(__FILE__,__LINE__,sizeof(struct UDP_PROTO_2092_DATA) + imgl + 16);
	rbuffer = sbuffer;//alloc_mem(__FILE__,__LINE__,512);
	
	
	length = make_0x1092(imgl,imgdata,frq_count,imgtype,img_frq,sbuffer);
	
	recvlen = push_data_A6(sbuffer,length,rbuffer,WAIT_UDP_PKG_TIME);
	
	if (recvlen > 0)
	{
		//debug_buf("recv",data,length);
		
		hdr = (struct UDP_PROTO_HDR*)rbuffer;
		data2092p = (struct UDP_PROTO_2092_DATA*)rbuffer;
		
		if (recvlen >= sizeof(struct UDP_PROTO_HDR))
		{
			switch(hdr->cmdcode)
			{
				case 0x9220:
					transfer16(&data2092p->hdr.messageSeq);
					printf("RECV 2092 SUCCESS \r\n");
					printf("RECV SEQ  %d \r\n",data2092p->hdr.messageSeq);
					if (img_frq == data2092p->hdr.messageSeq)
						ret1 = 0;
					else
						printf("SEQ ´íÎó \r\n");
					break;
				default:
					printf("RECV 2092 FORMAT ERROR \r\n");
					break;
				
			}
			//
		}else{
			printf("RECV 2092 ERROR \r\n");
		}
		
	}else{
		printf("wait 2092 error !\r\n");
	}
	
	free_mem(__FILE__,__LINE__,(unsigned char*)sbuffer);
	//free_mem((unsigned char*)rbuffer);
	
	return ret1;
	//
}

static int push_img_routing(void)
{
	int ret;
	int send_len;
	
	//»ñÈ¡Í¼Æ¬Êý¾Ý
	
	if (push_img_dat.send_err_cnt > PKG_RETRANS_COUNT)
	{
		return -1; //·¢ËÍÊ§°Ü
	}

	//ÐèÒª·¢ËÍ
	if (push_img_dat.img_total_len > push_img_dat.img_send_index)
	{
		if ((push_img_dat.img_total_len - push_img_dat.img_send_index) >= IMG_PKG_SIZE)
		{
			send_len = IMG_PKG_SIZE;
		}else{
			send_len = push_img_dat.img_total_len - push_img_dat.img_send_index;
		}
		
		ret = push_1092(send_len,push_img_dat.imgdata+push_img_dat.img_send_index,push_img_dat.seq_cnt,mdata.pictype,push_img_dat.seq + 1);
		if (ret == 0)
		{
			push_img_dat.send_err_cnt = 0;
			push_img_dat.seq ++;
			push_img_dat.img_send_index += send_len;
		}else{
			push_img_dat.send_err_cnt ++;
		}

		//
	}else{
		return 1;
	}
	
	
	return 0;
	//
}

int push_data_A6(unsigned char *data , int length , unsigned char *outdata , int timeout)
{
	int i=0;
	clear_uart2_buffer();
	start_uart_debug();
	send_data(data,length);
	//wait data:
	
	
	for(i=0;i<timeout;)
	{
		if (uart2_rx_buffer_index > 0)
		{
			utimer_sleep(500);
			break;
		}
		utimer_sleep(500);
		i+=500;
	}
	
	memcpy(outdata,uart2_rx_buffer,uart2_rx_buffer_index);
	
	stop_uart_debug();
	
	if (strstr(uart2_rx_buffer,"UDP ERROR"))
	{
		printf("UDP ERROR:%s\r\n",uart2_rx_buffer);
		return -1;
	}
	
	return uart2_rx_buffer_index;

	
}


static int process_1091pkg(unsigned char *rbuffer , int recvlen , unsigned short seq)
{
	int ret1 = -1;
	
	struct UDP_PROTO_2091_DATA *data2091p;
	struct UDP_PROTO_2092_DATA *data2092p;
	struct UDP_PROTO_HDR *hdr;
	
	if (recvlen >= sizeof(struct UDP_PROTO_HDR))
	{
		char *hexbuffer = 0;
		
		data2091p = (struct UDP_PROTO_2091_DATA *)rbuffer;
		data2092p = (struct UDP_PROTO_2092_DATA *)rbuffer;
	  hdr = (struct UDP_PROTO_HDR *)rbuffer;
		

		switch(hdr->cmdcode)
		{
			case 0x9120:
			{
			
			
				printf("³É¹¦ÊÕµ½2091°ü \r\n");
				printf("TIME1 ÅÄÕÕÊ±¼ä : %02X %02X %02X %02X \r\n",data2091p->time1[0],data2091p->time1[1],data2091p->time1[2],data2091p->time1[3]);
				printf("TIME2 ÉÏ´«Ê±¼ä : %02X %02X %02X %02X \r\n",data2091p->time2[0],data2091p->time2[1],data2091p->time2[2],data2091p->time2[3]);	
			
				mdata.time1 =  data2091p->time1[3] + (data2091p->time1[2]*256) + (data2091p->time1[1]*256*256) + (data2091p->time1[0]*256*256*256);
				mdata.time2 =  data2091p->time2[3] + (data2091p->time2[2]*256) + (data2091p->time2[1]*256*256) + (data2091p->time2[0]*256*256*256);
			
			
				ret1 = 0;
				break;
			}
			default:
				break;
		}
		//
	}else{
		printf("ÊÕµ½ÁË´íÎóµÄ·þÎñÆ÷ACK\r\n");
	}
	
	return ret1;
	
	return 0;
}


