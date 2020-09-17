#include "beep.h"

void Beep_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	BEEP_IO_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = BEEP_IO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(BEEP_IO_PORT, &GPIO_InitStruct);	
	HAL_GPIO_WritePin(BEEP_IO_PORT,BEEP_IO_PIN,GPIO_PIN_SET);	
}

void Delay_ms(uint32_t time)
{
	uint16_t i,j,k;
	for(i=0;i<time;i++)
	{
		for(j=0;j<45;j++)
		{
			for(k=0;k<101;k++)
			{
				
			}
		}
	}
}



/**
  *@brief  control beep to sound
  *@para   uint16-t time ：控制蜂鸣器发生的长久的时间变量
  *@retval  None
  */


/*无源蜂蜜器需要2-5KHZ的方波才能发声*/
void Beep_Sound(uint16_t time)
{
	while(time--)
	{
		BEEP_H;
		HAL_Delay(200);
		BEEP_L;
		HAL_Delay(200);
	}
}


