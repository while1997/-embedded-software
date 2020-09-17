#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"
#include <math.h>
#include "stdio.h"


u8 OLED_GRAM[128][8];																	/*OLED��ʾ������*/
extern __IO float battery_value;											/*��ص�ѹ*/
extern OLED_SHOW_FLAG  wave_flag;
extern 	uint32_t	Heart_Rate;												/*����ֵ*/
extern 	uint32_t Blood_Oxygen;										/*Ѫ��ֵ*/

//���Ժ���
void OLED_ColorTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xA6,OLED_CMD);//������ʾ
		}
	if(i==1)
		{
			OLED_WR_Byte(0xA7,OLED_CMD);//��ɫ��ʾ
		}
}

//��Ļ��ת180��
void OLED_DisplayTurn(u8 direction,uint8_t sensor,uint8_t *buff,u8 show_delay_flag)
{
	  //static uint8_t RecTangle_cnt = 0;     /*���д�������ʱ��ˢ��10֮��ʼ�����ο�ʼ��ʧ����ʼ��ʾ����*/
	  if(direction==SHOW_NOMAL)
		{

			OLED_Show_Battery(battery_value,1);
			OLED_ShowString(0,0,(unsigned char *)"%SpO2",16);
			OLED_ShowString(56,0,(unsigned char *)"PRbpm",16);	
	    if(sensor ==  SENSOR_OFF)  /*�޴�����*/
			{
				 OLED_Show_Heart(3,1);     /*��ʾ*/
				 OLED_Show_Sensor_off_Rec(direction);
				 OLED_Draw_RecTangle(8,20,92,32,3);
				 OLED_ShowString(10,25,(unsigned char *)"Sensor",16);
				 OLED_ShowString(65,25,(unsigned char *)"Off",16);
			}
			else if(sensor ==  SENSOR_ON)                      /*�д�����*/
			{
				 if(wave_flag.valid_data_flag == 0 && wave_flag.show_rail_flag == 0)
				 {
						 /*����ʾ����*/
						 OLED_Show_Heart(3,1);     /*��ʾ*/
					 
						OLED_Draw_RecTangle(8,20,92,32,3);
					 
						 /*��ʾ����*/	
						 OLED_Draw_RecTangle(16,35,14,6,1);
						 OLED_Draw_RecTangle(33,35,14,6,1);	
						 
						 OLED_Draw_RecTangle(67,35,14,6,1);
						 OLED_Draw_RecTangle(84,35,14,6,1);
						
						 //OLED_Refresh();
						 OLED_Show_Sensor_off_Rec(direction);				 
					} 
          else if(wave_flag.valid_data_flag == 1) //if(wave_flag.valid_data_flag == 1)
					{
						/*��ʼˢ������*/
						OLED_Draw_RecTangle(8,20,92,32,3);
            wave_flag.show_Draw_RecTangle = 1; 
            /*�ÿ�ʼ��ʾʵ�ı�־*/	
            wave_flag.show_full_heart_flag = 1;
						OLED_ShowNum(8,20,Blood_Oxygen,32);
						OLED_ShowNum(64,26,Heart_Rate,24);
				 }					 
			}			
		}
  	else if(direction== SHOW_OVERTURN)
		{
			OLED_WR_Byte(0xC0,OLED_CMD);   /*��ת��ʾ*/
			OLED_WR_Byte(0xA0,OLED_CMD);
			OLED_Show_Battery(battery_value,1);
			OLED_ShowString(0,0,(unsigned char *)"%SpO2",16);
			OLED_ShowString(56,0,(unsigned char *)"PRbpm",16);	
	    if(sensor ==  SENSOR_OFF)  /*�޴�����*/
			{
				 OLED_Show_Heart(3,1);     /*��ʾ*/
				 OLED_Show_Sensor_off_Rec(direction);
				 OLED_Draw_RecTangle(8,20,92,32,3);
				 OLED_ShowString(10,25,(unsigned char *)"Sensor",16);
				 OLED_ShowString(65,25,(unsigned char *)"Off",16);
			}
			else if(sensor ==  SENSOR_ON)                      /*�д�����*/
			{
				 if(wave_flag.valid_data_flag == 0)
				 {
						 /*����ʾ����*/
						 OLED_Show_Heart(3,1);     /*��ʾ*/
					 
						OLED_Draw_RecTangle(8,20,92,32,3);
					 
						 /*��ʾ����*/	
						 OLED_Draw_RecTangle(16,35,14,6,1);
						 OLED_Draw_RecTangle(33,35,14,6,1);	
						 
						 OLED_Draw_RecTangle(67,35,14,6,1);
						 OLED_Draw_RecTangle(84,35,14,6,1);
						
						 OLED_Refresh();
						 OLED_Show_Sensor_off_Rec(direction);				 
					} 
          else //if(wave_flag.valid_data_flag == 1)
					{
						/*��ʼˢ������*/
						OLED_Draw_RecTangle(8,20,92,32,3);
            wave_flag.show_Draw_RecTangle = 1; 
            /*�ÿ�ʼ��ʾʵ�ı�־*/	
            wave_flag.show_full_heart_flag = 1;
						OLED_ShowNum(8,20,Blood_Oxygen,32);
						OLED_ShowNum(64,26,Heart_Rate,24);
				 }					 
			}			
			OLED_Refresh();
		}
    else if(direction == SHOW_LEFT)  /*�����ʾ*/
		{
			OLED_Show_Battery(battery_value,2);
			/*��ʾ%Sp02*/
			OLED_ShowPicture(14,22,16,40,(uint8_t *)asc2_SpO2_left_bmp);
			/*��ʾPRbpm*/
			OLED_ShowPicture(68,22,16,40,(uint8_t *)asc2_PRbpm_left_bmp);
			if(sensor ==  SENSOR_OFF)  /*�޴�����*/
			{
		    /*Ѫ�����Ͷ���ʾ���*/
				 OLED_Show_Heart(3,3);
				 OLED_Show_Sensor_off_Rec(direction);
				 
				/*�����ʾ�������*/
				 OLED_Draw_RecTangle(30,22,38,44,3);
				 OLED_Draw_RecTangle(84,22,44,44,3);
				
				/*���ʺ��*/
			   OLED_Draw_RecTangle(45,25,6,16,1);
				 OLED_Draw_RecTangle(45,43,6,16,1);
				 OLED_Draw_RecTangle(105,25,6,16,1);
				 OLED_Draw_RecTangle(105,43,6,16,1);	
				 OLED_Refresh();
			}
			else
			{
				if(wave_flag.valid_data_flag == 0)
				{
						/*Ѫ�����Ͷ���ʾ���*/
					 OLED_Show_Heart(3,3);
					 OLED_Show_Sensor_off_Rec(direction);
					 /*�����ʾ�������*/
					 OLED_Draw_RecTangle(30,22,38,44,3);
					 OLED_Draw_RecTangle(84,22,44,44,3);
					
					 /*���ʡ�Ѫ�����ͶȺ��*/
					 OLED_Draw_RecTangle(45,25,6,16,1);
					 OLED_Draw_RecTangle(45,43,6,16,1);
					 OLED_Draw_RecTangle(105,25,6,16,1);
					 OLED_Draw_RecTangle(105,43,6,16,1);
					 //OLED_Refresh();
				}
				else if(wave_flag.valid_data_flag == 1)
				{
					/*�����ʾ��������*/
					OLED_Draw_RecTangle(30,22,38,44,3);
					OLED_Draw_RecTangle(84,22,44,44,3);
					/*��ʾ����*/
					OLED_ShowNum_left_Or_right(33,48,Blood_Oxygen,32,3);
					OLED_ShowNum_left_Or_right(92,48,Heart_Rate,24,3);
				}
				OLED_Refresh();
			}
		}
		else if(direction == SHOW_RIGHT) /*�ұ���ʾ*/
		{
	
			OLED_Show_Battery(battery_value,3);
			/*��ʾ%Sp02*/
			OLED_ShowPicture(96,18,16,40,(uint8_t *)asc2_SpO2_right_bmp);
			/*��ʾPRbpm*/
			OLED_ShowPicture(44,18,16,40,(uint8_t *)asc2_PRbpm_right_bmp);
			if(sensor ==  SENSOR_OFF)  /*�޴�����*/
			{				
	       OLED_Show_Heart(3,4);
				 OLED_Show_Sensor_off_Rec(direction);
				 OLED_Refresh();
				
				 /*�����ʾ�������*/
				 OLED_Draw_RecTangle(0,18,44,46,3);
				 OLED_Draw_RecTangle(60,18,36,46,3);
				
				 /*Ѫ�����Ͷ���ʾ���*/
				 OLED_Draw_RecTangle(73,25,6,16,1);
				 OLED_Draw_RecTangle(73,43,6,16,1);							
				/*���ʺ��*/
				 OLED_Draw_RecTangle(25,25,6,16,1);
				 OLED_Draw_RecTangle(25,43,6,16,1);
			}
			else
			{
				if(wave_flag.valid_data_flag == 0)
				{
						/*Ѫ�����Ͷ���ʾ���*/
					OLED_Show_Heart(3,4);
					OLED_Show_Sensor_off_Rec(direction);
					OLED_Refresh();
					
					/*�����ʾ�������*/
					OLED_Draw_RecTangle(0,18,44,46,3);
				 OLED_Draw_RecTangle(60,18,36,46,3);
					
					/*Ѫ�����Ͷ���ʾ���*/
					OLED_Draw_RecTangle(73,25,6,16,1);
					OLED_Draw_RecTangle(73,43,6,16,1);							
					/*���ʺ��*/
					OLED_Draw_RecTangle(25,25,6,16,1);
					OLED_Draw_RecTangle(25,43,6,16,1);
				}
				else if(wave_flag.valid_data_flag == 1)
				{
					OLED_Draw_RecTangle(0,18,44,46,3);
					OLED_Draw_RecTangle(60,18,36,46,3);
					/*��ʾ����*/
					OLED_ShowNum_left_Or_right(62,18,Blood_Oxygen,32,4);
					OLED_ShowNum_left_Or_right(10,18,Heart_Rate,24,4);
				}
				
			}
		}
}

