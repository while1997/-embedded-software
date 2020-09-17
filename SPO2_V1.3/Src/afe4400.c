#include "afe4400.h"
#include "afe_spi.h"
//#include <stdio.h>
#include "usart.h"
//#include "spo2datahandle.h"
__IO uint8_t  readflag=0;


unsigned long AFE4400_Current_Register_Settings[36] = {
  CONTROL0_VAL,           //Reg0:CONTROL0: CONTROL REGISTER 0
  LED2STC_VAL,            //Reg1:REDSTARTCOUNT: SAMPLE RED START COUNT
  LED2ENDC_VAL,           //Reg2:REDENDCOUNT: SAMPLE RED END COUNT
  LED2LEDSTC_VAL,         //Reg3:REDLEDSTARTCOUNT: RED LED START COUNT
  LED2LEDENDC_VAL,        //Reg4:REDLEDENDCOUNT: RED LED END COUNT
  ALED2STC_VAL,           //Reg5:AMBREDSTARTCOUNT: SAMPLE AMBIENT RED START COUNT
  ALED2ENDC_VAL,          //Reg6:AMBREDENDCOUNT: SAMPLE AMBIENT RED END COUNT02
  LED1STC_VAL,            //Reg7:IRSTARTCOUNT: SAMPLE IR START COUNT
  LED1ENDC_VAL,           //Reg8:IRENDCOUNT: SAMPLE IR END COUNT
  LED1LEDSTC_VAL,         //Reg9:IRLEDSTARTCOUNT: IR LED START COUNT
  LED1LEDENDC_VAL,        //Reg10:IRLEDENDCOUNT: IR LED END COUNT
  ALED1STC_VAL,           //Reg11:AMBIRSTARTCOUNT: SAMPLE AMBIENT IR START COUNT
  ALED1ENDC_VAL,          //Reg12:AMBIRENDCOUNT: SAMPLE AMBIENT IR END COUNT
  LED2CONVST_VAL,         //Reg13:REDCONVSTART: REDCONVST
  LED2CONVEND_VAL,        //Reg14:REDCONVEND: RED CONVERT END COUNT
  ALED2CONVST_VAL,        //Reg15:AMBREDCONVSTART: RED AMBIENT CONVERT START COUNT
  ALED2CONVEND_VAL,       //Reg16:AMBREDCONVEND: RED AMBIENT CONVERT END COUNT
  LED1CONVST_VAL,         //Reg17:IRCONVSTART: IR CONVERT START COUNT
  LED1CONVEND_VAL,        //Reg18:IRCONVEND: IR CONVERT END COUNT
  ALED1CONVST_VAL,        //Reg19:AMBIRCONVSTART: IR AMBIENT CONVERT START COUNT
  ALED1CONVEND_VAL,       //Reg20:AMBIRCONVEND: IR AMBIENT CONVERT END COUNT
  ADCRSTSTCT0_VAL,        //Reg21:ADCRESETSTCOUNT0: ADC RESET 0 START COUNT
  ADCRSTENDCT0_VAL,       //Reg22:ADCRESETENDCOUNT0: ADC RESET 0 END COUNT
  ADCRSTSTCT1_VAL,        //Reg23:ADCRESETSTCOUNT1: ADC RESET 1 START COUNT
  ADCRSTENDCT1_VAL,       //Reg24:ADCRESETENDCOUNT1: ADC RESET 1 END COUNT
  ADCRSTSTCT2_VAL,        //Reg25:ADCRESETENDCOUNT2: ADC RESET 2 START COUNT
  ADCRSTENDCT2_VAL,       //Reg26:ADCRESETENDCOUNT2: ADC RESET 2 END COUNT
  ADCRSTSTCT3_VAL,        //Reg27:ADCRESETENDCOUNT3: ADC RESET 3 START COUNT
  ADCRSTENDCT3_VAL,       //Reg28:ADCRESETENDCOUNT3: ADC RESET 3 END COUNT
  PRP,                    //Reg29:PRPCOUNT: PULSE REPETITION PERIOD COUNT
  CONTROL1_VAL,           //Reg30:CONTROL1: CONTROL REGISTER 1 //timer enabled, averages=3, RED and IR LED pulse ON PD_ALM AND LED_ALM pins
  0x00000,                //Reg31:?: ??          
  (ENSEPGAIN + STAGE2EN_LED1 + STG2GAIN_LED1_0DB + CF_LED1_5P + RF_LED1_1M),                      //Reg32:TIAGAIN: TRANS IMPEDANCE AMPLIFIER GAIN SETTING REGISTER
  (AMBDAC_0uA + FLTRCNRSEL_500HZ + STAGE2EN_LED2 + STG2GAIN_LED2_0DB + CF_LED2_5P + RF_LED2_1M),  //Reg33:TIA_AMB_GAIN: TRANS IMPEDANCE AAMPLIFIER AND AMBIENT CANELLATION STAGE GAIN
  (LEDCNTRL_VAL),                                                                                 //Reg34:LEDCNTRL: LED CONTROL REGISTER
  (TX_REF_1 + RST_CLK_ON_PD_ALM_PIN_DISABLE + ADC_BYP_DISABLE + TXBRGMOD_H_BRIDGE + DIGOUT_TRISTATE_DISABLE + XTAL_ENABLE + EN_FAST_DIAG + PDN_TX_OFF + PDN_RX_OFF + PDN_AFE_OFF)                 //Reg35:CONTROL2: CONTROL REGISTER 2 //bit 9
};


