#include "math.h"
//#include <intrinsics.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tim.h"
#include "usart.h"
#include "key.h"
//=====模拟前端驱动=====//
#include "afe4400.h"
//====显示功能头文件====//
//#include "oled.h"
#include "spo2datahandle.h"
#include "find_peak.h"
#include "oled.h"
#include "SEGGER_RTT.h"
#include <stdbool.h>

/**********************************参数********************************************/
__IO uint32_t Watchdog_time = 0;    								/*看门时间计算，无传感器的 时候 8s关机*/
__IO uint32_t	Key_Time=0;
__IO uint8_t press_short=0;
__IO uint8_t press_long=0;
__IO	int wave_data_cnt=0;
uint8_t Tim2_Open_flag=1;
int Temp_wave_data[250]={0};
int show_wave_data[250]={0};

extern OLED_SHOW_FLAG  wave_flag;
extern __IO uint8_t direction;
extern uint8_t watchdog_switch;
extern uint8_t show_waveform_cnt;
extern uint8_t use_buff[10];
extern 	uint32_t	Heart_Rate;											/*心率值*/
extern  uint32_t Blood_Oxygen;									/*血氧值*/
extern uint8_t mode_temp;														/*传感器模式，分为有手指按下和无手指按下*/
extern uint8_t watchdog_switch;												/*看门狗计数开启标志*/		
extern __IO uint8_t  readflag;

//extern TIM_HandleTypeDef htim2;
//extern TIM_HandleTypeDef htim3;
//extern TIM_HandleTypeDef htim4;


/************一些参数的宏定义************/
//====计算队列长度====//
#define MAX_LENGTH 	420
#define Array_Data_Length	420
//====窗口队列长度====//
#define WIN_LENGTH	8
//====IIR队列长度====//
#define IIR_NSEC			3
//====IIR滤波器结构体====//
typedef struct {
	long x1;	//x(n+1)
	long x0;	//x(n)
	long y1;	//y(n+1)
	long y2;	//y(n+2)
}IROLD;
typedef struct {
	long x1;	//x(n+1)
	long x0;	//x(n)
	long y1;	//y(n+1)
	long y2;	//y(n+2)
}RDOLD;
//====IIR FILTER 参数====//
const double IR_B[IIR_NSEC] = { 0.00782020 ,  0.01564041, 0.00782020 };
const double IR_A[IIR_NSEC] = { 1        , -1.73472576, 	0.76600660 };
const double RD_B[IIR_NSEC] = {0.00782020 ,  0.01564041, 0.00782020};
const double RD_A[IIR_NSEC] = {1        , -1.73472576, 	0.76600660 };
//========//
IROLD IR_Old;
RDOLD RD_Old;
/**********全局变量***********/

//====发送队列====//

//====脉搏血氧信号====//
unsigned long IR_RAW=0;							// 红外光
unsigned long RD_RAW=0;						// 红光
int IR_Signal=0;										// 红外光，滤波后进入计算窗口
int RD_Signal=0;										// 红光  ， 滤波后进入计算窗口

//====血氧计算变量====//
int R=0;

//====计算队列====//
int RD_group[MAX_LENGTH];					//用于显示，循环队列，存储几个周期内的信号
int IR_group[MAX_LENGTH];
//long Diff_RD_group[MAX_LENGTH];
//long Diff_IR_group[MAX_LENGTH];

//long moving_window[WIN_LENGTH];			//循环队列，以滑动窗口的形式判断当前是否为脉搏波波谷
u16 offset_wave = 0;
u8 window_offset_wave = 0;						//队列头位置
long min;													//队列中最小值
u8 location_min;				 							//最小值位置
u8 location_min_adjust;								//最小值相对与队列头的位置，如果是32则确认一个波谷
//====各种计算标记位====//
//u8 readflag = 0;					//可读标志
u8 flag_initial = 1;				//程序初始化标志位
u8 num_beat = 0;				//初始值为1，下一个脉搏后为2，计算脉搏周期，重新置1
volatile u8 update=0;						//读取数据后，更新标志位
u8 flag_jump = 0;				//脉搏波周期判断，是否处于离开波谷的状态
u8 sample_jump = 0;			//离开波谷时的采样计数，到20则已离开波谷，置0 ，flag_jump 置1

u8 bpm_show_cnt=0;

int sample_count = 0;			//采样计数，每个周期清空，重新计数
int beats=0;						//脉率计数标记
int in=0;							//中断进入标记
int s1=0;							//脉率跳变修正标记
u8 figner=1;						//检测手指是否放入标记

//====最终计算结果====//
#if 1
unsigned int heart_rate = 0;							//脉率最终测量结果，初始值为80.00
unsigned int group_heart_rate[3]={70,70,70};		//最近8秒内的脉率，循环队列，初始化为8000
unsigned int group_SpO2[4]={97,97,97,97};			//最近8秒内的血氧饱和度，循环队列，初始化为9500
int32_t sum_SpO2=388;
#else
unsigned int heart_rate = 0;							//脉率最终测量结果，初始值为80.00
unsigned int group_heart_rate[4]={0};		//最近8秒内的脉率，循环队列，初始化为8000
unsigned int group_SpO2[4]={0};			//最近8秒内的血氧饱和度，循环队列，初始化为9500
int32_t sum_SpO2=0;
#endif
unsigned int sample_heart_rate=80;				//脉率当前原始结果
int heartdiff=0;												//两个前后脉率差分值

unsigned int SpO2 = 97;							//血氧饱和度最终测量结果，初始值为97.00

int offset_SpO2 = 0;										//队列头
int offset_bpm=0;


u8 sample_cycle=10;
u8 cycle_cnt=2;

//====显示&按键标志位====//
char one=0;
char two=0;
char three=1;
int select1=10;
int select2=-1;
int display=0;
unsigned char calculate=0;
unsigned char step1=0;
unsigned char step2=0;
unsigned char send_wave=0;
unsigned char orgin_wave=0;
u8 xin=0;
unsigned char num1=0;
unsigned char num2=0;
u8 warning=0;
u8 way=0;

//====附加功能变量====//
int light=0;
int T=0;
int RH=0;
int UV=0;

//====函数声明====//
long IR_Filter(long input);
void IR_reset(void);
long RD_Filter(long input);
void RD_reset(void);
long diff(int *group, int j);
void WARNING(void);
int IR_FILTER_TEST(int sample);
void Smooth(double data[]);
void DB4DWT(double Data[],int n);
u32 wave_data[10];
unsigned long isqrt32( unsigned long h);



