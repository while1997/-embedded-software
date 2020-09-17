#ifndef __OLED_H
#define __OLED_H 

#include "stm32l1xx_hal.h"


#define u8 unsigned char
#define u32 unsigned int

#define CALCULATION_MODE   0x12
#define SENSOR_OFF_MODE    0x13


//-----------------����LED�˿ڶ���---------------- 

#define LED_ON    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15, GPIO_PIN_RESET)
#define LED_OFF   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15, GPIO_PIN_SET)

//-----------------OLED�˿ڶ���---------------- 

#define 		OLED_CLK_GPIO		GPIOA
#define 		OLED_CLK_Pin		GPIO_PIN_0

#define 		OLED_DIN_GPIO		GPIOA
#define 		OLED_DIN_Pin		GPIO_PIN_1

#define 		OLED_RES_GPIO		GPIOA
#define 		OLED_RES_Pin		GPIO_PIN_2

#define 		OLED_DC_GPIO		GPIOA
#define 		OLED_DC_Pin			GPIO_PIN_3

#define 		OLED_CS_GPIO		GPIOA
#define 		OLED_CS_Pin			GPIO_PIN_4




#define OLED_SCLK_Clr() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0, GPIO_PIN_RESET)//CLK
#define OLED_SCLK_Set() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0, GPIO_PIN_SET)

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1, GPIO_PIN_RESET)//DIN
#define OLED_SDIN_Set() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1, GPIO_PIN_SET)

#define OLED_RST_Clr() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2, GPIO_PIN_RESET)//RES
#define OLED_RST_Set() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2, GPIO_PIN_SET)

#define OLED_DC_Clr() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3, GPIO_PIN_RESET)//DC
#define OLED_DC_Set() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3, GPIO_PIN_SET)
 
#define OLED_CS_Clr()  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_RESET)//CS
#define OLED_CS_Set()  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_SET)

#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����


#define SENSOR_OFF  1
#define SENSOR_ON   2

#define	left	      3
#define	right	      4

typedef enum{
	SHOW_NOMAL = 1,     /*����     */
	SHOW_OVERTURN,      /*����     */
	SHOW_LEFT,          /*�����ʾ */
	SHOW_RIGHT,	        /*�ұ���ʾ */
}OLED_SHOW_DIRECTION;

typedef struct{
	
	uint8_t sensor_off_flag;     /*�������رձ�־ 0��δ�رգ�1�������ر�*/	
	uint8_t show_Draw_RecTangle; /*ÿ���ϵ��⵽����������off,��ʾ���α�־��0��ʾ������ʾ��1��ʾ����ʾ*/
	uint8_t show_full_heart_flag;/*��ʼ��ʾʵ�ı�־��1��ʼ��ʾ��0����ʾʵ��*/
	uint8_t show_delay_flag;     /*��ʱ��־��������л�����ĵ�һ������ʱ��Ҫ������ʱ�����л�������������ʱ��0��ʾ��ʱ��1��ʾ����ʱ*/
	uint8_t show_waveform_flag;	 /*��ʼˢ�����ʲ��α�־λ��1��ʾ����ˢ���ʲ��Σ�0��ʾ��ˢ*/
	uint8_t show_rail_flag;			 /*ˢ�º�˱�־��1��ʾ���Զ�̬ˢ�£�0��ʾֻ��ʾ�ĸ����*/
	uint8_t	valid_data_flag;		 /*��Ч���ݱ�־��1��ʾ������Ч��������ʾ��0��ʾ��Ч���ݣ���ʾ���*/
	
}OLED_SHOW_FLAG;

/*************************************************��������************************************************************/

void OLED_ClearPoint(u8 x,u8 y);
void OLED_ColorTurn(u8 i);
void OLED_WR_Byte(u8 dat,u8 cmd);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y);
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode);
void OLED_DrawCircle(u8 x,u8 y,u8 r);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 size1);
//void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1);
void OLED_ScrollDisplay(u8 num,u8 space);
void OLED_WR_BP(u8 x,u8 y);
void OLED_Init(void);
void OLED_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,u8 BMP[]);
void OLED_Show_Rec(uint8_t Heart_Value,uint8_t mode);
void OLED_Show_Waveform(uint32_t Waveform_cnt);
void OLED_Draw_RecTangle(uint8_t x0,uint8_t y0,uint8_t w,uint8_t h,uint8_t mode);
void OLED_Show_Battery(float Vol_Value,uint8_t mode);
void OLED_Show_Start(void);
void OLED_ShowChar_left_Or_right(u8 x,u8 y,u8 chr,u8 size1,uint8_t mode);
void OLED_ShowNum_left_Or_right(u8 x,u8 y,u32 num,u8 size1,uint8_t mode);
void OLED_Show_Heart(uint8_t mode,uint8_t direction);
void test_show(void);
void OLED_Turn_Normal(void);
void OLED_Show_Sensor_off_Rec(uint8_t direction);
void bubble_sort(uint8_t strlen,uint8_t*buff);
void OLED_DisplayTurn(u8 direction,uint8_t sensor,uint8_t *buff,uint8_t show_delay_flag);
void OLED_Show_Smooth_wave(uint32_t Waveform_cnt);

#endif