/*�ú������ڽ�OLED��Ļ��ʾ����*/
void OLED_Turn_Normal(void)
{
	  OLED_WR_Byte(0xC8,OLED_CMD);  //������ʾ
		OLED_WR_Byte(0xA1,OLED_CMD);
	  OLED_Refresh();	
}

void OLED_WR_Byte(u8 dat,u8 cmd)
{	
	u8 i;			  
	if(cmd)
	  OLED_DC_Set();
	else
	  OLED_DC_Clr();
	OLED_CS_Clr();
	for(i=0;i<8;i++)
	{
		OLED_SCLK_Clr();
		if(dat&0x80)
		   OLED_SDIN_Set();
		else 
		   OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;   
	}				 		  
	OLED_CS_Set();
	OLED_DC_Set();   	  
}

//����OLED��ʾ 
void OLED_DisPlay_On(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//��ɱ�ʹ��
	OLED_WR_Byte(0x14,OLED_CMD);//������ɱ�
	OLED_WR_Byte(0xAF,OLED_CMD);//������Ļ
}

//�ر�OLED��ʾ 
void OLED_DisPlay_Off(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//��ɱ�ʹ��
	OLED_WR_Byte(0x10,OLED_CMD);//�رյ�ɱ�
	OLED_WR_Byte(0xAF,OLED_CMD);//�ر���Ļ
}

