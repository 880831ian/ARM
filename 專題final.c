/***********************************************************
 * @file     LAB3-3.c
 * @brief    Keyboard + Seven Segment LAB
 *           for the NXP LPC11xx/LPC11Cxx Device Series
 * @version  V1.10
 * @date     2012
***********************************************************
 *  KEYBOARD 輸入 練習
 * 本開發板中的七段顯示器和KEYBOARD部分電路部分為重疊應用
 * Keyboard 的應用是透過行與列的掃描來判斷目前按下的按鈕為何
 * 本範例為透過移位方式來進行判斷，
 * 讀者可在後面章節中發現其它鍵盤掃描的方式，
 * 或可研究是否有其它更快更好的方式
 * 
 * 選擇的接腳是透過 
 *	PIO3_2		--> SCAN 0
 *	PIO3_4		--> SCAN 1
 *  PIO3_5		--> SCAN 2
 *  PIO1_11		--> DATA 3	(MSB)
 *  PIO1_10		--> DATA 2
 *  PIO1_9			--> DATA 1
 *  PIO1_8			--> DATA 0  (LSB)
***********************************************************/   
#include  "config.h" //組態及暫存器定義
#include  "stdio.h" 
#include  "stdlib.h" 
#include  "string.h"
#define D_SEL_7SEG4	0x020 	// 七段顯示器千位數	位置編碼
#define D_SEL_7SEG3	0x024	// 七段顯示器百位數	位置編碼
#define D_SEL_7SEG2	0x030   // 七段顯示器十位數	位置編碼
#define D_SEL_7SEG1	0x034	// 七段顯示器個位數	位置編碼

#define D_SEL_KC1	0x000
#define D_SEL_KC2	0x004
#define D_SEL_KC3	0x010
#define D_SEL_KC4	0x014

#define D_ST7920_E		(0x0010)	// P1
#define D_ST7920_RW		(0x0020)	// P1
#define D_ST7920_RES	(0x0001)	// P2
#define D_ST7920_RS		(0x0002)	// P0


#define D_ST7920_DB0	(1<<2)
#define D_ST7920_DB1	(1<<3)
#define D_ST7920_DB2	(1<<4)
#define D_ST7920_DB3	(1<<5)
#define D_ST7920_DB4	(1<<6)
#define D_ST7920_DB5	(1<<7)
#define D_ST7920_DB6	(1<<8)
#define D_ST7920_DB7	(1<<9)

#define D_ST7920_DB		0x03FC

#define D_ST7920_RES_EN	LPC_GPIO2->DATA &= ~D_ST7920_RES;
#define D_ST7920_RES_DN	LPC_GPIO2->DATA |= D_ST7920_RES;

#define D_ST7920_E_DN	LPC_GPIO1->DATA &= ~D_ST7920_E;
#define D_ST7920_E_EN	LPC_GPIO1->DATA |= D_ST7920_E;

#define D_ST7920_RW_WR	LPC_GPIO1->DATA &= ~D_ST7920_RW;
#define D_ST7920_RW_RD	LPC_GPIO1->DATA |= D_ST7920_RW;

#define D_ST7920_RS_IR	LPC_GPIO0->DATA &= ~D_ST7920_RS;
#define D_ST7920_RS_DR	LPC_GPIO0->DATA |= D_ST7920_RS;

void F_ST7920_IO_Init(void);
void F_ST7920_Init(void);
void F_ST7920_IR_WR(unsigned int);
void F_ST7920_DR_WR(unsigned int);
void F_ST7920_SetXY(uint8, uint8);

unsigned int G_FG_PB, G_DATA_PB;

