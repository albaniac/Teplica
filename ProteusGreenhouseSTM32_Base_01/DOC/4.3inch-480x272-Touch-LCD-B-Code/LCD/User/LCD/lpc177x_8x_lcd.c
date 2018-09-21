/**********************************************************************
* $Id$		lpc177x_8x_LCD.c			2012-08-13
*//**
* @file		lpc177x_8x_LCD.c
* @brief	Contains all functions support for LCD firmware library
*			on Open1788
* @version	1.0
* @date		13. August. 2012
* @author	waveshare  xuwenjie (jason)
* 
* Copyright(C) 2012, waveshare
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* waveshare Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. waveshare Semiconductors
* reserves the right to make changes in the software without
* notification. waveshare Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/
#include "LCD/lpc177x_8x_lcd.h"
#include"EX_SDRAM/EX_SDRAM.H"
#include "LCD/AsciiLib.h"

#define C_GLCD_CLK_PER_LINE     (C_GLCD_H_SIZE + C_GLCD_H_PULSE + C_GLCD_H_FRONT_PORCH + C_GLCD_H_BACK_PORCH)
#define C_GLCD_LINES_PER_FRAME  (C_GLCD_V_SIZE + C_GLCD_V_PULSE + C_GLCD_V_FRONT_PORCH + C_GLCD_V_BACK_PORCH)
#define C_GLCD_PIX_CLK          (C_GLCD_CLK_PER_LINE * C_GLCD_LINES_PER_FRAME * C_GLCD_REFRESH_FREQ)


/*************************************************************************
 * Function Name: GLCD_Ctrl
 * Parameters: Bool bEna
 *
 * Return: none
 *
 * Description: GLCD enable disabe sequence
 *
 *************************************************************************/
void GLCD_Ctrl (Bool bEna)
{
	volatile uint32_t i;
  if (bEna)
  {
    LPC_LCD->CTRL |= (1<<0);
    for(i = C_GLCD_PWR_ENA_DIS_DLY; i; i--)
    LPC_LCD->CTRL |= (1<<11);
  }
  else
  {
    LPC_LCD->CTRL &= ~(1<<11);
    for(i = C_GLCD_PWR_ENA_DIS_DLY; i; i--);
    LPC_LCD->CTRL &= ~(1<<0);
  }
}


/*************************************************************************
 * Function Name: GLCD_Init
 * Parameters: const unsigned long *pPain, const unsigned long * pPallete
 *
 * Return: none
 *
 * Description: GLCD controller init
 *
 *************************************************************************/

