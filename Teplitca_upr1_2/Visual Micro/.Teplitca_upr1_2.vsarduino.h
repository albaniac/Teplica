/* 
	Editor: http://www.visualmicro.com
	        visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
	        the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
	        all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
	        note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Arduino Mega w/ ATmega2560 (Mega 2560), Platform=avr, Package=arduino
*/

#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
#define __AVR_ATmega2560__
#define F_CPU 16000000L
#define ARDUINO 165
#define ARDUINO_AVR_MEGA2560
#define ARDUINO_ARCH_AVR
#define __cplusplus
#define __inline__
#define __asm__(x)
#define __extension__
//#define __ATTR_PURE__
//#define __ATTR_CONST__
#define __inline__
//#define __asm__ 
#define __volatile__
#define GCC_VERSION 40801
#define volatile(va_arg) 
#define _CONST
typedef void *__builtin_va_list;
#define __builtin_va_start
#define __builtin_va_end
//#define __DOXYGEN__
#define __attribute__(x)
#define NOINLINE __attribute__((noinline))
#define prog_void
#define PGM_VOID_P int
#ifndef __builtin_constant_p
#define __builtin_constant_p __attribute__((__const__))
#endif
#ifndef __builtin_strlen
#define __builtin_strlen  __attribute__((__const__))
#endif
#define NEW_H
/*
#ifndef __ATTR_CONST__
#define __ATTR_CONST__ __attribute__((__const__))
#endif

#ifndef __ATTR_MALLOC__
#define __ATTR_MALLOC__ __attribute__((__malloc__))
#endif

#ifndef __ATTR_NORETURN__
#define __ATTR_NORETURN__ __attribute__((__noreturn__))
#endif

#ifndef __ATTR_PURE__
#define __ATTR_PURE__ __attribute__((__pure__))
#endif            
*/
typedef unsigned char byte;
extern "C" void __cxa_pure_virtual() {;}



#include <arduino.h>
#include <pins_arduino.h> 
#undef F
#define F(string_literal) ((const PROGMEM char *)(string_literal))
#undef PSTR
#define PSTR(string_literal) ((const PROGMEM char *)(string_literal))
#undef cli
#define cli()
#define pgm_read_byte(address_short)
#define pgm_read_word(address_short)
#define pgm_read_word2(address_short)
#define digitalPinToPort(P)
#define digitalPinToBitMask(P) 
#define digitalPinToTimer(P)
#define analogInPinToBit(P)
#define portOutputRegister(P)
#define portInputRegister(P)
#define portModeRegister(P)

void dateTime(uint16_t* date, uint16_t* time);
void serial_print_date();
void clock_read();
void set_time();
void drawDisplay();
void drawMark(int h);
void drawSec(int s);
void drawMin(int m);
void drawHour(int h, int m);
void printDate();
void clearDate();
void AnalogClock();
void flash_time();
void serialEvent3();
void reset_klav();
void txt_pass_no_all();
void klav123();
void drawButtons1();
void updateStr(int val);
void waitForIt(int x1, int y1, int x2, int y2);
void control_command();
void draw_Glav_Menu();
void swichMenu();
void print_up();
void i2c_eeprom_write_byte( int deviceaddress, unsigned int eeaddress, byte data );
byte i2c_eeprom_read_byte( int deviceaddress, unsigned int eeaddress );
void i2c_test1();
void draw_Poliv_Menu();
void swich_Poliv_Menu();
void Draw_menu_Sector();
void print_set_avto();
void print_on_off();
void swich_menu_Sector();
void test_Time(int save_start_hour, int save_start_min, int save_stop_min, int save_stop_sec );
void save_Time(int save_start_hour, int save_start_min, int save_stop_min, int save_stop_sec );
void run_poliv_on_off(int temp_teplitca, int temp_sector, bool temp_on_off);
void poliv_ruchnoj();
void setup_compass();
void run_compass();
void test_display();
void test_mcp();
void test_input_mcp();
void setup_mcp();
void setup_regModbus();
//
//
void drawUpButton(int x, int y);
void drawDownButton(int x, int y);
void showDOW(byte dow);
int bin_to_bcd(int temp);
byte validateDate(byte d, byte m, word y);
byte validateDateForMonth(byte d, byte m, word y);
void setClock();
char uCase(char c);
void buttonWait(int x, int y);
byte calcDOW(byte d, byte m, int y);
void waitForTouchRelease();

#include <Teplitca_upr1_2.ino>
#include <setTimeDate.ino>
#include <utils.ino>
#endif