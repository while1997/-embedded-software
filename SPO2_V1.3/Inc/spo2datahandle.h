#ifndef 	_SPO2DATAHANDLE_H
#define  	_SPO2DATAHANDLE_H
#include "common.h"



/**********************************�궨��*************************************/
#define SIZE 				64		/*�����С*/
#define N 					6		/*����ƽ���˲�����ƽ��ֵʱ��ȡ�ĵ���*/

/********************************��������*****************************************/
//extern unsigned int heart_rate ;
//extern unsigned int SpO2;
//extern u8 s_flag;
//extern long sdata[SIZE];
//extern u8 send_cnt;
//extern int cnt;

/********************************��������*****************************************/
void Spo2DataHandle(void);
void return_wave_data(u32 wave_data[],u8 len);
void Smooth(double data[]);

void IR_reset(void);
void RD_reset(void);
void bpmdatahandle(void);
void bpm_data(void);




#endif