void GLCD_Init (const unsigned long *pPain, const unsigned long * pPallete)
{
	unsigned long i;
	unsigned long *pDst = (unsigned long *)LCD_VRAM_BASE_ADDR;
	unsigned long *pDst16;
	const unsigned long *pBmp16 = pBmp16;
  /* R */
  LPC_IOCON->P0_4  = 0x27;	 // LCD_VD_0
  LPC_IOCON->P0_5   = 0x27;  // LCD_VD_1
  LPC_IOCON->P4_28   = 0x27;  // LCD_VD_2

  LPC_IOCON->P4_29  = 0x27;	 // LCD_VD_3
  LPC_IOCON->P2_6   = 0x27;  // LCD_VD_4
  LPC_IOCON->P2_7   = 0x27;  // LCD_VD_5
  LPC_IOCON->P2_8   = 0x27;  // LCD_VD_6
  LPC_IOCON->P2_9  = 0x27;  // LCD_VD_7 

  /* G */     
  LPC_IOCON->P0_6  = 0x27;  // LCD_VD_8
  LPC_IOCON->P0_7  = 0x27;  // LCD_VD_9


  LPC_IOCON->P1_20  = 0x27;  // LCD_VD_10
  LPC_IOCON->P1_21  = 0x27;  // LCD_VD_11
  LPC_IOCON->P1_22  = 0x27;  // LCD_VD_12
  LPC_IOCON->P1_23  = 0x27;  // LCD_VD_13
  LPC_IOCON->P1_24  = 0x27;  // LCD_VD_14
  LPC_IOCON->P1_25  = 0x27;  // LCD_VD_15

	/* B */
  LPC_IOCON->P0_8  = 0x27;  // LCD_VD_16
  LPC_IOCON->P0_9  = 0x27;  // LCD_VD_17
  LPC_IOCON->P2_12  = 0x27;  // LCD_VD_18

  LPC_IOCON->P2_13  = 0x27;	 // LCD_VD_19
  LPC_IOCON->P1_26  = 0x27;  // LCD_VD_20
  LPC_IOCON->P1_27  = 0x27;  // LCD_VD_21
  LPC_IOCON->P1_28  = 0x27;  // LCD_VD_22
  LPC_IOCON->P1_29  = 0x27;  // LCD_VD_23

  LPC_IOCON->P2_2   = 0x27;  // LCD_DCLK
  LPC_IOCON->P2_5   = 0x27;  // LCD_LP -- HSYNC
  LPC_IOCON->P2_3   = 0x27;  // LCD_FP -- VSYNC
  LPC_IOCON->P2_4   = 0x27;  // LCD_ENAB_M -- LCDDEN
  LPC_IOCON->P2_0   = 0x27;  // LCD_PWR

	// <<< debug <<<
	
	/*Back light enable*/
	LPC_GPIO2->DIR = (1<<1);
	LPC_GPIO2->SET= (5<<1);

	//Turn on LCD clock
	LPC_SC->PCONP |= 1<<0;
	
	// Disable cursor
	LPC_LCD->CRSR_CTRL &=~(1<<0);
	
	// disable GLCD controller	
	LPC_LCD->CTRL = 0;
	
	LPC_LCD->CTRL &= ~(0x07 <<1);
//	#ifndef __RAM__
	// RGB565
//	LPC_LCD->CTRL |= (6<<1);
//	#else
	// RGB888
	LPC_LCD->CTRL |= (5<<1);
//	#endif
	
	// TFT panel
	LPC_LCD->CTRL |= (1<<5);
	// single panel
	LPC_LCD->CTRL &= ~(1<<7);
	// notmal output
	LPC_LCD->CTRL &= ~(1<<8);
	// little endian byte order
	LPC_LCD->CTRL &= ~(1<<9);
	// little endian pix order
	LPC_LCD->CTRL &= ~(1<<10);
	// disable power
	LPC_LCD->CTRL &= ~(1<<11);
	// init pixel clock
	LPC_SC->LCD_CFG = PeripheralClock / ((unsigned long) C_GLCD_PIX_CLK);
	// bypass inrenal clk divider
	LPC_LCD->POL |=(1<<26);
	// clock source for the LCD block is HCLK
	LPC_LCD->POL &= ~(1<<5);
	// LCDFP pin is active LOW and inactive HIGH
	LPC_LCD->POL |= (1<<11);
	// LCDLP pin is active LOW and inactive HIGH
	LPC_LCD->POL |= (1<<12);
	// data is driven out into the LCD on the falling edge
	LPC_LCD->POL &= ~(1<<13);
	// active high
	LPC_LCD->POL &= ~(1<<14);
	LPC_LCD->POL &= ~(0x3FF <<16);
	LPC_LCD->POL |= (C_GLCD_H_SIZE-1)<<16;
	
	// init Horizontal Timing
	LPC_LCD->TIMH = 0; //reset TIMH before set value
	LPC_LCD->TIMH |= (C_GLCD_H_BACK_PORCH - 1)<<24;
	LPC_LCD->TIMH |= (C_GLCD_H_FRONT_PORCH - 1)<<16;
	LPC_LCD->TIMH |= (C_GLCD_H_PULSE - 1)<<8;
	LPC_LCD->TIMH |= ((C_GLCD_H_SIZE/16) - 1)<<2;
	
	// init Vertical Timing
	LPC_LCD->TIMV = 0;  //reset TIMV value before setting
	LPC_LCD->TIMV |= (C_GLCD_V_BACK_PORCH)<<24;
	LPC_LCD->TIMV |= (C_GLCD_V_FRONT_PORCH)<<16;
	LPC_LCD->TIMV |= (C_GLCD_V_PULSE - 1)<<10;
	LPC_LCD->TIMV |= C_GLCD_V_SIZE - 1;
	// Frame Base Address doubleword aligned
	LPC_LCD->UPBASE = LCD_VRAM_BASE_ADDR & ~7UL ;
	LPC_LCD->LPBASE = LCD_VRAM_BASE_ADDR & ~7UL ;
        // init colour pallet

	if(NULL != pPallete)
	{
		GLCD_SetPallet(pPallete);
	}

    pDst16 = (unsigned long *) LCD_VRAM_BASE_ADDR;

	for( i = 0; (C_GLCD_H_SIZE * C_GLCD_V_SIZE) > i; i++)
	{
  		*pDst16++ = purple;
	}

  	for(i = C_GLCD_ENA_DIS_DLY; i; i--);
}

/*************************************************************************
 * Function Name: GLCD_SetPallet
 * Parameters: const unsigned long * pPallete
 *
 * Return: none
 *
 * Description: GLCD init colour pallete
 *
 *************************************************************************/
