#include "key.h"

void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
}

/*°´¼üÉ¨Ãèº¯Êý*/
uint8_t	Key_Scan(void)
{
	if(KEY == 1)
	{
		HAL_Delay(5);
		if(KEY == 1)
		{
			while(KEY == 1);
			return KEY_DOWN;			
		}		
	}
  return NO_KEY_DOWN;	
}


