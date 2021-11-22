#include "adc.h"

#define ADC1_DR_Address    ((uint32_t)0x4001244C)

__IO uint16_t ADCConvertedValue;


void FFT_RCC_Configuration(void)
{
  RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
}

/**
  * @brief  配置FFT所需要的ADC采样管脚
  * @param  None
  * @retval None
  */
void FFT_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure PA.1 (ADC Channel14) as analog input -------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * @brief  ??FFT????ADC??DMA??
  * @param  None
  * @retval None
  */
void FFT_DMA_Init(void)
{
	DMA_InitTypeDef DMA_InitStructure;

  /* DMA1 channel1 configuration ----------------------------------------------*/
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr =(u32) &ADC1->DR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize =1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);
}


/**
  * @brief  ?????ADC
  * @param  None
  * @retval None
  */
void FFT_ADC_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;

  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel1 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_13Cycles5);

  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 reset calibration register */   
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));

  ADC_SoftwareStartConvCmd(ADC1, DISABLE);
}

/**
  * @brief  ???????ADC?????
  * @param  None
  * @retval None
  */
void TIM2_Configuration(void)
{
   	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_DeInit(TIM2);                              //??TIM2???
        
    /* TIM2 configuration */
    TIM_TimeBaseStructure.TIM_Period = 24;        //??????=72M/71+1/25     
    TIM_TimeBaseStructure.TIM_Prescaler = 71;    // ??36000       
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;  // ????  
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //????????
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
 
    /* Clear TIM2 update pending flag[??TIM2??????] */
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
 
    /* Enable TIM2 Update interrupt [TIM2??????]*/
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); 
 
    /* TIM2 enable counter [??tim2??]*/
    TIM_Cmd(TIM2, ENABLE);   
}

/**
  * @brief  ??????????????
  * @param  None
  * @retval None
  */
void TIM2_NVIC_Configuration(void)
{
   NVIC_InitTypeDef NVIC_InitStructure;

   /* Enable the TIM1 global Interrupt */
   NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

   NVIC_Init(&NVIC_InitStructure);
}

