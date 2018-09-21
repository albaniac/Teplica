

#ifndef __EX_SDRAM_h
#define __EX_SDRAM_h

#include "LPC177x_8x.h"
#include "lpc_types.h"
#include "system_LPC177x_8x.h" 

#define SDRAM_CONFIG_32BIT	// Õ‚Ω”2ø≈16ŒªSDRAM

#ifdef SDRAM_CONFIG_16BIT 
#define SDRAM_SIZE               0x2000000	// 256Mbit
/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Blue           0xF800
#define Red            0x001F
#define Magenta        0xF81F
#define Green          0x07E0
#define Yellow         0x07FF

#elif defined SDRAM_CONFIG_32BIT	 //
/* LCD color */
#define White          0xFFFFFF
#define Black          0x000000
#define Blue           0xFF0000
#define Red            0x0000FF
#define Green          0x00FF00
#define SDRAM_SIZE               0x4000000	// 512Mbit
#else
	error Wrong SDRAM config, check ex_sdram.h
#endif







#define SDRAM_BASE               0xA0000000	   /* CS0 */


void SDRAM_Init (void);

#endif