/**********************************************************************************************************
* Init_AFE4400_DRDY_Interrupt												                                          *
**********************************************************************************************************/
void Init_AFE4400_DRDY_Interrupt (void)
{
	GPIO_InitTypeDef   GPIO_InitStructure={0};
//  /* Enable GPIOA clock */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* Configure PA12 pin as input floating */
  GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  GPIO_InitStructure.Pin = AFE_ADC_RDY_Pin;
  HAL_GPIO_Init(AFE_ADC_RDY_GPIO, &GPIO_InitStructure);

  /* Enable and set EXTI lines 15 to 10 Interrupt to the lowest priority */
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/**********************************************************************************************************
* Enable_AFE4400_DRDY_Interrupt												                                          *
**********************************************************************************************************/
void Enable_AFE4400_DRDY_Interrupt (void)
{
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/**********************************************************************************************************
* Disable_AFE4400_DRDY_Interrupt												                                          *
**********************************************************************************************************/
void Disable_AFE4400_DRDY_Interrupt(void)
{
	HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == AFE_ADC_RDY_Pin)
  {
    readflag=1;
  }
}
/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */

/**********************************************************************************************************
* Set_GPIO														                                          *
**********************************************************************************************************/

void Set_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/*Configure GPIO pin : PA11 AFE_RESETZ*/
  GPIO_InitStruct.Pin = AFE_RESET_Pin;
 	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(AFE_RESET_GPIO, &GPIO_InitStruct);
		
	/*Configure GPIO pin : PB11 DIAO_END*/
	GPIO_InitStruct.Pin = AFE_DIAG_END_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;	
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(AFE_DIAG_END_GPIO, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 AFE_PDN*/
  GPIO_InitStruct.Pin = AFE_PDN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(AFE_PDN_GPIO, &GPIO_InitStruct);
}



/**********************************************************************************************************
* Init_AFE4400_Resource						                                          *
**********************************************************************************************************/

void Init_AFE4400_Resource(void)
{
  Set_GPIO();										// Initializes AFE4400's input control lines
	SPI_Init();
	HAL_GPIO_WritePin(AFE_RESET_GPIO, AFE_RESET_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(AFE_PDN_GPIO, AFE_PDN_Pin, GPIO_PIN_SET);
}

/**********************************************************************************************************
*	        AFE4400 default Initialization          				                  					  *
**********************************************************************************************************/

void AFE4400_Set_READABLE(u8 isReadable)
{
	SPI_WriteOneByte(CONTROL0);  //Ð´Èë¼Ä´æÆ÷µØÖ·
  SPI_WriteOneByte(0x00);
  SPI_WriteOneByte(0x00);
  SPI_WriteOneByte(isReadable);
}
 
u32 AFE4400_Read_Register(u8 reg_address)
{
  u32 temp0=0, temp1=0, temp2=0;
  u32 temp=0;
	SPI_CS_L;
  AFE4400_Set_READABLE(1);
	SPI_WriteOneByte(reg_address);  //Ð´Èë¼Ä´æÆ÷µØÖ·
  temp0 = SPI_ReadOneByte();
	
  temp1 = SPI_ReadOneByte();
  temp2 = SPI_ReadOneByte();
	SPI_CS_H;
  temp = temp0<<16 | temp1<<8 | temp2;
  return temp;
}
 

void AFE4400_Set_Register(u8 reg_address, u32 data)
{
	SPI_CS_L;
  AFE4400_Set_READABLE(0);
  SPI_WriteOneByte(reg_address);  //Ð´Èë¼Ä´æÆ÷µØÖ·
  SPI_WriteOneByte(((data>>16)&0xFF));
  SPI_WriteOneByte(((data>>8)&0xFF));
  SPI_WriteOneByte((data&0xFF));
  SPI_CS_H;

}


void AFE4400_Default_Reg_Init(void)
{
//	 unsigned short N0  = 0 ;
//   unsigned short N1 = PRP>>2;
//   unsigned short N2 = PRP>>1;
//   unsigned short N3 = N1+N2;
//   unsigned short L = PRP>>5;  
//   unsigned short dn = 50;
//	AFE4400_Set_Register(LED2STC,         N3+dn);
//  AFE4400_Set_Register(LED2ENDC,        N3+(L)-2);
//  AFE4400_Set_Register(LED2LEDSTC,      N3);
//  AFE4400_Set_Register(LED2LEDENDC,     N3+(L)-1);
//  AFE4400_Set_Register(ALED2STC,        N0+dn);
//  AFE4400_Set_Register(ALED2ENDC,       N0+L-2);
//  AFE4400_Set_Register(LED1STC,         N1+dn);
//  AFE4400_Set_Register(LED1ENDC,        N1+L-2);
//  AFE4400_Set_Register(LED1LEDSTC,      N1);
//  AFE4400_Set_Register(LED1LEDENDC,     N1+L-1);
//  AFE4400_Set_Register(ALED1STC,        N2+dn);
//  AFE4400_Set_Register(ALED1ENDC,       N2+L-2);
//  AFE4400_Set_Register(LED2CONVST,       N0+4);
//  AFE4400_Set_Register(LED2CONVEND,     N1-1);
//  AFE4400_Set_Register(ALED2CONVST,     N1+4);
//  AFE4400_Set_Register(ALED2CONVEND,    N2-1);
//  AFE4400_Set_Register(LED1CONVST,      N2+4);
//  AFE4400_Set_Register(LED1CONVEND,     N3-1);
//  AFE4400_Set_Register(ALED1CONVST,     N3+4);
//  AFE4400_Set_Register(ALED1CONVEND,    PRP-2);
//  
//  AFE4400_Set_Register(ADCRSTSTCT0,     N0);
//  AFE4400_Set_Register(ADCRSTENDCT0,    N0+3);
//  AFE4400_Set_Register(ADCRSTSTCT1,     N1);
//  AFE4400_Set_Register(ADCRSTENDCT1,    N1+3);
//  AFE4400_Set_Register(ADCRSTSTCT2,     N2);
//  AFE4400_Set_Register(ADCRSTENDCT2,    N2+3);
//  AFE4400_Set_Register(ADCRSTSTCT3,     N3);
//  AFE4400_Set_Register(ADCRSTENDCT3,    N3+3);
//	 AFE4400_Set_Register(LEDCNTRL,        0x16666);
//	  AFE4400_Set_Register((unsigned char)PRPCOUNT, (unsigned long)PRP);
//	 AFE4400_Set_Register((unsigned char)CONTROL0, AFE4400_Current_Register_Settings[0]);            //0x00
//  AFE4400_Set_Register((unsigned char)CONTROL2, AFE4400_Current_Register_Settings[35]);           //0x23
//  AFE4400_Set_Register((unsigned char)TIAGAIN, AFE4400_Current_Register_Settings[32]);            //0x20
//  AFE4400_Set_Register((unsigned char)TIA_AMB_GAIN, 0x000020);       //0x21
// // AFE4400_Set_Register((unsigned char)LEDCNTRL, AFE4400_Current_Register_Settings[34]);           //0x22
//	AFE4400_Set_Register((unsigned char)CONTROL1, AFE4400_Current_Register_Settings[30]);           //0x1E
//	AFE44x0_Reg_Write_Short( 0x01, N3+dn);       //LED2STC
//  AFE44x0_Reg_Write_Short( 0x02, N3+(L)-2 );  //LED2ENDC 
//  AFE44x0_Reg_Write_Short( 0x03, N3 );      //LED2LEDSTC
//  AFE44x0_Reg_Write_Short( 0x04, N3+(L)-1 ); // LED2LEDENDC
//  AFE44x0_Reg_Write_Short( 0x05, N0+dn );   //ALED2STC
//  AFE44x0_Reg_Write_Short( 0x06, N0+L-2 ); //ALED2ENDC
//  AFE44x0_Reg_Write_Short( 0x07, N1+dn );      //LED1STC
//  AFE44x0_Reg_Write_Short( 0x08, N1+L-2 );  //LED1ENDC
//  AFE44x0_Reg_Write_Short( 0x09, N1 );      //LED1LEDSTC
//  AFE44x0_Reg_Write_Short( 0x0A, N1+L-1);    //LED1LEDENDC
//  AFE44x0_Reg_Write_Short( 0x0B, N2+dn);        //ALED1STC
//  AFE44x0_Reg_Write_Short( 0x0C, N2+L-2);     //ALED1STC
//  AFE44x0_Reg_Write_Short( 0x0D, N0+4);      //LED2CONVST
//  AFE44x0_Reg_Write_Short(0x0E, N1-1);      //LED2CONVEND
//  AFE44x0_Reg_Write_Short(0x0F,N1+4);        //ALED2CONVEND 
//  AFE44x0_Reg_Write_Short(0x10,N2-1);      //ALED2CONVEND
//  AFE44x0_Reg_Write_Short(0x11,N2+4);    //LED1CONVST
//  AFE44x0_Reg_Write_Short(0x12,N3-1);   //LED1CONVEND
//  AFE44x0_Reg_Write_Short(0x13,N3+4);   //ALED1CONVST
//  AFE44x0_Reg_Write_Short(0x14,PRP-2); //ALED1CONVEND
//  AFE44x0_Reg_Write_Short(0x15, N0);  // ADCRSTSTCT0
//  AFE44x0_Reg_Write_Short(0x16,N0+3);   //ADCRSTENDCT0
//  AFE44x0_Reg_Write_Short(0x17,N1);// ADCRSTSTCT1
//  AFE44x0_Reg_Write_Short(0x18,N1+3);//ADCRSTENDCT1
//  AFE44x0_Reg_Write_Short(0x19,N2);// ADCRSTSTCT2
//  AFE44x0_Reg_Write_Short(0x1A,N2+3);//ADCRSTENDCT2
//  AFE44x0_Reg_Write_Short(0x1B, N3);// ADCRSTSTCT3
//  AFE44x0_Reg_Write_Short(0x1C,N3+3);//ADCRSTENDCT3
//  AFE44x0_Reg_Write_Short(0x1D, PRP-1);
  #if 1
	#if 0
  AFE4400_Set_Register((unsigned char)PRPCOUNT, (unsigned long)PRP);
  AFE4400_Set_Register((unsigned char)LED2STC, (unsigned long)LED2STC_VAL);
  AFE4400_Set_Register((unsigned char)LED2ENDC, (unsigned long)LED2ENDC_VAL);
  AFE4400_Set_Register((unsigned char)LED2LEDSTC, (unsigned long)LED2LEDSTC_VAL);
  AFE4400_Set_Register((unsigned char)LED2LEDENDC, (unsigned long)LED2LEDENDC_VAL);
  AFE4400_Set_Register((unsigned char)ALED2STC, (unsigned long)ALED2STC_VAL);
  AFE4400_Set_Register((unsigned char)ALED2ENDC, (unsigned long)ALED2ENDC_VAL);
  AFE4400_Set_Register((unsigned char)LED1STC, (unsigned long)LED1STC_VAL);
  AFE4400_Set_Register((unsigned char)LED1ENDC, (unsigned long)LED1ENDC_VAL);
  AFE4400_Set_Register((unsigned char)LED1LEDSTC, (unsigned long)LED1LEDSTC_VAL);
  AFE4400_Set_Register((unsigned char)LED1LEDENDC, (unsigned long)LED1LEDENDC_VAL);
  AFE4400_Set_Register((unsigned char)ALED1STC, (unsigned long)ALED1STC_VAL);
  AFE4400_Set_Register((unsigned char)ALED1ENDC, (unsigned long)ALED1ENDC_VAL);
  AFE4400_Set_Register((unsigned char)LED2CONVST, (unsigned long)LED2CONVST_VAL);
  AFE4400_Set_Register((unsigned char)LED2CONVEND, (unsigned long)LED2CONVEND_VAL);
  AFE4400_Set_Register((unsigned char)ALED2CONVST, (unsigned long)ALED2CONVST_VAL);
  AFE4400_Set_Register((unsigned char)ALED2CONVEND, (unsigned long)ALED2CONVEND_VAL);
  AFE4400_Set_Register((unsigned char)LED1CONVST, (unsigned long)LED1CONVST_VAL);
  AFE4400_Set_Register((unsigned char)LED1CONVEND, (unsigned long)LED1CONVEND_VAL);
  AFE4400_Set_Register((unsigned char)ALED1CONVST, (unsigned long)ALED1CONVST_VAL);
  AFE4400_Set_Register((unsigned char)ALED1CONVEND, (unsigned long)ALED1CONVEND_VAL);
  AFE4400_Set_Register((unsigned char)ADCRSTSTCT0, (unsigned long)ADCRSTSTCT0_VAL);
  AFE4400_Set_Register((unsigned char)ADCRSTENDCT0, (unsigned long)ADCRSTENDCT0_VAL);
  AFE4400_Set_Register((unsigned char)ADCRSTSTCT1, (unsigned long)ADCRSTSTCT1_VAL);
  AFE4400_Set_Register((unsigned char)ADCRSTENDCT1, (unsigned long)ADCRSTENDCT1_VAL);
  AFE4400_Set_Register((unsigned char)ADCRSTSTCT2, (unsigned long)ADCRSTSTCT2_VAL);
  AFE4400_Set_Register((unsigned char)ADCRSTENDCT2, (unsigned long)ADCRSTENDCT2_VAL);
  AFE4400_Set_Register((unsigned char)ADCRSTSTCT3, (unsigned long)ADCRSTSTCT3_VAL);
  AFE4400_Set_Register((unsigned char)ADCRSTENDCT3, (unsigned long)ADCRSTENDCT3_VAL);
  
//	 AFE4400_Set_Register(CONTROL1,        0x100);
  AFE4400_Set_Register(TIA_AMB_GAIN,    0x004481);
 // AFE4400_Set_Register(TIA_AMB_GAIN,    0x00427E);
  AFE4400_Set_Register(LEDCNTRL,        0x11414);
//  AFE4400_Set_Register(CONTROL2,        0x100);
  //AFE4400_Set_Register((unsigned char)CONTROL0, AFE4400_Current_Register_Settings[0]);            //0x00
  AFE4400_Set_Register((unsigned char)CONTROL2, AFE4400_Current_Register_Settings[35]);           //0x23
  AFE4400_Set_Register((unsigned char)TIAGAIN, AFE4400_Current_Register_Settings[32]);            //0x20
 // AFE4400_Set_Register((unsigned char)TIA_AMB_GAIN, AFE4400_Current_Register_Settings[33]);       //0x21
 // AFE4400_Set_Register((unsigned char)LEDCNTRL, AFE4400_Current_Register_Settings[34]);           //0x22
	AFE4400_Set_Register((unsigned char)CONTROL1, AFE4400_Current_Register_Settings[30]);           //0x1E
  #else
  AFE4400_Set_Register(LED2STC,         6050);
  AFE4400_Set_Register(LED2ENDC,        7998);
  AFE4400_Set_Register(LED2LEDSTC,      6000);
  AFE4400_Set_Register(LED2LEDENDC,     7999);
  AFE4400_Set_Register(ALED2STC,        50);
  AFE4400_Set_Register(ALED2ENDC,       1998);
  AFE4400_Set_Register(LED1STC,         2050);
  AFE4400_Set_Register(LED1ENDC,        3998);
  AFE4400_Set_Register(LED1LEDSTC,      2000);
  AFE4400_Set_Register(LED1LEDENDC,     3999);
  AFE4400_Set_Register(ALED1STC,        4050);
  AFE4400_Set_Register(ALED1ENDC,       5998);
  AFE4400_Set_Register(LED2CONVST,      4);
  AFE4400_Set_Register(LED2CONVEND,     1999);
  AFE4400_Set_Register(ALED2CONVST,     2004);
  AFE4400_Set_Register(ALED2CONVEND,    3999);
  AFE4400_Set_Register(LED1CONVST,      4004);
  AFE4400_Set_Register(LED1CONVEND,     5999);
  AFE4400_Set_Register(ALED1CONVST,     6004);
  AFE4400_Set_Register(ALED1CONVEND,    7999);
  
  AFE4400_Set_Register(ADCRSTSTCT0,     0);
  AFE4400_Set_Register(ADCRSTENDCT0,    3);
  AFE4400_Set_Register(ADCRSTSTCT1,     2000);
  AFE4400_Set_Register(ADCRSTENDCT1,    2003);
  AFE4400_Set_Register(ADCRSTSTCT2,     4000);
  AFE4400_Set_Register(ADCRSTENDCT2,    4003);
  AFE4400_Set_Register(ADCRSTSTCT3,     6000);
  AFE4400_Set_Register(ADCRSTENDCT3,    6003);
  
  AFE4400_Set_Register(PRPCOUNT,        7999);
  
  AFE4400_Set_Register(CONTROL1,        0x100);
  //AFE4400_Set_Register(TIA_AMB_GAIN,    0x014481);
  AFE4400_Set_Register(TIA_AMB_GAIN,    0x014141);
	AFE4400_Set_Register(LEDCNTRL,        0x11010);
	//AFE4400_Set_Register(LEDCNTRL,        0x1FFFF);
  AFE4400_Set_Register(CONTROL2,        0x100);
	#endif
	#endif
}
u32 AFE4400_Read_Register(u8 reg_address);
/**********************************************************************************************************
*	        AFE4400_Read_All_Regs          				                  					  *
**********************************************************************************************************/

void AFE4400_Read_All_Regs(unsigned long AFE4400eg_buf[])
{
  unsigned char Regs_i;
  for ( Regs_i = 0; Regs_i < 50; Regs_i++)
  {
    AFE4400eg_buf[Regs_i] = AFE4400_Read_Register(Regs_i);
  }
}
/*********************************************************************************************************/
/**********************************************************************************************************
*	        AFE4400_PowerOn_Init          				                  					  			  *
***********************************************************************************************************/
void AFE4400_PowerOn_Init(void)
{
  Init_AFE4400_Resource();
	HAL_Delay(2000);
	AFE4400_Default_Reg_Init();	
	Init_AFE4400_DRDY_Interrupt();
}

u8 recv_buf[20];
u8 len=0;
u32 pack_len=0;
u32 len_1,len_2,len_3;
u32 data1=0,data2=0,data=0;
u8 send_wave_flag=0;
//void putchar_0(char ch)
//{
//	 while((USART1->SR & 0X40) == 0);
//    /* ´®¿Ú·¢ËÍÍê³É£¬½«¸Ã×Ö·û·¢ËÍ */
//    USART1->DR = (uint8_t) ch;
//}
//void send_data(u8 *buff,u8 len)
//{
//	for(u8 i=0;i<len;i++)
//	putchar_0(buff[i]);
//}
void return_reg_data()
{
	u8 i=0;
	u32 buff[35];
	u8 send_buff[112];
	u32 len=112;
	for(i=0;i<=28;i++)
	{
		buff[i]=AFE4400_Read_Register(CONTROL0+i);	
	}
	buff[29]=AFE4400_Read_Register(PRPCOUNT);
	buff[30]=AFE4400_Read_Register(CONTROL1);
	buff[31]=AFE4400_Read_Register(TIA_AMB_GAIN);
	buff[32]=AFE4400_Read_Register(LEDCNTRL);
	buff[33]=AFE4400_Read_Register(CONTROL2);
	buff[34]=AFE4400_Read_Register(TIAGAIN);
	send_buff[0]='U';
	send_buff[1]='Q';
	send_buff[2]=len&0xFF;
	send_buff[3]=(len>>8)&0xFF;
	send_buff[4]=(len>>16)&0xFF;
	send_buff[5]=(len>>24)&0xFF;
	for(i=0;i<35;i++)
	{
		send_buff[6+(i*3+0)]=buff[i]&0xFF;
		send_buff[6+(i*3+1)]=(buff[i]>>8)&0xFF;
		send_buff[6+(i*3+2)]=(buff[i]>>16)&0xFF;		
	}
	send_buff[111]='#';
	//send_data(send_buff,112);
	HAL_UART_Transmit(&huart1, (uint8_t*)send_buff, 112,0xFFFF);
}
extern u8 ch;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	static u8 len=0;
    if(huart ->Instance == USART1)
    {
       
				//len=strlen(recv_buf);
				recv_buf[len++]=ch;			
				HAL_UART_Receive_IT(&huart1, &ch, 1);
				if(ch=='#')
				{
						len_1=recv_buf[5];
						len_2=recv_buf[4];
						len_3=recv_buf[3];
						pack_len= len_1<<24 | len_2<<16 | len_3<<8 | recv_buf[2];				
						if(recv_buf[0]=='U'&& recv_buf[pack_len-1]=='#')//&& len==pack_len)
						{
							switch(recv_buf[1])
							{
								case 'Q':
												return_reg_data();
									break;
								case 'R':
												send_wave_flag=1;
									break;
								case 'S':
											 data1 = recv_buf[9];
											 data2 = recv_buf[8];
											 data = data1<<16 | data2<<8 | recv_buf[7];
											if(recv_buf[6]<=28)
													AFE4400_Set_Register(CONTROL0+recv_buf[6],data);
											else
											{
												switch(recv_buf[6])
												{
													case 29:
																	AFE4400_Set_Register(PRPCOUNT,data);
														break;
													case 30:
																	AFE4400_Set_Register(CONTROL1,data);
														break;
													case 31:
																	AFE4400_Set_Register(TIA_AMB_GAIN,data);
														break;
													case 32:
																	AFE4400_Set_Register(LEDCNTRL,data);
														break;
													case 33:
																	AFE4400_Set_Register(CONTROL2,data);
														break;
													case 34:
																	AFE4400_Set_Register(TIAGAIN,data);
														break;
													default:
														break;
												}									
											}
									break;
													default:
														break;
							}
						}
						len=0;
					}
    }
}


