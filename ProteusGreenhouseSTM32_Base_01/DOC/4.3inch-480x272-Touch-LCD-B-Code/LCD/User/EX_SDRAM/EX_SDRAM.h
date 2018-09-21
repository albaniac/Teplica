

#ifndef __EX_SDRAM_h
#define __EX_SDRAM_h

#include "LPC177x_8x.h"
#include "lpc_types.h"
#include "system_LPC177x_8x.h" 

#define SDRAM_CONFIG_32BIT	// Õ‚Ω”2ø≈16ŒªSDRAM


#ifdef SDRAM_CONFIG_16BIT 

    #define SDRAM_SIZE               0x2000000	// 256Mbit

#elif defined SDRAM_CONFIG_32BIT	 //

    #define SDRAM_SIZE               0x4000000	// 512Mbit

#else
	error Wrong SDRAM config, check ex_sdram.h
#endif







#define SDRAM_BASE               0xA0000000	   /* CS0 */


void SDRAM_Init (void);

#endif
