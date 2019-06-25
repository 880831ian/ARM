/***********************************************************
 * @file     LAB3-3.c
 * @brief    Keyboard + Seven Segment LAB
 *           for the NXP LPC11xx/LPC11Cxx Device Series
 * @version  V1.10
 * @date     2012
***********************************************************
 *  KEYBOARD øÈ§J Ωm≤ﬂ
 * •ª∂}µo™O§§™∫§C¨q≈„•‹æπ©MKEYBOARD≥°§¿πq∏Ù≥°§¿¨∞≠´≈|¿≥•Œ
 * Keyboard ™∫¿≥•Œ¨O≥zπL¶ÊªP¶C™∫±Ω¥y®”ßP¬_•ÿ´e´ˆ§U™∫´ˆ∂s¨∞¶Û
 * •ªΩd®“¨∞≥zπL≤æ¶Ï§Ë¶°®”∂i¶ÊßP¬_°A
 * ≈™™Ã•i¶b´·≠±≥π∏`§§µo≤{®‰•¶¡‰ΩL±Ω¥y™∫§Ë¶°°A
 * ©Œ•i¨„®s¨Oß_¶≥®‰•¶ßÛß÷ßÛ¶n™∫§Ë¶°
 * 
 * øÔæ‹™∫±µ∏}¨O≥zπL 
 *	PIO3_2		--> SCAN 0
 *	PIO3_4		--> SCAN 1
 *  PIO3_5		--> SCAN 2
 *  PIO1_11		--> DATA 3	(MSB)
 *  PIO1_10		--> DATA 2
 *  PIO1_9			--> DATA 1
 *  PIO1_8			--> DATA 0  (LSB)
***********************************************************/   
#include  "config.h" //≤’∫A§Œº»¶sæπ©w∏q
#include  "stdio.h" 
#include  "stdlib.h" 
#include  "string.h"
#define D_SEL_7SEG4	0x020 	// §C¨q≈„•‹æπ§d¶Ïº∆	¶Ï∏mΩsΩX
#define D_SEL_7SEG3	0x024	// §C¨q≈„•‹æπ¶ ¶Ïº∆	¶Ï∏mΩsΩX
#define D_SEL_7SEG2	0x030   // §C¨q≈„•‹æπ§Q¶Ïº∆	¶Ï∏mΩsΩX
#define D_SEL_7SEG1	0x034	// §C¨q≈„•‹æπ≠”¶Ïº∆	¶Ï∏mΩsΩX

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
	static const char correct[]="•øΩT";
	static const char error[]="ø˘ª~";
	static const char first[]="Ω–øÈ§J≈Á√“ΩX";
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

	//  ≤M∞£7¨q≈„•‹æπ§Wº∆≠» 
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
		
		
		
	// 1 ≥]©w∏ÍÆ∆Ωu§Ë¶V¨∞øÈ§J°A®”≈™®˙´ˆ¡‰™¨∫A
		LPC_GPIO1-> DIR  &= ~0xF00;
		
		G_FG_PB = 0xFFFF0000;

	// 2  §¿ßO±Ω¥y¡‰ΩL™∫•|¶Ê∏ÍÆ∆
		
		// ≥]©w±Ω¥y¡‰ΩL™∫≤ƒ§@¶Ê
		LPC_GPIO3-> DATA = D_SEL_KC1;
		Delay_ms(10);
		// ≈™®˙∏ÍÆ∆Ωu™∫∏ÍÆ∆°A®√©Ò¶‹G_FG_PB≈‹º∆ßC¶Ï§∏≤’§§ßC•|¶Ï§∏
		G_FG_PB = G_FG_PB + ((LPC_GPIO1-> DATA & 0xF00) >> 8);
		Delay_ms(10);
		
		// ≥]©w±Ω¥y¡‰ΩL™∫≤ƒ§G¶Ê
		LPC_GPIO3-> DATA = D_SEL_KC2;
		Delay_ms(10);
		// ≈™®˙∏ÍÆ∆Ωu™∫∏ÍÆ∆°A®√©Ò¶‹G_FG_PB≈‹º∆ßC¶Ï§∏≤’§§∞™•|¶Ï§∏
		G_FG_PB = G_FG_PB + ((LPC_GPIO1-> DATA & 0xF00) >> 4);
		Delay_ms(10);
		// ≥]©w±Ω¥y¡‰ΩL™∫≤ƒ§T¶Ê
		LPC_GPIO3-> DATA = D_SEL_KC3;
		Delay_ms(10);
		// ≈™®˙∏ÍÆ∆Ωu™∫∏ÍÆ∆°A®√©Ò¶‹G_FG_PB≈‹º∆ßC∞™§∏≤’§§ßC•|¶Ï§∏
		G_FG_PB = G_FG_PB + ((LPC_GPIO1-> DATA & 0xF00));
		Delay_ms(10);
		// ≥]©w±Ω¥y¡‰ΩL™∫≤ƒ•|¶Ê
		LPC_GPIO3-> DATA = D_SEL_KC4;
		Delay_ms(10);
		// ≈™®˙∏ÍÆ∆Ωu™∫∏ÍÆ∆°A®√©Ò¶‹G_FG_PB≈‹º∆ßC∞™§∏≤’§§∞™•|¶Ï§∏
		G_FG_PB = G_FG_PB + ((LPC_GPIO1-> DATA & 0xF00) << 4);
		Delay_ms(10);

		// ±N®˙±o∏ÍÆ∆§œ¶V
		G_FG_PB = ~G_FG_PB;


		// 3 ≥zπL≤æ¶Ï®”ßP¬_¨O®∫≠”´ˆ¡‰´ˆ§U
		j = 0;

		for(i = 0; i < 16; i++)
		{			
			if(G_FG_PB & 0x0001)
				j = i + 1;
			G_FG_PB = G_FG_PB >> 1;
		} 
			k = G_DATA_PB;
		// 4 ßÛ∑s§C¨q≈„•‹æπ∏ÍÆ∆
		// ¨∞¡◊ßKª~∞ ß@°Aµ•´›®S¶≥øÈ§JÆ…§~∂i¶ÊßÛ∑s§C¨q≈„•‹æπ
		// ®S¶≥øÈ§JÆ… J = 0
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
			// ≈„•‹§C¨q≈„•‹æπ™∫§d¶Ïº∆
			LPC_GPIO1-> DATA = ((a) << 8);
			LPC_GPIO3-> DATA = 0x020;
			Delay_ms(10);
			LPC_GPIO3-> DATA = 0x000;
			
			// ≈„•‹§C¨q≈„•‹æπ™∫¶ ¶Ïº∆
			
			LPC_GPIO1-> DATA = ((b) << 8);
			LPC_GPIO3-> DATA = 0x024;
			Delay_ms(10);
			LPC_GPIO3-> DATA = 0x000;

			// ≈„•‹§C¨q≈„•‹æπ™∫§Q¶Ïº∆
			  			
			LPC_GPIO1-> DATA = ((c) << 8);
			LPC_GPIO3-> DATA = 0x030;
			Delay_ms(10);
			LPC_GPIO3-> DATA = 0x000;

			// ≈„•‹§C¨q≈„•‹æπ™∫≠”¶Ïº∆
							
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
	// º»¶sæπ∞t∏m¨∞§@ØÎ•Œ≥~®œ•Œ
    //LPC_IOCON->PIO0_4 =  0x0100;
    //LPC_IOCON->PIO0_5 =  0x0100;
	// D_ST7920_E		
	LPC_IOCON->R_PIO1_0 |= 0x0001;
	// D_ST7920_RW
	//LPC_IOCON->PIO1_4 =  0xD0;
	// D_ST7920_RES		
	//LPC_IOCON->PIO2_0 =  0xD0;

	// º»¶sæπ≥]∏m§Ë¶V
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

	// LCM ¨€√ˆº»¶sæπ≥]©w
	F_ST7920_IO_Init();

	// ST7920 RESET
	D_ST7920_RES_EN;
  	Delay_ms(50);
	D_ST7920_RES_DN;

    
	// LCM ™Ï®œ§∆≥]©w
	// 8-bit mode
	F_ST7920_IR_WR(0x0030);	// 8-Bit Mode
	Delay_ms(10);
	F_ST7920_IR_WR(0x0004);	// 8-Bit Mode
	Delay_ms(10);

	F_ST7920_IR_WR(0x000C);	// Display ON, C On, B Onı
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
 *  ºg§J∏ÍÆ∆¶‹´¸•Oº»¶sæπ
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
 *  ºg§J∏ÍÆ∆¶‹∏ÍÆ∆º»¶sæπ 
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