//�����Դ浽OLED	
void OLED_Refresh(void)
{
	u8 i,n;
	for(i=0;i<8;i++)
	{
	   OLED_WR_Byte(0xb0+i,OLED_CMD); //��������ʼ��ַ
	   OLED_WR_Byte(0x00,OLED_CMD);   //���õ�����ʼ��ַ
	   OLED_WR_Byte(0x10,OLED_CMD);   //���ø�����ʼ��ַ
	   for(n=0;n<128;n++)
		 OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA);
  }
}
//��������
void OLED_Clear(void)
{
	u8 i,n;
	for(i=0;i<8;i++)
	{
	   for(n=0;n<128;n++)
			{
			 OLED_GRAM[n][i]=0;//�����������
			}
  }
	OLED_Refresh();//������ʾ
}

//���� 
//x:0~127
//y:0~63
void OLED_DrawPoint(u8 x,u8 y)
{
	u8 i,m,n;
	i=y/8;
	m=y%8;
	n=1<<m;
	OLED_GRAM[x][i]|=n;
}

//���һ����
//x:0~127
//y:0~63
void OLED_ClearPoint(u8 x,u8 y)
{
	u8 i,m,n;
	i=y/8;
	m=y%8;
	n=1<<m;
	OLED_GRAM[x][i]=~OLED_GRAM[x][i];
	OLED_GRAM[x][i]|=n;
	OLED_GRAM[x][i]=~OLED_GRAM[x][i];
}


//����
//x:0~128
//y:0~64
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode)
{
	u8 i,k,k1,k2;
	if((x1<0)||(x2>128)||(y1<0)||(y2>64)||(x1>x2)||(y1>y2))return;
	if(x1==x2)    //������
	{
			for(i=0;i<=(y2-y1);i++)
			{
				if(mode)	OLED_DrawPoint(x1,y1+i);
				else	OLED_ClearPoint(x1,y1+i);
				
			}
  }
	else if(y1==y2)   //������
	{
			for(i=0;i<=(x2-x1);i++)
			{
				if(mode)	OLED_DrawPoint(x1+i,y1);
				else	OLED_ClearPoint(x1+i,y1);
				
			}
  }
	else      //��б��
	{
		k1=y2-y1;
		k2=x2-x1;
		k=k1*10/k2;
		for(i=0;i<=(x2-x1);i++)
		{
				if(mode)	OLED_DrawPoint(x1+i,y1+i*k/10);
				else	OLED_ClearPoint(x1+i,y1+i*k/10);
			  
		}
	}
}
/*
@brief	Draws a rectangle in the specified area
@param	strat	x coordinate and start y coordinate
@param	the rectangle's	width and height
@param	mode:1->full of rectangle	2->hollow of rectangle 3->clear the area
@return void 
*/
void OLED_Draw_RecTangle(uint8_t x0,uint8_t y0,uint8_t w,uint8_t h,uint8_t mode)
{
	uint8_t i;
	if(mode==1)
	{
		for(i=0;i<h;i++)
		{
			OLED_DrawLine(x0,y0+i,x0+w-1,y0+i,1);
		}
	}
	else if(mode==2)
	{
		OLED_DrawLine(x0,y0,x0+w-1,y0,1);
		OLED_DrawLine(x0,y0+h-1,x0+w-1,y0+h-1,1);
		OLED_DrawLine(x0,y0,x0,y0+h-1,1);
		OLED_DrawLine(x0+w-1,y0,x0+w-1,y0+h-1,1);
	}
	else if(mode==3)
	{
		for(i=0;i<h;i++)
		{
			OLED_DrawLine(x0,y0+i,x0+w-1,y0+i,0);
		}
	}
}

//x,y:Բ������
//r:Բ�İ뾶
void OLED_DrawCircle(u8 x,u8 y,u8 r)
{
	int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)      
    {
        OLED_DrawPoint(x + a, y - b);
        OLED_DrawPoint(x - a, y - b);
        OLED_DrawPoint(x - a, y + b);
        OLED_DrawPoint(x + a, y + b);
 
        OLED_DrawPoint(x + b, y + a);
        OLED_DrawPoint(x + b, y - a);
        OLED_DrawPoint(x - b, y - a);
        OLED_DrawPoint(x - b, y + a);
        
        a++;
        num = (a * a + b * b) - r*r;//���㻭�ĵ���Բ�ĵľ���
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}



//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//size:ѡ������ 12/16/24
//ȡģ��ʽ ����ʽ,˳��
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1)
{
	u8 i,m,temp,size2,chr1;
	u8 y0=y;
	size2=(size1/8+((size1%8)?1:0))*(size1/2);        //�õ�����һ���ַ���Ӧ������ռ���ֽ���
	chr1=chr-' ';                                     //����ƫ�ƺ��ֵ
	for(i=0;i<size2;i++)
	{
		if(size1==12)
        {temp=asc2_1206[chr1][i];}                  //����1206����
		else if(size1==16)
        {temp=asc2_1608[chr1][i];}                  //����1608����
		else if(size1==24)
        {
					//temp=asc2_2412[chr1][i];                //2412���塣��ʹ�������ַ�
					chr1 = chr - '0';
					temp = asc2_2412_regular_script[chr1][i]; //����2412���壬�ַ����⣬ֻ��0-9������
				} 
		else if(size1==32)
        {temp=asc2_3216[chr-'0'][i];}               //����3216����		
		else return;
				for(m=0;m<8;m++)                            //д������
				{
					if(temp&0x80)OLED_DrawPoint(x,y);
					else OLED_ClearPoint(x,y);
					temp<<=1;
					y++;
					if((y-y0)==size1)
					{
						y=y0;
						x++;
						break;
          }
				}
  }
}

