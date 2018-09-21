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
#include "TouchPanel/TouchPanel.h"

/** @defgroup GPIO_LedBlinky	GPIO Blinky
 * @ingroup GPIO_Examples
 * @{
 */
/* Private variables ---------------------------------------------------------*/


/************************** PRIVATE DEFINITIONS *************************/

/************************** PRIVATE VARIABLES *************************/
/* SysTick Counter */
volatile unsigned long SysTickCnt;

/************************** PRIVATE FUNCTIONS *************************/
void SysTick_Handler (void);

void Delay (unsigned long tick);

void lcd_test(void);
uint32_t color_table[]={0x000000,0xFFFFFF,0x0000FF,0x00FF00,0xFF0000};

/* Private variables ---------------------------------------------------------*/
extern Matrix matrix ;
extern Coordinate  display ;

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

    debug_frmwrk_init();
    _printf("\r\n/* * * * * * * * LCD TouchPanel example program * * * * * * * */\r\n");
    _printf("CoreClock: %d\n",SystemCoreClock);   
    SDRAM_Init();
	/*Disable LCD controller*/
	GLCD_Ctrl (FALSE);

	/*Init LCD and copy picture in video RAM*/
	GLCD_Init (LogoPic.pPicStream, NULL);
   	/*Enable LCD*/
	GLCD_Ctrl (TRUE);

    touch_init();
    TouchPanel_Calibrate();

    while(1)
    {  
        getDisplayPoint(&display, Read_XTP2046(), &matrix );
        TP_DrawPoint(display.x,display.y);
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