long Transform(long input)        //定义函数返回值为long（32位数据需要）
{
     long output;                
     long preinput, temp; 
     preinput = input&0x00200000;  //数据的第22位是否为1，判断正负
     if (preinput == 0)             //preinput为0，说明输入为正数
     {
            output = input;           //输入数据的后21位即为输出值
               //经过与运算之后，清除前10位数据存储
     }
     else                       //输入数据为负数
     {
            temp = input & 0x001FFFFF;  //获得后21位补码
            output = ~temp + 1;          //补码取反再加1得到原码数据
     }
     return output & 0x001FFFFF;;                  //返回输出值，数据类型为long
}

u32 cnt_data2=0;
void TIM2_IRQHandler()
{
	static bool	heart_show_flag=0;
	static uint16_t heart_cnt=0;       /*心芯刷新*/
  static uint8_t flag=0;            /*1是显示，0是刷新*/
	static	uint8_t j=0;
	static  uint16_t rectangle_cnt=0;
	static	uint16_t	Rail_cnt=0; 
	if(TIM2->SR&0x01)
	{
		TIM2->SR&=~(1<<0);
			if(Tim2_Open_flag == 1)
		{
			if(readflag == 1)
			{
				cnt_data2++;
				
				#if 0
				RD_RAW = Transform(AFE4400_Read_Register(LED2_ALED2VAL));   	 // read RED - Ambient Data
				IR_RAW = Transform(AFE4400_Read_Register(LED1_ALED1VAL));  	 // read IR - Ambient Data
				#else
				
				RD_RAW = AFE4400_Read_Register(LED2_ALED2VAL);   	 // read RED - Ambient Data
				IR_RAW = AFE4400_Read_Register(LED1_ALED1VAL);  	 // read IR - Ambient Data

				#endif
		
				RD_Signal=RD_Filter(RD_RAW);//数据滤波
				IR_Signal=IR_Filter(IR_RAW);//数据滤波
			
				#if 0
				wave_data[cnt]=IR_Signal;
				return_wave_data(wave_data,1);//将脉搏数据可通过AFE4400调试助手显示波形
				#endif
				
				if(IR_Signal<15680000)//IR_Signal>15700000
				{
					mode_temp = SENSOR_OFF;
					watchdog_switch=1;
					wave_flag.sensor_off_flag=1;
					wave_flag.show_rail_flag=0;
					wave_data_cnt=0;
					Heart_Rate=0;
					Blood_Oxygen=0;
					bpm_show_cnt=0;
//					sample_cycle=15;
//					cycle_cnt=3;
					//for(u8 i =0;i<4;i++)group_heart_rate[i]=70;
//					memset(group_SpO2,0,sizeof(unsigned int)*4);
				}
				else
				{
					mode_temp = SENSOR_ON;
					watchdog_switch=0;
					wave_flag.sensor_off_flag = 0;
					
					if(Blood_Oxygen == 0 || Heart_Rate == 0)
					{
						wave_flag.valid_data_flag = 0;
						wave_flag.show_rail_flag = 0;
					}
					else
					{
						wave_flag.valid_data_flag = 1;
						wave_flag.show_rail_flag = 1;
					}
					if(cnt_data2%cycle_cnt==0)
					{
						IR_group[offset_wave/cycle_cnt] = IR_Signal;									/**/
						RD_group[offset_wave/cycle_cnt] =RD_Signal;										/**/
						cnt_data2=0;
					}
					offset_wave = (offset_wave + 1);
					if(offset_wave==MAX_LENGTH*cycle_cnt)
					{		
							offset_wave=0;
							update=1;
							Tim2_Open_flag=0;
							//__HAL_TIM_CLEAR_IT(htim, TIM_IT_UPDATE);
							//__HAL_TIM_DISABLE_IT(&htim2,TIM_FLAG_UPDATE);
							//__HAL_TIM_DISABLE(&htim2);
					}	
				}
				readflag=0;
			}	
		
		#if 1
		if(KEY == 0)
		{
			Key_Time++;
		}
		else
		{
			if(Key_Time>2 && Key_Time<200)
			{
				Key_Time=0;
				press_short=1;
			}
			if(Key_Time>200)
			{
				Key_Time=0;
				press_long=1;
			}
		}
		if(watchdog_switch)
		{
	    Watchdog_time++;
		}
		rectangle_cnt++;
		heart_cnt++;
		Rail_cnt++;
		if(wave_flag.sensor_off_flag == 0)
		{
			/*显示心脏跳动*/
			if(wave_flag.show_full_heart_flag == 1)
			{
				if(heart_cnt%(int)(60000/Heart_Rate/10)==0)
				{
					heart_show_flag=!heart_show_flag;
					heart_cnt=0;
					if(heart_show_flag)
					{
							OLED_Show_Heart(1,direction);
					}
					else
					{
							OLED_Show_Heart(2,direction);
					}
				}				
			}
			
			/*显示平滑波形，无手指按下时或者心率测量中时*/
			if(wave_flag.show_waveform_flag==1 )
			{
				if(wave_flag.valid_data_flag==0)
				{
					if(rectangle_cnt%5==0)
					{
						rectangle_cnt=0;
						show_waveform_cnt++;
						if(show_waveform_cnt == 101)show_waveform_cnt=0;
						OLED_Show_Smooth_wave(show_waveform_cnt);
					}
				}
				else //if(wave_flag.valid_data_flag==1)
				{
					
					if(rectangle_cnt%((int)((60000/Heart_Rate)/50))==0)
					{
							rectangle_cnt=0;
							OLED_Show_Waveform( show_waveform_cnt);
							show_waveform_cnt++;
							if(show_waveform_cnt > 100)show_waveform_cnt=0;
							if(wave_flag.show_rail_flag == 1)
							{
								OLED_Show_Rec(use_buff[j],direction);
								j++;
								if(j>=10)j=0;
							}
					}
					
				}
			}
		}
		/*关闭*/
		else //if(wave_flag.sensor_off_flag == 1)
		{
			if(wave_flag.show_waveform_flag==1)
			{
				if(rectangle_cnt%10==0)
				{
					rectangle_cnt=0;
					show_waveform_cnt++;
					if(show_waveform_cnt == 101)show_waveform_cnt=0;
					OLED_Show_Smooth_wave(show_waveform_cnt);
				}
			}
		}
		#endif
	}
}
	#if 0
	else if(htim->Instance == TIM3)
	{
		#if 1
		if(KEY == 0)
		{
			Key_Time++;
		}
		else //if(KEY == 1)
		{
			if(Key_Time>2 && Key_Time<200)
			{
				Key_Time=0;
				press_short=1;
			}
			if(Key_Time>200)
			{
				Key_Time=0;
				press_long=1;
			}
		}
		if(watchdog_switch)
		{
	    Watchdog_time++;
		}
		#endif
		
		rectangle_cnt++;
		heart_cnt++;
		Rail_cnt++;
		if(wave_flag.sensor_off_flag == 0)
		{
			/*显示心脏跳动*/
			if(wave_flag.show_full_heart_flag == 1)
			{
				if(heart_cnt%(int)(60000/Heart_Rate/5/2)==0)
				{
					flag=~flag;
					heart_cnt=0;
					if(flag)
					{
							OLED_Show_Heart(1,direction);
					}
					else
					{
							OLED_Show_Heart(2,direction);
					}
				}				
			}
			
			/*显示平滑波形，无手指按下时或者心率测量中时*/
			if(wave_flag.show_waveform_flag==1 )
			{
				if(wave_flag.valid_data_flag==0)
				{
					if(rectangle_cnt%5==0)
					{
						rectangle_cnt=0;
						show_waveform_cnt++;
						if(show_waveform_cnt == 101)show_waveform_cnt=0;
						OLED_Show_Smooth_wave(show_waveform_cnt);
					}
				}
				else //if(wave_flag.valid_data_flag==1)
				{
					
					if(rectangle_cnt%((int)((60000/60)/100))==0)
					{
							rectangle_cnt=0;
							OLED_Show_Waveform( show_waveform_cnt);
							show_waveform_cnt++;
							if(show_waveform_cnt > 100)show_waveform_cnt=0;
							if(wave_flag.show_rail_flag == 1)
							{
								OLED_Show_Rec(use_buff[j],direction);
								j++;
								if(j>=10)j=0;
							}
					}
					
				}
			}
		}
		/*关闭*/
		else //if(wave_flag.sensor_off_flag == 1)
		{
			if(wave_flag.show_waveform_flag==1)
			{
				if(rectangle_cnt%20==0)
				{
					rectangle_cnt=0;
					show_waveform_cnt++;
					if(show_waveform_cnt == 101)show_waveform_cnt=0;
					OLED_Show_Smooth_wave(show_waveform_cnt);
				}
			}
		}
	}
	#endif
	
//			else if(wave_flag.show_waveform_flag == 1	&& wave_flag.valid_data_flag==1)
//			{
//					if(rectangle_cnt%((int)((60000/Heart_Rate)/100))==0)
//					{
//							rectangle_cnt=0;
//							OLED_Show_Waveform( show_waveform_cnt);
//							show_waveform_cnt++;
//							if(show_waveform_cnt > 100)show_waveform_cnt=0;
//					}
//			}
//			if(wave_flag.show_rail_flag == 1 && wave_flag.valid_data_flag==1)
//			{
//				if(Rail_cnt%((int)((60000/Heart_Rate)/100))==0)
//				{
//						Rail_cnt=0;
//						OLED_Show_Rec(use_buff[j],direction);
//						j++;
//						if(j>=10)j=0;
//				}	
	
	
	
	
	
	
	#if 0
	else if(htim->Instance == TIM4)
	{
		#if 0
		if(KEY == 0)
		{
			Key_Time++;
		}
		else if(KEY == 1)
		{
			if(Key_Time>3 && Key_Time<200)
			{
				Key_Time=0;
				press_short=1;
			}
			if(Key_Time>200)
			{
				Key_Time=0;
				press_long=1;
			}
		}
		#endif	
		if(watchdog_switch)
		{
	    Watchdog_time++;
		}
		#if 1
		rectangle_cnt++;
		if(rectangle_cnt%((int)(60000/Heart_Rate/100))==0)
		{
			
			if(wave_flag.show_rail_flag == 1)
			{
				OLED_Show_Rec(use_buff[j],direction);
				j++;
				if(j>=10)j=0;
			}
			if(wave_flag.sensor_off_flag == 0 && wave_flag.show_waveform_flag == 1 )
			{
					OLED_Show_Waveform( show_waveform_cnt);
					show_waveform_cnt++;
					if(show_waveform_cnt > 100)show_waveform_cnt=0;
					rectangle_cnt=0;
			}
		}
		if((wave_flag.sensor_off_flag == 1 && wave_flag.show_waveform_flag==1) || (wave_flag.valid_data_flag==0 && wave_flag.sensor_off_flag==0 && wave_flag.show_waveform_flag==1))
		{
				if(rectangle_cnt%10==0)
				{
					rectangle_cnt=0;
					show_waveform_cnt++;
					if(show_waveform_cnt == 101)show_waveform_cnt=0;
					OLED_DrawLine(show_waveform_cnt,54,show_waveform_cnt,63,1);
					
					OLED_DrawLine(show_waveform_cnt+1,52,show_waveform_cnt+1,63,0);
					OLED_DrawLine(show_waveform_cnt+2,52,show_waveform_cnt+2,63,0);
					OLED_DrawLine(show_waveform_cnt+3,52,show_waveform_cnt+3,63,0);
					OLED_DrawLine(show_waveform_cnt+4,52,show_waveform_cnt+4,63,0);
				}
		}
		#endif
	}
	#endif



}
void  HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static bool	heart_show_flag=0;
	static uint16_t heart_cnt=0;       /*心芯刷新*/
  static uint8_t flag=0;            /*1是显示，0是刷新*/
	static	uint8_t j=0;
	static  uint16_t rectangle_cnt=0;
	static	uint16_t	Rail_cnt=0; 
	if(htim->Instance == TIM2)
	{
		if(Tim2_Open_flag == 1)
		{
			if(readflag == 1)
			{
				cnt_data2++;
				
				#if 0
				RD_RAW = Transform(AFE4400_Read_Register(LED2_ALED2VAL));   	 // read RED - Ambient Data
				IR_RAW = Transform(AFE4400_Read_Register(LED1_ALED1VAL));  	 // read IR - Ambient Data
				#else
				
				RD_RAW = AFE4400_Read_Register(LED2_ALED2VAL);   	 // read RED - Ambient Data
				IR_RAW = AFE4400_Read_Register(LED1_ALED1VAL);  	 // read IR - Ambient Data

				#endif
		
				RD_Signal=RD_Filter(RD_RAW);//数据滤波
				IR_Signal=IR_Filter(IR_RAW);//数据滤波
			
				#if 0
				wave_data[cnt]=IR_Signal;
				return_wave_data(wave_data,1);//将脉搏数据可通过AFE4400调试助手显示波形
				#endif
				
				if(IR_Signal<15680000)//IR_Signal>15700000
				{
					mode_temp = SENSOR_OFF;
					watchdog_switch=1;
					wave_flag.sensor_off_flag=1;
					wave_flag.show_rail_flag=0;
					wave_data_cnt=0;
					Heart_Rate=0;
					Blood_Oxygen=0;
					bpm_show_cnt=0;
				
				}
				else
				{
					mode_temp = SENSOR_ON;
					watchdog_switch=0;
					wave_flag.sensor_off_flag = 0;
					
					if(Blood_Oxygen == 0 || Heart_Rate == 0)
					{
						wave_flag.valid_data_flag = 0;
						wave_flag.show_rail_flag = 0;
					}
					else
					{
						wave_flag.valid_data_flag = 1;
						wave_flag.show_rail_flag = 1;
					}
					if(cnt_data2%cycle_cnt==0)
					{
						IR_group[offset_wave/cycle_cnt] = IR_Signal;									/**/
						RD_group[offset_wave/cycle_cnt] =RD_Signal;										/**/
						cnt_data2=0;
					}
					offset_wave = (offset_wave + 1);
					if(offset_wave==MAX_LENGTH*cycle_cnt)
					{		
							offset_wave=0;
							update=1;
							Tim2_Open_flag=0;
							//__HAL_TIM_CLEAR_IT(htim, TIM_IT_UPDATE);
							//__HAL_TIM_DISABLE_IT(&htim2,TIM_FLAG_UPDATE);
							//__HAL_TIM_DISABLE(&htim2);
					}	
				}
				readflag=0;
			}	
		}
		#if 1
		if(KEY == 0)
		{
			Key_Time++;
		}
		else
		{
			if(Key_Time>2 && Key_Time<200)
			{
				Key_Time=0;
				press_short=1;
			}
			if(Key_Time>200)
			{
				Key_Time=0;
				press_long=1;
			}
		}
		if(watchdog_switch)
		{
	    Watchdog_time++;
		}
		rectangle_cnt++;
		heart_cnt++;
		Rail_cnt++;
		if(wave_flag.sensor_off_flag == 0)
		{
			/*显示心脏跳动*/
			if(wave_flag.show_full_heart_flag == 1)
			{
				if(heart_cnt%(int)(60000/Heart_Rate/10/2)==0)
				{
					heart_show_flag=!heart_show_flag;
					heart_cnt=0;
					if(heart_show_flag)
					{
							OLED_Show_Heart(1,direction);
					}
					else
					{
							OLED_Show_Heart(2,direction);
					}
				}				
			}
			
			/*显示平滑波形，无手指按下时或者心率测量中时*/
			if(wave_flag.show_waveform_flag==1 )
			{
				if(wave_flag.valid_data_flag==0)
				{
					if(rectangle_cnt%10==0)
					{
						rectangle_cnt=0;
						show_waveform_cnt++;
						if(show_waveform_cnt == 101)show_waveform_cnt=0;
						OLED_Show_Smooth_wave(show_waveform_cnt);
					}
				}
				else //if(wave_flag.valid_data_flag==1)
				{
					
					if(rectangle_cnt%((int)((60000/Heart_Rate)/100))==0)
					{
							rectangle_cnt=0;
							OLED_Show_Waveform( show_waveform_cnt);
							show_waveform_cnt++;
							if(show_waveform_cnt > 100)show_waveform_cnt=0;
							if(wave_flag.show_rail_flag == 1)
							{
								OLED_Show_Rec(use_buff[j],direction);
								j++;
								if(j>=10)j=0;
							}
					}
					
				}
			}
		}
		/*关闭*/
		else //if(wave_flag.sensor_off_flag == 1)
		{
			if(wave_flag.show_waveform_flag==1)
			{
				if(rectangle_cnt%10==0)
				{
					rectangle_cnt=0;
					show_waveform_cnt++;
					if(show_waveform_cnt == 101)show_waveform_cnt=0;
					OLED_Show_Smooth_wave(show_waveform_cnt);
				}
			}
		}
		#endif
	}
	#if 0
	else if(htim->Instance == TIM3)
	{
		#if 1
		if(KEY == 0)
		{
			Key_Time++;
		}
		else //if(KEY == 1)
		{
			if(Key_Time>2 && Key_Time<200)
			{
				Key_Time=0;
				press_short=1;
			}
			if(Key_Time>200)
			{
				Key_Time=0;
				press_long=1;
			}
		}
		if(watchdog_switch)
		{
	    Watchdog_time++;
		}
		#endif
		
		rectangle_cnt++;
		heart_cnt++;
		Rail_cnt++;
		if(wave_flag.sensor_off_flag == 0)
		{
			/*显示心脏跳动*/
			if(wave_flag.show_full_heart_flag == 1)
			{
				if(heart_cnt%(int)(60000/Heart_Rate/5/2)==0)
				{
					flag=~flag;
					heart_cnt=0;
					if(flag)
					{
							OLED_Show_Heart(1,direction);
					}
					else
					{
							OLED_Show_Heart(2,direction);
					}
				}				
			}
			
			/*显示平滑波形，无手指按下时或者心率测量中时*/
			if(wave_flag.show_waveform_flag==1 )
			{
				if(wave_flag.valid_data_flag==0)
				{
					if(rectangle_cnt%5==0)
					{
						rectangle_cnt=0;
						show_waveform_cnt++;
						if(show_waveform_cnt == 101)show_waveform_cnt=0;
						OLED_Show_Smooth_wave(show_waveform_cnt);
					}
				}
				else //if(wave_flag.valid_data_flag==1)
				{
					
					if(rectangle_cnt%((int)((60000/60)/100))==0)
					{
							rectangle_cnt=0;
							OLED_Show_Waveform( show_waveform_cnt);
							show_waveform_cnt++;
							if(show_waveform_cnt > 100)show_waveform_cnt=0;
							if(wave_flag.show_rail_flag == 1)
							{
								OLED_Show_Rec(use_buff[j],direction);
								j++;
								if(j>=10)j=0;
							}
					}
					
				}
			}
		}
		/*关闭*/
		else //if(wave_flag.sensor_off_flag == 1)
		{
			if(wave_flag.show_waveform_flag==1)
			{
				if(rectangle_cnt%20==0)
				{
					rectangle_cnt=0;
					show_waveform_cnt++;
					if(show_waveform_cnt == 101)show_waveform_cnt=0;
					OLED_Show_Smooth_wave(show_waveform_cnt);
				}
			}
		}
	}
	#endif
	