void GLCD_SetPallet (const unsigned long * pPallete)
{
	unsigned long i;
	unsigned long * pDst = (unsigned long *)LPC_LCD->PAL;
	// assert(pPallete);
	for (i = 0; i < 128; i++)
	{
	*pDst++ = *pPallete++;
	}
}


/******************************************************************************
* Function Name  : LCD_SetPoint
* Description    : 
* Input          : - Xpos: Row Coordinate
*                  - Ypos: Line Coordinate 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_SetPoint(uint16_t Xpos,uint16_t Ypos,uint32_t color)
{
	unsigned long *pDst = (unsigned long *)LCD_VRAM_BASE_ADDR;
    unsigned long value;
	if( Xpos >= C_GLCD_H_SIZE || Ypos >= C_GLCD_V_SIZE )
	{
		return;
	}

    value=Ypos*480+Xpos;
    pDst = pDst +value ;
    *pDst = color; 
}



/******************************************************************************
* Function Name  : LCD_DrawLine
* Description    : Bresenham's line algorithm
* Input          : - x0:
*                  - y0:
*       				   - x1:
*       		       - y1:
*                  - color:
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/	 
void LCD_DrawLine( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 , uint32_t color )
{

unsigned int i1,j1,k1,l1; 
int n1=0; 
unsigned char temp=0; 
if(y2<y1)
    {i1=x1;
    l1=y1;
    x1=x2;
    y1=y2;
    x2=i1;
    y2=l1;}
k1=y2-y1; 
if (k1==0) 
    {if (x1>x2) 
        {i1=x1;
        x1=x2;
        x2=i1;} 
     for ( i1=x1;i1<=x2;i1++ ) 
        LCD_SetPoint(i1,y1,color); 
     } 
else
     {if(x2>=x1)
         {temp=1;
         j1=x2-x1;}
      else 
         j1=x1-x2;
      i1=x1;
      l1=y1;
      LCD_SetPoint(i1,l1,color);
      if(temp&&(k1<=j1))
          while(i1!=x2)
              {if(n1<0)
                  {i1=i1+1;
                  n1=n1+(y2-l1);}
               else 
                  {i1=i1+1;
                  l1=l1+1;
                  n1=n1+(y2-l1)-(x2-i1);} 
               LCD_SetPoint(i1,l1,color);}
      else if(temp&&(k1>j1))
          while(l1!=y2)
              {j1=x2-i1;
              k1=y2-l1; 
              if(n1<0)
                  {i1=i1+1;
                  l1=l1+1;
                  n1=n1+k1-j1;} 
              else 
                  {l1=l1+1;
                  n1=n1-j1;}
              LCD_SetPoint(i1,l1,color);} 
      else if(!temp&&(k1<=j1))
          while(i1!=x2) 
              {j1=i1-x2;
              k1=y2-l1; 
              if(n1<0)
                  {i1=i1-1;
                  n1=n1+k1;} 
              else 
                  {i1=i1-1;
                  l1=l1+1;
                  n1=n1+k1-j1;}
              LCD_SetPoint(i1,l1,color);}
      else if(!temp &&(k1>j1))
          while(l1!=y2) 
              {j1=i1-x2;
              k1=y2-l1; 
              if(n1<0)
                  {i1=i1-1;
                  l1=l1+1;
                  n1=n1+k1-j1;} 
              else 
                  {l1=l1+1;
                  n1=n1-j1;}
                  LCD_SetPoint(i1,l1,color);}
               }
} 
/******************************************************************************
* Function Name  : LCD_Drawcircle
* Description    : Draw round
* Input  
* int x,         : - x0:
* int y,         : - y0:
* int r,      	 : - radius:
* int color      : - color:
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/	 


void LCD_Drawcircle(int x0, int y0, int r, int color)
{
   int  draw_x0, draw_y0;                        // 刽图点坐标变量
    int  draw_x1, draw_y1;        
    int  draw_x2, draw_y2;        
    int  draw_x3, draw_y3;        
    int  draw_x4, draw_y4;        
    int  draw_x5, draw_y5;        
    int  draw_x6, draw_y6;        
    int  draw_x7, draw_y7;        
    int  xx, yy;                                        // 画圆控制变量

    int  di;                                                // 决策变量
   
   /* 参数过滤 */
   if(0==r) return;
   
   /* 计算出8个特殊点(0、45、90、135、180、225、270度)，进行显示 */
   draw_x0 = draw_x1 = x0;
   draw_y0 = draw_y1 = y0 + r;
   if(draw_y0<C_GLCD_V_SIZE) LCD_SetPoint(draw_x0, draw_y0, color);        // 90度
        
   draw_x2 = draw_x3 = x0;
   draw_y2 = draw_y3 = y0 - r;
   if(draw_y2>=0) LCD_SetPoint(draw_x2, draw_y2, color);                        // 270度
   
        
   draw_x4 = draw_x6 = x0 + r;
   draw_y4 = draw_y6 = y0;
   if(draw_x4<C_GLCD_H_SIZE) LCD_SetPoint(draw_x4, draw_y4, color);        // 0度
   
   draw_x5 = draw_x7 = x0 - r;
   draw_y5 = draw_y7 = y0;
   if(draw_x5>=0) LCD_SetPoint(draw_x5, draw_y5, color);                        // 180度   
   if(1==r) return;                                        // 若半径为1，则已圆画完
   
   
   /* 使用Bresenham法进行画圆 */
   di = 3 - 2*r;                                        // 初始化决策变量
   
   xx = 0;
   yy = r;        
   while(xx<yy)
   {  if(di<0)
          {  di += 4*xx + 6;              
          }
          else
          {  di += 4*(xx - yy) + 10;
          
             yy--;          
                 draw_y0--;
                 draw_y1--;
                 draw_y2++;
                 draw_y3++;
                 draw_x4--;
                 draw_x5++;
                 draw_x6--;
                 draw_x7++;                 
          }
          
          xx++;   
          draw_x0++;
          draw_x1--;
          draw_x2++;
          draw_x3--;
          draw_y4++;
          draw_y5++;
          draw_y6--;
          draw_y7--;
                
        
          /* 要判断当前点是否在有效范围内 */
          if( (draw_x0<=C_GLCD_H_SIZE)&&(draw_y0>=0) )        
          {  LCD_SetPoint(draw_x0, draw_y0, color);
          }            
          if( (draw_x1>=0)&&(draw_y1>=0) )        
          {  LCD_SetPoint(draw_x1, draw_y1, color);
          }
          if( (draw_x2<=C_GLCD_H_SIZE)&&(draw_y2<=C_GLCD_V_SIZE) )        
          {  LCD_SetPoint(draw_x2, draw_y2, color);   
          }
          if( (draw_x3>=0)&&(draw_y3<=C_GLCD_V_SIZE) )        
          {  LCD_SetPoint(draw_x3, draw_y3, color);
          }
          if( (draw_x4<=C_GLCD_H_SIZE)&&(draw_y4>=0) )        
          {  LCD_SetPoint(draw_x4, draw_y4, color);
          }
          if( (draw_x5>=0)&&(draw_y5>=0) )        
          {  LCD_SetPoint(draw_x5, draw_y5, color);
          }
          if( (draw_x6<=C_GLCD_H_SIZE)&&(draw_y6<=C_GLCD_V_SIZE) )        
          {  LCD_SetPoint(draw_x6, draw_y6, color);
          }
          if( (draw_x7>=0)&&(draw_y7<=C_GLCD_V_SIZE) )        
          {  LCD_SetPoint(draw_x7, draw_y7, color);
          }
   }
}



