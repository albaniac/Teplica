/**********************************************************************
* $Id$		Gpio_LedBlinky.c		2011-06-02
*//**
* @file		Gpio_LedBlinky.c
* @brief	This example describes how to use GPIO interrupt to drive
* 			LEDs
* @version	1.0
* @date		02. June. 2011
* @author	NXP MCU SW Application Team
*
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/
#include "lpc177x_8x_gpio.h"
#include "lpc177x_8x_clkpwr.h"
#include "EX_SDRAM/EX_SDRAM.H"
#include "LCD/lpc177x_8x_lcd.h"
#include "LCD/logo.h"


/** @defgroup GPIO_LedBlinky	GPIO Blinky
 * @ingroup GPIO_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS *************************/

/************************** PRIVATE VARIABLES *************************/
/* SysTick Counter */
volatile unsigned long SysTickCnt;

/************************** PRIVATE FUNCTIONS *************************/
void SysTick_Handler (void);

void Delay (unsigned long tick);

void lcd_test(void);
uint32_t color_table[]={0x000000,0xFFFFFF,0x0000FF,0x00FF00,0xFF0000};


/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int main(void)
{
	uint32_t cclk = CLKPWR_GetCLK(CLKPWR_CLKTYPE_CPU);
	/* Generate interrupt each 1 ms   */
	SysTick_Config(cclk/1000 - 1); 
    
    SDRAM_Init();
	/*Disable LCD controller*/
	GLCD_Ctrl (FALSE);

	/*Init LCD and copy picture in video RAM*/
	GLCD_Init (LogoPic.pPicStream, NULL);
   	/*Enable LCD*/
	GLCD_Ctrl (TRUE); 
    
//    lcd_test();
    /*LCD DrawLine*/
    LCD_DrawLine( 0, 0, 240, 272 , White );
    LCD_DrawLine( 53, 0, 240, 272 , Black );
    LCD_DrawLine( 53*2, 0, 240, 272 , Blue );
    LCD_DrawLine( 53*3,  0, 240, 272 , Red );
    LCD_DrawLine( 53*4, 0, 240, 272 , Green );

    LCD_DrawLine( 53*5, 0, 240, 272 , Green );
    LCD_DrawLine( 53*6, 0, 240, 272 , Red );
    LCD_DrawLine( 53*7, 0, 240, 272 , Blue );
    LCD_DrawLine( 53*8, 0, 240, 272 , Black );
    LCD_DrawLine( 53*9, 0, 240, 272 , White );
    /*LCD Drawcircle*/

    LCD_Drawcircle(240,70,40,Blue);
    LCD_Drawcircle(240,70,39,Blue); 
    LCD_Drawcircle(240,70,38,Blue); 

    LCD_Drawcircle(180,70,40,Black);
    LCD_Drawcircle(180,70,39,Black);
    LCD_Drawcircle(180,70,38,Black);

    LCD_Drawcircle(300,70,40,Red);
    LCD_Drawcircle(300,70,39,Red);
    LCD_Drawcircle(300,70,38,Red);

    LCD_Drawcircle(210,110,40,Yellow);
    LCD_Drawcircle(210,110,39,Yellow);
    LCD_Drawcircle(210,110,38,Yellow);

    LCD_Drawcircle(270,110,40,Green);
    LCD_Drawcircle(270,110,39,Green);
    LCD_Drawcircle(270,110,38,Green);

    GUI_Text(120,170,"Welcome use Open1788 dinner plate,",White,purple);
    GUI_Text(120,186,"more products,please login  ",White,purple);
    GUI_Text(120,202,"www.waveshar.net",White,purple);     
    GUI_Text(350,218,"--jason",White,purple);   
    /*LED1 (P1_14)  LED2 (P0_16  LED3 (P1_13)  LED4 (P4_27)*/
	GPIO_SetDir(1, (1<<14), GPIO_DIRECTION_OUTPUT);
	GPIO_SetDir(0, (1<<16), GPIO_DIRECTION_OUTPUT);
	GPIO_SetDir(1, (1<<13), GPIO_DIRECTION_OUTPUT);
	GPIO_SetDir(4, (1<<27), GPIO_DIRECTION_OUTPUT);
	GPIO_OutputValue(1, (1<<14), 0);
	GPIO_OutputValue(0, (1<<16), 0);
	GPIO_OutputValue(1, (1<<13), 0);
	GPIO_OutputValue(4, (1<<27), 0);

	while (1)
	{
		GPIO_OutputValue(1, (1<<14), 1);
		Delay(100);
		GPIO_OutputValue(1, (1<<14), 0);
		GPIO_OutputValue(0, (1<<16), 1);
		Delay(100);
		GPIO_OutputValue(0, (1<<16), 0);
		GPIO_OutputValue(1, (1<<13), 1);
		Delay(100);
		GPIO_OutputValue(1, (1<<13), 0);
		GPIO_OutputValue(4, (1<<27), 1);
		Delay(100);
		GPIO_OutputValue(4, (1<<27), 0);
	}

}


extern const unsigned long ac480x272Pic01[];

void lcd_test(void)
{

    unsigned long i,j;
    uint16_t value,value2;

	unsigned long *pDst = (unsigned long *)LCD_VRAM_BASE_ADDR;

	uint16_t *pBmp16;
    unsigned long *pDst16;

    for(j=0;j<5;j++)
    {    
        for( i = 0; (C_GLCD_H_SIZE * C_GLCD_V_SIZE) > i; i++)
        {
        	*pDst++ = color_table[j];        
        }
        Delay(100);

        pDst = (unsigned long *)LCD_VRAM_BASE_ADDR;
    }  
    
       
    pDst16 = (unsigned long *)LCD_VRAM_BASE_ADDR;

   	pBmp16 = (uint16_t *)ac480x272Pic01;
	for( i = 0; (C_GLCD_H_SIZE * C_GLCD_V_SIZE) > i; i++)
	{
  		*pDst16++ =*pBmp16+++(*pBmp16++<<7);
//            *pDst16++ = *pBmp16++;
	} 

}


/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		SysTick handler sub-routine (1ms)
 * @param[in]	None
 * @return 		None
 **********************************************************************/

void SysTick_Handler (void)
{
	SysTickCnt++;
}

/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief		Delay function
 * @param[in]	tick - number milisecond of delay time
 * @return 		None
 **********************************************************************/
void Delay (unsigned long tick)
{
	unsigned long systickcnt;

	systickcnt = SysTickCnt;
	while ((SysTickCnt - systickcnt) < tick);
}


/*****************************************************************************
**                            End Of File
******************************************************************************/