/*
��OLED ��Ļ�ϰ���ͬģʽ��ʾ�����ַ���mode=1->������ת90�ȣ�mode==2->������ת90��
*/
void OLED_ShowChar_left_Or_right(u8 x,u8 y,u8 chr,u8 size1,uint8_t mode)
{
	u8 i,m,temp,size2;
	u8 y0=y;
	size2=(size1/8+((size1%8)?1:0))*(size1/2);  
  if(size1 == 24)
	{
		size2 +=12;
	}
	for(i=0;i<size2;i++)
	{
		if(size1==32)
    {
			if(mode==SHOW_LEFT)
			{
				temp=asc2_3216_left[chr-'0'][i];
			}
			else if(mode==SHOW_RIGHT)
			{
				temp=asc2_3216_right[chr-'0'][i];
			}
			
		} 
		else if(size1 == 24)
		{
			if(mode==SHOW_LEFT)
			{
				temp=asc2_2412_left[chr-'0'][i];
			}
			else if(mode==SHOW_RIGHT)
			{
				temp=asc2_2412_right[chr-'0'][i];
			}
			
		}
		else return;
		for(m=0;m<8;m++)           //д������
		{
			if(temp&0x80)OLED_DrawPoint(x,y);
			else OLED_ClearPoint(x,y);
			temp<<=1;
			y++;
			if((y-y0)==size1/2)
			{
				y=y0;
				x++;
				break;
       }
		}
  }
}


//��ʾ�ַ���
//x,y:�������  
//size1:�����С 
//*chr:�ַ�����ʼ��ַ 
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1)
{
	while((*chr>=' ')&&(*chr<='~'))//�ж��ǲ��ǷǷ��ַ�!
	{
		OLED_ShowChar(x,y,*chr,size1);
		x+=size1/2;          //x��Ĵ�С��Y���һ�㣬�õ�	X�������ƫ��
		if(x>128-size1/2)  //����
		{
			x=0;
			y+=2;
    }
		chr++;
  }
}

//m^n
u32 OLED_Pow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)
	{
	  result*=m;
	}
	return result;
}

////��ʾ2������
////x,y :�������	 
////len :���ֵ�λ��
////size:�����С
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 size1)
{
	u32 t,temp;
	u8 Num_length=0;
	temp=num;
	while(temp!=0)
	{
		Num_length++;
		temp/=10;
	}
	temp=num;
	for(t=0;t<Num_length;t++)
	{
		temp=(num/OLED_Pow(10,Num_length-t-1))%10;
			if(temp==0)
			{
				OLED_ShowChar(x+(size1/2)*t,y,'0',size1);
      }
			else 
			{
			  OLED_ShowChar(x+(size1/2)*t,y,temp+'0',size1);
			}
  }
}

////��ʾn������
////x,y :�������	 
////len :���ֵ�λ��
////size:�����С
////mode:3->������ת90�ȣ�4->������ת90��
void OLED_ShowNum_left_Or_right(u8 x,u8 y,u32 num,u8 size1,uint8_t mode)
{
	u32 t,temp;
	u8 Num_length=0;
	temp=num;
	while(temp!=0)
	{
		Num_length++;
		temp/=10;
	}
	temp=num;
	for(t=0;t<Num_length;t++)
	{
		temp=(num/OLED_Pow(10,Num_length-t-1))%10;
			if(temp==0)
			{
				if(mode==left)
				{
					OLED_ShowChar_left_Or_right(x,y-(size1/2)*t,'0',size1,mode);
				}
				else if(mode==right)
				{
					OLED_ShowChar_left_Or_right(x,y+(size1/2)*t,'0',size1,mode);
				}
      }
			else 
			{
				if(mode==left)
				{
					OLED_ShowChar_left_Or_right(x,y-(size1/2)*t,temp+'0',size1,mode);
				}
				else if(mode==right)
				{
					OLED_ShowChar_left_Or_right(x,y+(size1/2)*t,temp+'0',size1,mode);
				}
			  //OLED_ShowChar(x+(size1/2)*t,y,temp+'0',size1);
			}
  }
}


//��ʾ����
//x,y:�������
//num:���ֶ�Ӧ�����
//ȡģ��ʽ ����ʽ
/*
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1)
{
	u8 i,m,n=0,temp,chr1;
	u8 x0=x,y0=y;
	u8 size3=size1/8;
	while(size3--)
	{
		chr1=num*size1/8+n;
		n++;
			for(i=0;i<size1;i++)
			{
				if(size1==16)
						{temp=Hzk1[chr1][i];}//����16*16����
				else if(size1==24)
						{temp=Hzk2[chr1][i];}//����24*24����
				else if(size1==32)       
						{temp=Hzk3[chr1][i];}//����32*32����
				else if(size1==64)
						{temp=Hzk4[chr1][i];}//����64*64����
				else return;
							
						for(m=0;m<8;m++)
							{
								if(temp&0x01)OLED_DrawPoint(x,y);
								else OLED_ClearPoint(x,y);
								temp>>=1;
								y++;
							}
							x++;
							if((x-x0)==size1)
							{x=x0;y0=y0+8;}
							y=y0;
			 }
	}
}
*/

//num ��ʾ���ֵĸ���
//space ÿһ����ʾ�ļ��
void OLED_ScrollDisplay(u8 num,u8 space)
{
	u8 i,n,t=0,m=0,r;
	while(1)
	{
		if(m==0)
		{
	    //OLED_ShowChinese(128,24,t,16); //д��һ�����ֱ�����OLED_GRAM[][]������
			t++;
		}
		if(t==num)
			{
				for(r=0;r<16*space;r++)      //��ʾ���
				 {
					for(i=0;i<144;i++)
						{
							for(n=0;n<8;n++)
							{
								OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
							}
						}
           OLED_Refresh();
				 }
        t=0;
      }
		m++;
		if(m==16){m=0;}
		for(i=0;i<144;i++)   //ʵ������
		{
			for(n=0;n<8;n++)
			{
				OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
			}
		}
		OLED_Refresh();
	}
}

//����д�����ݵ���ʼλ��
void OLED_WR_BP(u8 x,u8 y)
{
	OLED_WR_Byte(0xb0+y,OLED_CMD);//��������ʼ��ַ
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD);
}