/******************************************************************************
* Function Name  : PutChar
* Description    : 
* Input          : - Xpos:
*                  - Ypos:
*				           - ASCI:
*				           - charColor:
*				           - bkColor:
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void PutChar( uint16_t Xpos, uint16_t Ypos, uint8_t ASCI, uint32_t charColor, uint32_t bkColor )
{
	uint32_t i, j;
    uint8_t buffer[16], tmp_char;
    GetASCIICode(buffer,ASCI);
    for( i=0; i<16; i++ )
    {
        tmp_char = buffer[i];
        for( j=0; j<8; j++ )
        {
            if( (tmp_char >> 7 - j) & 0x01 == 0x01 )
            {
                LCD_SetPoint( Xpos + j, Ypos + i, charColor );
            }
            else
            {
                LCD_SetPoint( Xpos + j, Ypos + i, bkColor );
            }
        }
    }
}

/******************************************************************************
* Function Name  : GUI_Text
* Description    : 
* Input          : - Xpos: 
*                  - Ypos: 
*				           - str:
*				           - charColor:
*				           - bkColor:
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void GUI_Text(uint16_t Xpos, uint16_t Ypos, uint8_t *str,uint32_t Color, uint32_t bkColor)
{
    uint8_t TempChar;
    do
    {
        TempChar = *str++;  
        PutChar( Xpos, Ypos, TempChar, Color, bkColor );    
        if( Xpos < C_GLCD_H_SIZE - 8 )
        {
            Xpos += 8;
        } 
        else if ( Ypos < C_GLCD_V_SIZE - 16 )
        {
            Xpos = 0;
            Ypos += 16;
        }   
        else
        {
            Xpos = 0;
            Ypos = 0;
        }    
    }
    while ( *str != 0 );
}


