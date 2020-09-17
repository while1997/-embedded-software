/*
* 2016-5-30
* ���� ����-����ҵ��ʹ��-��ֹ���� 3.0 ������� 
*/

#ifndef	_FIND_PEAK
#define _FIND_PEAK

#include "stdio.h"
#include "stdlib.h" 

#define MAX_SIGNAL_LEN  420 //�����źŵ���󳤶� 
#define SCALE	0.5 //��ֵ����ϵ�� 
#define NEIGHBOUR 20  //��������뾶 
#define MAX_SUB_LEN 30 //��ֵ�˲����߾�ֵ�˲�ʱ��������󳤶� 


//�������� 
int input_signal(int sig[],FILE *inputfile);

//�������� 
void output_signal(int sig[],int n,FILE *outputfile);

//��ֵ�˲� 
int mean_filter(int in[],int n,int out[],int k);

//��ֵ�˲� 
int median_filter(int in[],int n,int out[],int k);

//Ѱ�Ҳ��� 
int find_peaks(int sig[],int n1,int peaks[] );

//Ѱ�Ҳ��� 
int find_troughs(int sig[],int n1,int troughs[] );

//Ԥ����ʹ������һ������ƫ�� 
void process(int sig[],int n);
 int find_maximums(int sig[],int n,int maximums[]);
#endif


