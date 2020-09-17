#ifndef   __BEEP_H__
#define  __BEEP_H__



#include "stm32l1xx_hal.h"


#define BEEP_IO_PORT             GPIOA
#define BEEP_IO_PIN              GPIO_PIN_12
#define BEEP_IO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()

#define BEEP_H                   HAL_GPIO_WritePin(BEEP_IO_PORT,BEEP_IO_PIN,GPIO_PIN_SET);	 
#define BEEP_L                   HAL_GPIO_WritePin(BEEP_IO_PORT,BEEP_IO_PIN,GPIO_PIN_RESET);


void Beep_IO_Init(void);
void Beep_Sound(uint16_t time);


#endif


