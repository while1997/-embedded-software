/**
  ******************************************************************************
  * File Name          : ADC.c
  * Description        : This file provides code for the configuration
  *                      of the ADC instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "adc.h"

/* USER CODE BEGIN 0 */
//__IO uint16_t VREFINT_CAL;
//__IO uint16_t VREFINT_DATA;
//VREFINT_CAL = *(__IO uint16_t *)(0x1FF80078);
#define  ADC_SAMPLE_SIZE    30
#define  ADC_FULL_SCALE     4095
__IO float VDDA_VAL;
__IO uint16_t VREFINT_CAL;
__IO uint16_t VREFINT_DATA=0;
 uint32_t	ADC_Temp=0;	
 __IO int16_t AdcValueBuf[30][2];
/*(3.3*(float)VREFINT_CAL)/(float)VREFINT_DATA;*/
/* USER CODE END 0 */

ADC_HandleTypeDef hadc;

/* ADC init function */
void MX_ADC_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
  */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = ADC_AUTOWAIT_DISABLE;
  hadc.Init.LowPowerAutoPowerOff = ADC_AUTOPOWEROFF_DISABLE;
  hadc.Init.ChannelsBank = ADC_CHANNELS_BANK_A;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.NbrOfConversion = 1;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_VREFINT;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_384CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* ADC1 clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();
  
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**ADC GPIO Configuration    
    PB1     ------> ADC_IN9 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();
  
    /**ADC GPIO Configuration    
    PB1     ------> ADC_IN9 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_1);

  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
float Get_Battery_Value(void)
{
	uint16_t i ;
	for(i=0;i<ADC_SAMPLE_SIZE;i++)
	{
		HAL_ADC_Start(&hadc);
		HAL_ADC_PollForConversion(&hadc,50);
		VREFINT_DATA += HAL_ADC_GetValue(&hadc);
	}
	HAL_ADC_Stop(&hadc);
  VREFINT_DATA = VREFINT_DATA/ADC_SAMPLE_SIZE;
	//ADC_Temp=ADC_Temp/ADC_SAMPLE_SIZE;
	//VREFINT_CAL=*(uint16_t *)(0x1FF80078);
	VDDA_VAL =  1.212/((float)(VREFINT_DATA)/(float)ADC_FULL_SCALE);/*3.0*(float)VREFINT_CAL/(float)VREFINT_DATA;*/
	//printf("%d\r\n",VREFINT_DATA);
	VREFINT_DATA = 0;
	return VDDA_VAL;
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	uint16_t i;
	HAL_ADC_Stop_DMA(hadc);
	for(i=0;i<ADC_SAMPLE_SIZE;i++)
	{
		VREFINT_DATA+=AdcValueBuf[i][1];
		ADC_Temp+=AdcValueBuf[i][0];
		AdcValueBuf[i][0]=0;
		AdcValueBuf[i][1]=0;
	}
	VREFINT_CAL=*(uint16_t *)(0x1FF80078);
	VREFINT_DATA/=ADC_SAMPLE_SIZE;
	ADC_Temp/=ADC_SAMPLE_SIZE;
	
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
