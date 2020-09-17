#ifndef	_AFE_SPI_H_
#define _AFE_SPI_H_

#include "stm32l1xx.h"

/**********************************宏定义*************************************/

#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int

#define 	AFE_CS_GPIO								GPIOA
#define		AFE_CS_Pin								GPIO_PIN_15

#define 	AFE_CLK_GPIO							GPIOB
#define		AFE_CLK_Pin								GPIO_PIN_3

#define 	AFE_MISO_GPIO							GPIOB
#define		AFE_MISO_Pin							GPIO_PIN_4

#define 	AFE_MOSI_GPIO							GPIOB
#define		AFE_MOSI_Pin							GPIO_PIN_5

#define SPI_CS_H		HAL_GPIO_WritePin(AFE_CS_GPIO, AFE_CS_Pin, GPIO_PIN_SET)
#define SPI_CS_L		HAL_GPIO_WritePin(AFE_CS_GPIO, AFE_CS_Pin, GPIO_PIN_RESET)

#define SPI_MISO 		HAL_GPIO_ReadPin(AFE_MISO_GPIO,AFE_MISO_Pin)

#define SPI_SCK_H		HAL_GPIO_WritePin(AFE_CLK_GPIO, AFE_CLK_Pin, GPIO_PIN_SET)
#define SPI_SCK_L		HAL_GPIO_WritePin(AFE_CLK_GPIO, AFE_CLK_Pin, GPIO_PIN_RESET)

#define SPI_MOSI_H	HAL_GPIO_WritePin(AFE_MOSI_GPIO, AFE_MOSI_Pin, GPIO_PIN_SET)
#define SPI_MOSI_L	HAL_GPIO_WritePin(AFE_MOSI_GPIO, AFE_MOSI_Pin, GPIO_PIN_RESET)



/********************************函数声明*****************************************/

u8 SPI_ReadWriteOneByte(u8 tx_data);
void SPI_Init(void);
u8 SPI_ReadOneByte(void);
void SPI_WriteOneByte(u8 tx_data);





#endif



