#ifndef __app_timer__
#define __app_timer__

struct APP_TIMER {
	unsigned int app_timer_second;
	unsigned int app_timer_ms;
};

extern struct APP_TIMER app_timer_data;
extern void init_app_timer(void);

void app_timer_add(struct APP_TIMER *tmr , unsigned int ms);
int app_timer_cmp(struct APP_TIMER *tmr1 , struct APP_TIMER *tmr2);

typedef struct {
	
	//setting
	unsigned char type;					//类型
	unsigned long ticks;				//周期时间
	
	//private:
	unsigned char num;
	unsigned long ticksleft;
	unsigned int his_time;

	//public:
	void (*timeout)(void *arg);
	void *timeout_arg;

	//Atomic:
	unsigned char enable;
}EI_TIMER;


#define EI_TIMER_COUNT 5
extern EI_TIMER ei_timer[EI_TIMER_COUNT];

#define EI_TIMER_TYPE_ONE 1


#endif