/*
@brief show a picture in oled
@param x,y��start x coordinate and start y coordinate
@param the size of picture 
@param BMP[]:the data of picture 
@note	���롢��������ʽ
*/
void OLED_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,u8 BMP[])
{
	uint16_t j=0;
	u8 i,n,temp,m;
	u8 x0=x,y0=y;
	sizey=sizey/8+((sizey%8)?1:0);
	for(n=0;n<sizey;n++)
	{
		 for(i=0;i<sizex;i++)
		 {
				temp=BMP[j];
				j++;
				for(m=0;m<8;m++)
				{
					if(temp&0x01)OLED_DrawPoint(x,y);
					else OLED_ClearPoint(x,y);
					temp>>=1;
					y++;
				}
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y0=y0+8;
				}
				y=y0;
     }
	 }
}
/*
@brief	oled show the rectangle
@param	Heart_Value:the data of heart rate
@param	mode:the oled show direction
@return void
*/
#if 1
void OLED_Show_Rec(uint8_t Heart_Value,uint8_t mode)
{
	uint8_t i;
	static uint8_t Value_start=0;
	static uint8_t Value_last=0;
	Value_start = Heart_Value;
	if(Value_start<Value_last)
	{
			if(mode == SHOW_NOMAL)                         /* ������ʾ���߷�����ʾ*/
			{
				 for(i=((Value_last-90)/2+(Value_last-90)%2-1);i>((Value_start-90)/2+(Value_start-90)%2-1);i--)
				 {
					 OLED_Draw_RecTangle(116,45-i*4,10,2,3);
				 }
			   
			}
			else if(mode == SHOW_OVERTURN)
			{
				for(i=((Value_last-90)/2+(Value_last-90)%2-1);i>((Value_start-90)/2+(Value_start-90)%2-1);i--)
				{
					OLED_Draw_RecTangle(116,45-i*4,10,2,3);
				}
			}
			else if(mode == SHOW_LEFT)                     /* �����ʾ*/
			{
				for(i=(Value_last-90-1);i>(Value_start-90-1);i--)
				{
					OLED_Draw_RecTangle(124-i*4,2,2,10,3);
				}
			}
			else if(mode == SHOW_RIGHT)                   /* �ұ���ʾ*/
			{
				for(i=(Value_last-90-1);i>(Value_start-90-1);i--)
				{
					OLED_Draw_RecTangle(2+i*4,2,2,10,3);
				}
			}
	}
	else
	{
			if(mode == SHOW_NOMAL)
			{
				for(i=0;i<((Heart_Value-90)/2+(Heart_Value%2));i++)
				{
					OLED_Draw_RecTangle(116,45-i*4,10,2,1);
				}
			}
			else if(mode == SHOW_OVERTURN)
			{
				for(i=0;i<((Heart_Value-90)/2+(Heart_Value%2));i++)
				{
					OLED_Draw_RecTangle(116,45-i*4,10,2,1);
				}
			}
			else if(mode == SHOW_LEFT)                     /*�����ʾ*/
			{
				for(i=0;i<(Heart_Value-90);i++)
				{
					OLED_Draw_RecTangle(124-i*4,2,2,10,1);
				}
			}
			else if(mode == SHOW_RIGHT)                   /*�ұ���ʾ*/
			{
				for(i=0;i<(Heart_Value-90);i++)
				{
					OLED_Draw_RecTangle(2+i*4,2,2,10,1);
				}
			}
	}
	Value_last=Value_start;
}
#else
void OLED_Show_Rec(uint8_t Heart_Value,uint8_t mode)
{
	static u8 SHOW_NOMAL_Rec_cnt=0;
	static u8 SHOW_OVERTURN_Rec_cnt=0;
	static u8 SHOW_LEFT_Rec_cnt=0;
	static u8 SHOW_RIGHT_Rec_cnt=0;
	static u8 last_Rec_cnt=0;
	if(mode == SHOW_NOMAL )                         /* ������ʾ���߷�����ʾ*/
	{
			if(last_Rec_cnt<=SHOW_NOMAL_Rec_cnt)
			{
				OLED_Draw_RecTangle(116,45-SHOW_NOMAL_Rec_cnt*4,10,2,1);
				last_Rec_cnt=SHOW_NOMAL_Rec_cnt;
				SHOW_NOMAL_Rec_cnt++;
				if(SHOW_NOMAL_Rec_cnt==6)
				{
					last_Rec_cnt=6;
					SHOW_NOMAL_Rec_cnt=5;
				}
			}
			else
			{
				OLED_Draw_RecTangle(116,45-SHOW_NOMAL_Rec_cnt*4,10,2,3);
				last_Rec_cnt=SHOW_NOMAL_Rec_cnt;
				SHOW_NOMAL_Rec_cnt--;
				if(SHOW_NOMAL_Rec_cnt==0)
				{
					last_Rec_cnt=SHOW_NOMAL_Rec_cnt;
				}
			}
	}
	else if(mode == SHOW_OVERTURN)
	{
		if(last_Rec_cnt<=SHOW_OVERTURN_Rec_cnt)
			{
				OLED_Draw_RecTangle(116,45-SHOW_OVERTURN_Rec_cnt*4,10,2,1);
				last_Rec_cnt=SHOW_OVERTURN_Rec_cnt;
				SHOW_OVERTURN_Rec_cnt++;
				if(SHOW_OVERTURN_Rec_cnt==6)
				{
					last_Rec_cnt=6;
					SHOW_OVERTURN_Rec_cnt=5;
				}
			}
			else
			{
				OLED_Draw_RecTangle(116,45-SHOW_OVERTURN_Rec_cnt*4,10,2,3);
				last_Rec_cnt=SHOW_OVERTURN_Rec_cnt;
				SHOW_OVERTURN_Rec_cnt--;
				if(SHOW_OVERTURN_Rec_cnt==0)
				{
					last_Rec_cnt=SHOW_OVERTURN_Rec_cnt;
				}
			}
	}
	else if(mode == SHOW_LEFT)
	{
		if(last_Rec_cnt<=SHOW_LEFT_Rec_cnt)
		{
			OLED_Draw_RecTangle(124-SHOW_LEFT_Rec_cnt*4,2,2,10,1);
			last_Rec_cnt=SHOW_LEFT_Rec_cnt;
			SHOW_LEFT_Rec_cnt++;
			if(SHOW_LEFT_Rec_cnt==11)
			{
				last_Rec_cnt=11;
				SHOW_LEFT_Rec_cnt=10;
			}
		}
		else
		{
			OLED_Draw_RecTangle(124-SHOW_LEFT_Rec_cnt*4,2,2,10,3);
			last_Rec_cnt=SHOW_LEFT_Rec_cnt;
			SHOW_LEFT_Rec_cnt--;
			if(SHOW_LEFT_Rec_cnt==0)
			{
				last_Rec_cnt=SHOW_LEFT_Rec_cnt;
			}
		}
	}
	else if(mode == SHOW_RIGHT)
	{
		if(last_Rec_cnt<=SHOW_RIGHT_Rec_cnt)
		{
			OLED_Draw_RecTangle(2+SHOW_RIGHT_Rec_cnt*4,2,2,10,1);
			last_Rec_cnt=SHOW_RIGHT_Rec_cnt;
			SHOW_RIGHT_Rec_cnt++;
			if(SHOW_RIGHT_Rec_cnt==11)
			{
				last_Rec_cnt=11;
				SHOW_RIGHT_Rec_cnt=10;
			}
		}
		else
		{
			OLED_Draw_RecTangle(2+SHOW_RIGHT_Rec_cnt*4,2,2,10,3);
			last_Rec_cnt=SHOW_RIGHT_Rec_cnt;
			SHOW_RIGHT_Rec_cnt--;
			if(SHOW_RIGHT_Rec_cnt==0)
			{
				last_Rec_cnt=SHOW_RIGHT_Rec_cnt;
			}
		}
	}
}
#endif