//			else if(wave_flag.show_waveform_flag == 1	&& wave_flag.valid_data_flag==1)
//			{
//					if(rectangle_cnt%((int)((60000/Heart_Rate)/100))==0)
//					{
//							rectangle_cnt=0;
//							OLED_Show_Waveform( show_waveform_cnt);
//							show_waveform_cnt++;
//							if(show_waveform_cnt > 100)show_waveform_cnt=0;
//					}
//			}
//			if(wave_flag.show_rail_flag == 1 && wave_flag.valid_data_flag==1)
//			{
//				if(Rail_cnt%((int)((60000/Heart_Rate)/100))==0)
//				{
//						Rail_cnt=0;
//						OLED_Show_Rec(use_buff[j],direction);
//						j++;
//						if(j>=10)j=0;
//				}	
	
	
	
	
	
	
	#if 0
	else if(htim->Instance == TIM4)
	{
		#if 0
		if(KEY == 0)
		{
			Key_Time++;
		}
		else if(KEY == 1)
		{
			if(Key_Time>3 && Key_Time<200)
			{
				Key_Time=0;
				press_short=1;
			}
			if(Key_Time>200)
			{
				Key_Time=0;
				press_long=1;
			}
		}
		#endif	
		if(watchdog_switch)
		{
	    Watchdog_time++;
		}
		#if 1
		rectangle_cnt++;
		if(rectangle_cnt%((int)(60000/Heart_Rate/100))==0)
		{
			
			if(wave_flag.show_rail_flag == 1)
			{
				OLED_Show_Rec(use_buff[j],direction);
				j++;
				if(j>=10)j=0;
			}
			if(wave_flag.sensor_off_flag == 0 && wave_flag.show_waveform_flag == 1 )
			{
					OLED_Show_Waveform( show_waveform_cnt);
					show_waveform_cnt++;
					if(show_waveform_cnt > 100)show_waveform_cnt=0;
					rectangle_cnt=0;
			}
		}
		if((wave_flag.sensor_off_flag == 1 && wave_flag.show_waveform_flag==1) || (wave_flag.valid_data_flag==0 && wave_flag.sensor_off_flag==0 && wave_flag.show_waveform_flag==1))
		{
				if(rectangle_cnt%10==0)
				{
					rectangle_cnt=0;
					show_waveform_cnt++;
					if(show_waveform_cnt == 101)show_waveform_cnt=0;
					OLED_DrawLine(show_waveform_cnt,54,show_waveform_cnt,63,1);
					
					OLED_DrawLine(show_waveform_cnt+1,52,show_waveform_cnt+1,63,0);
					OLED_DrawLine(show_waveform_cnt+2,52,show_waveform_cnt+2,63,0);
					OLED_DrawLine(show_waveform_cnt+3,52,show_waveform_cnt+3,63,0);
					OLED_DrawLine(show_waveform_cnt+4,52,show_waveform_cnt+4,63,0);
				}
		}
		#endif
	}
	#endif
}

	int troughs[20]={0};         //波谷位置
	int troughs1[20]={0};
	int peaks[20]={0};           //波峰位置
	int peaks1[20]={0};
	
	u8 spo2_count=0;
	int sum_spo2=0;   					 //血氧总和
	int SUM_SPO2=0;
	int SPO2_AV=0;
	int LAST_SPO2=0;
	int CURR_SPO2=0;
	int tmp_arry1[Array_Data_Length]={0};
	int tmp_arry2[Array_Data_Length]={0};
	float bpm_tmp=0;

	float r=0;
	float sum_r=0;
	u8 r_loop=0;
	u8 calculate_cnt=0;
	u8 peak_cnt=0;
	u8 cnt=0;
	u16 bpmdata=0;
	u8 change_cnt=0;
	int through_diff[8]={0};
	int sum=0;
	int max_diff=0;
	u8 max_index=0;
	u8 increase_cnt=0;
	u8 decrease_cnt=0;
	u16 increase_data[3]={0};
	u16 decrease_data[3]={0};
	#define SAMPLE_5MS 1
	#define SAMPLE_10MS 2 
	#define SAMPLE_15MS 3
	#define BPM_OFFSET  3