void F_ST7920_SetXY(uint8 x, uint8 y)
{
	uint8 tmp = 0x00;
	switch(y)
	{
	case 0:
		tmp = 0x80;
		break;
	case 1:
		tmp = 0x90;
		break;
	default:
		tmp = 0x80;
		break;
	}

	F_ST7920_IR_WR(tmp|x);
	return;
}
int main (void)
{	 
	static const char correct[]="正確";
	static const char error[]="錯誤";
	static const char first[]="請輸入驗證碼";
	unsigned int i, j;
	int k,p=0,b,c,a,d,cot=0;
	char tab[4],pad[4];
	SystemInit();
	F_ST7920_Init();
	Delay_ms(50);
	

	
	// DATA LINE
	LPC_GPIO1-> DIR  |=  0xF00;
	LPC_GPIO1-> DATA &= ~0xF00;

	// SCAN LINE
	LPC_GPIO3-> DIR  |=  0x034;
	LPC_GPIO3-> DATA &= ~0x034;

	//  清除7段顯示器上數值 
	LPC_GPIO1-> DATA = (LPC_GPIO1-> DATA & 0x0FF) + (0x00<<8);
	LPC_GPIO3-> DATA = D_SEL_7SEG4;
	Delay_ms(10);
	LPC_GPIO3-> DATA = D_SEL_7SEG3;
	Delay_ms(10);
	LPC_GPIO3-> DATA = D_SEL_7SEG2;
	Delay_ms(10);
	LPC_GPIO3-> DATA = D_SEL_7SEG1;
	Delay_ms(10);

	
	LPC_GPIO3-> DATA = D_SEL_KC1;
				   
	G_FG_PB = 0x0000;
	
	while (1) 
	{
		
	F_ST7920_SetXY(0,0);
	for(i = 0;i<sizeof(first);i++)
	F_ST7920_DR_WR(first[i]);
		
		
		
	// 1 設定資料線方向為輸入，來讀取按鍵狀態
		LPC_GPIO1-> DIR  &= ~0xF00;
		
		G_FG_PB = 0xFFFF0000;

	// 2  分別掃描鍵盤的四行資料
		
		// 設定掃描鍵盤的第一行
		LPC_GPIO3-> DATA = D_SEL_KC1;
		Delay_ms(10);
		// 讀取資料線的資料，並放至G_FG_PB變數低位元組中低四位元
		G_FG_PB = G_FG_PB + ((LPC_GPIO1-> DATA & 0xF00) >> 8);
		Delay_ms(10);
		
		// 設定掃描鍵盤的第二行
		LPC_GPIO3-> DATA = D_SEL_KC2;
		Delay_ms(10);
		// 讀取資料線的資料，並放至G_FG_PB變數低位元組中高四位元
		G_FG_PB = G_FG_PB + ((LPC_GPIO1-> DATA & 0xF00) >> 4);
		Delay_ms(10);
		// 設定掃描鍵盤的第三行
		LPC_GPIO3-> DATA = D_SEL_KC3;
		Delay_ms(10);
		// 讀取資料線的資料，並放至G_FG_PB變數低高元組中低四位元
		G_FG_PB = G_FG_PB + ((LPC_GPIO1-> DATA & 0xF00));
		Delay_ms(10);
		// 設定掃描鍵盤的第四行
		LPC_GPIO3-> DATA = D_SEL_KC4;
		Delay_ms(10);
		// 讀取資料線的資料，並放至G_FG_PB變數低高元組中高四位元
		G_FG_PB = G_FG_PB + ((LPC_GPIO1-> DATA & 0xF00) << 4);
		Delay_ms(10);

		// 將取得資料反向
		G_FG_PB = ~G_FG_PB;


		// 3 透過移位來判斷是那個按鍵按下
		j = 0;

		for(i = 0; i < 16; i++)
		{			
			if(G_FG_PB & 0x0001)
				j = i + 1;
			G_FG_PB = G_FG_PB >> 1;
		} 
			k = G_DATA_PB;
		// 4 更新七段顯示器資料
		// 為避免誤動作，等待沒有輸入時才進行更新七段顯示器
		// 沒有輸入時 J = 0
		if((j == 0) && (G_DATA_PB != 0) && (G_DATA_PB <= 10) )
		{
			if(cot!=4)
			{
				tab[cot] = k+47;
				F_ST7920_SetXY(p,1);
				F_ST7920_DR_WR(tab[cot]);
				p=p+1;
				G_DATA_PB = 0;
				cot++;
			}
		}
		else if((j == 0) && (G_DATA_PB != 0) && (G_DATA_PB == 13) )
		{
			
			a=(rand()%10);
			b=(rand()%10);
			c=(rand()%10);
			d=(rand()%10);
			pad[0]=a+48;
			pad[1]=b+48;
			pad[2]=c+48;
			pad[3]=d+48;
			LPC_GPIO1-> DIR  |=  0xF00;
	
			LPC_GPIO3-> DATA = 0x000;
			// 顯示七段顯示器的千位數
			LPC_GPIO1-> DATA = ((a) << 8);
			LPC_GPIO3-> DATA = 0x020;
			Delay_ms(10);
			LPC_GPIO3-> DATA = 0x000;
			
			// 顯示七段顯示器的百位數
			
			LPC_GPIO1-> DATA = ((b) << 8);
			LPC_GPIO3-> DATA = 0x024;
			Delay_ms(10);
			LPC_GPIO3-> DATA = 0x000;

			// 顯示七段顯示器的十位數
			  			
			LPC_GPIO1-> DATA = ((c) << 8);
			LPC_GPIO3-> DATA = 0x030;
			Delay_ms(10);
			LPC_GPIO3-> DATA = 0x000;

			// 顯示七段顯示器的個位數
							
			LPC_GPIO1-> DATA = ((d) << 8);
			LPC_GPIO3-> DATA = 0x034;
			Delay_ms(10);
			LPC_GPIO3-> DATA = 0x000;
			
			G_DATA_PB = 0;
		}
		else if((j == 0) && (G_DATA_PB != 0) && (G_DATA_PB == 14) )
		{
			if(tab[0]==pad[0] && tab[1]==pad[1] && tab[2]==pad[2] && tab[3]==pad[3]) 
				{
					F_ST7920_SetXY(4,1);
					for(i = 0;i<sizeof(correct);i++)
						F_ST7920_DR_WR(correct[i]);
				}
    else 
				{
					F_ST7920_SetXY(4,1);
					for(i = 0;i<sizeof(error);i++)
						F_ST7920_DR_WR(error[i]);
				}
			
		}
		else if (G_DATA_PB == 15)
		{
			
		}
		else
			G_DATA_PB = j;

		 
	}
}

