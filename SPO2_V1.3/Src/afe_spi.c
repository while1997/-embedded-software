#include "afe_spi.h"
#include "spi.h"
//#include "stm32l1xx_hal_gpio.h"

void SPI_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
//	__HAL_RCC_GPIOA_CLK_ENABLE();
  //__HAL_RCC_GPIOB_CLK_ENABLE();
	
	/*Configure GPIO pin : PB15 SPISTE/CS */
  GPIO_InitStruct.Pin = AFE_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(AFE_CS_GPIO, &GPIO_InitStruct);
	#if 0
  /*Configure GPIO pin : PB3 SPISCK*/
	GPIO_InitStruct.Pin = AFE_CLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(AFE_CLK_GPIO, &GPIO_InitStruct);
	
	/*Configure GPIO pin : PB4 SPIMISO*/
  GPIO_InitStruct.Pin = AFE_MISO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(AFE_MISO_GPIO, &GPIO_InitStruct);

	/*Configure GPIO pin : PB5 SPIMOSI*/
	GPIO_InitStruct.Pin = AFE_MOSI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(AFE_MOSI_GPIO, &GPIO_InitStruct);
	#endif
}
void delay_us(uint32_t us)
{
    uint32_t delay = (HAL_RCC_GetHCLKFreq() / 4000000 * us);
    while (delay--)
	{
		;
	}
}
/*
函数功能: SPI底层读写一个字节
特性:
1. 时钟线空闲电平为低电平。
2. 数据在时钟线为低电平的时候发出
3. 读数据是在高电平
4. 数据先发高位
*/
u8 SPI_ReadWriteOneByte(u8 tx_data)
{
		u8 rx_data=0;
	  u8 i;
		for(i=0;i<8;i++)
		{		
				SPI_SCK_L;
				//HAL_Delay(1);
				if(tx_data&0x80)SPI_MOSI_H;
			  else SPI_MOSI_L;
				delay_us(1);
				tx_data<<=1; //继续发送下一位		
			
				SPI_SCK_H;	
				rx_data<<=1;
				if(SPI_MISO)rx_data|=0x01;
				delay_us(1);							
		}
		SPI_SCK_L;
		delay_us(50);	
		return rx_data;
}

u8 SPI_ReadOneByte(void)
{
	#if 0
		u8 rx_data=0;
	  u8 i;
		for(i=0;i<8;i++)
		{		
				SPI_SCK_L;			
				rx_data<<=1;
				if(SPI_MISO)rx_data|=0x01;
				delay_us(1);				
				SPI_SCK_H;	
				delay_us(1);				
		}
		SPI_SCK_L;
		delay_us(1);	
		return rx_data;
	#else
		
		uint8_t RxData;
		HAL_SPI_Receive(&hspi1,&RxData,1,0xffff);
	  return RxData;
	#endif
}


void SPI_WriteOneByte(u8 tx_data)
{
	#if 0
	  u8 i;
		for(i=0;i<8;i++)
		{		
				SPI_SCK_L;
				//HAL_Delay(1);
				if(tx_data&0x80)SPI_MOSI_H;
			  else SPI_MOSI_L;
				delay_us(1);
				tx_data<<=1; //继续发送下一位		
				SPI_SCK_H;	
				delay_us(1);				
		}
		SPI_SCK_L;
		delay_us(1);	
	#else
		HAL_SPI_Transmit(&hspi1,&tx_data,1,0xffff);
		
	#endif
}


