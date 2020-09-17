#ifndef 	_SPO2DATAHANDLE_H
#define  	_SPO2DATAHANDLE_H
#include "common.h"



/**********************************宏定义*************************************/
#define SIZE 				64		/*数组大小*/
#define N 					6		/*滑动平均滤波计算平均值时所取的点数*/

/********************************变量声明*****************************************/
//extern unsigned int heart_rate ;
//extern unsigned int SpO2;
//extern u8 s_flag;
//extern long sdata[SIZE];
//extern u8 send_cnt;
//extern int cnt;

/********************************函数声明*****************************************/
void Spo2DataHandle(void);
void return_wave_data(u32 wave_data[],u8 len);
void Smooth(double data[]);

void IR_reset(void);
void RD_reset(void);
void bpmdatahandle(void);
void bpm_data(void);




#endif