void F_ST7920_IO_Init(void)
{
	// 暫存器配置為一般用途使用
    //LPC_IOCON->PIO0_4 =  0x0100;
    //LPC_IOCON->PIO0_5 =  0x0100;
	// D_ST7920_E		
	LPC_IOCON->R_PIO1_0 |= 0x0001;
	// D_ST7920_RW
	//LPC_IOCON->PIO1_4 =  0xD0;
	// D_ST7920_RES		
	//LPC_IOCON->PIO2_0 =  0xD0;

	// 暫存器設置方向
	LPC_GPIO0->DIR	|=  (D_ST7920_RS + D_ST7920_DB);
	LPC_GPIO1->DIR  |=  (D_ST7920_E + D_ST7920_RW);
	LPC_GPIO2->DIR  |=  D_ST7920_RES;
	
	LPC_GPIO0->DATA	|=  (D_ST7920_RS + D_ST7920_DB);
	LPC_GPIO1->DATA |=  (D_ST7920_E + D_ST7920_RW);
	LPC_GPIO2->DATA |=  D_ST7920_RES;

	LPC_GPIO0->DATA	&=  ~(D_ST7920_RS + D_ST7920_DB);
	LPC_GPIO1->DATA &=  ~(D_ST7920_E + D_ST7920_RW);
	LPC_GPIO2->DATA &=  ~D_ST7920_RES;	  
}

void F_ST7920_Init(void)
{

	// LCM 相關暫存器設定
	F_ST7920_IO_Init();

	// ST7920 RESET
	D_ST7920_RES_EN;
  	Delay_ms(50);
	D_ST7920_RES_DN;

    
	// LCM 初使化設定
	// 8-bit mode
	F_ST7920_IR_WR(0x0030);	// 8-Bit Mode
	Delay_ms(10);
	F_ST7920_IR_WR(0x0004);	// 8-Bit Mode
	Delay_ms(10);

	F_ST7920_IR_WR(0x000C);	// Display ON, C On, B On�
	Delay_ms(10);

	F_ST7920_IR_WR(0x0001);	// Display Clear
	Delay_ms(50);
	F_ST7920_IR_WR(0x0002);	// Return Home
	Delay_ms(1);
	F_ST7920_IR_WR(0x0080);	// Entry Mode Set
	Delay_ms(1);
}

/******************************************************************************
 *  F_ST7920_IR_WR 
 *  寫入資料至指令暫存器
*******************************************************************************/
void F_ST7920_IR_WR(unsigned int CMD)
{
	D_ST7920_E_DN;
	Delay_ms(1);

	D_ST7920_RS_IR;
	D_ST7920_RW_WR;

	D_ST7920_E_EN;

	LPC_GPIO0->DATA	= (LPC_GPIO0->DATA & 0xFC03) + (CMD << 2);
	
	Delay_ms(1);

	D_ST7920_E_DN;
}

/******************************************************************************
 *  F_ST7920_DR_WR 
 *  寫入資料至資料暫存器 
*******************************************************************************/
void F_ST7920_DR_WR(unsigned int DATA)
{
	uint32	tmp;
	D_ST7920_E_DN;
	Delay_ms(1);

	D_ST7920_RS_DR;
	D_ST7920_RW_WR;

	D_ST7920_E_EN;

   	tmp = (LPC_GPIO0->DATA & 0xFC03);
	tmp |= 	  (DATA << 2);
	Delay_ms(1);
	LPC_GPIO0->DATA	= tmp;
	
	Delay_ms(4);

	D_ST7920_E_DN;
}
