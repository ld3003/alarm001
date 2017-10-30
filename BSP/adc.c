#include <stm32f10x.h>
#include <stdio.h>
#include "adc.h"




void adc_init(void)
{
	__IO uint16_t  ADC1ConvertedValue = 0, ADC1ConvertedVoltage = 0;
	__IO uint32_t ADCmvoltp = 0 ;
	ADC_InitTypeDef          ADC_InitStructure;
	GPIO_InitTypeDef         GPIO_InitStructure;
	//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	//	TIM_OCInitTypeDef        TIM_OCInitStructure;
	
	 /* GPIOC Periph clock enable */
  //RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  
  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
  
  /* ADCs DeInit */  
  ADC_DeInit(ADC1);
  
  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStructure);
  
  /* Configure the ADC1 in continous mode withe a resolutuion equal to 12 bits  */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
  ADC_Init(ADC1, &ADC_InitStructure); 
  
  /* Convert the ADC1 Channel 11 with 239.5 Cycles as sampling time */ 
  ADC_ChannelConfig(ADC1, ADC_Channel_1 , ADC_SampleTime_239_5Cycles);   

  /* ADC Calibration */
  ADC_GetCalibrationFactor(ADC1);
  
  /* Enable ADCperipheral[PerIdx] */
  ADC_Cmd(ADC1, ENABLE);     
  
  /* Wait the ADCEN falg */
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN)); 
  
  /* ADC1 regular Software Start Conv */ 
  ADC_StartOfConversion(ADC1);

}

unsigned short get_adc_val()
{
	__IO uint16_t  ADC1ConvertedValue = 0, ADC1ConvertedVoltage = 0;
	__IO uint32_t ADCmvoltp = 0 ;
	
	/* Test EOC flag */
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	
	/* Get ADC1 converted data */
	ADC1ConvertedValue =ADC_GetConversionValue(ADC1);
	
	/* Compute the voltage */
	ADC1ConvertedVoltage = (ADC1ConvertedValue * 3300)/0xFFF;
	
	return ADC1ConvertedVoltage;
}