extern int show_wave_data[250];
extern int Temp_wave_data[250];
extern __IO int wave_data_cnt;
int last_wave_cnt=0;
//��ʾ����
#if 0
void OLED_Show_Waveform(uint32_t Waveform_cnt)
{
	uint8_t i,j;
	int min=show_wave_data[0];
	int max=0;
	int dif_value=0;
	static uint8_t cnt=0;
	for(i=0;i<wave_data_cnt;i++)
	{
		if(show_wave_data[i]<min)
		{
			min=show_wave_data[i];
		}
		if(show_wave_data[i]>max)
		{
			max=show_wave_data[i];
		}
	}
	dif_value=max-min;
	if(((show_wave_data[cnt]-min)/(int)(dif_value/12))>12)
	{
		OLED_DrawLine(Waveform_cnt,63-12,Waveform_cnt,64,1);
	}
	else
	{
		OLED_DrawLine(Waveform_cnt,62-((show_wave_data[cnt]-min)/(int)(dif_value/12)),Waveform_cnt,63,1);       /*������*/  
	}		
	if((Waveform_cnt+1)>100)
	{
		OLED_DrawLine(Waveform_cnt+1-101,50,Waveform_cnt+1-101,64,0);        /*�������*/
		OLED_DrawLine(Waveform_cnt+2-101,50,Waveform_cnt+2-101,64,0);
		OLED_DrawLine(Waveform_cnt+3-101,50,Waveform_cnt+3-101,64,0);
		OLED_DrawLine(Waveform_cnt+4-101,50,Waveform_cnt+4-101,64,0);
	}
	else
	{
		OLED_DrawLine(Waveform_cnt+1,50,Waveform_cnt+1,64,0);
		OLED_DrawLine(Waveform_cnt+2,50,Waveform_cnt+2,64,0);
		OLED_DrawLine(Waveform_cnt+3,50,Waveform_cnt+3,64,0);
		OLED_DrawLine(Waveform_cnt+4,50,Waveform_cnt+4,64,0);       /*�������*/
	}
	cnt+=3;
	if(cnt >= wave_data_cnt)cnt=0;
}

#else
void OLED_Show_Waveform(uint32_t Waveform_cnt)
{
	uint16_t i;
	int	Dif_Value=0;
	static int min=0;
	static int max=0;
	static int mid=0;
	static int wave_cnt=0;
	static int Min_Cnt=0;
	static int Mid_Cnt=0;
	static int Max_Cnt=0;
	//static int last_wave_cnt=0;
	if(last_wave_cnt != wave_data_cnt && wave_cnt==0)// wave_data_cnt!=1)
	{
		memset(Temp_wave_data,0,1000);
		last_wave_cnt=wave_data_cnt;
		for(i=0;i<last_wave_cnt;i++)
		{
			Temp_wave_data[i]=show_wave_data[i];
		}
		min=Temp_wave_data[0];
		Min_Cnt=0;
		mid=Temp_wave_data[last_wave_cnt-1];
		Mid_Cnt=last_wave_cnt-1;
		max=Temp_wave_data[0];
		for(i=0;i<last_wave_cnt;i++)
		{
			if(Temp_wave_data[i]>max)
			{
				max=Temp_wave_data[i];
				Max_Cnt=i;
			}
		}	
	}
	if(wave_cnt<Max_Cnt && wave_cnt>=0)
	{
		Dif_Value=(int)((Temp_wave_data[wave_cnt]-min)/((max-min)/12));
		if(Dif_Value>=12)	//(Temp_wave_data[cnt]-min)%((max-min)/12)
		{
			OLED_DrawLine(Waveform_cnt,52,Waveform_cnt,63,1);
		}
		else if(Dif_Value<=0)
		{
			OLED_DrawLine(Waveform_cnt,63,Waveform_cnt,63,1);
		}
		else
		{
			OLED_DrawLine(Waveform_cnt,63-Dif_Value,Waveform_cnt,63,1);
		}
		if(last_wave_cnt<50 && last_wave_cnt>0)
		{
			wave_cnt+=2;
		}
		else
		{
			wave_cnt+=3;
		}
		if(wave_cnt>=Max_Cnt)wave_cnt=Max_Cnt;
	}
	else if(wave_cnt>=Max_Cnt && wave_cnt<Mid_Cnt)
	{
		Dif_Value=(int)(Temp_wave_data[wave_cnt]-mid)/((max-mid)/12);
		if(Dif_Value>=12)
		{
			OLED_DrawLine(Waveform_cnt,52,Waveform_cnt,63,1);
		}
		else if(Dif_Value<=0)
		{
			OLED_DrawLine(Waveform_cnt,63,Waveform_cnt,63,1);
		}
		else
		{
			OLED_DrawLine(Waveform_cnt,63-Dif_Value,Waveform_cnt,63,1);
		}
		if(last_wave_cnt<50 && last_wave_cnt>0)
		{
			wave_cnt+=3;
		}
		else
		{
			wave_cnt+=5;
		}
		if(wave_cnt>=Mid_Cnt)wave_cnt=Mid_Cnt;
	} 
	else
	{
		Dif_Value=(int)(Temp_wave_data[wave_cnt]-mid)/((max-mid)/12);
		if(Dif_Value>=12)
		{
			OLED_DrawLine(Waveform_cnt,52,Waveform_cnt,63,1);
		}
		else if(Dif_Value<=0)
		{
			OLED_DrawLine(Waveform_cnt,63,Waveform_cnt,63,1);
		}
		else
		{
			OLED_DrawLine(Waveform_cnt,63-Dif_Value,Waveform_cnt,63,1);
		}
		wave_cnt=0;
	}
	if((Waveform_cnt+1)>100)
	{
		OLED_DrawLine(Waveform_cnt+1-101,50,Waveform_cnt+1-101,63,0);        /*�������*/
		OLED_DrawLine(Waveform_cnt+2-101,50,Waveform_cnt+2-101,63,0);
		OLED_DrawLine(Waveform_cnt+3-101,50,Waveform_cnt+3-101,63,0);
		OLED_DrawLine(Waveform_cnt+4-101,50,Waveform_cnt+4-101,63,0);
	}
	else
	{
		OLED_DrawLine(Waveform_cnt+1,50,Waveform_cnt+1,63,0);
		OLED_DrawLine(Waveform_cnt+2,50,Waveform_cnt+2,63,0);
		OLED_DrawLine(Waveform_cnt+3,50,Waveform_cnt+3,63,0);
		OLED_DrawLine(Waveform_cnt+4,50,Waveform_cnt+4,63,0);       /*�������*/
	}
}

