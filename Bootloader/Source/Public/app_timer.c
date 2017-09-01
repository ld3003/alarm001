#include "app_timer.h"

struct APP_TIMER app_timer_data;

#define TIMER_PREIOD  10  //定时器周期，单位为 毫秒


void app_timer_add(struct APP_TIMER *tmr , unsigned int ms)
{
	unsigned int sec;
	unsigned int mms;
	
	mms = ms + tmr->app_timer_ms;
	
	sec = mms/1000;
	tmr->app_timer_second += sec;
	tmr->app_timer_ms = mms%1000;
	//
}

int app_timer_cmp(struct APP_TIMER *tmr1 , struct APP_TIMER *tmr2)
{
	if (tmr1->app_timer_second > tmr2->app_timer_second)
		return 1;
	else if (tmr1->app_timer_second < tmr2->app_timer_second)
		return -1;
	else if (tmr1->app_timer_second == tmr2->app_timer_second)
	{
		
		if (tmr1->app_timer_ms > tmr2->app_timer_ms)
			return 1;
		else if (tmr1->app_timer_ms < tmr2->app_timer_ms)
			return -1;
			
		if (tmr1->app_timer_ms == tmr2->app_timer_ms)
			return 0;
	}
	//
	
	return 0;
}

static void __timerout_in(void)
{
	unsigned int i=0;
	for(i=0;i<EI_TIMER_COUNT;i++)
	{
		if (ei_timer[i].enable == 1)
		{
			if ((app_timer_data.app_timer_second - ei_timer[i].his_time) > ei_timer[i].ticks )
			{
				ei_timer[i].his_time = app_timer_data.app_timer_second;
				ei_timer[i].timeout(ei_timer[i].timeout_arg);
				
				if ( ei_timer[i].type == EI_TIMER_TYPE_ONE)
				{
					ei_timer[i].enable = 0;
				}
				
			}
		}
		//
	}
}

static void second_timer(void *p)
{
	__timerout_in();
}

static void app_tmr_timeout(void *arg)
{
	app_timer_data.app_timer_ms += TIMER_PREIOD;
	if (app_timer_data.app_timer_ms >= 1000)
	{
		app_timer_data.app_timer_second ++;
		app_timer_data.app_timer_ms = 0;
		second_timer(0);
	}

}

void init_app_timer(void)
{
	app_timer_data.app_timer_ms = 0;
	app_timer_data.app_timer_second = 0;
}

EI_TIMER ei_timer[EI_TIMER_COUNT];