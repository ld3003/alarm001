/*
********************************************************************************
*
*                                 bsp.c
*
* File          : bsp.c
* Version       : V1.0
* Author        : whq
* Mode          : Thumb2
* Toolchain     : 
* Description   :   系统初始化
                    IO初始化
                    时钟初始化
*                
* History       :
* Date          : 2013.07.21
*******************************************************************************/



#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_iwdg.h"
#include "bsp.h"
#include "BSP_Uart.h"



typedef const struct {
    GPIO_TypeDef    *port;      //IO 时钟
    uint32_t        clk;        //IO 端口
    uint16_t        pin;        //IO 引脚
    uint8_t         state;      //IO 输入输出状态 0)输出 1)输入 2)上拉输入 3)下拉输入 4)开漏输出
    uint8_t         id;         //IO id号
}PORT_INFO_t;


//端口定义 所要进行AD采集的端口修改此数组
static PORT_INFO_t m_PortInfo[] = {
    {GPIOF, RCC_APB2Periph_GPIOA,  GPIO_Pin_15,     0,     eLED_0},   
    //{GPIOF, RCC_APB2Periph_GPIOF,  GPIO_Pin_7,     0,     eLED_1},
    //{GPIOF, RCC_APB2Periph_GPIOF,  GPIO_Pin_8,     0,     eLED_2},
};

static uc16 m_PortCount = BSP_CONEOF(m_PortInfo);




/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    PORT_INFO_t *pPort = m_PortInfo;
    uint8_t i = 0;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    for (i = 0; i < m_PortCount; i++, pPort++)
    {
        RCC_APB2PeriphClockCmd(pPort->clk, ENABLE);
        GPIO_InitStructure.GPIO_Pin = pPort->pin;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
        
        if (pPort->state == 0) GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        else if (pPort->state == 1) GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        else if (pPort->state == 2) GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        else if (pPort->state == 3) GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
        else if (pPort->state == 4) GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
        else if (pPort->state == 5) GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
            
        GPIO_Init(pPort->port, &GPIO_InitStructure);
    }
}


/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length.
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(volatile s32 nCount)
{
    for(; nCount != 0; nCount--);
}

/*******************************************************************************
函 数 名：void IWDG_Init(void)
功能描述：看门狗初始化						
入口参数：							
返回参数：
创建时间: 2011.6.24
********************************************************************************/
void IWDG_Init(void)
{
    IWDG_WriteAccessCmd( IWDG_WriteAccess_Enable );
    IWDG_SetPrescaler( IWDG_Prescaler_128);	//最小
    IWDG_SetReload(0x138 * 5);		//40KHz内部时钟 (1/40000 * 128 * 0x138 * 5= 5s )
    IWDG_WriteAccessCmd( IWDG_WriteAccess_Disable );
    IWDG_Enable();
    IWDG_ReloadCounter();
}

/*******************************************************************************
* Function Name : void IWDG_Clear(void)
* Description   : 清看门狗
* Input         : 
* Output        : 
* Other         : 
* Date          : 2014.04.13
*******************************************************************************/
void WDG_Clear(void)
{
    IWDG_ReloadCounter();
}

/*******************************************************************************
* Function Name :void SysTickInit(void)
* Description   :系统定时器时间配置
* Input         :
* Output        :
* Other         :时基为1ms
* Date          :2011.11.03  12:59:13
*******************************************************************************/


void SysTickInit(void)
{
    RCC_ClocksTypeDef rccClk = {0};
    
    RCC_GetClocksFreq(&rccClk);
    
    SysTick_Config(rccClk.HCLK_Frequency / 100);

		
}

/*******************************************************************************
* Function Name :void InterruptOrder(void)
* Description   :中断向量，优先级
* Input         :
* Output        :
* Other         :
* Date          :2011.10.27  11:50:05
*******************************************************************************/
void NVIC_Configuration(void)
{
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4);//优先级设置  全为抢占式优先级
}

/*******************************************************************************
* Function Name :void SystemConfig(void)
* Description   :系统初始化
* Input         :
* Output        :
* Other         :
* Date          :2011.10.27  13:14:59
*******************************************************************************/
void BSP_Init(void)
{
    NVIC_Configuration();	//中断优先级设置
    GPIO_Configuration();	//
}

/*******************************************************************************
* Function Name : void BSP_GpioSet(u32 n)
* Description   : IO 置高
* Input         : 
* Output        : 
* Other         : 
* Date          : 2013.07.21
*******************************************************************************/
void BSP_GpioSet(int n)
{
    if (n < 0) return;
    if (n >= eGPIO_MAX_COUNT) return;
    
    m_PortInfo[n].port->BSRR = m_PortInfo[n].pin;
}


/*******************************************************************************
* Function Name : void BSP_GpioReset(u32 n)
* Description   : IO 置低
* Input         : 
* Output        : 
* Other         : 
* Date          : 2013.07.21
*******************************************************************************/
void BSP_GpioReset(int n)
{
    if (n < 0) return;
    if (n >= eGPIO_MAX_COUNT) return;
    
    m_PortInfo[n].port->BRR = m_PortInfo[n].pin;
}

/*******************************************************************************
* Function Name : void BSP_GpioToggle(int n)
* Description   : IO 电平翻转
* Input         : 
* Output        : 
* Other         : 
* Date          : 2014.10.11
*******************************************************************************/
void BSP_GpioToggle(int n)
{
    if (n < 0) return;
    if (n >= eGPIO_MAX_COUNT) return;
    
    m_PortInfo[n].port->ODR ^= m_PortInfo[n].pin;
}


/*******************************************************************************
* Function Name : int32_t BSP_GpioRead(int32_t n)
* Description   : 读GPIO
* Input         : 
* Output        : 错误返回 -1
* Other         : 
* Date          : 2013.12.29
*******************************************************************************/
int32_t BSP_GpioRead(int32_t n)
{
    if (n < 0) return -1;
    if (n >= eGPIO_MAX_COUNT) return -1;

    return GPIO_ReadInputDataBit(m_PortInfo[n].port, m_PortInfo[n].pin);
}

/*******************************************************************************
* Function Name : void BSP_PortWrite(int32_t n, int32_t value)
* Description   : 端口写函数
* Input         : 
* Output        : 
* Other         : 
* Date          : 2014.03.25
*******************************************************************************/
void BSP_PortWrite(int32_t n, int32_t value)
{
    if (n < 0) return;
    if (n >= eGPIO_MAX_COUNT) return;

    m_PortInfo[n].port->BSRR = value | ((value ^ 0xFF) << 16); 
}

/*******************************************************************************
* Function Name : void BSP_SystemReset(void)
* Description   : 系统复位
* Input         : 
* Output        : 
* Other         : 
* Date          : 2014.08.25
*******************************************************************************/
void BSP_SystemReset(void)
{
    NVIC_SystemReset();
}