#endif

void OLED_Show_Smooth_wave(uint32_t show_waveform_cnt)
{
	
	OLED_DrawLine(show_waveform_cnt,54,show_waveform_cnt,63,1);
					
	OLED_DrawLine(show_waveform_cnt+1,52,show_waveform_cnt+1,63,0);
	OLED_DrawLine(show_waveform_cnt+2,52,show_waveform_cnt+2,63,0);
	OLED_DrawLine(show_waveform_cnt+3,52,show_waveform_cnt+3,63,0);
	OLED_DrawLine(show_waveform_cnt+4,52,show_waveform_cnt+4,63,0);
}


/*
@brief	oled show battery capacity
@param	battery voltage
@param	mode:1->normal	2->unnormal	3->left 4->right
@return void
*/
void OLED_Show_Battery(float Vol_Value,uint8_t mode)
{
	if(mode==1)   /* ������ʾ*/
	{
		if(Vol_Value<=3.5&&Vol_Value>=2.8)
		{
			OLED_ShowPicture(106,2,22,12,(uint8_t *)Battery_bmp3);
			
		}
		else if(Vol_Value<2.8&&Vol_Value>=2.3)
		{
			OLED_ShowPicture(106,2,22,12,(uint8_t *)Battery_bmp2);
		}
		else if(Vol_Value<2.3&&Vol_Value>=1.8)
		{
			OLED_ShowPicture(106,2,22,12,(uint8_t *)Battery_bmp1);
		}
		else
		{
			OLED_ShowPicture(106,2,22,12,(uint8_t *)Battery_bmp0);
		}
	}
	else if(mode==2) /*���*/
	{
		if(Vol_Value<=3.5&&Vol_Value>=2.8)
		{
			OLED_ShowPicture(0,35,12,22,(uint8_t *)Battery3_left);
		}
		else if(Vol_Value<2.8&&Vol_Value>=2.3)
		{
			OLED_ShowPicture(0,35,12,22,(uint8_t *)Battery2_left);
		}
		else if(Vol_Value<2.3&&Vol_Value>=1.8)
		{
			OLED_ShowPicture(0,35,12,22,(uint8_t *)Battery1_left);
		}

	}
	else if(mode==3) /*�ұ�*/
	{
		if(Vol_Value<=3.5&&Vol_Value>=2.8)
		{
			OLED_ShowPicture(114,42,12,22,(uint8_t *)Battery3_right);
		}
		else if(Vol_Value<2.8&&Vol_Value>=2.3)
		{
			OLED_ShowPicture(114,42,12,22,(uint8_t *)Battery2_right);
		}
		else if(Vol_Value<2.3&&Vol_Value>=1.8)
		{
			OLED_ShowPicture(114,42,12,22,(uint8_t *)Battery1_right);
		}

	}
	
}