void bpm_data(void)						 
{
		
		
		int i=0,j;
		u8 tmp;
		u8 red_peak_cnt=0;
		u8 red_troughs_cnt=0;
		int loop=0;
		
	
		long red_ac[20]={0};
		long red_dc[20]={0};
		long ir_ac[20]={0};
		long ir_dc[20]={0};
		for(j = 0; j < Array_Data_Length; j++ )
		{
			tmp_arry1[j] = IR_group[j];
			tmp_arry2[j] = RD_group[j];
		}
//		SEGGER_RTT_printf(0, "bpm\n" );
//		for(j=0;j<Array_Data_Length;j++)
//		{
//			SEGGER_RTT_printf(0, "%d\n",IR_group[j] );
//		}
//		SEGGER_RTT_printf(0, "spo2\n" );
//		for(j=0;j<Array_Data_Length;j++)
//		{
//			SEGGER_RTT_printf(0, "%d\n",RD_group[j] );
//		}

		memset(troughs,0,80);
		memset(peaks,0,80);
		memset(troughs1,0,80);
		memset(peaks1,0,80);
		sum=0;
		peak_cnt  =	find_peaks(tmp_arry1,Array_Data_Length,peaks);//找出波峰
		cnt =	find_troughs(tmp_arry1,Array_Data_Length,troughs);//找出波谷
		red_peak_cnt =	find_peaks(tmp_arry2,Array_Data_Length,peaks1);
		red_troughs_cnt = find_troughs(tmp_arry2,Array_Data_Length,troughs1);
		
//		if(cnt>=3)
//		{
//			max_diff=through_diff[0];
//			for( loop=0;loop<cnt-1;loop++)
//			{
//				through_diff[loop]=troughs[loop+1]-troughs[loop];
//				if(through_diff[loop]>=max_diff)
//				{
//					max_diff=through_diff[loop];
//					max_index=loop;
//				}
//			}
//			through_diff[max_index]=0;
//			loop-=2;
//		}
		if(cnt<=1)calculate_cnt=peak_cnt;
		else calculate_cnt=cnt;
		
		if(calculate_cnt<2  )
		{
			switch(cycle_cnt)
			{	
				case SAMPLE_5MS:
							cycle_cnt=SAMPLE_10MS;
					break;
				case SAMPLE_10MS:
							cycle_cnt=SAMPLE_15MS;
					break;
//				case SAMPLE_15MS:
//							cycle_cnt=SAMPLE_5MS;
//					break;	
			}
//			if(cycle_cnt==1)cycle_cnt=2;
//			else cycle_cnt=1;
			sample_cycle=5*cycle_cnt;
		}
		else if(cnt==0 || peak_cnt == 0|| cnt >= peak_cnt+2 || cnt <= peak_cnt-2)
		{
		}
		else 
		{
			i=0;
			memset(show_wave_data,0,250*4);
			if(cnt<peak_cnt)
			{
				wave_data_cnt=peaks[1]-peaks[0]+1;
				for(j=peaks[1];j>=peaks[0];j--)
				{
					show_wave_data[i++]=-tmp_arry1[j];
				}
				for(i=0;i<calculate_cnt-1;i++)
				{
					tmp=peaks[i+1]-peaks[i];
					sum+=tmp;
				}
		 }
		 else //if(cnt>=2 )
		 {
				wave_data_cnt=troughs[1]-troughs[0]+1;
				for(j=troughs[0];j<=troughs[1];j++)
				{
					show_wave_data[i++]=tmp_arry1[j];
				}
				for(i=0;i<calculate_cnt-1;i++)
				{
					tmp=troughs[i+1]-troughs[i];//计算每两个波峰之间的采样点数之差
					sum+=tmp;
				}
		 }
		r_loop=(cnt>peak_cnt?peak_cnt:cnt);
		
		for(i=0;i<r_loop;i++)
		{
				red_ac[i]=(tmp_arry2[peaks1[i]%Array_Data_Length]-tmp_arry2[troughs1[i]%Array_Data_Length]) >>4 ;
				red_dc[i] = tmp_arry2[troughs1[i]%Array_Data_Length] >> 8;
				ir_ac[i]=(tmp_arry1[peaks[i]%Array_Data_Length]-tmp_arry1[troughs[i]%Array_Data_Length]) >>4 ;
				ir_dc[i] = tmp_arry1[troughs[i]%Array_Data_Length] >> 8;
				r =  (  (float) ( red_ac[i] * (ir_dc[i]) ) / (float) ( ir_ac[i] * (red_dc[i]) )  );
				r *= 10000 ;
				sum_r+=r;
		}
		r=sum_r/i;
		SpO2 = (int )((1220000-23.7*r)/10000+0);
		if(SpO2>100)SpO2=100;
		//if(SpO2>=80 && SpO2<90)SpO2+=2;
		sum_r=0;
//		if(cnt>=3)
//		{
//			sum-=through_diff[max_index];
//			calculate_cnt--;
//		}
		//SPO2_AV=(int)((SUM_SPO2/(float)(cnt>peak_cnt?peak_cnt:cnt))+1.5);
//		if(sum>150 && calculate_cnt== 2 && sample_cycle==10 && Heart_Rate>70)
//			bpmdata=(int)(1.0*(60000/(((float)sum/(calculate_cnt))*sample_cycle )));
//		else
		
			bpmdata=(int)(1.0*(60000/(((float)sum/(calculate_cnt-1))*sample_cycle )));
//		if(bpmdata<60&&cycle_cnt!=4)
//		{
//			cycle_cnt=4;
//			sample_cycle=5*cycle_cnt;
//			Tim2_Open_flag=1;
//			return ;
//		}
//		if(bpmdata<100)
//		{
//			cycle_cnt=SAMPLE_15MS;
//			sample_cycle=5*cycle_cnt;
//			Tim2_Open_flag=1;
//			return ;
//		}
//		if(cycle_cnt == SAMPLE_15MS && bpmdata > 100)
//		{
//			cycle_cnt=SAMPLE_10MS;
//			sample_cycle=5*cycle_cnt;
//			Tim2_Open_flag=1;
//			return ;
//		}
//		if( sample_cycle==10 && bpmdata > 105)
//		{
//			sample_cycle=5;
//			cycle_cnt=1;
//			Tim2_Open_flag=1;
//			return ;
//		}
//		if ( sample_cycle==5 && bpmdata  <105)
//		{
//			sample_cycle=10;
//			cycle_cnt=2;
//			Tim2_Open_flag=1;
//			return ;
//		}
	//	if(bpmdata-Heart_Rate>10||Heart_Rate-bpmdata>10)bpmdata=Heart_Rate;
		//data=(int)(60000/((float)((sum+cnt*7)/(cnt-1))*10));
			
																														
		if (group_SpO2[offset_SpO2]>100)
		group_SpO2[offset_SpO2] = 100;
			
			//8秒内血氧饱和度之和加上当前的值
		
	
		//if(Blood_Oxygen>100)Blood_Oxygen=100;
		if(4 == bpm_show_cnt)
		{	
			sum_SpO2=0;
			for(u8 jj=0;jj<4;jj++)
			{
				sum_SpO2+=group_SpO2[jj];
			}
			sum_SpO2 -= group_SpO2[offset_SpO2];				
			group_SpO2[offset_SpO2] = SpO2;
			sum_SpO2 += group_SpO2[offset_SpO2];
			offset_SpO2 ++;
			if(offset_SpO2 == 4)offset_SpO2=0;			
			SpO2=sum_SpO2/4;
			Blood_Oxygen=SpO2;
			
			if( bpmdata > 25 && bpmdata <= 300)// &&( bpmdata - Heart_Rate < 20|| Heart_Rate - bpmdata <20))
			{
//				if(bpmdata  > Heart_Rate+8)
//				{
//					increase_data[increase_cnt]=bpmdata;
//					increase_cnt++;
//					if(increase_cnt>=3)
//					{
//						Heart_Rate=(increase_data[0]+increase_data[1]+increase_data[2])/3;
//						increase_cnt=0;
//						offset_bpm=0;
//						for(u8 ii=0;ii<3;ii++)group_heart_rate[ii]=Heart_Rate;
//					}
//				}
//				if( bpmdata < Heart_Rate-8 )
//				{
//					decrease_data[decrease_cnt]=bpmdata;
//					decrease_cnt++;
//					if(decrease_cnt>=3)
//					{
//						Heart_Rate=(decrease_data[0]+decrease_data[1]+decrease_data[2])/3;
//						decrease_cnt=0;
//						offset_bpm=0;
//						for(u8 ii=0;ii<3;ii++)group_heart_rate[ii]=Heart_Rate;
//					}
//					
//				}
//				if(bpmdata  < Heart_Rate + 8 &&  bpmdata > Heart_Rate - 8)
//				{		
					heart_rate=0;
					for(u8 ii=0;ii<BPM_OFFSET;ii++)
					{
						heart_rate+=group_heart_rate[ii];			
					}						
					heart_rate-=group_heart_rate[offset_bpm];
					group_heart_rate[offset_bpm]=bpmdata;
					heart_rate+=group_heart_rate[offset_bpm];
					Heart_Rate=(int)(heart_rate/(BPM_OFFSET*1.0)+1.5);
					offset_bpm++;
					if(offset_bpm == BPM_OFFSET ) offset_bpm=0;
//					Heart_Rate=bpmdata;
	//			}	
			}	
			
		}
		else
		{
			if(bpmdata>300)bpmdata=300;
			if(SpO2>100)SpO2=100;
			Heart_Rate=bpmdata;
			heart_rate=0;
			for(u8 ii=0;ii<BPM_OFFSET;ii++)
			{
				group_heart_rate[ii]=Heart_Rate;
				heart_rate+=group_heart_rate[ii];			
			}	
			for(u8 jj=0;jj<4;jj++)
			{
				group_SpO2[jj]=SpO2;
			}
			Blood_Oxygen=SpO2;
			bpm_show_cnt++;
		}		
		//Heart_Rate=data;
		if(Blood_Oxygen>100)Blood_Oxygen=100;
		if(Heart_Rate>300)Heart_Rate=300;
		//if(Heart_Rate<20)Heart_Rate=25;
		//bpmdata=0;
		//SpO2=0;
	}
		Tim2_Open_flag=1;
		//__HAL_TIM_ENABLE_IT(&htim2,TIM_FLAG_UPDATE);
}
#if 0
//开源参考算法 实际测试时无法运行 估计和算法所需定时器中断频率太高 导致无法有时间执行
void Spo2DataHandle(void)
{
	u16 i=0;
 	while (1) {
 	printf("220\n");
	// 刚开始要积累点数据，大概200数据再进行脉搏周期的判断
			if(flag_initial == 1) 
			{
				if(offset_wave >= MAX_LENGTH-1) 
					{
					flag_initial = 0;
					}
			}
			else
			{
				//等待数据更新，100hz
					if(update==1)
					{
							update=0;
						printf("235\n");
						// 脉搏周期判断
						if (flag_jump == 0) 
						{		 // flag_jump==0，表示处在寻找波谷状态
									sample_jump = 0;		 // 离开波谷时的采样计数置0
									min = moving_window[0]; //寻找group_caculate [SIZE]循环队列中的最小值及其位置
									location_min = 0;
									for (i = 1; i<WIN_LENGTH; i++)
									{
											if (min>moving_window[i])
											{
												min = moving_window[i];
												location_min = i;
											}
									}
								//计算最小值位置距离队列头距离
									if (location_min <= window_offset_wave) 
									{
										location_min_adjust = window_offset_wave - location_min;
									}
									else 
									{
										location_min_adjust = window_offset_wave + WIN_LENGTH - location_min;
									}
								//最小值是否在队列正中
									if(location_min_adjust == (WIN_LENGTH/2) || location_min_adjust == ((WIN_LENGTH/2)+1) ) 
									{
										flag_jump = 1;	//如果是，找到波谷，进入离开波谷状态
										num_beat++;		//脉搏计数增加，如果是程序第一次找到，则由0到1，以后则总是由1到2
										//P1OUT ^= BIT0;
										beats++;
									}
							}
							else 
							{
								sample_jump++;// 离开波谷时的采样计数
								if (sample_jump >=5) 
								{	// 离开波谷时的采样计数到达20
									flag_jump = 0;//认为已离开波谷，则重新寻找下一个波谷
								}
							}
					}
 			
 				// flag_jump==1，表示处在离开波谷状态
 			
			}
 		/*
 		 * 初始值为1，下一个脉搏后为2，计算平均功率和血氧饱和度，重新置1
 		 * 是否处于找到波谷的状态（num_beat由1变为2，找到，否则未找到）
 		 * 未找到则计算两路平方和，采样计数累加，
 		 * 找到则计算脉搏血氧饱和度和脉率，平方和、采样计数置0
 		 */
 		if((num_beat >= 1)) 
			{
				sample_count++;					//采样计数累加
				x = ((RD_group[offset_wave]>>8) * (Diff_IR_group[offset_wave]));
				y =((IR_group[offset_wave]>>8) * (Diff_RD_group[offset_wave]));
				SUM_RD_Diff += (x);	// SUM_X
				SUM_IR_Diff += (y);	// SUM_Y
				SUM_X2 += ((x * x));
				SUM_XY += ((x * y));
 		 }
 		if ((num_beat >= 2)) 
			{

				NUM1 = ( (sample_count * SUM_XY)) ;
				NUM2 =( (SUM_RD_Diff * SUM_IR_Diff));
				DEN1  =(( sample_count * SUM_X2))  ;
				DEN2  = ((SUM_RD_Diff * SUM_RD_Diff));
				R = (100*(NUM1-NUM2))  /  ( DEN1-DEN2);

				//SpO2 = 11000 - 25 * R;
				sum_SpO2 -= group_SpO2[offset_SpO2];		//8秒内血氧饱和度之和减去8秒前的值
				group_SpO2[offset_SpO2] = 11200 - 25 * R;	//计算当先新的脉搏血氧饱和度，拟合公式110-25×R，R为平均功率之比
				//限幅正负300，(offset_SpO2-1)取低3位，使小于8，再加300
				//调整新的脉搏血氧饱和度，变化不能超过3个百分点，范围在85到100之间，文献说明
				if (group_SpO2[offset_SpO2]>(group_SpO2[(offset_SpO2 - 1) & 0x07] + 300))
				group_SpO2[offset_SpO2] = group_SpO2[(offset_SpO2 - 1) & 0x07] + 300;
				else if (group_SpO2[offset_SpO2]<(group_SpO2[(offset_SpO2 - 1) & 0x07] - 300))
					group_SpO2[offset_SpO2] = group_SpO2[(offset_SpO2 - 1) & 0x07] - 300;
				else;
				//限幅8500---10000
				if (group_SpO2[offset_SpO2]>10000)
					group_SpO2[offset_SpO2] = 10000;
				else if (group_SpO2[offset_SpO2]<8500)
					group_SpO2[offset_SpO2] = 8500;
				else;
				//8秒内血氧饱和度之和加上当前的值
				sum_SpO2 += group_SpO2[offset_SpO2];
				offset_SpO2 = (offset_SpO2 + 1) & 0x07;
				//计算平均值，得到最终结果
				SpO2 = sum_SpO2 / 800;
				printf("spo2=%d,heart_rate=%d\n",SpO2,heart_rate);
				//回复初始状态
				SUM_RD_Diff = 0;	// SUM_X
				SUM_IR_Diff = 0;	// SUM_Y
				SUM_X2 = 0;
				SUM_XY = 0;
				num_beat = 1;
				heart_rate = (int)(60000/(sample_count)*40);
				//printf("heart_rate=%d\n",heart_rate);
				sample_count = 0;
				
 		}
 	
 	//else{flag_initial = 1;}
 	}
}
#endif
/*
*  输入：IR 和 R 的变量
*  输出：滤波后的数值
*  设计是0-8.5HZ，采样率是200HZ
*/
long  IR_Filter(long  input) {
	long y0, x2;
	//这里是更新数据
	x2 = IR_Old.x1;
	IR_Old.x1 = IR_Old.x0;
	IR_Old.x0 = input;
	//这里是计算差分方程
	y0 =IR_Old.x0 * IR_B[0] + IR_Old.x1 * IR_B[1] + x2 * IR_B[2] - IR_Old.y1 * IR_A[1] - IR_Old.y2 * IR_A[2];
	y0 = y0 / IR_A[0];
	//这里是更新数据
	IR_Old.y2 = IR_Old.y1;
	IR_Old.y1 = y0;
	//返回计算结果
	return y0;
}

