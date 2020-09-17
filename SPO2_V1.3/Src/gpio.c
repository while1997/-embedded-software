/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
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
#include "gpio.h"
/* USER CODE BEGIN 0 */
#include <stdio.h>
__IO uint8_t  KEY1_PRESS = 0;    /*0：表示按键未被按下，1表示按键按下*/
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin : PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 2 */
void Battery_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	BATTERY_GPIO_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = BATTERY_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(BATTERY_PORT, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(BATTERY_PORT,BATTERY_PIN,GPIO_PIN_RESET);
	HAL_Delay(5);
	HAL_GPIO_WritePin(BATTERY_PORT,BATTERY_PIN,GPIO_PIN_SET);
	
}

void Battery_Switch(uint8_t mode)
{
	if(mode == ON)
	{
		HAL_GPIO_WritePin(BATTERY_PORT,BATTERY_PIN,GPIO_PIN_SET);		
	}
	else
	{
		HAL_GPIO_WritePin(BATTERY_PORT,BATTERY_PIN,GPIO_PIN_RESET);		
	}
	
}

void Read_Data(uint8_t *buff,uint8_t mode)
{
	uint8_t i = 0 ;
	if(mode == 0)             /*正常采集数据测试*/
	{
			buff[0]=91;
			buff[1]=93;
			buff[2]=94;
			buff[3]=96;
			buff[4]=95;
			buff[5]=97;
			buff[6]=98;
			buff[7]=98;
			buff[8]=99;
			buff[9]=99;
 }
 else                     /*模拟sensor off 没有手指伸入的状态*/
 {
	 for(i=0;i<10;i++)
		{				
		  buff[i]  = 0xff;								
		}
 }
	
}

/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
