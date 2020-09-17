/*
* 2016-5-30
* ���� ����-����ҵ��ʹ��-��ֹ���� 3.0 ������� 
*/

#include "find_peak.h"

static int get_max(int sig[],int n);
static int get_min(int sig[],int n);
static int is_exist(int a[],int n,int key);
int find_maximums(int sig[],int n,int maximums[]);
static int get_median(int a[],int n);

/*
* ��inputfile�л�ȡ�ź����У�����ֵ�Ƕ�ȡ�źŵ����� 
* sig:��ȡ�����ź����� 
*/
int input_signal(int sig[],FILE *inputfile){
	int num=0;
	while(!feof(inputfile)){
		int tmp;
		fscanf(inputfile,"%d",&tmp);
		sig[num++] =  tmp;
		if(num >= MAX_SIGNAL_LEN)
			break;
	}
	return num;
} 

/*
* ���ź�������ļ���
* sig:��������У�n:���г��ȣ�outputfile:����ļ� 
*/
void output_signal(int sig[],int n,FILE *outputfile){
	int i;
	for(i=0;i<n;i++){
		fprintf(outputfile,"%d",sig[i]);
		if(i<n-1)
			fprintf(outputfile," ");
	}		
}

/*
* Ѱ�Ҳ��� 
* sig���������У�n1�����г��� ��peaks[]:���沨���λ�� 
* ����ֵ�ǲ�������� 
*/
int find_peaks(int sig[],int n1,int peaks[] )
{
	int maximums[MAX_SIGNAL_LEN];
	int n2 = find_maximums(sig,n1,maximums);
	int max = get_max(sig,n1);
	int min = get_min(sig,n1);
	double threshold = (max - min)*SCALE;
	int i,j,num=0;
	int found = 0;
	for(i=0;i<n2;i++)
	{
		int index = maximums[i];
		if(max - sig[index] <= threshold)
			found = 1;
		else
			continue;
		for(j=index-NEIGHBOUR;j<=index+NEIGHBOUR;j++){
			if(j<0 || j>=n1)
				continue;
			
			if(sig[index] < sig[j])
				found = 0;
			if(sig[index] == sig[j] && is_exist(peaks,num,j))
				found = 0;
		}
		if(index<80||index>MAX_SIGNAL_LEN-100)found = 0;
		if(found == 1)
			peaks[num++] = index;
	}
	return num;
}

/*
* Ѱ�Ҳ��� 
* sig���������У�n1�����г���, troughs[]:���沨�ȵ�λ�� 
* ����ֵ�ǲ��ȵ����� 
* �㷨��ԭʼ���е��෴���Ĳ������ԭʼ���еĲ��� 
*/
int find_troughs(int sig[],int n1,int troughs[] )
{
	int tmp[MAX_SIGNAL_LEN];
	int i;
	for(i=0;i<n1;i++)
		tmp[i] = sig[i] * -1;
	return find_peaks(tmp,n1,troughs); 
} 

/*
* ��ֵ�˲�
* in:�������У�n:�������г���
* out:�˲������У�k:���ֵ�������г��� 
* ����ֵ���˲������г��� 
*/ 
int mean_filter(int in[],int n,int out[],int k){
	if(k > n || k > MAX_SUB_LEN){
		fprintf(stderr,"��ֵ�˲������������\n");
		//exit(1);
	}
	int i,num = 0; 
	for(i=0;i<n-k+1;i++){
		int sum = 0;
		int tmp;
		for(tmp=i;tmp<=i+k-1;tmp++)
			sum = sum + in[tmp];
		out[num++] = sum/k;
	}
	return num;
}

/*
* ��ֵ�˲�
* in:�������У�n:�������г���
* out:�˲������У�k:����ֵ�������г��� 
* ����ֵ���˲������г��� 
*/ 
int median_filter(int in[],int n,int out[],int k){
	if(k > n || k > MAX_SUB_LEN){
		fprintf(stderr,"��ֵ�˲������������\n");
		//exit(1);
	}
	int i,num = 0;
	for(i=0;i<n-k+1;i++){
		int j,index=0;
		int tmp[MAX_SUB_LEN];
		for(j=i;j<=i+k-1;j++){
			tmp[index++] = in[j];
		}
		out[num++] = get_median(tmp,index);
	}
	return num;
}

/*
static int find_maximums(int sig[],int n,int maximums_index[]){
	int i,num=0;
	if(n<=2){
		return num;
	}
	int diff[MAX_SIGNAL_LEN]; //���
	int tmp=0;
	for(i=1;i<n;i++){
		if(sig[i] - sig[i-1] > 0)
			diff[tmp++] = 1;
		else if(sig[i] - sig[i-1] == 0)
			diff[tmp++] = 0;
		else
			diff[tmp++] = -1;		 
	}
	for(i=1;i<tmp;i++){ //�ڶ��β���󼫴�ֵ 
		if(diff[i] - diff[i-1] < 0)
			maximums_index[num++] = i; 
	}
	return num;	
}*/

/*
* Ѱ��sig[]�еļ���ֵ������ 
* ��������� maximums�У����ص��Ǽ�ֵ������ 
*/
 int find_maximums(int sig[],int n,int maximums[]){
	int i,j,num=0;
	for(i=1;i<n-1;i++)
	 {
		if(sig[i] <= sig[i-1])   // sig[i-1] >= sig[i] 
			continue;
		if(sig[i] < sig[i+1])   //  sig[i-1] < sig[i] < sig[i+1]
			continue;
		if(sig[i] > sig[i+1]){  //  sig[i-1] < sig[i] > sig[i+1]  
			maximums[num++] = i;
			continue;
		}
		j = i+2;               // sig[i-1] < sig[i] == sig[i+1]
		while(j<n && sig[j] == sig[i]) 
			j++;
		if(j == n){
			break;
		}
		if(sig[i] > sig[j])
			maximums[num++] = i;
		i = j-1;		
	}
	return num;
}

/*
* Ԥ����ʹ������һ������ƫ�� 
*/
void process(int sig[],int n){
	int i;
	int max = get_max(sig,n);
	int min = get_min(sig,n);
	if(max - min > 1000)
		return ;
	for(i=0;i<n;i++){
		sig[i] = (sig[i] - min)*1000+200;
	}
} 

/*
* �����е����ֵ 
*/
static int get_max(int sig[],int n){
	int i,max = sig[0];
	for(i=1;i<n;i++)
	{
		if(sig[i] > max)
		{
			max = sig[i];
		}	
	}
	return max;
} 

/*
* �����е���Сֵ 
*/
static int get_min(int sig[],int n){
	int i,min = sig[0];
	for(i=1;i<n;i++){
		if(sig[i] < min)
			min = sig[i];
	}
	return min;
} 

/*
* �������Ƿ����ĳ���� 
*/
static int is_exist(int a[],int n,int key){
	int i;
	for(i=0;i<n;i++){
		if(a[i] == key)
			return 1;
	}
	return 0;
}

/*
* ��һ�����е���λֵ,��nΪż��ʱȡ�м���������ƽ��ֵ 
*/
static int get_median(int a[],int n){
	int i,j,tmp;
	for(i=0;i<n;i++){  //���� 
		for(j=i+1;j<n;j++){
			if(a[i] < a[j]){
				tmp = a[i];
				a[i] = a[j];
				a[j] = a[i];
			}
		}
	}
	if(n%2==1)
		return a[(n-1)/2];
	return (a[n/2] + a[n/2-1])/2;
} 