/*
mode:1->ʵ�İ��ģ�2->������ģ�3->����
direction:1->��Ļ����	��2->��Ļ��ת180�ȣ�3->��Ļ������ת90�ȣ�4->��Ļ������ת90��
*/
#if 0
void OLED_Show_Heart(uint8_t mode,uint8_t direction)
{
	if(direction==1||direction==2)
	{
		if(mode==1)OLED_ShowPicture(113,50,14,13,(uint8_t *)Heart1_bmp);
		else if(mode==2)OLED_ShowPicture(113,50,14,13,(uint8_t *)Heart2_bmp);
		else if(mode==3) OLED_ShowPicture(113,50,14,13,(uint8_t *)Heart3_bmp);
	}
	else if(direction==3)
	{
		if(mode==1)OLED_ShowPicture(0,2,13,14,(uint8_t *)Heart1_left);
		else if(mode==2)OLED_ShowPicture(0,2,13,14,(uint8_t *)Heart2_bmp);
		else if(mode==3) OLED_ShowPicture(0,2,13,14,(uint8_t *)Heart3_left);
	}
	else if(direction==4)
	{
		if(mode==1)OLED_ShowPicture(115,2,13,14,(uint8_t *)Heart1_right);
		else if(mode==2)OLED_ShowPicture(115,2,13,14,(uint8_t *)Heart2_bmp);
		else if(mode==3) OLED_ShowPicture(115,2,13,14,(uint8_t *)Heart3_right);
	}
}
#else
void OLED_Show_Heart(uint8_t mode,uint8_t direction)
{
	if(direction == SHOW_NOMAL || direction == SHOW_OVERTURN)
	{
		if(mode==1)OLED_ShowPicture(113,50,14,13,(uint8_t *)Heart1_bmp);
		else if(mode==2)OLED_ShowPicture(113,50,14,13,(uint8_t *)Heart2_bmp);
		else if(mode==3) OLED_ShowPicture(113,50,14,13,(uint8_t *)Heart3_bmp);
	}
	else if(direction == SHOW_LEFT)
	{
		if(mode==1)OLED_ShowPicture(0,2,13,14,(uint8_t *)Heart1_left);
		else if(mode==2)OLED_ShowPicture(0,2,13,14,(uint8_t *)Heart2_bmp);
		else if(mode==3) OLED_ShowPicture(0,2,13,14,(uint8_t *)Heart3_left);
	}
	else if(direction == SHOW_RIGHT)
	{
		if(mode==1)OLED_ShowPicture(115,2,13,14,(uint8_t *)Heart1_right);
		else if(mode==2)OLED_ShowPicture(115,2,13,14,(uint8_t *)Heart2_bmp);
		else if(mode==3) OLED_ShowPicture(115,2,13,14,(uint8_t *)Heart3_right);
	}
}
#endif

void OLED_Show_Start(void)
{
	OLED_ShowPicture(32,16,16,32,(uint8_t *)Q_bmp);
	OLED_ShowPicture(48,16,16,32,(uint8_t *)Q_bmp);
	OLED_ShowPicture(64,16,16,32,(uint8_t *)Z_bmp);
	OLED_ShowPicture(80,16,16,32,(uint8_t *)M_bmp);
	
}
#define DUBUG_LEFT
void test_show(void)
{
#ifdef DUBUG_LEFT
		/*��ʾ��*/
	OLED_Show_Heart(1,3);
	OLED_Show_Battery(3.3,2);
	/*��ʾ%Sp02*/
	OLED_ShowPicture(14,22,16,40,(uint8_t *)asc2_SpO2_left_bmp);
	/*��ʾPRbpm*/
	OLED_ShowPicture(68,22,16,40,(uint8_t *)asc2_PRbpm_left_bmp);
	/*��ʾ����*/
	OLED_ShowNum_left_Or_right(33,45,97,32,3);

	OLED_ShowNum_left_Or_right(92,35,74,32,3);
	OLED_Refresh();
#endif
	
#ifdef DEBUG_RIGHT
	/*��ʾ��*/
	OLED_Show_Heart(1,4);
	OLED_Show_Battery(3.3,3);
	/*��ʾ%Sp02*/
  OLED_ShowPicture(96,18,16,40,(uint8_t *)asc2_SpO2_right_bmp);
  /*��ʾPRbpm*/
  OLED_ShowPicture(44,18,16,40,(uint8_t *)asc2_PRbpm_right_bmp);
  /*��ʾ����*/
  OLED_ShowNum_left_Or_right(62,30,97,2,32,4);

  OLED_ShowNum_left_Or_right(2,30,74,2,32,4);
	OLED_Refresh();
#endif
}


/*ð������*/

void bubble_sort(uint8_t strlen,uint8_t*buff)
{
	uint8_t i,j,temp;
	 for(i=0;i<strlen-1;i++)
	 {
				 
		 for(j =0;j<strlen-2;j++)
		 {
			 if(buff[j]>buff[j+1])
			 {
				 temp = buff[j]; 
				 buff[j] = buff[j+1]; 
				 buff[j+1] = temp;
			 }										 
		 }									 
	}
	

}

/*
�޴�����ʱ�ĺ����ʾ����
���ҷ����ʱ��8����Σ�����4�����
*/
void OLED_Show_Sensor_off_Rec(uint8_t direction)
{
	uint8_t i ;
	if(direction == SHOW_NOMAL)
	{
		/*��ʾ���*/
		for(i=0;i<4;i++)
		{			
			OLED_Draw_RecTangle(116,45-i*4,10,2,1);
			OLED_Refresh();
		}
	}
	else if(direction == SHOW_LEFT)
	{
    for(i=0;i<8;i++)
		{
		  OLED_Draw_RecTangle(124-i*4,2,2,10,1);
			OLED_Refresh();
		}
	}
	else if(direction == SHOW_RIGHT)
	{
		for(i=0;i<8;i++)
		{
		  OLED_Draw_RecTangle(2+i*4,2,2,10,1);
			OLED_Refresh();
		}
	}
	else
	{
		return;
	}	
}


//OLED�ĳ�ʼ��
void OLED_Init(void)
{
GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();


  /*Configure GPIO pins : PA0 PA1 PA2 PA3 PA4  */
  GPIO_InitStruct.Pin = OLED_CLK_Pin|OLED_DIN_Pin|OLED_RES_Pin|OLED_DC_Pin|OLED_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, OLED_CLK_Pin|OLED_DIN_Pin|OLED_RES_Pin|OLED_DC_Pin|OLED_CS_Pin, GPIO_PIN_SET);

	
	OLED_RST_Set();
	HAL_Delay(100);
	OLED_RST_Clr();//��λ
	HAL_Delay(200);
	OLED_RST_Set();
	
	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
	OLED_WR_Byte(0xCF,OLED_CMD);// Set SEG Output Current Brightness
	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
	OLED_WR_Byte(0x12,OLED_CMD);
	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//
	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
	OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	OLED_WR_Byte(0xAF,OLED_CMD);
	OLED_Clear();
}