void IR_reset(void) {
	IR_Old.x0 = 0;
	IR_Old.x1 = 0;
	IR_Old.y1 = 0;
	IR_Old.y2 = 0;
}
int IR_FILTER_TEST(int sample)
{
	static int buff[32];
	static int offset;
	u32 z=0;
	buff[offset]=sample;
	for(int i=0;i<8;i++)
	{
		z+=buff[(offset-i)&0x1f];
	}
	offset=(offset+1)&0x1f;
	z=z>>3;
	return z;
}
/*
*  输入：IR 和 R 的变量
*  输出：滤波后的数值
*  设计是0-8.5HZ，采样率是200HZ
*/
long  RD_Filter(long  input) 
{

	long y0, x2;
	//这里是更新数据
	x2 = RD_Old.x1;
	RD_Old.x1 = RD_Old.x0;
	RD_Old.x0 = input;
	//这里是计算差分方程
	y0 =RD_Old.x0 * RD_B[0] + RD_Old.x1 * RD_B[1] + x2 * RD_B[2] - RD_Old.y1 * RD_A[1] - RD_Old.y2 * RD_A[2];
	y0 = y0 / RD_A[0];
	//这里是更新数据
	RD_Old.y2 = RD_Old.y1;
	RD_Old.y1 = y0;
	//返回计算结果
	return y0;
}

