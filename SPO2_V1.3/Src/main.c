/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "key.h"
#include "beep.h"
#include "afe_spi.h"
#include "afe4400.h"
#include "spo2datahandle.h"
#include <stdio.h>
#include <string.h>
#include "SEGGER_RTT.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define DEBUG 
uint8_t use_buff[10] ={91,93,94,96,95,97,98,98,99,99,};
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
__IO float battery_value ;												/*Battery Voltage*/
__IO uint8_t direction  = SHOW_NOMAL;							/*上电默认为正常显示模式*/
uint8_t key_press_num = 1;        								/*2:反向,3：左边 4：右边，1：正向，上电默认正向*/
uint8_t mode_temp = 2;														/*传感器模式，分为有手指按下和无手指按下*/
uint8_t watchdog_switch=0;												/*看门狗计数开启标志*/							
OLED_SHOW_FLAG  wave_flag;												/*标志位结构体*/
__IO uint8_t show_waveform_cnt=0;											/*波形显示计数值，当该值大于100时从0开始重新开始*/
uint32_t	Heart_Rate=0;											/*心率值*/
uint32_t Blood_Oxygen=0;											/*血氧值*/


uint8_t ch;
extern __IO uint32_t Watchdog_time;								/*看门狗计数，达到8s关机*/	
extern  uint32_t	ADC_Temp;	
extern ADC_HandleTypeDef hadc;
extern __IO uint16_t VREFINT_DATA;
extern __IO uint16_t VREFINT_CAL;
extern __IO int16_t AdcValueBuf[30][2];
extern volatile u8 update;						//读取数据后，更新标志位
extern __IO uint8_t press_short;
extern __IO uint8_t press_long;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	//uint8_t temp;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  //MX_USART1_UART_Init();
  MX_ADC_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
	Battery_IO_Init();
	OLED_Init();
	Key_Init();
	OLED_Show_Start();
	OLED_Refresh();
	AFE4400_PowerOn_Init();
	OLED_Clear();
  battery_value = Get_Battery_Value();
	OLED_Show_Battery(battery_value,1);
	//printf("battery_value=%.3f\r\n",battery_value);
	OLED_ShowString(0,0,(unsigned char *)"%SpO2",16);
	OLED_ShowString(56,0,(unsigned char *)"PRbpm",16);
	OLED_Refresh();
	MX_TIM2_Init();	
	//MX_TIM3_Init();
  //MX_TIM4_Init();
	HAL_TIM_Base_Start_IT(&htim2);
  //HAL_TIM_Base_Start_IT(&htim3);
	//HAL_TIM_Base_Start_IT(&htim4);
  memset(&wave_flag,0,sizeof(wave_flag));
	IR_reset();
	RD_reset();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		
		#if 1
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		  /*读取数据*/
		  //Read_Data(use_buff,0);
			
      /*对数据初步处理*/
			if(Blood_Oxygen == 0 || Heart_Rate == 0)
			{
					watchdog_switch=1;
					if(Watchdog_time>0 && Watchdog_time<=200)
					{
						//mode_temp = SENSOR_ON;
						wave_flag.valid_data_flag= 0;
					}
					else if(Watchdog_time>200 && Watchdog_time<1600)
					{
						mode_temp = SENSOR_OFF;
					}
					else if(Watchdog_time>=1600)
					{
						mode_temp = SENSOR_OFF;
						Watchdog_time = 0;
						watchdog_switch=0;
						Battery_Switch(0);
						/*刷平滑的波*/
						wave_flag.sensor_off_flag = 1;
						wave_flag.show_rail_flag = 0;
					}
			}
			else 
			{
				mode_temp = SENSOR_ON;
				watchdog_switch=0;
				Watchdog_time=0;
				wave_flag.sensor_off_flag = 0;
				wave_flag.show_rail_flag = 1;
				wave_flag.valid_data_flag= 1;
				wave_flag.show_full_heart_flag=1;
			}
      /*按键扫描*/			
      //temp = Key_Scan();				 
		  switch(direction)
			{
				 case SHOW_NOMAL:  /*正面*/				 
				   { 
							wave_flag.show_waveform_flag=1;
							if(mode_temp ==SENSOR_OFF)
							{	                							
								OLED_DisplayTurn(SHOW_NOMAL,SENSOR_OFF,use_buff,wave_flag.show_delay_flag);
								OLED_Refresh(); 
							}
							else
							{								
                OLED_DisplayTurn(SHOW_NOMAL,SENSOR_ON,use_buff,wave_flag.show_delay_flag);
  						  OLED_Refresh(); 
							}
							if(press_short ==1)	//temp == KEY_DOWN
							{
								press_short=0;
								key_press_num++;
							  if(key_press_num == 2)
							  {
									/*反向显示*/
									show_waveform_cnt=0;
									wave_flag.show_waveform_flag=1;
									OLED_Draw_RecTangle(0,54,101,16,3);
									direction = SHOW_OVERTURN;								
							  }
							}
						}						
					   break;
				 case SHOW_OVERTURN:   /*反面*/
					  
				    if(mode_temp ==SENSOR_OFF)
						{
							OLED_DisplayTurn(SHOW_OVERTURN,SENSOR_OFF,use_buff,wave_flag.show_delay_flag);   
							OLED_Refresh();
						}
						else
						{
							OLED_DisplayTurn(SHOW_OVERTURN,SENSOR_ON,use_buff,wave_flag.show_delay_flag);   
							OLED_Refresh();
						}
				    if(press_short ==1)	//temp == KEY_DOWN
						{
								press_short=0;
								key_press_num++;
								if(key_press_num == 3)
								{
									wave_flag.show_waveform_flag=0;
									wave_flag.show_delay_flag = 0;
									direction = SHOW_LEFT;
									OLED_Turn_Normal();
									OLED_Clear();								 
								}
						}
						 break;
				case SHOW_LEFT:   /*左边*/
					   if(mode_temp ==SENSOR_OFF)
						 {							 
				       OLED_DisplayTurn(SHOW_LEFT,SENSOR_OFF,use_buff,wave_flag.show_delay_flag);
							 OLED_Refresh();							 
						 }
						 else
						 {							 
							 OLED_DisplayTurn(SHOW_LEFT,SENSOR_ON,use_buff,wave_flag.show_delay_flag);
							 OLED_Refresh();
						 }
						  if(press_short ==1)	//temp == KEY_DOWN
						 {
							 press_short=0;
							 key_press_num++;
							 if(key_press_num == 4)
							 {
								 wave_flag.show_delay_flag = 0;
								 direction = SHOW_RIGHT;
								 OLED_Clear();
							 }							 
						 }
					   break;
				case SHOW_RIGHT:   /*右边*/
						 
					   if(mode_temp ==SENSOR_OFF)
						 {
							 OLED_DisplayTurn(SHOW_RIGHT,SENSOR_OFF,use_buff,wave_flag.show_delay_flag);
							 OLED_Refresh();														
						 }
						 else
						 {							 
							 OLED_DisplayTurn(SHOW_RIGHT,SENSOR_ON,use_buff,wave_flag.show_delay_flag);
							 OLED_Refresh();
						 }
						  if(press_short ==1)	//temp == KEY_DOWN
						 {
							 press_short=0;
							 key_press_num++;
							 if(key_press_num == 5)
							 {
								 show_waveform_cnt=0;
								 wave_flag.show_waveform_flag=1;
								 OLED_Draw_RecTangle(0,54,101,16,3);
								 key_press_num = 1;
								 direction = SHOW_NOMAL;
								 OLED_Clear();
							 }							 
						 }
					   break;
				default:
					  break;				
			}
	#endif
		if(update==1)
		{
				bpm_data();
				update=0;
		}	
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
