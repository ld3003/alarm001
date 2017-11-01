#ifndef __BSP__
#define __BSP__

#include "RTC_Time.h"
#include "SCCB.h"
#include "IOI2C.h"
#include "standby.h"
#include "dcmi_ov2640.h"
#include "mma845x.h"
#include "bkpreg.h"
#include "adc.h"
#include "standby.h"
#include "wd.h"

#define MODEM_POWER_RCC_TYPE	RCC_APB2PeriphClockCmd
#define MODEM_POEWR_RCC				RCC_APB2Periph_GPIOB
#define MODEM_POEWR_GPIO			GPIOB
#define MODEM_POEWR_PIN				GPIO_Pin_10

#define OV_POWER_RCC_TYPE		RCC_APB2PeriphClockCmd
#define OV_POEWR_RCC				RCC_APB2Periph_GPIOB
#define OV_POEWR_GPIO				GPIOB
#define OV_POEWR_PIN				GPIO_Pin_12

#define LED0_RCC_TYPE		RCC_APB2PeriphClockCmd
#define LED0_RCC				RCC_APB2Periph_GPIOA
#define LED0_GPIO				GPIOA
#define LED0_PIN				GPIO_Pin_15

#define USBEN_RCC_TYPE		RCC_APB2PeriphClockCmd
#define USBEN_RCC				RCC_APB2Periph_GPIOC
#define USBEN_GPIO				GPIOC
#define USBEN_PIN				GPIO_Pin_14


#define CHECK_USB_RCC_TYPE		RCC_APB2PeriphClockCmd
#define CHECK_USB_RCC					RCC_APB2Periph_GPIOB
#define CHECK_USB_GPIO				GPIOB
#define CHECK_USB_PIN					GPIO_Pin_9

void init_uart1(void);
void init_uart2(void);

void uart1_putchar(unsigned char data);
void uart2_putchar(unsigned char data);

void modem_poweron(void);
void modem_poweroff(void);

void ov_poweron(void);
void ov_poweroff(void);

void led0_on(void);
void led0_off(void);
void led0_fanzhuan(void);

void usben_on(void);
void usben_off(void);


void led_ctrl(unsigned char index , unsigned char status);

void setfrq(unsigned char mod);

int read_usb_status(void);

#define ENABLE_USB			usben_on()
#define DISABLE_USB			usben_off()

unsigned short read_vdd_voltage(void);

#endif

