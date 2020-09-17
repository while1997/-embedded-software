#ifndef __KEY_H
#define __KEY_H	
#include "stm32l1xx_hal.h"

/***************************************按键端口定义******************************************************/

#define 	KEY        HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6)  //KEY按键PA6
#define KEY_DOWN           1
#define KEY_UP             2
#define NO_KEY_DOWN        0



/*************************************************函数声明************************************************************/

void Key_Init(void);
uint8_t	Key_Scan(void);



#endif