void RD_reset(void) 
{
	RD_Old.x0 = 0;
	RD_Old.x1 = 0;
	RD_Old.y1 = 0;
	RD_Old.y2 = 0;
}

long  diff(int *group, int j) 
{

	if(j==0) return 0;//group[0];
	else{
	return (group[j] - group[j - 1]);
	}
}


void Smooth(double data[])//平滑滤波
{
    static long Sum1=0;
    for(int j=0;j<SIZE;j++)
    {
        if(j<N/2)
        {
            for(int k=0;k<N;k++)
            {
                Sum1+=data[j+k];
            }
            data[j]=Sum1/N;
        }
        else
            if(j<SIZE -N/2)
            {
                for(int k=0;k<N/2;k++)
                {
                    Sum1+=(data[j+k]+data[j-k]);
                }
                data[j]=Sum1/N;
            }
            else
            {
                for(int k=0;k<SIZE-j;k++)
                {
                    Sum1+=data[j+k];
                }
                for(int k=0;k<(N-SIZE+j);k++)
                {
                    Sum1+=data[j-k];
                }
                data[j]=Sum1/N;
            }
        Sum1=0;
    }
}
void DB4DWT(double Data[],int n)//DB4小波变换
{
	if(n>=4)
	{
		float h0,h1,h2,h3,g0,g1,g2,g3;
		h0=0.48295;
		h1=0.83651;
		h2=0.22415;
		h3=-0.12940;
		g0=h3;
		g1=-h2;
		g2=h1;
		g3=h0;
		int i,j;
		int half=n>>1;
		double tmp[n];
		for(j=0;j<half;j++)
		{
			tmp[j]=Data[(2*j)%n]*h0+
			Data[(2*j+1)%n]*h1+
			Data[(2*j+2)%n]*h2+
			Data[(2*j+3)%n]*h3;
			tmp[j+half]=Data[(2*j)%n]*g0+
			Data[(2*j+1)%n]*g1+
			Data[(2*j+2)%n]*g2+
			Data[(2*j+3)%n]*g3;
		}
		for(i=0;i<n;i++)
		{
			Data[i]=tmp[i];
		}
	}
}
unsigned long isqrt32( unsigned long h)//开方运算
{
		unsigned long x;
		unsigned long y;
		int i;
		x=0;
		y=0;
	for(i=0;i<32;i++)
		{
				x=(x<<1)| 1;
				if(y<x)
					x-=2;
				else
					y-=x;
				x++;
				y<<= 1;
				if((h&0x80000000))
					y|=1;
				h<<=1;
				y<<= 1;
				if((h&0x80000000))
					y|=1;
				h<<=1;
				y<<= 1;
	  }
	return x;
}

