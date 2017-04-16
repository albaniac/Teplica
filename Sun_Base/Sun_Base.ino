/*
ArduinoNanoCabelTest.ino
Visual Studio 2010
VisualMicro

��������� ������������ ����������� �������.
������:               - 1.0
�����������:          - ��� "������"
�����:                - �������� �.�.
���� ������ �����:    - 01.09.2016�.
���� ��������� �����: - 01.03.2017�.
 
*/

#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <MsTimer2.h>
#include <modbus.h>
#include <modbusDevice.h>
#include <modbusRegBank.h>
#include <modbusSlave.h>
#include "MCP23017.h"
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <stdlib.h> // div, div_t
#include <UTFT.h>
#include <UTouch.h>
#include <UTFT_Buttons.h>
#include <Arduino.h>
#include <SdFat.h>
#include <SdFatUtil.h>

#define led_Green 12                                     // ��������� �� �������� ������ �������
#define led_Red   13                                     // ��������� �� �������� ������ �������

#define Chanal_A   A8                                    // ����� ������ � ����� ������������
#define Chanal_B   A9                                    // ����� ������ B ����� ������������
#define Rele1       8                                    // ���������� ���� 1
#define Rele2       9                                    // ���������� ���� 2
#define Rele3      10                                    // ���������� ���� 3

MCP23017 mcp_Out1;                                       // ���������� ������ ���������� MCP23017  4 A - Out, B - Out
MCP23017 mcp_Out2;                                       // ���������� ������ ���������� MCP23017  6 A - Out, B - Out

//+++++++++++++++++++ MODBUS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

modbusDevice regBank;
modbusSlave slave;

//+++++++++++++++++++++++ ��������� ������������ ��������� +++++++++++++++++++++++++++++++++++++
byte resistance          = 0x00;                        // ������������� 0x00..0xFF - 0��..100���

//+++++++++++++++++++++++++++++ ������� ������ +++++++++++++++++++++++++++++++++++++++
int deviceaddress        = 80;                          // ����� ���������� ������
unsigned int eeaddress   =  0;                          // ����� ������ ������
byte hi;                                                // ������� ���� ��� �������������� �����
byte low;                                               // ������� ���� ��� �������������� �����

//********************* ��������� �������� ***********************************
UTFT        myGLCD(ITDB32S,38,39,40,41);                // ������� 3.2"
UTouch        myTouch(6, 5, 4, 3, 2);                   // Standard Arduino Mega/Due shield            : 6,5,4,3,2
UTFT_Buttons  myButtons(&myGLCD, &myTouch);             // Finally we set up UTFT_Buttons :)

boolean default_colors = true;                          //
uint8_t menu_redraw_required = 0;
// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t Dingbats1_XL[];
extern uint8_t SmallSymbolFont[];

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

boolean connect_gnd1 = false;
boolean connect_gnd2 = false;
//byte N_block = 1;
boolean view_tab_run = false;

//+++++++++++++++++++++++++++ ��������� ����� +++++++++++++++++++++++++++++++
uint8_t second = 0;                                    //Initialization time
uint8_t minute = 10;
uint8_t hour   = 10;
uint8_t dow    = 2;
uint8_t day    = 15;
uint8_t month  = 3;
uint16_t year  = 16;
RTC_DS1307 RTC;                                       // define the Real Time Clock object

int clockCenterX               = 119;
int clockCenterY               = 119;
int oldsec                     = 0;
const char* str[]              = {"MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};
const char* str1[]             = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
const char* str_mon[]          = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
unsigned long wait_time        = 0;                               // ����� ������� �������
unsigned long wait_time_Old    = 0;                               // ����� ������� �������
int time_minute                = 5;                               // ����� ������� �������
int pin_cable                  = 0;                               // ���������� ������� ������
//------------------------------------------------------------------------------

const unsigned int adr_control_command    PROGMEM       = 40001;  // ����� �������� ������� �� ����������
const unsigned int adr_reg_count_err      PROGMEM       = 40002;  // ����� �������� ���� ������
//-------------------------------------------------------------------------------------------------------

//++++++++++++++++++++++++++++ ���������� ��� �������� ���������� +++++++++++++++++++++++++++++
int x, y, z;
char stCurrent[20]    = "";                                       // ���������� �������� ��������� ������
int stCurrentLen      = 0;                                        // ���������� �������� ����� ��������� ������
int stCurrentLen1     = 0;                                        // ���������� ���������� �������� ����� ��������� ������
char stLast[20]       = "";                                       // ������ � ��������� ������ ������.
int ret               = 0;                                        // ������� ���������� ��������
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------
//���������� ���������� ��� �������� � ����� ���� (������)
int but1, but2, but3, but4, but5, but6, but7, but8, but9, but10, butX, butY, butA, butB, butC, butD, but_m1, but_m2, but_m3, but_m4, but_m5, pressed_button;
//int kbut1, kbut2, kbut3, kbut4, kbut5, kbut6, kbut7, kbut8, kbut9, kbut0, kbut_save,kbut_clear, kbut_exit;
//int kbutA, kbutB, kbutC, kbutD, kbutE, kbutF;
int m2 = 1; // ���������� ������ ����

//------------------------------------------------------------------------------------------------------------------
// ���������� ���������� ��� �������� �������

char  txt_menu1_1[]            = "Tec\xA4 ""\x9F""a\x96""e\xA0\xAF N1";                                    // ���� ������ N 1
char  txt_menu1_2[]            = "Tec\xA4 ""\x9F""a\x96""e\xA0\xAF N2";                                    // ���� ������ N 2
char  txt_menu1_3[]            = "Tec\xA4 ""\x9F""a\x96""e\xA0\xAF N3";                                    // ���� ������ N 3
char  txt_menu1_4[]            = "Tec\xA4 ""\x9F""a\x96""e\xA0\xAF N4";                                    // ���� ������ N 4
char  txt_menu2_1[]            = "Tec""\xA4"" ""\x80""BC";                                                 // ���� ���                                           
char  txt_menu2_2[]            = "=========";                                                              // ================      
char  txt_menu2_3[]            = "=========";                                                              // ================      
char  txt_menu2_4[]            = "=========";                                                              // ================  
char  txt_menu3_1[]            = "Ko""\xA3\x9D\xAF"" ""\x9F""a""\x96""e""\xA0\xAC"" N1";                   // ����� ������ N 1
char  txt_menu3_2[]            = "Ko""\xA3\x9D\xAF"" ""\x9F""a""\x96""e""\xA0\xAC"" N2";                   // ����� ������ N 2
char  txt_menu3_3[]            = "Ko""\xA3\x9D\xAF"" ""\x9F""a""\x96""e""\xA0\xAC"" N3";                   // ����� ������ N 3 
char  txt_menu3_4[]            = "Ko""\xA3\x9D\xAF"" ""\x9F""a""\x96""e""\xA0\xAC"" N4";                   // ����� ������ N 4
char  txt_menu4_1[]            = "Ta""\x96\xA0\x9D\xA6""a coe""\x99"".";                                   // ������� ����.
char  txt_menu4_2[]            = "Ko""\xA3\x9D\xAF"" ""\x9F""a""\x96""e""\xA0\xAF";                        // ����� ������
char  txt_menu4_3[]            = "\x85""a""\x98""py""\x9C"".\xA4""a""\x96\xA0\x9D\xA6";                    // ������. �����.
char  txt_menu4_4[]            = "Tec""\xA4"" pa""\x9C\xAA""e""\xA1""o""\x97";                             // ���� �������� 
char  txt_menu5_1[]            = "=========";                                                              // ================  
char  txt_menu5_2[]            = "=========";                                                              // ================  
char  txt_menu5_3[]            = "=========";                                                              // ================  
char  txt_menu5_4[]            = "Bpe""\xA1\xAF"" ""\xA3""poc""\xA4""o""\xAF";                             // ����� �������

const char  txt_head_instr[]        PROGMEM  = "=========";                                                              // ================  
const char  txt_head_disp[]         PROGMEM  = "=========";                                                              // ================  
const char  txt_info1[]             PROGMEM  = "Tec\xA4 ""\x9F""a\x96""e\xA0""e\x9E";                                    // ���� �������
const char  txt_info2[]             PROGMEM  = "=========";                                                              // ================  
const char  txt_info3[]             PROGMEM  = "Tec""\xA4"" o""\x96""y""\xA7"". ""\xA4""a""\x96\xA0\x9D\xA6""ax ";       // ���� �� ����. �������� 
const char  txt_info4[]             PROGMEM  = "Hac\xA4po\x9E\x9F""a c\x9D""c\xA4""e\xA1\xAB";                           // ��������� ������� 
const char  txt_info5[]             PROGMEM  = "=========";                                                              // ================  
const char  txt_MTT[]               PROGMEM  = "=========";                                                              // ================  
const char  txt_botton_otmena[]     PROGMEM  = "O""\xA4\xA1""e""\xA2""a";                                                // "������"
const char  txt_botton_vvod[]       PROGMEM  = "B\x97o\x99 ";                                                            // ����
const char  txt_botton_ret[]        PROGMEM  = "B""\xAB""x";                                                             // "���"
const char  txt_system_clear3[]     PROGMEM  = " ";                                                                      //
const char  txt9[]                  PROGMEM  = "=========";                                                              // ================  
const char  txt10[]                 PROGMEM  = "=========";                                                              // ================  
const char  txt_time_wait[]         PROGMEM  = "\xA1\x9D\xA2"".""\x97""pe""\xA1\xAF"" ""\xA3""poc""\xA4""o""\xAF";       //  ���. ����� �������
const char  txt_info29[]            PROGMEM  = "Stop->PUSH Disp";
const char  txt_info30[]            PROGMEM  = " ";
const char  txt_test_all[]          PROGMEM  = "Tec""\xA4"" ""\x97""cex pa""\x9C\xAA""e""\xA1""o""\x97";                 // ���� ���� ��������
const char  txt_test_all_exit1[]    PROGMEM  = "\x82\xA0\xAF"" ""\x97\xAB""xo""\x99""a";                                 // ��� ������
const char  txt_test_all_exit2[]    PROGMEM  = "\xA3""p""\x9D\x9F""oc""\xA2\x9D""c""\xAC"" ""\x9F"" ""\xAD\x9F""pa""\xA2""y";  // ���������� � ������
const char  txt_test_end[]          PROGMEM  = "\x85""a""\x97""ep""\xA8\x9D\xA4\xAC";                                    // ���������
const char  txt_test_repeat[]       PROGMEM  = "\x89""o""\x97\xA4""op""\x9D\xA4\xAC";                                    // ���������
const char  txt_error_connect1[]    PROGMEM  = "O""\x8E\x86\x80""KA";                                                    // ������
const char  txt_error_connect2[]    PROGMEM  = "\xA3""o""\x99\x9F\xA0\xAE\xA7""e""\xA2\x9D\xAF"" ""\x9F""a""\x96""e""\xA0\xAF"; //����������� ������
const char  txt_error_connect3[]    PROGMEM  = "O""\xA8\x9D\x96""o""\x9F"" ""\xA2""e""\xA4";                             // ������ ���
const char  txt_error_connect4[]    PROGMEM  = "O""\xA8\x9D\x96""o""\x9F"" -         ";                                  // ������  -
const char  txt__connect1[]         PROGMEM  = "O""\x96\xA2""apy""\x9B""e""\xA2"" ""\x9F""a""\x96""e""\xA0\xAC"" N1";    // ��������� ������ N1
const char  txt__connect2[]         PROGMEM  = "O""\x96\xA2""apy""\x9B""e""\xA2"" ""\x9F""a""\x96""e""\xA0\xAC"" N2";    // ��������� ������ N2
const char  txt__connect3[]         PROGMEM  = "O""\x96\xA2""apy""\x9B""e""\xA2"" ""\x9F""a""\x96""e""\xA0\xAC"" N3";    // ��������� ������ N3
const char  txt__connect4[]         PROGMEM  = "O""\x96\xA2""apy""\x9B""e""\xA2"" ""\x9F""a""\x96""e""\xA0\xAC"" N4";    // ��������� ������ N4
const char  txt__test_end[]         PROGMEM  = "TECT ""\x85""A""KOH""\x8D""EH";                                          // ���� ��������
const char  txt__panel[]            PROGMEM  = "=========";                                                              // ================  
const char  txt__panel0[]           PROGMEM  = "                     ";                                                  //
const char  txt__disp[]             PROGMEM  = "=========";                                                              // ================  
const char  txt__instr[]            PROGMEM  = "=========";                                                              // ================  
const char  txt__MTT[]              PROGMEM  = "=========";                                                              // ================  
const char  txt__disp_connect[]     PROGMEM  = "=========";                                                              // ================  
const char  txt__disp_disconnect[]  PROGMEM  = "=========";                                                              // ================  
const char  txt__instr_connect[]    PROGMEM  = "=========";                                                              // ================  
const char  txt__instr_disconnect[] PROGMEM  = "=========";                                                              // ================  
const char  txt__mtt_disconnect[]   PROGMEM  = "=========";                                                              // ================  
const char  txt__cont1_connect[]    PROGMEM  = "Ko""\xA2\xA4"". N1 - O""\x9F";                                            // ����. N1 - ��
const char  txt__cont2_connect[]    PROGMEM  = "Ko""\xA2\xA4"". N2 - O""\x9F";                                            // ����. N2 - ��
const char  txt__cont3_connect[]    PROGMEM  = "Ko""\xA2\xA4"". N3 - O""\x9F";                                            // ����. N3 - ��
const char  txt__cont4_connect[]    PROGMEM  = "Ko""\xA2\xA4"". N4 - O""\x9F";                                            // ����. N4 - ��
const char  txt__cont5_connect[]    PROGMEM  = "Ko""\xA2\xA4"". N5 - O""\x9F";                                            // ����. N5 - ��
const char  txt__cont6_connect[]    PROGMEM  = "Ko""\xA2\xA4"". N6 - O""\x9F";                                            // ����. N6 - ��
const char  txt__cont7_connect[]    PROGMEM  = "Ko""\xA2\xA4"". N7 - O""\x9F";                                            // ����. N7 - ��
const char  txt__cont8_connect[]    PROGMEM  = "Ko""\xA2\xA4"". N8 - O""\x9F";                                            // ����. N8 - ��
const char  txt__cont9_connect[]    PROGMEM  = "Ko""\xA2\xA4"". N9 - O""\x9F";                                            // ����. N9 - ��
const char  txt__clear2[]           PROGMEM  = " ";                                                                       //
const char  txt__cont1_disconnect[] PROGMEM  = "Ko""\xA2\xA4"". N1 - He""\xA4""!";                                        // ����. N1 - ���!
const char  txt__cont2_disconnect[] PROGMEM  = "Ko""\xA2\xA4"". N2 - He""\xA4""!";                                        // ����. N2 - ���!
const char  txt__cont3_disconnect[] PROGMEM  = "Ko""\xA2\xA4"". N3 - He""\xA4""!";                                        // ����. N3 - ���!
const char  txt__cont4_disconnect[] PROGMEM  = "Ko""\xA2\xA4"". N4 - He""\xA4""!";                                        // ����. N4 - ���!
const char  txt__cont5_disconnect[] PROGMEM  = "Ko""\xA2\xA4"". N5 - He""\xA4""!";                                        // ����. N5 - ���!
const char  txt__cont6_disconnect[] PROGMEM  = "Ko""\xA2\xA4"". N6 - He""\xA4""!";                                        // ����. N6 - ���!
const char  txt__cont7_disconnect[] PROGMEM  = "Ko""\xA2\xA4"". N7 - He""\xA4""!";                                        // ����. N7 - ���!
const char  txt__cont8_disconnect[] PROGMEM  = "Ko""\xA2\xA4"". N8 - He""\xA4""!";                                        // ����. N8 - ���!
const char  txt__cont9_disconnect[] PROGMEM  = "Ko""\xA2\xA4"". N9 - He""\xA4""!";                                        // ����. N9 - ���!
const char  txt__test_cabel_soft[]  PROGMEM  = "Tec""\xA4"" ""\x9F""o""\xA3\x9D\xAF"" ""\x9F""a""\x96""e""\xA0\xAF""N";   // ���� ����� ������ N

char buffer[40];

const char* const table_message[] PROGMEM =
{
  txt_head_instr,          // 0 "=========";                                                                // ================  
  txt_head_disp,           // 1 "=========";                                                                // ================  
  txt_info1,               // 2 "Tec\xA4 ""\x9F""a\x96""e\xA0""e\x9E";                                      // ���� �������
  txt_info2,               // 3 "=========";                                                                // ================  
  txt_info3,               // 4 "Tec""\xA4"" ""\xA3""o o""\x96""y""\xA7"". ""\xA4""a""\x96\xA0\x9D\xA6""ax "; // ���� �� ����. �������� 
  txt_info4,               // 5 "Hac\xA4po\x9E\x9F""a c\x9D""c\xA4""e\xA1\xAB";                             // ��������� �������   
  txt_info5,               // 6 "=========";                                                                // ================  
  txt_MTT,                 // 7 "=========";                                                                // ================  
  txt_botton_otmena,       // 8 " ";                                                                        //
  txt_botton_vvod,         // 9 " ";                                                                        //
  txt_botton_ret,          // 10 ""B""\xAB""x" ";                                                           //  ���
  txt_system_clear3,       // 11 " ";                                                                       //
  txt9,                    // 12 "B\x97o\x99";                                                              // ����
  txt10,                   // 13 "O""\xA4\xA1""e""\xA2""a";                                                 // "������"
  txt_time_wait,           // 14 "\xA1\x9D\xA2"".""\x97""pe""\xA1\xAF"" ""\xA3""poc""\xA4""o""\xAF";        //  ���. ����� �������
  txt_info29,              // 15 "Stop->PUSH Disp";
  txt_info30,              // 16 " ";
  txt_test_all,            // 17 "Tec""\xA4"" ""\x97""cex pa""\x9C\xAA""e""\xA1""o""\x97";                  // ���� ���� ��������
  txt_test_all_exit1,      // 18 "\x82\xA0\xAF"" ""\x97\xAB""xo""\x99""a";                                  // ��� ������
  txt_test_all_exit2,      // 19 "\xA3""p""\x9D\x9F""oc""\xA2\x9D""c""\xAC"" ""\x9F"" ""\xAD\x9F""pa""\xA2""y";  // ���������� � ������
  txt_test_end,            // 20 "\x85""a""\x97""ep""\xA8\x9D\xA4\xAC";                                     // ���������
  txt_test_repeat,         // 21 "\x89""o""\x97\xA4""op""\x9D\xA4\xAC";                                     // ���������
  txt_error_connect1,      // 22 "O""\x8E\x86\x80""KA";                                                     // ������
  txt_error_connect2,      // 23 "\xA3""o""\x99\x9F\xA0\xAE\xA7""e""\xA2\x9D\xAF"" ""\x9F""a""\x96""e""\xA0\xAF"; //����������� ������
  txt_error_connect3,      // 24 "O""\xA8\x9D\x96""o""\x9F"" ""\xA2""e""\xA4";                              // ������ ���
  txt_error_connect4,      // 25 "O""\xA8\x9D\x96""o""\x9F"" -         ";                                   // ������  -
  txt__connect1,           // 26 "O""\x96\xA2""apy""\x9B""e""\xA2"" ""\x9F""a""\x96""e""\xA0\xAC"" N1";     // ��������� ������ N1
  txt__connect2,           // 27 "O""\x96\xA2""apy""\x9B""e""\xA2"" ""\x9F""a""\x96""e""\xA0\xAC"" N2";     // ��������� ������ N2
  txt__connect3,           // 28 "O""\x96\xA2""apy""\x9B""e""\xA2"" ""\x9F""a""\x96""e""\xA0\xAC"" N3";     // ��������� ������ N3
  txt__connect4,           // 29 "O""\x96\xA2""apy""\x9B""e""\xA2"" ""\x9F""a""\x96""e""\xA0\xAC"" N4";     // ��������� ������ N4
  txt__test_end,           // 30 "TECT ""\x85""A""KOH""\x8D""EH";                                           // ���� ��������
  txt__panel,              // 31 "=========";                                                               // ================  
  txt__panel0,             // 32 "                          ";                                              //
  txt__disp,               // 33 "=========";                                                               // ================  
  txt__instr,              // 34 "=========";                                                               // ================  
  txt__MTT,                // 35 "=========";                                                               // ================  
  txt__disp_connect,       // 36 "=========";                                                               // ================  
  txt__disp_disconnect,    // 37 "=========";                                                               // ================  
  txt__instr_connect,      // 38 "=========";                                                               // ================  
  txt__instr_disconnect,   // 39 "=========";                                                               // ================  
  txt__mtt_disconnect,     // 40 " ";                                                                       //
  txt__cont1_connect,      // 41 "Ko""\xA2\xA4"". N1 - O""\x9F";                                            // ����. N1 - ��
  txt__cont2_connect,      // 42 "Ko""\xA2\xA4"". N2 - O""\x9F";                                            // ����. N2 - ��
  txt__cont3_connect,      // 43 "Ko""\xA2\xA4"". N3 - O""\x9F";                                            // ����. N3 - ��
  txt__cont4_connect,      // 44 "Ko""\xA2\xA4"". N4 - O""\x9F";                                            // ����. N4 - ��
  txt__cont5_connect,      // 45 "Ko""\xA2\xA4"". N5 - O""\x9F";                                            // ����. N5 - ��
  txt__cont6_connect,      // 46 "Ko""\xA2\xA4"". N6 - O""\x9F";                                            // ����. N6 - ��
  txt__cont7_connect,      // 47 "Ko""\xA2\xA4"". N7 - O""\x9F";                                            // ����. N7 - ��
  txt__cont8_connect,      // 48 "Ko""\xA2\xA4"". N8 - O""\x9F";                                            // ����. N8 - ��
  txt__cont9_connect,      // 49 "Ko""\xA2\xA4"". N9 - O""\x9F";                                            // ����. N9 - ��
  txt__clear2,             // 50 " ";                                                                       //
  txt__cont1_disconnect,   // 51 "Ko""\xA2\xA4"". N1 - He""\xA4""!";                                        // ����. N1 - ���!
  txt__cont2_disconnect,   // 52 "Ko""\xA2\xA4"". N2 - He""\xA4""!";                                        // ����. N2 - ���!
  txt__cont3_disconnect,   // 53 "Ko""\xA2\xA4"". N3 - He""\xA4""!";                                        // ����. N3 - ���!
  txt__cont4_disconnect,   // 54 "Ko""\xA2\xA4"". N4 - He""\xA4""!";                                        // ����. N4 - ���!
  txt__cont5_disconnect,   // 55 "Ko""\xA2\xA4"". N5 - He""\xA4""!";                                        // ����. N5 - ���!
  txt__cont6_disconnect,   // 56 "Ko""\xA2\xA4"". N6 - He""\xA4""!";                                        // ����. N6 - ���!
  txt__cont7_disconnect,   // 57 "Ko""\xA2\xA4"". N7 - He""\xA4""!";                                        // ����. N7 - ���!
  txt__cont8_disconnect,   // 58 "Ko""\xA2\xA4"". N8 - He""\xA4""!";                                        // ����. N8 - ���!
  txt__cont9_disconnect,   // 59 "Ko""\xA2\xA4"". N9 - He""\xA4""!";                                        // ����. N9 - ���!
  txt__test_cabel_soft     // 60 "Tec""\xA4"" ""\x9F""o""\xA3\x9D\xAF"" ""\x9F""a""\x96""e""\xA0\xAF"" N";  // ���� ����� ������ N
};

byte   temp_buffer[40] ;                                                                                    // ����� �������� ��������� ����������

const byte connektN1_default[]    PROGMEM  = { 20,
											   1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,                                             // ������ �
											   1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
											   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
											   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                                        // ������ B
											   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1                                                         // 1- ���������� ����, 0- ���������� ���
											 }; // 20 x 5 �����
const byte connektN2_default[]    PROGMEM  = { 26,
											   1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,                    // ������ �
											   1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,                    // ������ B
											   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
											   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
											   1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1                                      // 1- ���������� ����, 0- ���������� ���
											 }; // 26 x 5 �����
const byte connektN3_default[]    PROGMEM  = { 39,
											    1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39, // ������ �
											   20,22,24,26,28,30,32,34,36,39, 3, 5, 7, 9,11,13,15,17,19,21,23,25,27,29,31,33,35,37, 2, 4, 6, 8,10,12,14,16,18, 0,20, // ������ B
											    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
											    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
											    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1    // 1- ���������� ����, 0- ���������� ���
											 }; // 39 x 5 �����
const byte connektN4_default[]    PROGMEM  = { 32,
											   1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, // ������ �
											   1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, // ������ B
											   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
											   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
											   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1                     // 1- ���������� ����, 0- ���������� ���
											 }; // 32 x 2 �����

const byte connekBVS_default[]    PROGMEM = { 24,
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, // ������ � 
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, // ������ B
0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
1, 1, 1, 1, 1, 1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1                    // 1- ���������� ����, 0- ���������� ���
}; // 24 x 2 �����

//++++++++++++++++++ ������� � 1 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ����������� ������ ������������ � ���������  set_adr_EEPROM()
unsigned int adr_memN1_1 = 0;                       // ��������� ����� ������ ������� ������������ ��������� �������� �1�, �1�
unsigned int adr_memN1_2 = 0;                       // ��������� ����� ������ ������� ������������ ��������� �������� �2�, �2�
unsigned int adr_memN1_3 = 0;                       // ��������� ����� ������ ������� ������������ ��������� �������� �3�, �3�
unsigned int adr_memN1_4 = 0;                       // ��������� ����� ������ ������� ������������ ��������� �������� �4�, �4�

//++++++++++++++++++ ������� � 2 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned int adr_memN1_5 = 0;                       // ��������� ����� ������ ������� ������������ ��������� �������� �1�, �1�
													
// ++++++++++++++++++++++++++++++ ������� ��� ��������� ��������� ������ ++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned int adr_memN2_1 = 0;                       // ��������� ����� ������ ������� ������������ ��������� �������� �1�, �2�
unsigned int adr_memN2_2 = 0;                       // ��������� ����� ������ ������� ������������ ��������� �������� �2�, �2�
unsigned int adr_memN2_3 = 0;                       // ��������� ����� ������ ������� ������������ ��������� �������� �3�, �3�
unsigned int adr_memN2_4 = 0;                       // ��������� ����� ������ ������� ������������ ��������� �������� �4�, �4�

//==========================================================================================================================

void serial_print_date()                           // ������ ���� � �������
{
  DateTime now = RTC.now();
  Serial.print(now.day(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.year(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.print("  ");
  Serial.println(str1[now.dayOfWeek() - 1]);
}
void clock_read()
{
  DateTime now = RTC.now();
  second = now.second();
  minute = now.minute();
  hour   = now.hour();
  dow    = now.dayOfWeek();
  day    = now.day();
  month  = now.month();
  year   = now.year();
}

void set_time()
{
  RTC.adjust(DateTime(__DATE__, __TIME__));
  DateTime now = RTC.now();
  second = now.second();       //Initialization time
  minute = now.minute();
  hour   = now.hour();
  day    = now.day();
  day++;
  if (day > 31)day = 1;
  month  = now.month();
  year   = now.year();
  DateTime set_time = DateTime(year, month, day, hour, minute, second); // ������� ������ � ������� � ������ "set_time"
  RTC.adjust(set_time);
}
void i2c_eeprom_write_byte( int deviceaddress, unsigned int eeaddress, byte data )
{
  int rdata = data;
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8)); // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(rdata);
  Wire.endTransmission();
  delay(10);
}
byte i2c_eeprom_read_byte( int deviceaddress, unsigned int eeaddress ) {
  byte rdata = 0xFF;
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8)); // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress, 1);
  if (Wire.available()) rdata = Wire.read();
  return rdata;
}
void i2c_eeprom_read_buffer( int deviceaddress, unsigned int eeaddress, byte *buffer, int length )
{

  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8)); // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress, length);
  int c = 0;
  for ( c = 0; c < length; c++ )
	if (Wire.available()) buffer[c] = Wire.read();

}
void i2c_eeprom_write_page( int deviceaddress, unsigned int eeaddresspage, byte* data, byte length )
{

  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddresspage >> 8)); // MSB
  Wire.write((int)(eeaddresspage & 0xFF)); // LSB
  byte c;
  for ( c = 0; c < length; c++)
	Wire.write(data[c]);
  Wire.endTransmission();

}

void drawDisplay()
{
  // Clear screen
  myGLCD.clrScr();

  // Draw Clockface
  myGLCD.setColor(0, 0, 255);
  myGLCD.setBackColor(0, 0, 0);
  for (int i = 0; i < 5; i++)
  {
	myGLCD.drawCircle(clockCenterX, clockCenterY, 119 - i);
  }
  for (int i = 0; i < 5; i++)
  {
	myGLCD.drawCircle(clockCenterX, clockCenterY, i);
  }

  myGLCD.setColor(192, 192, 255);
  myGLCD.print("3", clockCenterX + 92, clockCenterY - 8);
  myGLCD.print("6", clockCenterX - 8, clockCenterY + 95);
  myGLCD.print("9", clockCenterX - 109, clockCenterY - 8);
  myGLCD.print("12", clockCenterX - 16, clockCenterY - 109);
  for (int i = 0; i < 12; i++)
  {
	if ((i % 3) != 0)
	  drawMark(i);
  }
  clock_read();
  drawMin(minute);
  drawHour(hour, minute);
  drawSec(second);
  oldsec = second;

  // Draw calendar
  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRoundRect(240, 0, 319, 85);
  myGLCD.setColor(0, 0, 0);
  for (int i = 0; i < 7; i++)
  {
	myGLCD.drawLine(249 + (i * 10), 0, 248 + (i * 10), 3);
	myGLCD.drawLine(250 + (i * 10), 0, 249 + (i * 10), 3);
	myGLCD.drawLine(251 + (i * 10), 0, 250 + (i * 10), 3);
  }

  // Draw SET button
  myGLCD.setColor(64, 64, 128);
  myGLCD.fillRoundRect(260, 200, 319, 239);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect(260, 200, 319, 239);
  myGLCD.setBackColor(64, 64, 128);
  myGLCD.print("SET", 266, 212);
  myGLCD.setBackColor(0, 0, 0);

  /* myGLCD.setColor(64, 64, 128);
	myGLCD.fillRoundRect(260, 140, 319, 180);
	myGLCD.setColor(255, 255, 255);
	myGLCD.drawRoundRect(260, 140, 319, 180);
	myGLCD.setBackColor(64, 64, 128);
	myGLCD.print("RET", 266, 150);
	myGLCD.setBackColor(0, 0, 0);*/

}
void drawMark(int h)
{
  float x1, y1, x2, y2;

  h = h * 30;
  h = h + 270;

  x1 = 110 * cos(h * 0.0175);
  y1 = 110 * sin(h * 0.0175);
  x2 = 100 * cos(h * 0.0175);
  y2 = 100 * sin(h * 0.0175);

  myGLCD.drawLine(x1 + clockCenterX, y1 + clockCenterY, x2 + clockCenterX, y2 + clockCenterY);
}
void drawSec(int s)
{
  float x1, y1, x2, y2;
  int ps = s - 1;

  myGLCD.setColor(0, 0, 0);
  if (ps == -1)
	ps = 59;
  ps = ps * 6;
  ps = ps + 270;

  x1 = 95 * cos(ps * 0.0175);
  y1 = 95 * sin(ps * 0.0175);
  x2 = 80 * cos(ps * 0.0175);
  y2 = 80 * sin(ps * 0.0175);

  myGLCD.drawLine(x1 + clockCenterX, y1 + clockCenterY, x2 + clockCenterX, y2 + clockCenterY);

  myGLCD.setColor(255, 0, 0);
  s = s * 6;
  s = s + 270;

  x1 = 95 * cos(s * 0.0175);
  y1 = 95 * sin(s * 0.0175);
  x2 = 80 * cos(s * 0.0175);
  y2 = 80 * sin(s * 0.0175);

  myGLCD.drawLine(x1 + clockCenterX, y1 + clockCenterY, x2 + clockCenterX, y2 + clockCenterY);
}
void drawMin(int m)
{
  float x1, y1, x2, y2, x3, y3, x4, y4;
  int pm = m - 1;

  myGLCD.setColor(0, 0, 0);
  if (pm == -1)
	pm = 59;
  pm = pm * 6;
  pm = pm + 270;

  x1 = 80 * cos(pm * 0.0175);
  y1 = 80 * sin(pm * 0.0175);
  x2 = 5 * cos(pm * 0.0175);
  y2 = 5 * sin(pm * 0.0175);
  x3 = 30 * cos((pm + 4) * 0.0175);
  y3 = 30 * sin((pm + 4) * 0.0175);
  x4 = 30 * cos((pm - 4) * 0.0175);
  y4 = 30 * sin((pm - 4) * 0.0175);

  myGLCD.drawLine(x1 + clockCenterX, y1 + clockCenterY, x3 + clockCenterX, y3 + clockCenterY);
  myGLCD.drawLine(x3 + clockCenterX, y3 + clockCenterY, x2 + clockCenterX, y2 + clockCenterY);
  myGLCD.drawLine(x2 + clockCenterX, y2 + clockCenterY, x4 + clockCenterX, y4 + clockCenterY);
  myGLCD.drawLine(x4 + clockCenterX, y4 + clockCenterY, x1 + clockCenterX, y1 + clockCenterY);

  myGLCD.setColor(0, 255, 0);
  m = m * 6;
  m = m + 270;

  x1 = 80 * cos(m * 0.0175);
  y1 = 80 * sin(m * 0.0175);
  x2 = 5 * cos(m * 0.0175);
  y2 = 5 * sin(m * 0.0175);
  x3 = 30 * cos((m + 4) * 0.0175);
  y3 = 30 * sin((m + 4) * 0.0175);
  x4 = 30 * cos((m - 4) * 0.0175);
  y4 = 30 * sin((m - 4) * 0.0175);

  myGLCD.drawLine(x1 + clockCenterX, y1 + clockCenterY, x3 + clockCenterX, y3 + clockCenterY);
  myGLCD.drawLine(x3 + clockCenterX, y3 + clockCenterY, x2 + clockCenterX, y2 + clockCenterY);
  myGLCD.drawLine(x2 + clockCenterX, y2 + clockCenterY, x4 + clockCenterX, y4 + clockCenterY);
  myGLCD.drawLine(x4 + clockCenterX, y4 + clockCenterY, x1 + clockCenterX, y1 + clockCenterY);
}
void drawHour(int h, int m)
{
  float x1, y1, x2, y2, x3, y3, x4, y4;
  int ph = h;

  myGLCD.setColor(0, 0, 0);
  if (m == 0)
  {
	ph = ((ph - 1) * 30) + ((m + 59) / 2);
  }
  else
  {
	ph = (ph * 30) + ((m - 1) / 2);
  }
  ph = ph + 270;

  x1 = 60 * cos(ph * 0.0175);
  y1 = 60 * sin(ph * 0.0175);
  x2 = 5 * cos(ph * 0.0175);
  y2 = 5 * sin(ph * 0.0175);
  x3 = 20 * cos((ph + 5) * 0.0175);
  y3 = 20 * sin((ph + 5) * 0.0175);
  x4 = 20 * cos((ph - 5) * 0.0175);
  y4 = 20 * sin((ph - 5) * 0.0175);

  myGLCD.drawLine(x1 + clockCenterX, y1 + clockCenterY, x3 + clockCenterX, y3 + clockCenterY);
  myGLCD.drawLine(x3 + clockCenterX, y3 + clockCenterY, x2 + clockCenterX, y2 + clockCenterY);
  myGLCD.drawLine(x2 + clockCenterX, y2 + clockCenterY, x4 + clockCenterX, y4 + clockCenterY);
  myGLCD.drawLine(x4 + clockCenterX, y4 + clockCenterY, x1 + clockCenterX, y1 + clockCenterY);

  myGLCD.setColor(255, 255, 0);
  h = (h * 30) + (m / 2);
  h = h + 270;

  x1 = 60 * cos(h * 0.0175);
  y1 = 60 * sin(h * 0.0175);
  x2 = 5 * cos(h * 0.0175);
  y2 = 5 * sin(h * 0.0175);
  x3 = 20 * cos((h + 5) * 0.0175);
  y3 = 20 * sin((h + 5) * 0.0175);
  x4 = 20 * cos((h - 5) * 0.0175);
  y4 = 20 * sin((h - 5) * 0.0175);

  myGLCD.drawLine(x1 + clockCenterX, y1 + clockCenterY, x3 + clockCenterX, y3 + clockCenterY);
  myGLCD.drawLine(x3 + clockCenterX, y3 + clockCenterY, x2 + clockCenterX, y2 + clockCenterY);
  myGLCD.drawLine(x2 + clockCenterX, y2 + clockCenterY, x4 + clockCenterX, y4 + clockCenterY);
  myGLCD.drawLine(x4 + clockCenterX, y4 + clockCenterY, x1 + clockCenterX, y1 + clockCenterY);
}
void printDate()
{
  clock_read();
  myGLCD.setFont(BigFont);
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(255, 255, 255);

  myGLCD.print(str[dow - 1], 256, 8);
  if (day < 10)
	myGLCD.printNumI(day, 272, 28);
  else
	myGLCD.printNumI(day, 264, 28);

  myGLCD.print(str_mon[month - 1], 256, 48);
  myGLCD.printNumI(year, 248, 65);
}
void clearDate()
{
  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRect(248, 8, 312, 81);
}
void AnalogClock()
{
  int x, y;
  drawDisplay();
  printDate();
  while (true)
  {
	if (oldsec != second)
	{
	  if ((second == 0) && (minute == 0) && (hour == 0))
	  {
		clearDate();
		printDate();
	  }
	  if (second == 0)
	  {
		drawMin(minute);
		drawHour(hour, minute);
	  }
	  drawSec(second);
	  oldsec = second;
	  wait_time_Old =  millis();
	}

	if (myTouch.dataAvailable())
	{
	  myTouch.read();
	  x = myTouch.getX();
	  y = myTouch.getY();
	  if (((y >= 200) && (y <= 239)) && ((x >= 260) && (x <= 319))) //��������� �����
	  {
		myGLCD.setColor (255, 0, 0);
		myGLCD.drawRoundRect(260, 200, 319, 239);
		setClock();
	  }

	  if (((y >= 1) && (y <= 239)) && ((x >= 1) && (x <= 260))) //�������
	  {
		myGLCD.clrScr();
		myGLCD.setFont(BigFont);
		break;
	  }
	  if (((y >= 1) && (y <= 180)) && ((x >= 260) && (x <= 319))) //�������
	  {
		myGLCD.clrScr();
		myGLCD.setFont(BigFont);
		break;
	  }
	}
	delay(10);
	clock_read();
  }
}

void flash_time()                                              // ��������� ���������� ����������
{
  // PORTB = B00000000; // ��� 12 ��������� � ��������� LOW
  slave.run();
  // PORTB = B01000000; // ��� 12 ��������� � ��������� HIGH
}
void serialEvent3()
{
  control_command();
}

void reset_klav()
{
  myGLCD.clrScr();
  myButtons.deleteAllButtons();
  but1 = myButtons.addButton( 10,  20, 250,  35, txt_menu5_1);
  but2 = myButtons.addButton( 10,  65, 250,  35, txt_menu5_2);
  but3 = myButtons.addButton( 10, 110, 250,  35, txt_menu5_3);
  but4 = myButtons.addButton( 10, 155, 250,  35, txt_menu5_4);
  butX = myButtons.addButton(279, 199,  40,  40, "W", BUTTON_SYMBOL); // ������ ����
  but_m1 = myButtons.addButton(  10, 199, 45,  40, "1");
  but_m2 = myButtons.addButton(  61, 199, 45,  40, "2");
  but_m3 = myButtons.addButton(  112, 199, 45,  40, "3");
  but_m4 = myButtons.addButton(  163, 199, 45,  40, "4");
  but_m5 = myButtons.addButton(  214, 199, 45,  40, "5");

}

void klav123() // ���� ������ � �������� ����������
{
  ret = 0;

  while (true)
  {
	if (myTouch.dataAvailable())
	{
	  myTouch.read();
	  x = myTouch.getX();
	  y = myTouch.getY();

	  if ((y >= 10) && (y <= 60)) // Upper row
	  {
		if ((x >= 10) && (x <= 60)) // Button: 1
		{
		  waitForIt(10, 10, 60, 60);
		  updateStr('1');
		}
		if ((x >= 70) && (x <= 120)) // Button: 2
		{
		  waitForIt(70, 10, 120, 60);
		  updateStr('2');
		}
		if ((x >= 130) && (x <= 180)) // Button: 3
		{
		  waitForIt(130, 10, 180, 60);
		  updateStr('3');
		}
		if ((x >= 190) && (x <= 240)) // Button: 4
		{
		  waitForIt(190, 10, 240, 60);
		  updateStr('4');
		}
		if ((x >= 250) && (x <= 300)) // Button: 5
		{
		  waitForIt(250, 10, 300, 60);
		  updateStr('5');
		}
	  }

	  if ((y >= 70) && (y <= 120)) // Center row
	  {
		if ((x >= 10) && (x <= 60)) // Button: 6
		{
		  waitForIt(10, 70, 60, 120);
		  updateStr('6');
		}
		if ((x >= 70) && (x <= 120)) // Button: 7
		{
		  waitForIt(70, 70, 120, 120);
		  updateStr('7');
		}
		if ((x >= 130) && (x <= 180)) // Button: 8
		{
		  waitForIt(130, 70, 180, 120);
		  updateStr('8');
		}
		if ((x >= 190) && (x <= 240)) // Button: 9
		{
		  waitForIt(190, 70, 240, 120);
		  updateStr('9');
		}
		if ((x >= 250) && (x <= 300)) // Button: 0
		{
		  waitForIt(250, 70, 300, 120);
		  updateStr('0');
		}
	  }
	  if ((y >= 130) && (y <= 180)) // Upper row
	  {
		if ((x >= 10) && (x <= 130)) // Button: Clear
		{
		  waitForIt(10, 130, 120, 180);
		  stCurrent[0] = '\0';
		  stCurrentLen = 0;
		  myGLCD.setColor(0, 0, 0);
		  myGLCD.fillRect(0, 224, 319, 239);
		}
		if ((x >= 250) && (x <= 300)) // Button: Exit
		{
		  waitForIt(250, 130, 300, 180);
		  myGLCD.clrScr();
		  myGLCD.setBackColor(VGA_BLACK);
		  ret = 1;
		  stCurrent[0] = '\0';
		  stCurrentLen = 0;
		  break;
		}
		if ((x >= 130) && (x <= 240)) // Button: Enter
		{
		  waitForIt(130, 130, 240, 180);
		  if (stCurrentLen > 0)
		  {
			for (x = 0; x < stCurrentLen + 1; x++)
			{
			  stLast[x] = stCurrent[x];
			}
			stCurrent[0] = '\0';
			stLast[stCurrentLen + 1] = '\0';
			//i2c_eeprom_write_byte(deviceaddress,adr_stCurrentLen1,stCurrentLen);
			stCurrentLen1 = stCurrentLen;
			stCurrentLen = 0;
			myGLCD.setColor(0, 0, 0);
			myGLCD.fillRect(0, 200, 319, 239);
			myGLCD.setColor(0, 255, 0);
			myGLCD.print(stLast, LEFT, 208);
			break;
		  }
		  else
		  {
			myGLCD.setColor(255, 0, 0);
			myGLCD.print("\x80\x8A\x8B\x8B""EP \x89\x8A""CTO\x87!", CENTER, 192);//"������ ������!"
			delay(500);
			myGLCD.print("                ", CENTER, 192);
			delay(500);
			myGLCD.print("\x80\x8A\x8B\x8B""EP \x89\x8A""CTO\x87!", CENTER, 192);//"������ ������!"
			delay(500);
			myGLCD.print("                ", CENTER, 192);
			myGLCD.setColor(0, 255, 0);
		  }
		}
	  }
	}
  }
}
void drawButtons1() // ����������� �������� ����������
{
  // Draw the upper row of buttons
  for (x = 0; x < 5; x++)
  {
	myGLCD.setColor(0, 0, 255);
	myGLCD.fillRoundRect (10 + (x * 60), 10, 60 + (x * 60), 60);
	myGLCD.setColor(255, 255, 255);
	myGLCD.drawRoundRect (10 + (x * 60), 10, 60 + (x * 60), 60);
	myGLCD.printNumI(x + 1, 27 + (x * 60), 27);
  }
  // Draw the center row of buttons
  for (x = 0; x < 5; x++)
  {
	myGLCD.setColor(0, 0, 255);
	myGLCD.fillRoundRect (10 + (x * 60), 70, 60 + (x * 60), 120);
	myGLCD.setColor(255, 255, 255);
	myGLCD.drawRoundRect (10 + (x * 60), 70, 60 + (x * 60), 120);
	if (x < 4)
	  myGLCD.printNumI(x + 6, 27 + (x * 60), 87);
  }

  myGLCD.print("0", 267, 87);
  // Draw the lower row of buttons
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (10, 130, 120, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (10, 130, 120, 180);
  strcpy_P(buffer,(char*)pgm_read_word(&(table_message[8])));
  myGLCD.print(buffer, 20, 147);                                   // "������"


  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (130, 130, 240, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (130, 130, 240, 180);
  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[9])));
  myGLCD.print(buffer, 155, 147);                                  // "����"


  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (250, 130, 300, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (250, 130, 300, 180);
  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[10])));
  myGLCD.print(buffer, 252, 147);                                  // ���
  myGLCD.setBackColor (0, 0, 0);
}
void updateStr(int val)
{
  if (stCurrentLen < 20)
  {
	stCurrent[stCurrentLen] = val;
	stCurrent[stCurrentLen + 1] = '\0';
	stCurrentLen++;
	myGLCD.setColor(0, 255, 0);
	myGLCD.print(stCurrent, LEFT, 224);
  }
  else
  { // ����� ������ "������������!"
	myGLCD.setColor(255, 0, 0);
	myGLCD.print("\x89""EPE""\x89O\x88HEH\x86""E!", CENTER, 224);// ������������!
	delay(500);
	myGLCD.print("              ", CENTER, 224);
	delay(500);
	myGLCD.print("\x89""EPE""\x89O\x88HEH\x86""E!", CENTER, 224);// ������������!
	delay(500);
	myGLCD.print("              ", CENTER, 224);
	myGLCD.setColor(0, 255, 0);
  }
}
void waitForIt(int x1, int y1, int x2, int y2)
{
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  while (myTouch.dataAvailable())
  myTouch.read();
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
}

void control_command()
{
  /*
	��� ������ ������������ �������� ���������� �������� ����� �������� �� ������ adr_control_command (40120)
	��� ��������
	0 -   ���������� ������� ��������
	1 -   ��������� �������� ������ �1
	2 -   ��������� �������� ������ �2
	3 -   ��������� �������� ������ �3
	4 -   ��������� �������� ������ �4
	5 -   ��������� �������� ������ ��������
	6 -   �������� ������� �������� �1 �� ���������
	7 -   �������� ������� �������� �2 �� ���������
	8 -   �������� ������� �������� �3 �� ���������
	9 -   �������� ������� �������� �4 �� ���������
	10 -  ���������� ������� ������� ���������� �1
	11 -  ���������� ������� ������� ���������� �2
	12 -  ������ ������ �� EEPROM ��� �������� � ��
	13 -  �������� ������� �� �K � �������� � EEPROM
	14 -
	15 -
	16 -
	17 -
	18 -
	19 -
	20 -
	21 -
	22 -
	23 -
	24 -
	25 -
	26 -
	27 -
	28 -
	29 -
	30 -

  */


  int test_n = regBank.get(adr_control_command);   //�����  40000
  if (test_n != 0)
  {
	if (test_n != 0) Serial.println(test_n);
	switch (test_n)
	{
	  case 1:
		test_cabel_N1();             // ��������� �������� ������ �1
		break;
	  case 2:
		test_cabel_N2();             // ��������� �������� ������ �2
		break;
	  case 3:
		test_cabel_N3();             // ��������� �������� ������ �3
		break;
	  case 4:
		test_cabel_N4();             // ��������� �������� ������ �4
		break;
	  case 5:
		break;
	  case 6:
		save_default_pc();           // �������� ������� �������� � �� ���������
		break;
	  case 7:

		break;
	  case 8:

		break;
	  case 9:

		break;
	  case 10:
 
		break;
	  case 11:
 
		break;
	  case 12:
		mem_byte_trans_readPC();     // ������ ������ �� EEPROM ��� �������� � ��
		break;
	  case 13:
		mem_byte_trans_savePC();     // �������� ������� �� �K � �������� � EEPROM
		break;
	  case 14:
		//
		break;
	  case 15:
		//
		break;
	  case 16:
		//
		break;
	  case 17:
		//
		break;
	  case 18:
		//
		break;
	  case 19:
		//
		break;
	  case 20:                                         //
		//
		break;
	  case 21:                      		 		     //
		//
		break;
	  case 22:                                         //
		//
		break;
	  case 23:
		//
		break;
	  case 24:
		//
		break;
	  case 25:
		//
		break;
	  case 26:
		//
		break;
	  case 27:
		//
		break;
	  case 28:
		//
		break;
	  case 29:
		//
		break;
	  case 30:
		//
		break;

	  default:
		regBank.set(adr_control_command, 0);       // ���������� ���������� �1,�2  ������� �������
		break;
	}

  }
  else
  {
	regBank.set(adr_control_command, 0);
  }
}

void draw_Glav_Menu()
{
  but1   = myButtons.addButton( 10,  20, 250,  35, txt_menu1_1);
  but2   = myButtons.addButton( 10,  65, 250,  35, txt_menu1_2);
  but3   = myButtons.addButton( 10, 110, 250,  35, txt_menu1_3);
  but4   = myButtons.addButton( 10, 155, 250,  35, txt_menu1_4);
  butX   = myButtons.addButton( 279, 199,  40,  40, "W", BUTTON_SYMBOL); // ������ ����
  but_m1 = myButtons.addButton(  10, 199, 45,  40, "1");
  but_m2 = myButtons.addButton(  61, 199, 45,  40, "2");
  but_m3 = myButtons.addButton(  112, 199, 45,  40, "3");
  but_m4 = myButtons.addButton(  163, 199, 45,  40, "4");
  but_m5 = myButtons.addButton(  214, 199, 45,  40, "5");
  myButtons.drawButtons(); // ������������ ������
  myGLCD.setColor(VGA_BLACK);
  myGLCD.setBackColor(VGA_WHITE);
  myGLCD.setColor(0, 255, 0);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.print("                      ", CENTER, 0);

  switch (m2)
  {
	case 1:
	  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[2])));
	  myGLCD.print(buffer, CENTER, 0);                               // txt_info1
	  break;
	case 2:
	  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[3])));
	  myGLCD.print(buffer, CENTER, 0);                               // txt_info2
	  break;
	case 3:
	  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[4])));
	  myGLCD.print(buffer, CENTER, 0);                               // txt_info3
	  break;
	case 4:
	  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[5])));
	  myGLCD.print(buffer, CENTER, 0);                               // txt_info4
	  break;
	case 5:
	  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[6])));
	  myGLCD.print(buffer, CENTER, 0);                               // txt_info5
	  break;
  }
}

void swichMenu()                                             // ������ ���� � ������� "txt....."
{
  m2 = 1;                                                    // ���������� ������ �������� ����
  while (1)
  {
	wait_time = millis();                                    // ��������� ������ ����� ��� �������
	if (wait_time - wait_time_Old > 60000 * time_minute)
	{
	  wait_time_Old =  millis();
	  AnalogClock();
	  myGLCD.clrScr();
	  myButtons.drawButtons();                               // ������������ ������
	  print_up();                                            // ������������ ������� ������
	}

	myButtons.setTextFont(BigFont);                          // ���������� ������� ����� ������

	if (myTouch.dataAvailable() == true)                     // ��������� ������� ������
	{
	  pressed_button = myButtons.checkButtons();             // ���� ������ - ��������� ��� ������
	  wait_time_Old =  millis();

	  if (pressed_button == butX)                            // ������ - ����� ����
	  {
		AnalogClock();
		myGLCD.clrScr();
		myButtons.drawButtons();                             // ������������ ������
		print_up();                                          // ������������ ������� ������
	  }

	  if (pressed_button == but_m1)                          // ������ 1 �������� ����
	  {
		myButtons.setButtonColors(VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_BLUE); // ������� ��� ����
		myButtons.drawButtons();                             // ������������ ������
		default_colors = true;
		m2 = 1;                                              // ���������� ������ �������� ����
		myButtons.relabelButton(but1, txt_menu1_1, m2 == 1);
		myButtons.relabelButton(but2, txt_menu1_2, m2 == 1);
		myButtons.relabelButton(but3, txt_menu1_3, m2 == 1);
		myButtons.relabelButton(but4, txt_menu1_4, m2 == 1);
		myGLCD.setColor(0, 255, 0);
		myGLCD.setBackColor(0, 0, 0);
		myGLCD.print("                      ", CENTER, 0);
		strcpy_P(buffer, (char*)pgm_read_word(&(table_message[2])));
		myGLCD.print(buffer, CENTER, 0);                     // txt_info1 "���� �������"

	  }
	  if (pressed_button == but_m2)
	  {
		myButtons.setButtonColors(VGA_WHITE, VGA_RED, VGA_YELLOW, VGA_BLUE, VGA_TEAL);
		myButtons.drawButtons();
		default_colors = false;
		m2 = 2;
		myButtons.relabelButton(but1, txt_menu2_1 , m2 == 2);
		myButtons.relabelButton(but2, txt_menu2_2 , m2 == 2);
		myButtons.relabelButton(but3, txt_menu2_3 , m2 == 2);
		myButtons.relabelButton(but4, txt_menu2_4 , m2 == 2);
		myGLCD.setColor(0, 255, 0);
		myGLCD.setBackColor(0, 0, 0);
		myGLCD.print("                      ", CENTER, 0);
		strcpy_P(buffer, (char*)pgm_read_word(&(table_message[3])));
		myGLCD.print(buffer, CENTER, 0);                     // txt_info2 ���� ����� ��������
	  }

	  if (pressed_button == but_m3)
	  {
		myButtons.setButtonColors(VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_GREEN);
		myButtons.drawButtons();
		default_colors = false;
		m2 = 3;
		myButtons.relabelButton(but1, txt_menu3_1 , m2 == 3);
		myButtons.relabelButton(but2, txt_menu3_2 , m2 == 3);
		myButtons.relabelButton(but3, txt_menu3_3 , m2 == 3);
		myButtons.relabelButton(but4, txt_menu3_4 , m2 == 3);
		myGLCD.setColor(0, 255, 0);
		myGLCD.setBackColor(0, 0, 0);
		myGLCD.print("                      ", CENTER, 0);
		strcpy_P(buffer, (char*)pgm_read_word(&(table_message[4])));
		myGLCD.print(buffer, CENTER, 0);                     // txt_info3 ��������� �������
	  }
	  if (pressed_button == but_m4)
	  {
		myButtons.setButtonColors(VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_RED);
		myButtons.drawButtons();
		default_colors = false;
		m2 = 4;
		myButtons.relabelButton(but1, txt_menu4_1 , m2 == 4);
		myButtons.relabelButton(but2, txt_menu4_2 , m2 == 4);
		myButtons.relabelButton(but3, txt_menu4_3 , m2 == 4);
		myButtons.relabelButton(but4, txt_menu4_4 , m2 == 4);
		myGLCD.setColor(0, 255, 0);
		myGLCD.setBackColor(0, 0, 0);
		myGLCD.print("                      ", CENTER, 0);
		strcpy_P(buffer, (char*)pgm_read_word(&(table_message[5])));
		myGLCD.print(buffer, CENTER, 0);                     // txt_info4 
	  }

	  if (pressed_button == but_m5)
	  {
		myButtons.setButtonColors(VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_NAVY);
		myButtons.drawButtons();
		default_colors = false;
		m2 = 5;
		myButtons.relabelButton(but1, txt_menu5_1 , m2 == 5);
		myButtons.relabelButton(but2, txt_menu5_2 , m2 == 5);
		myButtons.relabelButton(but3, txt_menu5_3 , m2 == 5);
		myButtons.relabelButton(but4, txt_menu5_4 , m2 == 5);
		myGLCD.setColor(0, 255, 0);
		myGLCD.setBackColor(0, 0, 0);
		myGLCD.print("                      ", CENTER, 0);
		strcpy_P(buffer, (char*)pgm_read_word(&(table_message[6])));
		myGLCD.print(buffer, CENTER, 0);                     // txt_info5  �����������
	  }

	  //*****************  ���� �1  **************

	  if (pressed_button == but1 && m2 == 1)
	  {
		// ���� ������ �1
		myGLCD.clrScr();                                     // �������� �����
		test_cabel_N1();
		myGLCD.clrScr();
		myButtons.drawButtons();
		print_up();
	  }

	  if (pressed_button == but2 && m2 == 1)
	  {
		// ���� ������ �2
		myGLCD.clrScr();                                     // �������� �����
		test_cabel_N2();
		myGLCD.clrScr();
		myButtons.drawButtons();
		print_up();
	  }

	  if (pressed_button == but3 && m2 == 1)
	  {
		// ���� ������ �3
		myGLCD.clrScr();                                     // �������� �����
		test_cabel_N3();
		myGLCD.clrScr();
		myButtons.drawButtons();
		print_up();
	  }
	  if (pressed_button == but4 && m2 == 1)
	  {
		// ���� ������ �4
		myGLCD.clrScr();                                     // �������� �����
		test_cabel_N4();
		myGLCD.clrScr();
		myButtons.drawButtons();
		print_up();
	  }

	  //*****************  ���� �2  **************


	  if (pressed_button == but1 && m2 == 2)
	  {
		myGLCD.clrScr();//���� ������ ���
		test_BVS();
		myGLCD.clrScr();
		myButtons.drawButtons();
		print_up();
	  }

	  if (pressed_button == but2 && m2 == 2)
	  {
		//���� ������ �������� 2
 
		myGLCD.clrScr();
		myButtons.drawButtons();
		print_up();
	  }

	  if (pressed_button == but3 && m2 == 2)
	  {
 
		myGLCD.clrScr();
		myButtons.drawButtons();
		print_up();
	  }
	  if (pressed_button == but4 && m2 == 2)
	  {

		myGLCD.clrScr();
		myButtons.drawButtons();
		print_up();
	  }

	  //*****************  ���� �3  **************
	  if (pressed_button == but1 && m2 == 3)                 // ������ ����� ���� 3
	  {
		 test_cabel_soft(1);
		 myGLCD.clrScr();                                    // �������� �����
		 myButtons.drawButtons();
		 print_up();
	  }

	  //--------------------------------------------------------------
	  if (pressed_button == but2 && m2 == 3)                 // ������ ����� ���� 3
	  {
		test_cabel_soft(2);
		myGLCD.clrScr();
		myButtons.drawButtons();
		print_up();
	  }

	  //------------------------------------------------------------------

	  if (pressed_button == but3 && m2 == 3)                 // ������ ����� ���� 3
	  {
		test_cabel_soft(3);
		myGLCD.clrScr();
		myButtons.drawButtons();
		print_up();
	  }

	  //------------------------------------------------------------------
	  if (pressed_button == but4 && m2 == 3)                 // ��������� ����� ���� 3
	  {
		test_cabel_soft(4);
		myGLCD.clrScr();
		myButtons.drawButtons();
		print_up();
	  }

	  //*****************  ���� �4  **************

	  if (pressed_button == but1 && m2 == 4) //
	  {
		  table_cont();
		  myGLCD.clrScr();                                    // �������� �����
		  myButtons.drawButtons();
		  print_up();
	  }

	  if (pressed_button == but2 && m2 == 4)
	  {
		  myGLCD.clrScr();                                     // �������� �����
		  copy_cable();
		  myGLCD.clrScr();                                     // �������� �����
		  myButtons.drawButtons();
		  print_up();;
	  }

	  if (pressed_button == but3 && m2 == 4) //
	  {
		  myGLCD.clrScr();
		  save_tab_def();
		  myGLCD.clrScr();
		  myButtons.drawButtons();
		  print_up();
	  }
	  if (pressed_button == but4 && m2 == 4) //
	  {
		  // ���� ������ ���� ��������
		  test_all_pin();
		  myGLCD.clrScr();
		  myButtons.drawButtons();
		  print_up();
	  }
	  //*****************  ���� �5  **************

	  if (pressed_button == but1 && m2 == 5)                 // ����� ������
	  {
		myGLCD.clrScr();

		myGLCD.clrScr();
		myButtons.drawButtons();
		print_up();
	  }
	  if (pressed_button == but2 && m2 == 5)
	  {
		myGLCD.clrScr();

		myGLCD.clrScr();                                     // �������� �����
		delay (500);
		myButtons.drawButtons();
		print_up();
	  }

	  if (pressed_button == but3 && m2 == 5)                 // ���� 
	  {
		  myGLCD.clrScr();
		  myButtons.drawButtons();
		  print_up();
	  }

	  if (pressed_button == but4 && m2 == 5) //
	  {
		  myGLCD.clrScr();
		  myGLCD.setFont(BigFont);
		  myGLCD.setBackColor(0, 0, 255);
		  myGLCD.clrScr();
		  drawButtons1();                                      // ���������� �������� ����������
		  myGLCD.printNumI(time_minute, LEFT, 208);
		  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[14])));
		  myGLCD.print(buffer, 35, 208);                       // txt_time_wait
		  klav123();                                           // ������� ���������� � ����������
		  if (ret == 1)                                        // ���� "�������" - ���������
		  {
			  goto bailout41;                                  // ������� �� ��������� ���������� ������ ����
		  }
		  else                                                 // ����� ��������� ����� ����
		  {
			  time_minute = atol(stLast);
		  }
	  bailout41:                                               // ������������ ������ ����
		  myGLCD.clrScr();
		  myButtons.drawButtons();
		  print_up();
	  }

	  if (pressed_button == -1)
	  {
		//  myGLCD.print("HET", 220, 220);
	  }
	}
  }
}
void print_up() // ������ ������� ������� ��� ����
{
  myGLCD.setColor(0, 255, 0);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.print("                      ", CENTER, 0);
  switch (m2)
  {
	case 1:
	  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[2])));
	  myGLCD.print(buffer, CENTER, 0);                                 // txt_info1
	  break;
	case 2:
	  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[3])));
	  myGLCD.print(buffer, CENTER, 0);                                 // txt_info2
	  break;
	case 3:
	  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[4])));
	  myGLCD.print(buffer, CENTER, 0);                                 // txt_info3
	  break;
	case 4:
	  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[5])));
	  myGLCD.print(buffer, CENTER, 0);                                 // txt_info4
	  break;
	case 5:
	  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[6])));
	  myGLCD.print(buffer, CENTER, 0);                                 // txt_info5
	  break;
  }
}

void save_tab_def()                        // ������ � EEPROM  ������ �������� �� ���������
{
  for (int i = 1; i < 6; i++)
  {
	myGLCD.setColor(255, 255, 255);
	myGLCD.print("Save block N ", 25, 70);//
	myGLCD.printNumI(i, 230, 70);
	save_default(i);                 //������������ ������ � EEPROM ����� ������ �������� �� ���������

	for (int x = 10; x < 65; x++)
	{
	  myGLCD.setColor(0, 0, 255);
	  myGLCD.fillRoundRect (30, 100, 30 + (x * 4), 110);
	  myGLCD.setColor(255, 255, 255);
	  myGLCD.drawRoundRect (28, 98, 32 + (x * 4), 112);
	}
	myGLCD.clrScr();
  }
}
void save_default(byte adrN_eeprom)                                               //������������ ������ � EEPROM ����� ������ �������� �� ���������
{
  byte _u_konnekt     = 0;                                                      // ��������� �������� ����������� ��������.
  byte _step_mem      = 0;                                                      // ����� ����� � �������
  int adr_memN        = 0;
  int connekt_default = 0;                                                      // ����� � ���������� ������
  switch (adrN_eeprom)
  {
	case 1:
	  adr_memN   = adr_memN1_1;                                        // ����� ����� EEPROM � 1
	  _step_mem  = (pgm_read_byte_near(connektN1_default));            // ����� ����� � �������
	  for (int i = 0; i < (_step_mem * 5) + 1; i++)                    // �������� 5 ������ �������
	  {
		_u_konnekt = pgm_read_byte_near(connektN1_default + i);
		i2c_eeprom_write_byte(deviceaddress, adr_memN + i, _u_konnekt);
	  }
	  break;
	case 2:
	  adr_memN   = adr_memN1_2;                                        // ����� ����� EEPROM � 2
	  _step_mem  = (pgm_read_byte_near(connektN2_default));            // ����� ����� � �������
	  for (int i = 0; i < (_step_mem * 5) + 1; i++)                    // �������� 5 ������ �������
	  {
		_u_konnekt = pgm_read_byte_near(connektN2_default + i);
		i2c_eeprom_write_byte(deviceaddress, adr_memN + i, _u_konnekt);
	  }
	  break;
	case 3:
	  adr_memN   = adr_memN1_3;                                       // ����� ����� EEPROM � 3
	  _step_mem  = (pgm_read_byte_near(connektN3_default));           // ����� ����� � �������
	  for (int i = 0; i < (_step_mem * 5) + 1; i++)                   // �������� 5 ������ �������
	  {
		_u_konnekt = pgm_read_byte_near(connektN3_default + i);
		i2c_eeprom_write_byte(deviceaddress, adr_memN + i, _u_konnekt);
	  }
	  break;
	case 4:
	  adr_memN   = adr_memN1_4;                                       // ����� ����� EEPROM � 4
	  _step_mem  = (pgm_read_byte_near(connektN4_default));           // ����� ����� � �������
	  for (int i = 0; i < (_step_mem * 5) + 1; i++)                   // �������� 5 ������ �������
	  {
		_u_konnekt = pgm_read_byte_near(connektN4_default + i);
		i2c_eeprom_write_byte(deviceaddress, adr_memN + i, _u_konnekt);
	  }
	  break;
	case 5:
		adr_memN = adr_memN1_5;                                       // ����� ����� EEPROM � 4
		_step_mem = (pgm_read_byte_near(connekBVS_default));          // ����� ����� � �������
		for (int i = 0; i < (_step_mem * 5) + 1; i++)                 // �������� 5 ������ �������
		{
			_u_konnekt = pgm_read_byte_near(connekBVS_default + i);
			i2c_eeprom_write_byte(deviceaddress, adr_memN + i, _u_konnekt);
		}
		break;
	default:
	  adr_memN   = adr_memN1_1;                                       // ����� ����� EEPROM � 1
	  _step_mem  = (pgm_read_byte_near(connektN1_default));           // ����� ����� � �������
	  for (int i = 0; i < (_step_mem * 5) + 1; i++)                   // �������� 5 ������ �������
	  {
		_u_konnekt = pgm_read_byte_near(connektN1_default + i);
		i2c_eeprom_write_byte(deviceaddress, adr_memN + i, _u_konnekt);
	  }
	  break;
  }
}
void save_default_pc()                                                       // ������ ��������� ��������� ������� �������� �1
{
  int _step_mem       = 0;                                                 // ����� ����� � �������
  byte _u_konnekt     = 0;                                                 // ��������� �������� ����������� ��������.
  int adr_memN        = 0;
  int adrN_eeprom     = regBank.get(40008);                                // �������� ����� ������� �� ��������

  switch (adrN_eeprom)
  {
	case 1:
	  adr_memN = adr_memN1_1;                                     // ����� ����� EEPROM � 1
	  _step_mem = (pgm_read_byte_near(connektN1_default));        // ����� ����� � �������
	  for (int i = 1; i < (_step_mem * 2) + 1; i++)
	  {
		_u_konnekt = pgm_read_byte_near(connektN1_default + i);
		i2c_eeprom_write_byte(deviceaddress, adr_memN + i, _u_konnekt);
	  }
	  break;
	case 2:
	  adr_memN = adr_memN1_2;                                     // ����� ����� EEPROM � 2
	  _step_mem = (pgm_read_byte_near(connektN2_default));        // ����� ����� � �������
	  for (int i = 1; i < (_step_mem * 2) + 1; i++)
	  {
		_u_konnekt = pgm_read_byte_near(connektN2_default + i);
		i2c_eeprom_write_byte(deviceaddress, adr_memN + i, _u_konnekt);
	  }
	  break;
	case 3:
	  adr_memN = adr_memN1_3;                                     // ����� ����� EEPROM � 3
	  _step_mem = (pgm_read_byte_near(connektN3_default));        // ����� ����� � �������
	  for (int i = 1; i < (_step_mem * 2) + 1; i++)
	  {
		_u_konnekt = pgm_read_byte_near(connektN3_default + i);
		i2c_eeprom_write_byte(deviceaddress, adr_memN + i, _u_konnekt);
	  }
	  break;
	case 4:
	  adr_memN = adr_memN1_4;                                     // ����� ����� EEPROM � 4
	  _step_mem = (pgm_read_byte_near(connektN4_default));        // ����� ����� � �������
	  for (int i = 1; i < (_step_mem * 2) + 1; i++)
	  {
		_u_konnekt = pgm_read_byte_near(connektN4_default + i);
		i2c_eeprom_write_byte(deviceaddress, adr_memN + i, _u_konnekt);
	  }
	  break;
	default:
	  adr_memN = adr_memN1_1;                                     // ����� ����� EEPROM � 1
	  _step_mem = (pgm_read_byte_near(connektN1_default));        // ����� ����� � �������
	  for (int i = 1; i < (_step_mem * 2) + 1; i++)
	  {
		_u_konnekt = pgm_read_byte_near(connektN1_default + i);
		i2c_eeprom_write_byte(deviceaddress, adr_memN + i, _u_konnekt);
	  }
	  break;
  }
  regBank.set(adr_control_command, 0);                                     // ��������� ���������
}

void set_komm_mcp(char chanal_a_b, int chanal_n, char chanal_in_out ) // ��������� ��������� ��������� ������
{
  char _chanal_a_b     = chanal_a_b;                                  // ���� ������� ������������  
																	  // � - ���� U13,U17,U23 - �������� �� ����/�����. U14,U19,U21 - ���������� ����� � ������ �������.
																	  // � - ���� U15,U18,U22 - �������� �� ����/�����. U16,U20,U24 - ���������� ����� � ������ �������.

  int _chanal_n        = chanal_n;                                    // ����� � ������ (1- 48).
  int _chanal_in_out   = chanal_in_out;                               // ������� ��������� ����� ������: 'O' - ����������, 'G' - ���������� �� �����(���������).

  if (_chanal_a_b == 'A')                                             // ��������� ������� �
  {
	if (_chanal_in_out == 'O')                                        // ���������  ��������� ������ �  �� ����/�����
	{
	  mcp_Out1.digitalWrite(8,  HIGH);                                // ����� ������ EN ���������� ��������� �����������  1E1  U13
	  mcp_Out1.digitalWrite(9,  HIGH);                                // ����� ������ EN ���������� ��������� �����������  1E2  U17
	  mcp_Out1.digitalWrite(10, HIGH);                                // ����� ������ EN ���������� ��������� �����������  1E3  U23
	  if (_chanal_n < 17)
	  {
		set_mcp_byte_1a(_chanal_n - 1);                               // ������������ ���� ������ ������ (0 - 15)
		mcp_Out1.digitalWrite(8, LOW);                                // ������� EN ���������� ��������� �����������  1E1  U13
	  }
	  else if (_chanal_n > 16 && _chanal_n < 33)
	  {
		set_mcp_byte_1a(_chanal_n - 17);                              //  ������������ ���� ������ ������ (15 - 31)
		mcp_Out1.digitalWrite(9, LOW);                                // ������� EN ���������� ��������� �����������  1E2  U17
	  }
	  else if (_chanal_n > 32 && _chanal_n < 49)
	  {
		set_mcp_byte_1a(_chanal_n - 33);                              // ������������ ���� ������ ������ (32 - 48)
		mcp_Out1.digitalWrite(10, LOW);                               // ������� EN ���������� ��������� �����������  1E3  U23
	  }

	}
	if (_chanal_in_out == 'G')                                        // ��������� ����� �
	{
	  mcp_Out1.digitalWrite(11, HIGH);                                // ����� ������ EN ���������� ��������� �����������  1E4  U14
	  mcp_Out1.digitalWrite(12, HIGH);                                // ����� ������ EN ���������� ��������� �����������  1E5  U19
	  mcp_Out1.digitalWrite(13, HIGH);                                // ����� ������ EN ���������� ��������� �����������  1E6  U21
	  if (_chanal_n < 17)
	  {
		set_mcp_byte_1b(_chanal_n - 1);                               // ������������ ���� ������ ������ (0 - 15)
		mcp_Out1.digitalWrite(11, LOW);                               // �������  EN ���������� ��������� �����������  1E4  U14
	  }
	  else if (_chanal_n > 16 && _chanal_n < 33)
	  {
		set_mcp_byte_1b(_chanal_n - 17);                              // ������������ ���� ������ ������ (16 - 31)
		mcp_Out1.digitalWrite(12, LOW);                               // ������� EN ���������� ��������� �����������  1E5  U19
	  }
	  else if (_chanal_n > 32 && _chanal_n < 49)
	  {
		set_mcp_byte_1b(_chanal_n - 33);                              // ������������ ���� ������ ������ (32 - 48)
		mcp_Out1.digitalWrite(13, LOW);                               // �������  EN ���������� ��������� �����������  1E6  U21
	  }

	}
  }
  else if (_chanal_a_b == 'B')                                        // ��������� ������� �
  {
	if (_chanal_in_out == 'O')                                        // ���������  ��������� ������ �  �� ����/�����
	{
	  mcp_Out2.digitalWrite(8,  HIGH);                                // ����� ������ EN ���������� ��������� �����������  2E1  U15
	  mcp_Out2.digitalWrite(9,  HIGH);                                // ����� ������ EN ���������� ��������� �����������  2E2  U18
	  mcp_Out2.digitalWrite(10, HIGH);                                // ����� ������ EN ���������� ��������� �����������  2E3  U22
	  if (_chanal_n < 17)
	  {
		set_mcp_byte_2a(_chanal_n - 1);                               // ������������ ���� ������ ������ (0 - 15)
		mcp_Out2.digitalWrite(8, LOW);                                // ������� EN ���������� ��������� �����������  2E1  U15
	  }
	  else if (_chanal_n > 16 && _chanal_n < 33)
	  {
		set_mcp_byte_2a(_chanal_n - 17);                              // ������������ ���� ������ ������ (16 - 31)
		mcp_Out2.digitalWrite(9, LOW);                                // ������� EN ���������� ��������� �����������  2E2  U18
	  }
	  else if (_chanal_n > 32 && _chanal_n < 49)
	  {
		set_mcp_byte_2a(_chanal_n - 33);                              // ������������ ���� ������ ������ (32 - 48)
		mcp_Out2.digitalWrite(10, LOW);                               // ������� EN ���������� ��������� �����������  2E3  U22
	  }

	}
	if (_chanal_in_out == 'G')                                        // ��������� ����� B
	{
	  mcp_Out2.digitalWrite(11, HIGH);                                // ����� ������ EN ���������� ��������� �����������  2E4  U16
	  mcp_Out2.digitalWrite(12, HIGH);                                // ����� ������ EN ���������� ��������� �����������  2E5  U20
	  mcp_Out2.digitalWrite(13, HIGH);                                // ����� ������ EN ���������� ��������� �����������  2E6  U24
	  if (_chanal_n < 17)
	  {
		set_mcp_byte_2b(_chanal_n - 1);                               // ������������ ���� ������ ������ (0 - 15)
		mcp_Out2.digitalWrite(11, LOW);                               // ������� EN ���������� ��������� �����������  2E4  U16
	  }
	  else if (_chanal_n > 16 && _chanal_n < 33)
	  {
		set_mcp_byte_2b(_chanal_n - 17);                              // ������������ ���� ������ ������ (16 - 31)
		mcp_Out2.digitalWrite(12, LOW);                               // ������� EN ���������� ��������� �����������  2E5  U20
	  }
	  else if (_chanal_n > 32 && _chanal_n < 49)
	  {
		set_mcp_byte_2b(_chanal_n - 33);                              // ������������ ���� ������ ������ (32 - 48)
		mcp_Out2.digitalWrite(13, LOW);                               // ������� EN ���������� ��������� �����������  2E6  U24
	  }
	}
  }
  //delay(10);
}
void set_mcp_byte_1a(int set_byte)
{

  int _chanal_n = set_byte;

  if (bitRead(_chanal_n, 0) == 1)     // ���������� ��� 0
  {
	mcp_Out1.digitalWrite(0, HIGH);
  }
  else
  {
	mcp_Out1.digitalWrite(0, LOW);
  }

  if (bitRead(_chanal_n, 1) == 1)     // ���������� ��� 1
  {
	mcp_Out1.digitalWrite(1, HIGH);
  }
  else
  {
	mcp_Out1.digitalWrite(1, LOW);
  }

  if (bitRead(_chanal_n, 2) == 1)     // ���������� ��� 2
  {
	mcp_Out1.digitalWrite(2, HIGH);
  }
  else
  {
	mcp_Out1.digitalWrite(2, LOW);
  }


  if (bitRead(_chanal_n, 3) == 1)     // ���������� ��� 3
  {
	mcp_Out1.digitalWrite(3, HIGH);
  }
  else
  {
	mcp_Out1.digitalWrite(3, LOW);
  }
}
void set_mcp_byte_1b(int set_byte)
{
  int _chanal_n = set_byte;

  if (bitRead(_chanal_n, 0) == 1)     // ���������� ��� 0
  {
	mcp_Out1.digitalWrite(4, HIGH);
  }
  else
  {
	mcp_Out1.digitalWrite(4, LOW);
  }

  if (bitRead(_chanal_n, 1) == 1)     // ���������� ��� 1
  {
	mcp_Out1.digitalWrite(5, HIGH);
  }
  else
  {
	mcp_Out1.digitalWrite(5, LOW);
  }

  if (bitRead(_chanal_n, 2) == 1)     // ���������� ��� 2
  {
	mcp_Out1.digitalWrite(6, HIGH);
  }
  else
  {
	mcp_Out1.digitalWrite(6, LOW);
  }


  if (bitRead(_chanal_n, 3) == 1)     // ���������� ��� 3
  {
	mcp_Out1.digitalWrite(7, HIGH);
  }
  else
  {
	mcp_Out1.digitalWrite(7, LOW);
  }
}
void set_mcp_byte_2a(int set_byte)
{
  int _chanal_n = set_byte;

  if (bitRead(_chanal_n, 0) == 1)     // ���������� ��� 0
  {
	mcp_Out2.digitalWrite(0, HIGH);
  }
  else
  {
	mcp_Out2.digitalWrite(0, LOW);
  }

  if (bitRead(_chanal_n, 1) == 1)     // ���������� ��� 1
  {
	mcp_Out2.digitalWrite(1, HIGH);
  }
  else
  {
	mcp_Out2.digitalWrite(1, LOW);
  }

  if (bitRead(_chanal_n, 2) == 1)     // ���������� ��� 2
  {
	mcp_Out2.digitalWrite(2, HIGH);
  }
  else
  {
	mcp_Out2.digitalWrite(2, LOW);
  }


  if (bitRead(_chanal_n, 3) == 1)     // ���������� ��� 3
  {
	mcp_Out2.digitalWrite(3, HIGH);
  }
  else
  {
	mcp_Out2.digitalWrite(3, LOW);
  }
}
void set_mcp_byte_2b(int set_byte)
{
  int _chanal_n = set_byte;

  if (bitRead(_chanal_n, 0) == 1)     // ���������� ��� 0
  {
	mcp_Out2.digitalWrite(4, HIGH);
  }
  else
  {
	mcp_Out2.digitalWrite(4, LOW);
  }

  if (bitRead(_chanal_n, 1) == 1)     // ���������� ��� 1
  {
	mcp_Out2.digitalWrite(5, HIGH);
  }
  else
  {
	mcp_Out2.digitalWrite(5, LOW);
  }

  if (bitRead(_chanal_n, 2) == 1)     // ���������� ��� 2
  {
	mcp_Out2.digitalWrite(6, HIGH);
  }
  else
  {
	mcp_Out2.digitalWrite(6, LOW);
  }


  if (bitRead(_chanal_n, 3) == 1)     // ���������� ��� 3
  {
	mcp_Out2.digitalWrite(7, HIGH);
  }
  else
  {
	mcp_Out2.digitalWrite(7, LOW);
  }
}
void mem_byte_trans_readPC()                                      //  ������ ������ �� EEPROM ��� �������� � ��
{
  unsigned int _adr_reg = regBank.get(40005) + 40000;           //  ����� ����� ��������� ��� �������� � �� ������.
  unsigned int _adr_mem = regBank.get(40006);                   //  ����� ����� ������ ��� �������� � �� ������.
  unsigned int _size_block = regBank.get(40007);                //  ����� ����� ����� ������

  for (unsigned int x_mem = 0; x_mem < _size_block; x_mem++)
  {
	regBank.set(_adr_reg + x_mem, i2c_eeprom_read_byte(deviceaddress, _adr_mem + x_mem));
  }
  regBank.set(adr_control_command, 0);                          // ��������� ���������
  delay(200);
}
void mem_byte_trans_savePC()                                      //  �������� ������� �� �K � �������� � EEPROM
{
  unsigned int _adr_reg = regBank.get(40005);                   //  ����� ����� ��������� ��� �������� � �� ������.
  unsigned int _adr_mem = regBank.get(40006);                   //  ����� ����� ������ ��� �������� � �� ������.
  unsigned int _size_block = regBank.get(40007);                //  ����� ����� ����� ������

  for (unsigned int x_mem = 0; x_mem < _size_block; x_mem++)
  {
	i2c_eeprom_write_byte(deviceaddress, _adr_mem + x_mem, regBank.get(_adr_reg + x_mem));
  }
  regBank.set(adr_control_command, 0);                          // ��������� ���������
  delay(200);
}
int search_cabel(int sc)
{
  pinMode(Chanal_A, OUTPUT);                                                        // ���������� �� ����� ����� ������������ U13,U17,U23 (������� ����� � �� ������ ������)
  digitalWrite(Chanal_A, LOW);                                                      // ���������� ����������� ������� �� �����������
  pinMode(Chanal_B, INPUT);                                                         // ���������� �� ����  ����� ������������ U15,U18,U22 (������� ����� � �� �������� ������)
  digitalWrite(Chanal_B, HIGH);                                                     // ���������� ������� ������� �� ������ Chanal_B
  int n_connect = 0;
  Serial.println(sc);
  switch (sc)
  {
	case 1:
	  set_komm_mcp('A', 1, 'O');
	  set_komm_mcp('B', 1, 'O');
	  if (digitalRead(Chanal_B) == LOW )
	  {
		n_connect = 2;
		strcpy_P(buffer, (char*)pgm_read_word(&(table_message[27])));
		myGLCD.print(buffer, CENTER, 20);                                  // txt__connect2
	  }
	  break;
	case 39:
	  set_komm_mcp('A', 39, 'O');
	  set_komm_mcp('B', 39, 'O');
	  Serial.println(sc);
	  Serial.println(digitalRead(Chanal_B));
	  if (digitalRead(Chanal_B) == LOW )
	  {
		n_connect = 3;
		strcpy_P(buffer, (char*)pgm_read_word(&(table_message[28])));
		myGLCD.print(buffer, CENTER, 20);                                 // txt__connect3
	  }
	  break;
	case 40:
	  set_komm_mcp('A', 40, 'O');
	  set_komm_mcp('B', 40, 'O');
	  if (digitalRead(Chanal_B) == LOW )
	  {
		n_connect = 1;
		strcpy_P(buffer, (char*)pgm_read_word(&(table_message[26])));
		myGLCD.print(buffer, CENTER, 20);                                 // txt__connect1
	  }
	  break;
	case 41:
	  set_komm_mcp('A', 41, 'O');
	  set_komm_mcp('B', 41, 'O');
	  if (digitalRead(Chanal_B) == LOW )
	  {
		n_connect = 4;
		strcpy_P(buffer, (char*)pgm_read_word(&(table_message[29])));
		myGLCD.print(buffer, CENTER, 20);                                 // txt__connect4
	  }
	  break;
  }
  if (n_connect == 0) Serial.println(F("Connector is not detected"));
  return n_connect;
}

void test_cabel_N1()
{
  myGLCD.clrScr();
  myGLCD.print(txt_menu1_1, CENTER, 1);                                      // "���� ������ N 1"
  myGLCD.setColor(255, 255, 255);                                            // ����� ���������
  myGLCD.drawRoundRect (5, 200, 155, 239);
  myGLCD.drawRoundRect (160, 200, 315, 239);
  myGLCD.drawLine( 10, 60, 310, 60);
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (6, 201, 154, 238);
  myGLCD.fillRoundRect (161, 201, 314, 238);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor( 0, 0, 255);
  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[21])));
  myGLCD.print(buffer, 10, 210);                                             //txt_test_repeat  ���������
  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[20])));
  myGLCD.print(buffer, 168, 210);                                            //txt_test_end ���������
  myGLCD.setBackColor( 0, 0, 0);                                             //
  mcp_Out2.digitalWrite(14, LOW);                                            // ��������� ���� +12v
  if (search_cabel(40) == 1)                                                 // ������ ������ �1
  {
	test_cabel_N1_run();                                                   // ��������� ��������
	while (true)                                                           // �������� ��������� �������
	{

	  if (myTouch.dataAvailable())
	  {
		myTouch.read();
		x = myTouch.getX();
		y = myTouch.getY();

		if (((y >= 200) && (y <= 239)) && ((x >= 5) && (x <= 155)))   //������ ������ "��������� ��������"
		{
		  waitForIt(5, 200, 155, 239);
		  myGLCD.setFont(BigFont);
		  test_cabel_N1_run();                                  // ��������� ��������� ��������
		}
		if (((y >= 200) && (y <= 239)) && ((x >= 160) && (x <= 315))) //������ ������ "���������  ��������"
		{
		  waitForIt(160, 200, 315, 239);
		  myGLCD.setFont(BigFont);
		  break;                                                // ����� �� ���������
		}
	  }

	}
  }
  else
  {
	myGLCD.setColor(VGA_RED);
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[22])));
	myGLCD.print(buffer, CENTER, 80);                                       // txt_error_connect1 "������"
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[23])));
	myGLCD.print(buffer, CENTER, 110);                                      // txt_error_connect2 "����������� ������"
	myGLCD.setColor(255, 255, 255);
	delay(1000);
  }
}
void test_cabel_N2()
{
  myGLCD.clrScr();
  myGLCD.print(txt_menu1_2, CENTER, 1);
  myGLCD.setColor(255, 255, 255);                                             // ����� ���������
  myGLCD.drawRoundRect (5, 200, 155, 239);
  myGLCD.drawRoundRect (160, 200, 315, 239);
  myGLCD.drawLine( 10, 60, 310, 60);
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (6, 201, 154, 238);
  myGLCD.fillRoundRect (161, 201, 314, 238);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor( 0, 0, 255);
  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[21])));
  myGLCD.print(buffer, 10, 210);                                             //txt_test_repeat  ���������
  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[20])));
  myGLCD.print(buffer, 168, 210);                                            //txt_test_end ���������
  myGLCD.setBackColor( 0, 0, 0);
  mcp_Out2.digitalWrite(14, LOW);                                            // ��������� ���� +12v

  if (search_cabel(1) == 2)
  {
	test_cabel_N2_run();

	while (true)
	{

	  if (myTouch.dataAvailable())
	  {
		myTouch.read();
		x = myTouch.getX();
		y = myTouch.getY();

		if (((y >= 200) && (y <= 239)) && ((x >= 5) && (x <= 155)))            //���������
		{
		  waitForIt(5, 200, 155, 239);
		  myGLCD.setFont(BigFont);
		  test_cabel_N2_run();
		}
		if (((y >= 200) && (y <= 239)) && ((x >= 160) && (x <= 315)))         //���������
		{
		  waitForIt(160, 200, 315, 239);
		  myGLCD.setFont(BigFont);
		  break;
		}
	  }
	}
  }
  else
  {
	myGLCD.setColor(VGA_RED);
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[22])));
	myGLCD.print(buffer, CENTER, 80);                                       // txt_error_connect1 "������"
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[23])));
	myGLCD.print(buffer, CENTER, 110);                                      // txt_error_connect2 "����������� ������"
	myGLCD.setColor(255, 255, 255);
	delay(1000);
  }
}
void test_cabel_N3()
{
  myGLCD.clrScr();
  myGLCD.print(txt_menu1_3, CENTER, 1);
  myGLCD.setColor(255, 255, 255);                                             // ����� ���������
  myGLCD.drawRoundRect (5, 200, 155, 239);
  myGLCD.drawRoundRect (160, 200, 315, 239);
  myGLCD.drawLine( 10, 60, 310, 60);
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (6, 201, 154, 238);
  myGLCD.fillRoundRect (161, 201, 314, 238);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor( 0, 0, 255);
  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[21])));
  myGLCD.print(buffer, 10, 210);                                             //txt_test_repeat  ���������
  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[20])));
  myGLCD.print(buffer, 168, 210);                                            //txt_test_end ���������
  myGLCD.setBackColor( 0, 0, 0);
  mcp_Out2.digitalWrite(14, LOW);                                            // ��������� ���� +12v

  //if (search_cabel(39) == 20)
  //{
	test_cabel_N3_run();

	while (true)
	{
	  if (myTouch.dataAvailable())
	  {
		myTouch.read();
		x = myTouch.getX();
		y = myTouch.getY();

		if (((y >= 200) && (y <= 239)) && ((x >= 5) && (x <= 155)))            //���������
		{
		  waitForIt(5, 200, 155, 239);
		  myGLCD.setFont(BigFont);
		  test_cabel_N3_run();
		}
		if (((y >= 200) && (y <= 239)) && ((x >= 160) && (x <= 315)))         //���������
		{
		  waitForIt(160, 200, 315, 239);
		  myGLCD.setFont(BigFont);
		  break;
		}
	  }
	}
 // }
 // else
 // {
	//myGLCD.setColor(VGA_RED);
	//strcpy_P(buffer, (char*)pgm_read_word(&(table_message[22])));
	//myGLCD.print(buffer, CENTER, 80);                                       // txt_error_connect1 "������"
	//strcpy_P(buffer, (char*)pgm_read_word(&(table_message[23])));
	//myGLCD.print(buffer, CENTER, 110);                                      // txt_error_connect2 "����������� ������"
	//myGLCD.setColor(255, 255, 255);
	//delay(1000);
 // }
}
void test_cabel_N4()
{
  myGLCD.clrScr();
  myGLCD.print(txt_menu1_4, CENTER, 1);
  myGLCD.setColor(255, 255, 255);                                             // ����� ���������
  myGLCD.drawRoundRect (5, 200, 155, 239);
  myGLCD.drawRoundRect (160, 200, 315, 239);
  myGLCD.drawLine( 10, 60, 310, 60);
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (6, 201, 154, 238);
  myGLCD.fillRoundRect (161, 201, 314, 238);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor( 0, 0, 255);
  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[21])));
  myGLCD.print(buffer, 10, 210);                                             //txt_test_repeat  ���������
  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[20])));
  myGLCD.print(buffer, 168, 210);                                            //txt_test_end ���������
  myGLCD.setBackColor( 0, 0, 0);
  mcp_Out2.digitalWrite(14, LOW);                                            // ��������� ���� +12v

  if (search_cabel(41) == 4)
  {
	test_cabel_N4_run();

	while (true)
	{

	  if (myTouch.dataAvailable())
	  {
		myTouch.read();
		x = myTouch.getX();
		y = myTouch.getY();

		if (((y >= 200) && (y <= 239)) && ((x >= 5) && (x <= 155)))            //���������
		{
		  waitForIt(5, 200, 155, 239);
		  myGLCD.setFont(BigFont);
		  test_cabel_N4_run();
		}
		if (((y >= 200) && (y <= 239)) && ((x >= 160) && (x <= 315)))         //���������
		{
		  waitForIt(160, 200, 315, 239);
		  myGLCD.setFont(BigFont);
		  break;
		}
	  }

	}
  }
  else
  {
	myGLCD.setColor(VGA_RED);
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[22])));
	myGLCD.print(buffer, CENTER, 80);                                       // txt_error_connect1 "������"
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[23])));
	myGLCD.print(buffer, CENTER, 110);                                      // txt_error_connect2 "����������� ������"
	myGLCD.setColor(255, 255, 255);
	delay(1000);
  }
}

void test_BVS()
{
	myGLCD.clrScr();
	test_GND();
	myGLCD.clrScr();
	myGLCD.print(txt_menu2_1, CENTER, 1);
	myGLCD.setColor(255, 255, 255);                                             // ����� ���������
	myGLCD.drawRoundRect(5, 200, 155, 239);
	myGLCD.drawRoundRect(160, 200, 315, 239);
	myGLCD.drawLine(10, 60, 310, 60);
	myGLCD.setColor(0, 0, 255);
	myGLCD.fillRoundRect(6, 201, 154, 238);
	myGLCD.fillRoundRect(161, 201, 314, 238);
	myGLCD.setColor(255, 255, 255);
	myGLCD.setBackColor(0, 0, 255);
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[21])));
	myGLCD.print(buffer, 10, 210);                                             //txt_test_repeat  ���������
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[20])));
	myGLCD.print(buffer, 168, 210);                                            //txt_test_end ���������
	myGLCD.setBackColor(0, 0, 0);
	mcp_Out2.digitalWrite(14, LOW);                                            // ��������� ���� +12v
  
	test_BVS_run();

		while (true)
		{

			if (myTouch.dataAvailable())
			{
				myTouch.read();
				x = myTouch.getX();
				y = myTouch.getY();

				if (((y >= 200) && (y <= 239)) && ((x >= 5) && (x <= 155)))            //���������
				{
					waitForIt(5, 200, 155, 239);
					myGLCD.setFont(BigFont);
					test_BVS_run();
				}
				if (((y >= 200) && (y <= 239)) && ((x >= 160) && (x <= 315)))         //���������
				{
					waitForIt(160, 200, 315, 239);
					myGLCD.setFont(BigFont);
					break;
				}
			}

		}
}

void test_cabel_N1_run()
{

  byte  _size_block = i2c_eeprom_read_byte(deviceaddress, adr_memN1_1);        // �������� ���������� ������� ������������ �������
  pinMode(Chanal_A, OUTPUT);                                                   // ���������� �� ����� ����� ������������ U13,U17,U23 (������� ����� � �� ������ ������)
  pinMode(Chanal_B, INPUT);                                                    // ���������� �� ����  ����� ������������ U15,U18,U22 (������� ����� � �� �������� ������)
  digitalWrite(Chanal_B, HIGH);                                                // ���������� ������� ������� �� ������ Chanal_B
  myGLCD.print("                    ", 1, 40);                                 // �������� ������� ����������� ��������
  byte canal_N     = 0;                                                        // ���������� �������� � ������ � ������
  unsigned int x_A = 1;                                                        // ���������� ������������ ������ �
  unsigned int x_B = 1;                                                        // ���������� ������������ ������ �
  int x_p          = 1;                                                        // ���������� ������ ������ ������ �� �
  int y_p          = 82;                                                       // ���������� ������ ������ ������ �� �
  int count_error  = 0;                                                        // ������� ���������� ������
  int ware_on      = 0;                                                        // �������� ������ �� ���� ���������
 
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRoundRect(1, 82, 319, 199);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawLine(10, 60, 310, 60);
  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[24])));
  myGLCD.print(buffer, 50, 65);                                                // txt_error_connect3 "������ ���"
  if (search_cabel(40) == 1)                                                   // ��������� ������������ ����������� ������ �1
  {
	digitalWrite(Chanal_A, LOW);                                                     // ���������� ����������� ������� �� ������������ U13,U17,U23
	delay(10);                                                                 // ����� �� ������������ ������ Chanal_A
	for (x_A = 1; x_A < _size_block + 1; x_A++)                                // ���������������� ������ ��������� ��������.
	{
	  canal_N = i2c_eeprom_read_byte(deviceaddress, adr_memN1_1 + x_A);        // �������� � ������ �� EEPROM
	  ware_on = i2c_eeprom_read_byte(deviceaddress, adr_memN1_1 + x_A + (_size_block * 4)); // �������� �� ������� ������� ����������.
	  if (canal_N == 1)                                                        // 40 ����� ��� �������� ������ ������������ �������
	  {
		set_komm_mcp('A', 40, 'O');                                            // ���������� ���� ����������� �� ����������� 40 �����
	  }
	  else
	  {
		set_komm_mcp('A', canal_N, 'O');                                 // ���������� ������� ���� �����������
	  }
	  // ��������������� ��������� ��� ������ ������� "�"
	  // ��������� ��� ������ ������� "�"
	  for (x_B = 1; x_B < _size_block + 1; x_B++)                          // ���������������� ������ ��������� �������� "�" .
	  {
		canal_N = i2c_eeprom_read_byte(deviceaddress, adr_memN1_1 + x_B + _size_block); // �������� �� ������� ����� ����� �����������.

		if (canal_N == 1)                                                // 40 ����� ��� �������� ������ ������������ �������
		{
		  set_komm_mcp('B', 40, 'O');                                     // ���������� ����������� ���� �����������
		}
		else
		{
		  set_komm_mcp('B', canal_N, 'O');                                // ���������� ������� ���� �����������
		}
		// ++++++++++++++++++++++++ �������� �� ���������� � - � +++++++++++++++++++++++++++++++++++
		if (x_A == x_B)
		{
		  myGLCD.printNumI(x_A, 30, 40);
		  if (ware_on == 1)myGLCD.print("<->", 66, 40);
		  else myGLCD.print("<X>", 66, 40);
		  myGLCD.print("  ",130, 40);
		  myGLCD.printNumI(canal_N, 130, 40);
		  if (digitalRead(Chanal_B) == LOW && ware_on == 1)
		  {
			myGLCD.print(" - Pass", 170, 40);
		  }
		  else
		  {
			if (digitalRead(Chanal_B) != LOW && ware_on == 0)                  // ������ ���� ��������
			{
			  myGLCD.print(" - Pass", 170, 40);
			}
			else
			{
			  count_error++;
			  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[25])));
			  myGLCD.print(buffer, 50, 65);                            // txt_error_connect4
			  myGLCD.printNumI(count_error, 190, 65);

			  if ( ware_on == 1)
			  {
				if (x_A < 10)
				{
				  myGLCD.printNumI(x_A, x_p + 13, y_p);            // ������������ ��������� ���������
				  myGLCD.print("-", x_p + 29, y_p);
				}
				else
				{
				  myGLCD.printNumI(x_A, x_p, y_p);                 // ������������ ��������� ���������
				  myGLCD.print("-", x_p + 29, y_p);
				}
				if (canal_N < 10)
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 26, y_p);   // ������������ ��������� ���������
				}
				else
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 10, y_p);   // ������������ ��������� ���������
				}
			  }
			  else
			  {
				if (x_A < 10)
				{
				  myGLCD.printNumI(x_A, x_p + 13, y_p);            // ������������ ��������� ���������
				  myGLCD.print("+", x_p + 29, y_p);
				}
				else
				{
				  myGLCD.printNumI(x_A, x_p, y_p);                 // ������������ ��������� ���������
				  myGLCD.print("+", x_p + 29, y_p);
				}
				if (canal_N < 10)
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 26, y_p);   // ������������ ��������� ���������
				}
				else
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 10, y_p);   // ������������ ��������� ���������
				}
			  }
			  y_p += 19;
			  if ( y_p > 190)                                          // ����� �� ����� ������� ������
			  {
				myGLCD.drawLine( x_p + 75, 85, x_p + 75, 190);
				x_p += 80;
				y_p = 82;
			  }
			}
		  }
		}

		//------------------------ ����� �������� �� ���������� ---------------------------------------

		//++++++++++++++++++++++++ �������� ��������� �������� �� ��������� ---------------------------
		if (x_A != x_B)                                                      //����������� ������� �� �� ������ ���� ���������
		{
		  if (digitalRead(Chanal_B) == LOW)                                      // ��� ���� ��������
		  {
			// �������� �������������� 3 �������, �������� ������ ����� ����������
			int canal_N_err = i2c_eeprom_read_byte(deviceaddress, adr_memN1_1 + x_A + (_size_block * 2)); // �������� �� ������� ����� ����� �����������.
			if (x_B != canal_N_err)                                      // ����������� ���������� �� �������� � �������
			{
			  // �������� �������������� 4 �������
			  int canal_N_err = i2c_eeprom_read_byte(deviceaddress, adr_memN1_1 + x_A + (_size_block * 3)); // �������� �� ������� ����� ����� �����������.
			  if (x_B != canal_N_err)                                  // ����������� ���������� �� �������� � �������
			  {
				count_error++;
				strcpy_P(buffer, (char*)pgm_read_word(&(table_message[25])));
				myGLCD.print(buffer, 50, 65);                        // txt_error_connect4
				myGLCD.printNumI(count_error, 190, 65);
				if (x_A < 10)
				{
				  myGLCD.printNumI(x_A, x_p + 13, y_p);            // ������������ ��������� ���������
				  myGLCD.print("+", x_p + 29, y_p);
				}
				else
				{
				  myGLCD.printNumI(x_A, x_p, y_p);                 // ������������ ��������� ���������
				  myGLCD.print("+", x_p + 29, y_p);
				}
				if (canal_N < 10)
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 26, y_p);   // ������������ ��������� ���������
				}
				else
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 10, y_p);   // ������������ ��������� ���������
				}
				y_p += 19;
				if ( y_p > 190)                                      // ����� �� ����� ������� ������
				{
				  myGLCD.drawLine( x_p + 75, 85, x_p + 75, 190);
				  x_p += 80;
				  y_p = 82;
				}
			  }
			}
		  }
		} 	//----------------------- ����� �������� �� ��������� -----------------------------------------
	  }
	}
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[30])));
	if (count_error == 0) myGLCD.print(buffer, CENTER, 120);               // txt__test_end
  }
  else
  {
	myGLCD.setColor(VGA_RED);
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[22])));
	myGLCD.print(buffer, CENTER, 82 + 19);                                // txt_error_connect1 �������� ��� ������ �� ���������
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[23])));
	myGLCD.print(buffer, CENTER, 82 + 38);                                // txt_error_connect2
	myGLCD.setColor(255, 255, 255);                                       // ������������ ����� �����
	delay(2000);
  }
}
void test_cabel_N2_run()
{
  byte  _size_block = i2c_eeprom_read_byte(deviceaddress, adr_memN1_2);        // �������� ���������� ������� ������������ �������
  pinMode(Chanal_A, OUTPUT);                                                         // ���������� �� ����� ����� ������������ U13,U17,U23 (������� ����� � �� ������ ������)
  pinMode(Chanal_B, INPUT);                                                          // ���������� �� ����  ����� ������������ U15,U18,U22 (������� ����� � �� �������� ������)
  digitalWrite(Chanal_B, HIGH);                                                      // ���������� ������� ������� �� ������ Chanal_B
  myGLCD.print("                    ", 1, 40);                                 // �������� ������� ����������� ��������
  byte canal_N     = 0;                                                        // ���������� �������� � ������ � ������
  unsigned int x_A = 1;                                                        // ���������� ������������ ������ �
  unsigned int x_B = 1;                                                        // ���������� ������������ ������ �
  int x_p          = 1;                                                        // ���������� ������ ������ ������ �� �
  int y_p          = 82;                                                       // ���������� ������ ������ ������ �� �
  int count_error  = 0;                                                        // ������� ���������� ������
  int ware_on      = 0;                                                        // �������� ������ �� ���� ���������

  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRoundRect(1, 82, 319, 199);
  myGLCD.setColor(255, 255, 255);

  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[24])));
  myGLCD.print(buffer, 50, 65);                                                // txt_error_connect3 "������ ���"
  if (search_cabel(1) == 2)                                                    // ��������� ������������ ����������� ������ �1
  {
	digitalWrite(Chanal_A, LOW);                                                   // ���������� ����������� ������� �� ������������ U13,U17,U23
	delay(10);                                                               // ����� �� ������������ ������ Chanal_A
	// ������ ��������
	for (x_A = 1; x_A < _size_block + 1; x_A++)                              // ���������������� ������ ��������� ��������.
	{
	  canal_N = i2c_eeprom_read_byte(deviceaddress, adr_memN1_2 + x_A);    // �������� � ������ �� EEPROM
	  ware_on = i2c_eeprom_read_byte(deviceaddress, adr_memN1_2 + x_A + (_size_block * 4)); // �������� �� ������� ������� ����������.
	  if (canal_N == 1)                                                    // 40 ����� ��� �������� ������ ������������ �������
	  {
		set_komm_mcp('A', 1, 'O');                                       // ���������� ���� ����������� �� ����������� 40 �����
	  }
	  else
	  {
		set_komm_mcp('A', canal_N, 'O');                                 // ���������� ������� ���� �����������
	  }
	  // ��������������� ��������� ��� ������ ������� "�"
	  // ��������� ��� ������ ������� "�"
	  for (x_B = 1; x_B < _size_block + 1; x_B++)                          // ���������������� ������ ��������� �������� "�" .
	  {
		canal_N = i2c_eeprom_read_byte(deviceaddress, adr_memN1_2 + x_B + _size_block); // �������� �� ������� ����� ����� �����������.
		if (canal_N == 1)                                                // 40 ����� ��� �������� ������ ������������ �������
		{
		  set_komm_mcp('B', 1, 'O');                                   // ���������� ����������� ���� �����������
		}
		else
		{
		  set_komm_mcp('B', canal_N, 'O');                             // ���������� ������� ���� �����������
		}
		// ++++++++++++++++++++++++ �������� �� ���������� � - � +++++++++++++++++++++++++++++++++++
		if (x_A == x_B)
		{
		  myGLCD.printNumI(x_A, 30, 40);
		  if (ware_on == 1)myGLCD.print("<->", 66, 40);
		  else myGLCD.print("<X>", 66, 40);
		  myGLCD.print("  ",130, 40);
		  myGLCD.printNumI(canal_N, 130, 40);
		  if (digitalRead(Chanal_B) == LOW && ware_on == 1)
		  {
			myGLCD.print(" - Pass", 170, 40);
		  }
		  else
		  {
			if (digitalRead(Chanal_B) != LOW && ware_on == 0)                  // ������ ���� ��������
			{
			  myGLCD.print(" - Pass", 170, 40);
			}
			else
			{
			  count_error++;
			  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[25])));
			  myGLCD.print(buffer, 50, 65);                            // txt_error_connect4							myGLCD.printNumI(count_error, 190, 65);
			  myGLCD.printNumI(count_error, 190, 65);
			  if ( ware_on == 1)
			  {
				if (x_A < 10)
				{
				  myGLCD.printNumI(x_A, x_p + 13, y_p);            // ������������ ��������� ���������
				  myGLCD.print("-", x_p + 29, y_p);
				}
				else
				{
				  myGLCD.printNumI(x_A, x_p, y_p);                 // ������������ ��������� ���������
				  myGLCD.print("-", x_p + 29, y_p);
				}
				if (canal_N < 10)
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 26, y_p);   // ������������ ��������� ���������
				}
				else
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 10, y_p);   // ������������ ��������� ���������
				}
			  }
			  else
			  {
				if (x_A < 10)
				{
				  myGLCD.printNumI(x_A, x_p + 13, y_p);            // ������������ ��������� ���������
				  myGLCD.print("+", x_p + 29, y_p);
				}
				else
				{
				  myGLCD.printNumI(x_A, x_p, y_p);                 // ������������ ��������� ���������
				  myGLCD.print("+", x_p + 29, y_p);
				}
				if (canal_N < 10)
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 26, y_p);   // ������������ ��������� ���������
				}
				else
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 10, y_p);   // ������������ ��������� ���������
				}
			  }
			  y_p += 19;
			  if ( y_p > 190)                                          // ����� �� ����� ������� ������
			  {
				myGLCD.drawLine( x_p + 75, 85, x_p + 75, 190);
				x_p += 80;
				y_p = 82;
			  }
			}
		  }
		}

		//------------------------ ����� �������� �� ���������� ---------------------------------------


		//++++++++++++++++++++++++ �������� ��������� �������� �� ��������� ---------------------------
		if (x_A != x_B)                                                      //����������� ������� �� �� ������ ���� ���������
		{
		  if (digitalRead(Chanal_B) == LOW)                                      // ��� ���� ��������
		  {
			// �������� �������������� 3 �������, �������� ������ ����� ����������
			int canal_N_err = i2c_eeprom_read_byte(deviceaddress, adr_memN1_2 + x_A + (_size_block * 2)); // �������� �� ������� ����� ����� �����������.
			if (x_B != canal_N_err)                                      // ����������� ���������� �� �������� � �������
			{
			  // �������� �������������� 4 �������
			  int canal_N_err = i2c_eeprom_read_byte(deviceaddress, adr_memN1_2 + x_A + (_size_block * 3)); // �������� �� ������� ����� ����� �����������.
			  if (x_B != canal_N_err)                                  // ����������� ���������� �� �������� � �������
			  {
				count_error++;
				strcpy_P(buffer, (char*)pgm_read_word(&(table_message[25])));
				myGLCD.print(buffer, 50, 65);                        // txt_error_connect4
				myGLCD.printNumI(count_error, 190, 65);
				if (x_A < 10)
				{
				  myGLCD.printNumI(x_A, x_p + 13, y_p);            // ������������ ��������� ���������
				  myGLCD.print("+", x_p + 29, y_p);
				}
				else
				{
				  myGLCD.printNumI(x_A, x_p, y_p);                 // ������������ ��������� ���������
				  myGLCD.print("+", x_p + 29, y_p);
				}
				if (canal_N < 10)
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 26, y_p);   // ������������ ��������� ���������
				}
				else
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 10, y_p);   // ������������ ��������� ���������
				}
				y_p += 19;
				if ( y_p > 190)                                      // ����� �� ����� ������� ������
				{
				  myGLCD.drawLine( x_p + 75, 85, x_p + 75, 190);
				  x_p += 80;
				  y_p = 82;
				}
			  }
			}
		  }
		}
		//----------------------- ����� �������� �� ��������� -----------------------------------------
	  }
	}
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[30])));
	if (count_error == 0) myGLCD.print(buffer, CENTER, 120);                  // txt__test_end
  }
  else
  {
	myGLCD.setColor(VGA_RED);
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[22])));
	myGLCD.print(buffer, CENTER, 82 + 19);                                // txt_error_connect1 �������� ��� ������ �� ���������
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[23])));
	myGLCD.print(buffer, CENTER, 82 + 38);                                // txt_error_connect2
	myGLCD.setColor(255, 255, 255);                                       // ������������ ����� �����
	delay(2000);
  }
}
void test_cabel_N3_run()
{
  byte  _size_block = i2c_eeprom_read_byte(deviceaddress, adr_memN1_3);        // �������� ���������� ������� ������������ �������
  pinMode(Chanal_A, OUTPUT);                                                   // ���������� �� ����� ����� ������������ U13,U17,U23 (������� ����� � �� ������ ������)
  pinMode(Chanal_B, INPUT);                                                    // ���������� �� ����  ����� ������������ U15,U18,U22 (������� ����� � �� �������� ������)
  digitalWrite(Chanal_B, HIGH);                                                // ���������� ������� ������� �� ������ Chanal_B
  myGLCD.print("                    ", 1, 40);                                 // �������� ������� ����������� ��������
  byte canal_N     = 0;                                                        // ���������� �������� � ������ � ������
  unsigned int x_A = 1;                                                        // ���������� ������������ ������ �
  unsigned int x_B = 1;                                                        // ���������� ������������ ������ �
  int x_p          = 1;                                                        // ���������� ������ ������ ������ �� �
  int y_p          = 82;                                                       // ���������� ������ ������ ������ �� �
  int count_error  = 0;                                                        // ������� ���������� ������
  int ware_on      = 0;                                                        // �������� ������ �� ���� ���������

  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRoundRect(1, 82, 319, 199);
  myGLCD.setColor(255, 255, 255);

  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[24])));
  myGLCD.print(buffer, 50, 65);                                                // txt_error_connect3 "������ ���"
  //if (search_cabel(39) == 3)                                                   // ��������� ������������ ����������� ������ �1
  //{
	digitalWrite(Chanal_A, LOW);                                               // ���������� ����������� ������� �� ������������ U13,U17,U23
	delay(10);                                                                 // ����� �� ������������ ������ Chanal_A
	for (x_A = 1; x_A < _size_block + 1; x_A++)                                // ���������������� ������ ��������� ��������.
	{
	  canal_N = i2c_eeprom_read_byte(deviceaddress, adr_memN1_3 + x_A);        // �������� � ������ �� EEPROM
	  ware_on = i2c_eeprom_read_byte(deviceaddress, adr_memN1_3 + x_A + (_size_block * 4)); // �������� �� ������� ������� ����������.

	  if (canal_N == 1)                                                        // 40 ����� ��� �������� ������ ������������ �������
	  {
		set_komm_mcp('A', 39, 'O');                                             // ���������� ���� ����������� �� ����������� 39 �����
	  }
	  else
	  {
		set_komm_mcp('A', canal_N, 'O');                                 // ���������� ������� ���� �����������
	  }
	  // ��������������� ��������� ��� ������ ������� "�"
	  // ��������� ��� ������ ������� "�"
	  for (x_B = 1; x_B < _size_block + 1; x_B++)                          // ���������������� ������ ��������� �������� "�" .
	  {
		canal_N = i2c_eeprom_read_byte(deviceaddress, adr_memN1_3 + x_B + _size_block); // �������� �� ������� ����� ����� �����������.

		if (canal_N == 1)                                                // 39 ����� ��� �������� ������ ������������ �������
		{
		  set_komm_mcp('B', 39, 'O');                                  // ���������� ����������� ���� �����������
		}
		else
		{
		  set_komm_mcp('B', canal_N, 'O');                             // ���������� ������� ���� �����������
		}
		// ++++++++++++++++++++++++ �������� �� ���������� � - � +++++++++++++++++++++++++++++++++++
		if (x_A == x_B)
		{
		  myGLCD.printNumI(x_A, 30, 40);
		  if (ware_on == 1)myGLCD.print("<->", 66, 40);
		  else myGLCD.print("<X>", 66, 40);
		  myGLCD.print("  ",130, 40);
		  myGLCD.printNumI(canal_N, 130, 40);
		  if (digitalRead(Chanal_B) == LOW && ware_on == 1)
		  {
			myGLCD.print(" - Pass", 170, 40);
		  }
		  else
		  {
			if (digitalRead(Chanal_B) != LOW && ware_on == 0)                  // ������ ���� ��������
			{
			  myGLCD.print(" - Pass", 170, 40);
			}
			else
			{
			  count_error++;
			  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[25])));
			  myGLCD.print(buffer, 50, 65);                            // txt_error_connect4							myGLCD.printNumI(count_error, 190, 65);
			  myGLCD.printNumI(count_error, 190, 65);
			  if ( ware_on == 1)
			  {
				if (x_A < 10)
				{
				  myGLCD.printNumI(x_A, x_p + 13, y_p);            // ������������ ��������� ���������
				  myGLCD.print("-", x_p + 29, y_p);
				}
				else
				{
				  myGLCD.printNumI(x_A, x_p, y_p);                 // ������������ ��������� ���������
				  myGLCD.print("-", x_p + 29, y_p);
				}
				if (canal_N < 10)
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 26, y_p);   // ������������ ��������� ���������
				}
				else
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 10, y_p);   // ������������ ��������� ���������
				}
			  }
			  else
			  {
				if (x_A < 10)
				{
				  myGLCD.printNumI(x_A, x_p + 13, y_p);            // ������������ ��������� ���������
				  myGLCD.print("+", x_p + 29, y_p);
				}
				else
				{
				  myGLCD.printNumI(x_A, x_p, y_p);                 // ������������ ��������� ���������
				  myGLCD.print("+", x_p + 29, y_p);
				}
				if (canal_N < 10)
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 26, y_p);   // ������������ ��������� ���������
				}
				else
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 10, y_p);   // ������������ ��������� ���������
				}
			  }
			  y_p += 19;
			  if ( y_p > 190)                                          // ����� �� ����� ������� ������
			  {
				myGLCD.drawLine( x_p + 75, 85, x_p + 75, 190);
				x_p += 80;
				y_p = 82;
			  }
			}
		  }
		}

		//------------------------ ����� �������� �� ���������� ---------------------------------------

		//++++++++++++++++++++++++ �������� ��������� �������� �� ��������� ---------------------------
		if (x_A != x_B)                                                      //����������� ������� �� �� ������ ���� ���������
		{
		  if (digitalRead(Chanal_B) == LOW)                                      // ��� ���� ��������
		  {
			// �������� �������������� 3 �������, �������� ������ ����� ����������
			int canal_N_err = i2c_eeprom_read_byte(deviceaddress, adr_memN1_3 + x_A + (_size_block * 2)); // �������� �� ������� ����� ����� �����������.
			if (x_B != canal_N_err)                                      // ����������� ���������� �� �������� � �������
			{
			  // �������� �������������� 4 �������
			  int canal_N_err = i2c_eeprom_read_byte(deviceaddress, adr_memN1_3 + x_A + (_size_block * 3)); // �������� �� ������� ����� ����� �����������.
			  if (x_B != canal_N_err)                                  // ����������� ���������� �� �������� � �������
			  {
				count_error++;
				strcpy_P(buffer, (char*)pgm_read_word(&(table_message[25])));
				myGLCD.print(buffer, 50, 65);                        // txt_error_connect4
				myGLCD.printNumI(count_error, 190, 65);
				if (x_A < 10)
				{
				  myGLCD.printNumI(x_A, x_p + 13, y_p);            // ������������ ��������� ���������
				  myGLCD.print("+", x_p + 29, y_p);
				}
				else
				{
				  myGLCD.printNumI(x_A, x_p, y_p);                 // ������������ ��������� ���������
				  myGLCD.print("+", x_p + 29, y_p);
				}
				if (canal_N < 10)
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 26, y_p);   // ������������ ��������� ���������
				}
				else
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 10, y_p);   // ������������ ��������� ���������
				}
				y_p += 19;
				if ( y_p > 190)                                      // ����� �� ����� ������� ������
				{
				  myGLCD.drawLine( x_p + 75, 85, x_p + 75, 190);
				  x_p += 80;
				  y_p = 82;
				}
			  }
			}
		  }
		}	//----------------------- ����� �������� �� ��������� -----------------------------------------
	  }
	}
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[30])));
	if (count_error == 0) myGLCD.print(buffer, CENTER, 120);                  // txt__test_end
 // }
 // else
 // {
	//myGLCD.setColor(VGA_RED);
	//strcpy_P(buffer, (char*)pgm_read_word(&(table_message[22])));
	//myGLCD.print(buffer, CENTER, 82 + 19);                                // txt_error_connect1 �������� ��� ������ �� ���������
	//strcpy_P(buffer, (char*)pgm_read_word(&(table_message[23])));
	//myGLCD.print(buffer, CENTER, 82 + 38);                                // txt_error_connect2
	//myGLCD.setColor(255, 255, 255);                                       // ������������ ����� �����
	//delay(2000);
 // }
}
void test_cabel_N4_run()
{
  byte  _size_block = i2c_eeprom_read_byte(deviceaddress, adr_memN1_4);        // �������� ���������� ������� ������������ �������
  pinMode(Chanal_A, OUTPUT);                                                         // ���������� �� ����� ����� ������������ U13,U17,U23 (������� ����� � �� ������ ������)
  pinMode(Chanal_B, INPUT);                                                          // ���������� �� ����  ����� ������������ U15,U18,U22 (������� ����� � �� �������� ������)
  digitalWrite(Chanal_B, HIGH);                                                      // ���������� ������� ������� �� ������ Chanal_B
  myGLCD.print("                    ", 1, 40);                                 // �������� ������� ����������� ��������
  byte canal_N     = 0;                                                        // ���������� �������� � ������ � ������
  unsigned int x_A = 1;                                                        // ���������� ������������ ������ �
  unsigned int x_B = 1;                                                        // ���������� ������������ ������ �
  int x_p          = 1;                                                        // ���������� ������ ������ ������ �� �
  int y_p          = 82;                                                       // ���������� ������ ������ ������ �� �
  int count_error  = 0;                                                        // ������� ���������� ������
  int ware_on      = 0;                                                        // �������� ������ �� ���� ���������

  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRoundRect(1, 82, 319, 199);
  myGLCD.setColor(255, 255, 255);

  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[24])));
  myGLCD.print(buffer, 50, 65);                                                // txt_error_connect3 "������ ���"
  if (search_cabel(41) == 4)                                                   // ��������� ������������ ����������� ������ �1
  {
	digitalWrite(Chanal_A, LOW);                                                   // ���������� ����������� ������� �� ������������ U13,U17,U23
	delay(10);                                                               // ����� �� ������������ ������ Chanal_A
	for (x_A = 1; x_A < _size_block + 1; x_A++)                              // ���������������� ������ ��������� ��������.
	{
	  canal_N = i2c_eeprom_read_byte(deviceaddress, adr_memN1_4 + x_A);    // �������� � ������ �� EEPROM
	  ware_on = i2c_eeprom_read_byte(deviceaddress, adr_memN1_4 + x_A + (_size_block * 4)); // �������� �� ������� ������� ����������.
	  if (canal_N == 1)                                                    // 40 ����� ��� �������� ������ ������������ �������
	  {
		set_komm_mcp('A', 41, 'O');                                      // ���������� ���� ����������� �� ����������� 40 �����
	  }
	  else
	  {
		set_komm_mcp('A', canal_N, 'O');                                 // ���������� ������� ���� �����������
	  }
	  // ��������������� ��������� ��� ������ ������� "�"
	  // ��������� ��� ������ ������� "�"
	  for (x_B = 1; x_B < _size_block + 1; x_B++)                          // ���������������� ������ ��������� �������� "�" .
	  {
		canal_N = i2c_eeprom_read_byte(deviceaddress, adr_memN1_4 + x_B + _size_block); // �������� �� ������� ����� ����� �����������.

		if (canal_N == 1)                                                   // 40 ����� ��� �������� ������ ������������ �������
		{
		  set_komm_mcp('B', 41, 'O');                                       // ���������� ����������� ���� �����������
		}
		else
		{
		  set_komm_mcp('B', canal_N, 'O');                                   // ���������� ������� ���� �����������
		}
		// ++++++++++++++++++++++++ �������� �� ���������� � - � +++++++++++++++++++++++++++++++++++
		if (x_A == x_B)
		{
		  myGLCD.printNumI(x_A, 30, 40);
		  if (ware_on == 1)myGLCD.print("<->", 66, 40);
		  else myGLCD.print("<X>", 66, 40);
		  myGLCD.print("  ",130, 40);
		  myGLCD.printNumI(canal_N, 130, 40);
		  if (digitalRead(Chanal_B) == LOW && ware_on == 1)
		  {
			myGLCD.print(" - Pass", 170, 40);
		  }
		  else
		  {
			if (digitalRead(Chanal_B) != LOW && ware_on == 0)                  // ������ ���� ��������
			{
			  myGLCD.print(" - Pass", 170, 40);
			}
			else
			{
			  count_error++;
			  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[25])));
			  myGLCD.print(buffer, 50, 65);                            // txt_error_connect4
			  myGLCD.printNumI(count_error, 190, 65);

			  if ( ware_on == 1)
			  {
				if (x_A < 10)
				{
				  myGLCD.printNumI(x_A, x_p + 13, y_p);            // ������������ ��������� ���������
				  myGLCD.print("-", x_p + 29, y_p);
				}
				else
				{
				  myGLCD.printNumI(x_A, x_p, y_p);                 // ������������ ��������� ���������
				  myGLCD.print("-", x_p + 29, y_p);
				}
				if (canal_N < 10)
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 26, y_p);   // ������������ ��������� ���������
				}
				else
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 10, y_p);   // ������������ ��������� ���������
				}
			  }
			  else
			  {
				if (x_A < 10)
				{
				  myGLCD.printNumI(x_A, x_p + 13, y_p);            // ������������ ��������� ���������
				  myGLCD.print("+", x_p + 29, y_p);
				}
				else
				{
				  myGLCD.printNumI(x_A, x_p, y_p);                 // ������������ ��������� ���������
				  myGLCD.print("+", x_p + 29, y_p);
				}
				if (canal_N < 10)
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 26, y_p);   // ������������ ��������� ���������
				}
				else
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 10, y_p);   // ������������ ��������� ���������
				}
			  }
			  y_p += 19;
			  if ( y_p > 190)                                          // ����� �� ����� ������� ������
			  {
				myGLCD.drawLine( x_p + 75, 85, x_p + 75, 190);
				x_p += 80;
				y_p = 82;
			  }
			}
		  }
		}

		//------------------------ ����� �������� �� ���������� ---------------------------------------

		//++++++++++++++++++++++++ �������� ��������� �������� �� ��������� ---------------------------
		if (x_A != x_B)                                                      //����������� ������� �� �� ������ ���� ���������
		{
		  if (digitalRead(Chanal_B) == LOW)                                      // ��� ���� ��������
		  {
			// �������� �������������� 3 �������, �������� ������ ����� ����������
			int canal_N_err = i2c_eeprom_read_byte(deviceaddress, adr_memN1_4 + x_A + (_size_block * 2)); // �������� �� ������� ����� ����� �����������.
			if (x_B != canal_N_err)                                      // ����������� ���������� �� �������� � �������
			{
			  // �������� �������������� 4 �������
			  int canal_N_err = i2c_eeprom_read_byte(deviceaddress, adr_memN1_4 + x_A + (_size_block * 3)); // �������� �� ������� ����� ����� �����������.
			  if (x_B != canal_N_err)                                  // ����������� ���������� �� �������� � �������
			  {
				count_error++;
				strcpy_P(buffer, (char*)pgm_read_word(&(table_message[25])));
				myGLCD.print(buffer, 50, 65);                        // txt_error_connect4
				myGLCD.printNumI(count_error, 190, 65);
				if (x_A < 10)
				{
				  myGLCD.printNumI(x_A, x_p + 13, y_p);            // ������������ ��������� ���������
				  myGLCD.print("+", x_p + 29, y_p);
				}
				else
				{
				  myGLCD.printNumI(x_A, x_p, y_p);                 // ������������ ��������� ���������
				  myGLCD.print("+", x_p + 29, y_p);
				}
				if (canal_N < 10)
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 26, y_p);   // ������������ ��������� ���������
				}
				else
				{
				  myGLCD.printNumI(canal_N, x_p + 32 + 10, y_p);   // ������������ ��������� ���������
				}
				y_p += 19;
				if ( y_p > 190)                                      // ����� �� ����� ������� ������
				{
				  myGLCD.drawLine( x_p + 75, 85, x_p + 75, 190);
				  x_p += 80;
				  y_p = 82;
				}
			  }
			}
		  }
		}  //----------------------- ����� �������� �� ��������� -----------------------------------------
	  }
	}
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[30])));
	if (count_error == 0) myGLCD.print(buffer, CENTER, 120);                  // txt__test_end
  }
  else
  {
	myGLCD.setColor(VGA_RED);
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[22])));
	myGLCD.print(buffer, CENTER, 82 + 19);                                // txt_error_connect1 �������� ��� ������ �� ���������
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[23])));
	myGLCD.print(buffer, CENTER, 82 + 38);                                // txt_error_connect2
	myGLCD.setColor(255, 255, 255);                                       // ������������ ����� �����
	delay(2000);
  }
}

void test_BVS_run()
{
	byte  _size_block = i2c_eeprom_read_byte(deviceaddress, adr_memN1_5);        // �������� ���������� ������� ������������ �������
	pinMode(Chanal_A, INPUT);                                                   // ���������� �� ���� ����� ������������ U13,U17,U23 (������� ����� � �� ������ ������)
	pinMode(Chanal_B, INPUT);                                                    // ���������� �� ����  ����� ������������ U15,U18,U22 (������� ����� � �� �������� ������)
	digitalWrite(Chanal_A, HIGH);                                                // ���������� ������������� �������� �� ������ Chanal_B
	digitalWrite(Chanal_B, HIGH);                                                // ���������� ������������� �������� �� ������ Chanal_B
	myGLCD.print("                    ", 1, 40);                                 // �������� ������� ����������� ��������
	byte canal_N = 0;                                                            // ���������� �������� � ������ � ������
	unsigned int x_A = 1;                                                        // ���������� ������������ ������ �
	unsigned int x_B = 1;                                                        // ���������� ������������ ������ �
	int x_p = 1;                                                                 // ���������� ������ ������ ������ �� �
	int y_p = 82;                                                                // ���������� ������ ������ ������ �� �
	int count_error = 0;                                                         // ������� ���������� ������
	int ware_on = 0;                                                             // �������� ������ �� ���� ���������

	myGLCD.setColor(0, 0, 0);                                                    // ���������� ������ ����� 
	myGLCD.fillRoundRect(1, 40, 319, 199);                                       // �������� �����
	myGLCD.setColor(255, 255, 255);                                              // ���������� ����� ����
	y_p = 82;                                                                    // ������������ ������ ������ ������ �� �

	myGLCD.drawLine(10, 60, 310, 60);

	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[24])));                // "������ ���"
	myGLCD.print(buffer, 50, 65);                                                // txt_error_connect3 "������ ���"
	pinMode(Chanal_A, OUTPUT);                                                   // ���������� �� ���� ����� ������������ U13,U17,U23 (������� ����� � �� ������ ������)
	digitalWrite(Chanal_A, LOW);                                                 // ���������� ����������� ������� �� ������������ U13,U17,U23
	delay(10);                                                                   // ����� �� ������������ ������ Chanal_A
	for (x_A = 1; x_A < _size_block + 1; x_A++)                                  // ���������������� ������ ��������� ��������.
	{
		canal_N = i2c_eeprom_read_byte(deviceaddress, adr_memN1_5 + x_A);        // �������� � ������ �� EEPROM
		ware_on = i2c_eeprom_read_byte(deviceaddress, adr_memN1_5 + x_A + (_size_block * 4)); // �������� �� ������� ������� ����������.
		
		set_komm_mcp('A', canal_N, 'O');                                         // ���������� ������� ���� �����������
			
		// ��������������� ��������� ��� ������ ������� "�"
		// ��������� ��� ������ ������� "�"
		for (x_B = 1; x_B < _size_block + 1; x_B++)                              // ���������������� ������ ��������� �������� "�" .
		{
			canal_N = i2c_eeprom_read_byte(deviceaddress, adr_memN1_5 + x_B + _size_block); // �������� �� ������� ����� ����� �����������.

				set_komm_mcp('B', canal_N, 'O');                                 // ���������� ������� ���� �����������
			
			// ++++++++++++++++++++++++ �������� �� ���������� � - � +++++++++++++++++++++++++++++++++++
			if (x_A == x_B)
			{
				myGLCD.printNumI(x_A, 30, 40);
				if (ware_on == 1)myGLCD.print("<->", 66, 40);
				else myGLCD.print("<x>", 66, 40);
				myGLCD.print("  ", 130, 40);
				myGLCD.printNumI(canal_N, 130, 40);
				if (digitalRead(Chanal_B) == LOW && ware_on == 1)
				{
					myGLCD.print(" - Pass", 170, 40);
				}
				else
				{
					if (digitalRead(Chanal_B) != LOW && ware_on == 0)                  // ������ ���� ��������
					{
						myGLCD.print(" - Pass", 170, 40);
					}
					else
					{
						count_error++;
						strcpy_P(buffer, (char*)pgm_read_word(&(table_message[25])));
						myGLCD.print(buffer, 50, 65);                                 // txt_error_connect4
						myGLCD.printNumI(count_error, 190, 65);

						if (ware_on == 1)
						{
							if (x_A < 10)
							{
								myGLCD.printNumI(x_A, x_p + 13, y_p);                // ������������ ��������� ���������
								myGLCD.print("x", x_p + 29, y_p);
							}
							else
							{
								myGLCD.printNumI(x_A, x_p, y_p);                 // ������������ ��������� ���������
								myGLCD.print("x", x_p + 29, y_p);
							}
							if (canal_N < 10)
							{
								myGLCD.printNumI(canal_N, x_p + 32 + 26, y_p);   // ������������ ��������� ���������
							}
							else
							{
								myGLCD.printNumI(canal_N, x_p + 32 + 10, y_p);   // ������������ ��������� ���������
							}
						}
						else
						{
							if (x_A < 10)
							{
								myGLCD.printNumI(x_A, x_p + 13, y_p);            // ������������ ��������� ���������
								myGLCD.print("+", x_p + 29, y_p);
							}
							else
							{
								myGLCD.printNumI(x_A, x_p, y_p);                 // ������������ ��������� ���������
								myGLCD.print("+", x_p + 29, y_p);
							}
							if (canal_N < 10)
							{
								myGLCD.printNumI(canal_N, x_p + 32 + 26, y_p);   // ������������ ��������� ���������
							}
							else
							{
								myGLCD.printNumI(canal_N, x_p + 32 + 10, y_p);   // ������������ ��������� ���������
							}
						}
						y_p += 19;
						if (y_p > 190)                                          // ����� �� ����� ������� ������
						{
							myGLCD.drawLine(x_p + 75, 85, x_p + 75, 190);
							x_p += 80;
							y_p = 82;
						}
					}
				}
			}

			//------------------------ ����� �������� �� ���������� ---------------------------------------

			//++++++++++++++++++++++++ �������� ��������� �������� �� ��������� ---------------------------
			if (x_A != x_B)                                                            //����������� ������� �� �� ������ ���� ���������
			{
				if (digitalRead(Chanal_B) == LOW)                                      // ��� ���� ��������
				{
					count_error++;
					strcpy_P(buffer, (char*)pgm_read_word(&(table_message[25])));
					myGLCD.print(buffer, 50, 65);                             // txt_error_connect4
					myGLCD.printNumI(count_error, 190, 65);
					if (x_A < 10)
					{
						myGLCD.printNumI(x_A, x_p + 13, y_p);                 // ������������ ��������� ��������� 
						myGLCD.print("+", x_p + 29, y_p);
					}
					else
					{
						myGLCD.printNumI(x_A, x_p, y_p);                      // ������������ ��������� ���������
						myGLCD.print("+", x_p + 29, y_p);
					}
					if (canal_N < 10)
					{
						myGLCD.printNumI(canal_N, x_p + 32 + 26, y_p);       // ������������ ��������� ���������
					}
					else
					{
						myGLCD.printNumI(canal_N, x_p + 32 + 10, y_p);       // ������������ ��������� ���������
					}
					y_p += 19;
					if (y_p > 190)                                           // ����� �� ����� ������� ������
					{
						myGLCD.drawLine(x_p + 75, 85, x_p + 75, 190);
						x_p += 80;
						y_p = 82;
					}
				}
			}  //----------------------- ����� �������� �� ��������� -----------------------------------------
		}
	}
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[30])));
	if (count_error == 0) myGLCD.print(buffer, CENTER, 120);                  // txt__test_end
}

void test_GND()
{
	myGLCD.clrScr();
	myGLCD.setColor(255, 255, 255);
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[18])));
	myGLCD.print(buffer, CENTER, 200);                                           // txt_test_all_exit1 
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[19])));
	myGLCD.print(buffer, CENTER, 220);                                           // txt_test_all_exit2
	byte canal_N = 0;
	pinMode(Chanal_B, INPUT);                                                    // ���������� �� ����  ����� ������������ U15,U18,U22 (������� ����� � �� �������� ������)
	pinMode(Chanal_A, INPUT);                                                    // ���������� �� ����  ����� ������������ U13,U17,U23 (������� ����� � �� ������ ������)
	digitalWrite(Chanal_B, HIGH);                                                // ���������� ������� ������� �� ������ Chanal_B
	digitalWrite(Chanal_A, HIGH);                                                // ���������� ������� ������� �� ������ Chanal_A
	int i_step = 1;

	byte  _size_block = i2c_eeprom_read_byte(deviceaddress, adr_memN1_5);        // �������� ���������� ������� ������������ �������

	myGLCD.print("                    ", 1, 40);                                 // �������� ������� ����������� ��������
	unsigned int x_A = 1;                                                        // ���������� ������������ ������ �
	unsigned int x_B = 1;                                                        // ���������� ������������ ������ �
	int x_p = 1;                                                                 // ���������� ������ ������ ������ �� �
	int y_p = 82;                                                                // ���������� ������ ������ ������ �� �
	int count_error = 0;                                                         // ������� ���������� ������
	int ware_on = 0;                                                             // �������� ������ �� ���� ���������
	for (int p = 0; p < 6; p++)                                                  // �������� ���� ������ �� �������
	{
		myGLCD.print("                    ", x_p, y_p);                          // �������� 6 �����
		y_p += 19;
	}
	y_p = 82;                                                                    // ������������ ������ ������ ������ �� �


	while (true)
	{
		if (myTouch.dataAvailable())
		{
			myTouch.read();
			x = myTouch.getX();
			y = myTouch.getY();

			if (((y >= 1) && (y <= 150)) && ((x >= 10) && (x <= 319)))           //�������
			{
				myGLCD.setFont(BigFont);
				break;
			}
		}

		set_komm_mcp('A', 25, 'O');                                             // ����������� ���������� �������� ����� "�" �� ����
		set_komm_mcp('B', 25, 'O');                                             // ����������� ���������� �������� ����� "�" �� ����
		delay(10);

		if (digitalRead(Chanal_B) == LOW)
		{
			myGLCD.print("                 ", CENTER, 80);
			myGLCD.print("\x85""a""\x9C""e""\xA1\xA0""e""\xA2""o", CENTER, 80);    // 
			connect_gnd1 = true;
		}
		else
		{
			myGLCD.print("\x89""o""\x99\x9F\xA0\xAE\xA7\x9D\xA4""e ""\x9C""e""\xA1\xA0\xAE", CENTER, 80);    //  ���������� �����
			connect_gnd1 = false;

		}

		if (digitalRead(Chanal_A) == LOW)
		{
			myGLCD.print("                 ", CENTER, 105);
			myGLCD.print("\x85""a""\x9C""e""\xA1\xA0""e""\xA2""o", CENTER, 105);    // 
			connect_gnd2 = true;
		}
		else
		{
			myGLCD.print("\x89""o""\x99\x9F\xA0\xAE\xA7\x9D\xA4""e ""\x9C""e""\xA1\xA0\xAE", CENTER, 105);   // ���������� �����
			connect_gnd2 = false;

		}

		if (connect_gnd1 && connect_gnd2)
		{
			break;
		}
	}

	myGLCD.clrScr();

	myGLCD.print("\x89""po""\x97""ep""\x9F""a ""\x9C""a""\xA1\xAB\x9F""a""\xA2\x9D\xAF"" ", CENTER, 3);
	myGLCD.print("\xA2""a ""\x9C""e""\xA1\xA0\xAE"", o""\x96\xA9\x9D\x9E", CENTER,20);                       // �� �����, �����
	myGLCD.drawLine(10, 60, 310, 60);

	//++++++++++++++++++++++++++++++++++++++ �������� ��������� �� ����� +++++++++++++++++++++++++++++++++++++++++++++++++++
	for (x_A = 1; x_A < 49; x_A++)                                               // ���������������� ������ ��������� ��������.
	{
		set_komm_mcp('A', x_A, 'O');                                             // ���������� ������� ���� �����������
		set_komm_mcp('B', x_A, 'O');                                             // ���������� ������� ���� �����������
		if (x_A < 10)
		{
			myGLCD.printNumI(x_A, 30 + 12, 40);
		}
		else
		{
			myGLCD.printNumI(x_A, 30, 40);
		}
		myGLCD.print("<->", 66, 40);
		myGLCD.print("  ", 130, 40);
		//myGLCD.printNumI(canal_N, 130, 40);
		if (digitalRead(Chanal_A) == HIGH)                                       // ��������������� ��������� ��� ������ ������� "A"
		{
			myGLCD.print(" - Pass ", 170, 40);
		}
		else
		{
			myGLCD.print(" - Error", 170, 40);
			count_error++;
			strcpy_P(buffer, (char*)pgm_read_word(&(table_message[25])));
			myGLCD.print(buffer, 50, 65);                                       // txt_error_connect4
			myGLCD.printNumI(count_error, 190, 65);

			if (x_A < 10)
			{
				myGLCD.printNumI(x_A, x_p + 13, y_p);                          // ������������ ��������� ���������
				myGLCD.print(" - A", x_p + 29, y_p);
			}
			else
			{
				myGLCD.printNumI(x_A, x_p, y_p);                               // ������������ ��������� ���������
				myGLCD.print(" - A", x_p + 29, y_p);
			}
			y_p += 19;
			if (y_p > 190)                                                     // ����� �� ����� ������� ������
			{
				myGLCD.drawLine(x_p + 75, 85, x_p + 75, 190);
				x_p += 80;
				y_p = 82;
			}
		}

		if (digitalRead(Chanal_B) == HIGH)                                     // ��������������� ��������� ��� ������ ������� "�"
		{
			myGLCD.print(" - Pass ", 170, 40);
		}
		else
		{
			myGLCD.print(" - Error", 170, 40);
			count_error++;
			strcpy_P(buffer, (char*)pgm_read_word(&(table_message[25])));
			myGLCD.print(buffer, 50, 65);                                     // txt_error_connect4
			myGLCD.printNumI(count_error, 190, 65);
			if (x_A < 10)
			{
				myGLCD.printNumI(x_A, x_p + 13, y_p);                         // ������������ ��������� ���������
				myGLCD.print(" - B", x_p + 29, y_p);
			}
			else
			{
				myGLCD.printNumI(x_A, x_p, y_p);                              // ������������ ��������� ���������
				myGLCD.print(" - B", x_p + 29, y_p);
			}
			y_p += 19;
			if (y_p > 190)                                                    // ����� �� ����� ������� ������
			{
				myGLCD.drawLine(x_p + 75, 85, x_p + 75, 190);
				x_p += 80;
				y_p = 82;
			}
		}
	}

	while(!myTouch.dataAvailable())
	{
		myTouch.read();
		x = myTouch.getX();
		y = myTouch.getY();

		if (((y >= 1) && (y <= 150)) && ((x >= 10) && (x <= 319))) //�������
		{
			myGLCD.setFont(BigFont);
			break;
		}
	}
}
 
void test_all_pin()
{
  myGLCD.clrScr();
  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[17])));
  myGLCD.print(buffer, CENTER, 20);                                           // txt_test_all
  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[18])));
  myGLCD.print(buffer, CENTER, 180);                                          // txt_test_all_exit1
  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[19])));
  myGLCD.print(buffer, CENTER, 200);                                          // txt_test_all_exit2
  byte canal_N = 0;
  pinMode(Chanal_B, INPUT);                                                         // ���������� �� ����  ����� ������������ U15,U18,U22 (������� ����� � �� �������� ������)
  pinMode(Chanal_A, INPUT);                                                         // ���������� �� ����  ����� ������������ U13,U17,U23 (������� ����� � �� ������ ������)
  digitalWrite(Chanal_B, HIGH);                                                     // ���������� ������� ������� �� ������ Chanal_B
  digitalWrite(Chanal_A, HIGH);                                                     // ���������� ������� ������� �� ������ Chanal_A
  int i_step = 1;

  while (true)
  {
	if (myTouch.dataAvailable())
	{
	  myTouch.read();
	  x = myTouch.getX();
	  y = myTouch.getY();

	  if (((y >= 1) && (y <= 150)) && ((x >= 10) && (x <= 319))) //�������
	  {
		myGLCD.setFont(BigFont);
		break;
	  }
	}
	set_komm_mcp('A', i_step, 'O');                                         // ����������� ���������� �������� ����� "�" �� ����
	set_komm_mcp('B', i_step, 'O');                                         // ����������� ���������� �������� ����� "�" �� ����
	delay(10);
	if (digitalRead(Chanal_B) == LOW)
	{
	  myGLCD.print("B", CENTER, 80);
	  myGLCD.print("  ", CENTER, 105);
	  if (i_step == 39 || i_step == 40 ||  i_step == 41)
	  {
		myGLCD.print("1", CENTER, 105);
	  }
	  else
	  {
		myGLCD.printNumI(i_step, CENTER, 105);
	  }
	}
	else if (digitalRead(Chanal_A) == LOW)
	{
	  myGLCD.print("A", CENTER, 80);
	  myGLCD.print("  ", CENTER, 105);
	  if (i_step == 39 || i_step == 40 ||  i_step == 41)
	  {
		myGLCD.print("1", CENTER, 105);
	  }
	  else
	  {
		myGLCD.printNumI(i_step, CENTER, 105);
	  }
	}
	i_step++;
	if (i_step == 42) i_step = 1;
  }
}
void kommut_off()
{
  mcp_Out1.digitalWrite(8,  HIGH);                                // ����� ������ EN ���������� ��������� �����������  1E1  U13
  mcp_Out1.digitalWrite(9,  HIGH);                                // ����� ������ EN ���������� ��������� �����������  1E2  U17
  mcp_Out1.digitalWrite(10, HIGH);                                // ����� ������ EN ���������� ��������� �����������  1E3  U23
  mcp_Out1.digitalWrite(11, HIGH);                                // ����� ������ EN ���������� ��������� �����������  1E4  U14
  mcp_Out1.digitalWrite(12, HIGH);                                // ����� ������ EN ���������� ��������� �����������  1E5  U19
  mcp_Out1.digitalWrite(13, HIGH);                                // ����� ������ EN ���������� ��������� �����������  1E6  U21

  mcp_Out2.digitalWrite(8,  HIGH);                                // ����� ������ EN ���������� ��������� �����������  2E1  U15
  mcp_Out2.digitalWrite(9,  HIGH);                                // ����� ������ EN ���������� ��������� �����������  2E2  U18
  mcp_Out2.digitalWrite(10, HIGH);                                // ����� ������ EN ���������� ��������� �����������  2E3  U22
  mcp_Out2.digitalWrite(11, HIGH);                                // ����� ������ EN ���������� ��������� �����������  2E4  U16
  mcp_Out2.digitalWrite(12, HIGH);                                // ����� ������ EN ���������� ��������� �����������  2E5  U20
  mcp_Out2.digitalWrite(13, HIGH);                                // ����� ������ EN ���������� ��������� �����������  2E6  U24
}

void view_tab()
{
	myGLCD.clrScr();
	myGLCD.print("Ta""\x96\xA0\x9D\xA6""a coe""\x99\x9D\xA2""e""\xA2\x9D\x9E", CENTER, 1);
	myGLCD.setColor(255, 255, 255);                                    // ���������� ����� ���� ������
	myGLCD.drawRoundRect(160, 200, 315, 239);                          // ����� ��������� ������ ���������
	myGLCD.setColor(0, 0, 255);                                        // ���������� ����� ���� ������
	myGLCD.fillRoundRect(6, 201, 154, 238);                            // ��������� ��� �����
	myGLCD.fillRoundRect(161, 201, 314, 238);                          // ��������� ��� �����
	myGLCD.setColor(255, 255, 255);                                    // ���������� ����� ���� ������
	myGLCD.drawRoundRect(5, 200, 155, 239);                            // ����� ��������� ������ ���������
	myGLCD.setBackColor(0, 0, 255);                                    // ���������� ����� ���� ����
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[21])));
	myGLCD.print(buffer, 10, 210);                                     //txt_test_repeat  ���������
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[20])));
	myGLCD.print(buffer, 168, 210);                                    //txt_test_end ���������
	myGLCD.setBackColor(0, 0, 0);                                      // ������ ���

	myGLCD.print("N1", 27, 32);
	myGLCD.print("N2", 105, 32);
	myGLCD.print("N3", 183, 32);
	myGLCD.print("N4", 261, 32);
	myGLCD.print("\x80""BC", 20, 72);

	myGLCD.setColor(255, 255, 255);                                   // ���������� ����� ���� ������
	myGLCD.drawRoundRect(5, 25, 78, 55);                              // ����� ���������
	myGLCD.drawRoundRect(83, 25, 156, 55);                            // ����� ���������
	myGLCD.drawRoundRect(161, 25, 234, 55);                           // ����� ���������
	myGLCD.drawRoundRect(239, 25, 312, 55);                           // ����� ���������
	myGLCD.drawRoundRect(5, 65, 78, 95);                              // ����� ���������

	myGLCD.setColor(255, 0, 0);
	myGLCD.print("Ta""\x96\xA0\x9D\xA6\xAB"" o""\x96""y""\xA7""e""\xA2\x9D\xAF", CENTER, 100);   // 

	myGLCD.setColor(255, 255, 255);
	myGLCD.drawRoundRect(5, 125, 78, 155);
	myGLCD.drawRoundRect(83, 125, 156, 155);
	myGLCD.drawRoundRect(161, 125, 234, 155);
	myGLCD.drawRoundRect(239, 125, 312, 155);


	myGLCD.print("N1", 27, 132);
	myGLCD.print("N2", 105, 132);
	myGLCD.print("N3", 183, 132);
	myGLCD.print("N4", 261, 132);
}

void table_cont()                                                             // ����� ������� ���������� ���������
{
	int tab_n     = 0;
	int x_p       = 1;                                                        // ���������� ������ ������ �� �
	int y_p       = 70;                                                       // ���������� ������ ������ �� �
	int canal_N   = 1;
	bool info_run = false;                                                    // ������������� ��������� ����� ���������                                
	view_tab();                                                               // ���������� ���� ������ 
	while (true)                                                              // �������� ��������� �������
	{
	  if (myTouch.dataAvailable())
	  {
		myTouch.read();
		x = myTouch.getX();
		y = myTouch.getY();
		
		/*if (!info_run)
		{*/
			// ***************************  ����� ������������� ������� **************************************
			if (((x >= 5) && (x <= 78)) && ((y >= 25) && (y <= 55)))                 //������ ������ 1
			{
				waitForIt(5, 25, 78, 55);
				info_run = true;
				tab_n = adr_memN1_1;
				info_table(tab_n);
			}
			else if (((x >= 83) && (x <= 156)) && ((y >= 25) && (y <= 55)))         //������ ������ 2
			{
				waitForIt(83, 25, 156, 55);
				info_run = true;
				tab_n = adr_memN1_2;
				info_table(tab_n);
			}

			else if (((x >= 161) && (x <= 234)) && ((y >= 25) && (y <= 55)))         //������ ������ 3
			{
				waitForIt(161, 25, 234, 55);
				info_run = true;
				tab_n = adr_memN1_3;
				info_table(tab_n);
			}
			else if (((x >= 239) && (x <= 312)) && ((y >= 25) && (y <= 55)))         //������ ������ 4
			{
				waitForIt(239, 25, 312, 55);
				info_run = true;
				tab_n = adr_memN1_4;
				info_table(tab_n);
			}
			// ***************************  ����� ������������� ������� (������� ��������) **************************************
			else if (((x >= 5) && (x <= 78)) && ((y >= 125) && (y <= 155)))           //������ ������ 1
			{
				waitForIt(5, 125, 78, 155);
				info_run = true;
				tab_n = adr_memN2_1;
				info_table(tab_n);
			}
			else if (((x >= 83) && (x <= 156)) && ((y >= 125) && (y <= 155)))         //������ ������ 2
			{
				waitForIt(83, 125, 156, 155);
				info_run = true;
				tab_n = adr_memN2_2;
				info_table(tab_n);
			}

			else if (((x >= 161) && (x <= 234)) && ((y >= 125) && (y <= 155)))         //������ ������ 3
			{
				waitForIt(161, 125, 234, 155);
				info_run = true;
				tab_n = adr_memN2_3;
				info_table(tab_n);
			}
			else if (((x >= 239) && (x <= 312)) && ((y >= 125) && (y <= 155)))         //������ ������ 4
			{
				waitForIt(239, 125, 312, 155);
				info_run = true;
				tab_n = adr_memN2_4;
				info_table(tab_n);
			}
			else if (((x >= 5) && (x <= 78)) && ((y >= 65) && (y <= 95)))             //������ ������ ���
			{
				waitForIt(5, 65, 78, 95);
				info_run = true;
				tab_n = adr_memN1_5;
				info_table(tab_n);
			}
		//}

		else if ((y >= 200) && (y <= 239))
		{
			if ((x >= 5) && (x <= 155))                                     //������ ������ "��������� ��������"  
			{
				waitForIt(5, 200, 155, 239);
				if (view_tab_run == false)                                  // ��������� ��������� ������ �������
				{
					info_table(tab_n);
				}

			}
			if ((x >= 160) && (x <= 315))                                   //������ ������ "���������  ��������"
			{
				waitForIt(160, 200, 315, 239);
				break;                                                      // ����� �� ���������
			}
		}
	  }
	}
}
void info_table(int adr_mem)
{
	
	int _adr_mem = adr_mem;
	byte  _size_block = i2c_eeprom_read_byte(deviceaddress, _adr_mem);              // �������� ���������� ������� ������������ �������
	byte canal_N = 0;                                                               // ���������� �������� � ������ � ������
	int x_p = 0;                                                                    // ���������� ������ ������ ������ �� �
	int y_p = 20;                                                                   // ���������� ������ ������ ������ �� �
	myGLCD.setColor(0, 0, 0);
	myGLCD.fillRoundRect(0, 20, 319, 199);

	myGLCD.setColor(0, 0, 255);                                                     // ���������� ����� ���� ������
	myGLCD.fillRoundRect(6, 201, 154, 238);                                         // ��������� ��� �����
	myGLCD.setColor(255, 255, 255);
	//Serial.println(_size_block);
	view_tab_run = true;
	
	for (int i = 1; i < _size_block + 1; i++)                                       // ���������������� ������ ��������� ��������.
	{
		myGLCD.drawLine(x_p + 75, 20, x_p + 75, 194);
		myGLCD.drawLine(x_p + 75+42, 20, x_p + 75+42, 194);
		
		//canal_N = i2c_eeprom_read_byte(deviceaddress, _adr_mem + i);                // �������� � ������ �� EEPROM
		if (i < 10)
		{
			myGLCD.printNumI(i, x_p + 13, y_p);                                     // ����� �� ����� ������ ��������� ������ 10
			myGLCD.print("-", x_p + 29, y_p);
		}
		else
		{
			myGLCD.printNumI(i, x_p, y_p);                                          // ����� �� ����� ������ ��������� ������ 9
			myGLCD.print("-", x_p + 29, y_p);
		}
		Serial.print(_adr_mem + i); Serial.print("\t"); Serial.print(canal_N); Serial.print("\t");
		canal_N = i2c_eeprom_read_byte(deviceaddress, _adr_mem + i );               // �������� �� ������� ����� ����� �����������.
		Serial.println(canal_N);
		if (canal_N < 10)
		{
			if (canal_N == 0)
			{
				myGLCD.print("X", x_p + 58, y_p);                                   // ����� �� ����� "�" (�� ���������)
			}
			else
			{
				myGLCD.printNumI(canal_N, x_p + 58, y_p);                           // ����� �� ����� � ���������
			}
		}
		else
		{
			if (canal_N == 0)
			{
				myGLCD.print("X", x_p + 58, y_p);                                   // ����� �� �����  "�" (�� ���������)
			}
			else
			{
				myGLCD.printNumI(canal_N, x_p + 42, y_p);                             // ����� �� ����� � ���������
			}
		}

		canal_N = i2c_eeprom_read_byte(deviceaddress, _adr_mem + i + _size_block);    // �������� �� ������� (������ ���� ������)����� ����� �����������.

		if (canal_N < 10)
		{
			if ((canal_N == 0) || (canal_N == 255))
			{
				myGLCD.print("X", x_p + 58+40, y_p);                                  // ����� �� �����  "�" (�� ���������)
			}
			else
			{
				myGLCD.printNumI(canal_N, x_p + 58+40, y_p);                         // ����� �� ����� � ���������
			}
		}
		else
		{
			if ((canal_N == 0) || (canal_N == 255))
			{
				myGLCD.print("X", x_p + 58+40, y_p);                                //  ����� �� �����  "�" (�� ���������)
			}
			else
			{
				myGLCD.printNumI(canal_N, x_p + 42+40, y_p);                        // ����� �� ����� � ���������
			}
		}

		canal_N = i2c_eeprom_read_byte(deviceaddress, _adr_mem + i + _size_block * 2);  // �������� �� ������� (������ ���� ������) ����� ����� �����������.

		if (canal_N < 10)
		{
			if ((canal_N == 0) || (canal_N == 255))
			{
				myGLCD.print("X", x_p + 58 + 40+40, y_p);                              //  ����� �� �����  "�" (�� ���������)
			}
			else
			{
				myGLCD.printNumI(canal_N, x_p + 58 + 40+40, y_p);                         // ����� �� ����� � ���������
			}
		}
		else
		{
			if ((canal_N == 0) || (canal_N == 255))
			{ 
				myGLCD.print("X", x_p + 58 + 40+40, y_p);                                //  ����� �� �����  "�" (�� ���������)
			}
			else
			{
				myGLCD.printNumI(canal_N, x_p + 42 + 40+40, y_p);                        // ����� �� ����� � ���������
			}
		}

		y_p += 18;                                                                      // ������� �� ��������� ������
	
		if ((y_p > 190))                                                                //  ���� �������� ��������� - ������� � ������
		{
			myGLCD.drawLine(161, 20, 161, 199);
			myGLCD.drawLine(159, 20, 159, 199);
			x_p += 162;                                                                 //  ������ �������
			y_p = 20;                                                                   //  ���� �������� ��������� - ������� � ������
			if (x_p > 240)                                                              // ���� ������� ������������ ���������.
			{
				if (i >= _size_block)        break;                                   // ���������� ��������� �������� ������ �������  
				while (!myTouch.dataAvailable()){}                                      // �������� ��������� �������
				if (myTouch.dataAvailable())                                            
				{
					myTouch.read();
					x = myTouch.getX();
					y = myTouch.getY();
					if (((x >= 1) && (x <= 319)) && ((y >= 1) && (y <= 199))||((x >= 1) && (x <= 155)) && ((y >= 200) && (y <= 239)))          //����� ����� (���������� �����)
					{
						waitForIt(1, 1, 319, 199);
						if (i < _size_block)                                           // ���������� ��������� �������� ������ �������  
						{
							x_p = 0;                                                   // ������� �� ��������� �����    
							myGLCD.setColor(0, 0, 0);
							myGLCD.fillRoundRect(0, 20, 319, 199);                     // �������� �����
							myGLCD.setColor(255, 255, 255);
							view_tab_run = true;
						}
					}
					if (((x >= 160) && (x <= 315)) && ((y >= 200) && (y <= 239)))      //������ ������ "���������  ��������"
					{
						waitForIt(160, 200, 315, 239);
						view_tab_run = false;
						break;                                                         // ����� �� ��������� ���������
					}
				}
			}
		}
	}

	if(view_tab_run) while (!myTouch.dataAvailable()) {}                              // �������� ����������� ������� �� ����� (������ �� ��������)
	myGLCD.setColor(255, 255, 255);                                                   // ���������� ����� ���� ������
	myGLCD.setBackColor(0, 0, 255);                                                   // ���������� ����� ���� ����
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[21])));
	myGLCD.print(buffer, 10, 210);                                                    //txt_test_repeat  ���������
	myGLCD.setBackColor(0, 0, 0);
	view_tab();
	view_tab_run = false;
}

void view_menu_search()
{
	myGLCD.clrScr();

	myGLCD.setColor(255, 255, 255);                                             // ����� ���������
	myGLCD.drawRoundRect(5, 200, 155, 239);
	myGLCD.drawRoundRect(160, 200, 315, 239);

	myGLCD.drawRoundRect(5, 20, 285, 55);
	myGLCD.drawRoundRect(5, 65, 285, 100);
	myGLCD.drawRoundRect(5, 110, 285, 145);
	myGLCD.drawRoundRect(5, 155, 285, 190);

	//myGLCD.drawLine(10, 60, 310, 60);
	myGLCD.setColor(0, 0, 255);
	myGLCD.fillRoundRect(6, 201, 154, 238);
	myGLCD.fillRoundRect(161, 201, 314, 238);

	myGLCD.fillRoundRect(6, 21, 284, 54);
	myGLCD.fillRoundRect(6, 66, 284, 99);
	myGLCD.fillRoundRect(6, 111, 284, 144);
	myGLCD.fillRoundRect(6, 156, 284, 189);



	myGLCD.setColor(255, 255, 255);
	myGLCD.setBackColor(0, 0, 255);

	myGLCD.print("Bap""\x9D""a""\xA2\xA4"" ""\x9F""a""\x96""e""\xA0\xAF"" N1", 10, 30);    // ������� ������ N1
	myGLCD.print("Bap""\x9D""a""\xA2\xA4"" ""\x9F""a""\x96""e""\xA0\xAF"" N2", 10, 75);    // ������� ������ N2
	myGLCD.print("Bap""\x9D""a""\xA2\xA4"" ""\x9F""a""\x96""e""\xA0\xAF"" N3", 10, 120);   // ������� ������ N3
	myGLCD.print("Bap""\x9D""a""\xA2\xA4"" ""\x9F""a""\x96""e""\xA0\xAF"" N4", 10, 165);   // ������� ������ N4

	myGLCD.print("B""\xAB\xA3""o""\xA0\xA2\x9D\xA4\xAC", 10, 210);                         // ���������
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[20])));
	myGLCD.print(buffer, 168, 210);                                                        // ���������
	myGLCD.setBackColor(0, 0, 0);

}
void copy_cable()
{
	myGLCD.setFont(BigFont);
	myGLCD.setBackColor(0, 0, 255);
	myGLCD.clrScr();
	view_menu_search();
	byte N_block;

	while (true)
	{
		if (myTouch.dataAvailable())
		{
			myTouch.read();
			x = myTouch.getX();
			y = myTouch.getY();


			if (((y >= 20) && (y <= 55)) && ((x >= 5) && (x <= 285)))          //���������
			{
				waitForIt(5, 20, 285, 55);
				myGLCD.setColor(255, 0, 0);                                    // ������� ���������
				myGLCD.drawRoundRect(5, 20, 285, 55);
				myGLCD.setColor(255, 255, 255);                                // ����� ���������
				myGLCD.drawRoundRect(5, 65, 285, 100);
				myGLCD.drawRoundRect(5, 110, 285, 145);
				myGLCD.drawRoundRect(5, 155, 285, 190);
				N_block = 1;

			}
			if (((y >= 65) && (y <= 100)) && ((x >= 5) && (x <= 285)))          //���������
			{
				waitForIt(5, 65, 285, 100);
				myGLCD.setColor(255, 0, 0);                                     // ������� ���������
				myGLCD.drawRoundRect(5, 65, 285, 100);
				myGLCD.setColor(255, 255, 255);                                 // ����� ���������
				myGLCD.drawRoundRect(5, 20, 285, 55);
				myGLCD.drawRoundRect(5, 110, 285, 145);
				myGLCD.drawRoundRect(5, 155, 285, 190);
				N_block = 2;

			}

			if (((y >= 110) && (y <= 145)) && ((x >= 5) && (x <= 285)))         //���������
			{
				waitForIt(5, 110, 285, 145);
				myGLCD.setColor(255, 0, 0);                                     // ������� ���������
				myGLCD.drawRoundRect(5, 110, 285, 145);
				myGLCD.setColor(255, 255, 255);                                 // ����� ���������
				myGLCD.drawRoundRect(5, 20, 285, 55);
				myGLCD.drawRoundRect(5, 65, 285, 100);
				myGLCD.drawRoundRect(5, 155, 285, 190);
				N_block = 3;
			}

			if (((y >= 155) && (y <= 190)) && ((x >= 5) && (x <= 285)))         //���������
			{
				waitForIt(5, 155, 285, 190);
				myGLCD.setColor(255, 0, 0);                                     // ������� ���������
				myGLCD.drawRoundRect(5, 155, 285, 190);
				myGLCD.setColor(255, 255, 255);                                 // ����� ���������
				myGLCD.drawRoundRect(5, 20, 285, 55);
				myGLCD.drawRoundRect(5, 65, 285, 100);
				myGLCD.drawRoundRect(5, 110, 285, 145);
				N_block = 4;
			}

			if (((y >= 200) && (y <= 239)) && ((x >= 5) && (x <= 155)))       //���������
			{
				waitForIt(5, 200, 155, 239);
				myGLCD.setFont(BigFont);
				myGLCD.clrScr();
				search_contacts(N_block);
				view_menu_search();
			}
			if (((y >= 200) && (y <= 239)) && ((x >= 160) && (x <= 315)))     //���������
			{
				waitForIt(160, 200, 315, 239);
				myGLCD.setFont(BigFont);
				break;
			}
		}
	}
}

void search_contacts(byte N_block)
{
	byte  _size_block = 48;                                                      // �������� ���������� ������� ������������ �������
	pinMode(Chanal_A, OUTPUT);                                                   // ���������� �� ����� ����� ������������ U13,U17,U23 (������� ����� � �� ������ ������)
	pinMode(Chanal_B, INPUT);                                                    // ���������� �� ����  ����� ������������ U15,U18,U22 (������� ����� � �� �������� ������)
	digitalWrite(Chanal_B, HIGH);                                                // ���������� ������� ������� �� ������ Chanal_B
	myGLCD.print("                    ", 1, 40);                                 // �������� ������� ����������� ��������
	byte canal_N = 0;                                                            // ���������� �������� � ������ � ������
	unsigned int x_A = 1;                                                        // ���������� ������������ ������ �
	unsigned int x_B = 1;                                                        // ���������� ������������ ������ �
	int x_p = 1;                                                                 // ���������� ������ ������ ������ �� �
	int y_p = 1;                                                                 // ���������� ������ ������ ������ �� �
	int count_error = 0;                                                         // ������� ���������� ������
	int ware_on = 0;                                                             // �������� ������ �� ���� ���������
	myGLCD.setColor(0, 0, 0);
	myGLCD.fillRoundRect(1, 1 , 319, 209);
	myGLCD.setColor(255, 255, 255);

	y_p = 40;                                                                    // ������������ ������ ������ ������ �� �
	x_p = 1;                                                                     // ������������ ������ ������ ������ �� �

	int adr_block = adr_memN2_1;                                                 // ���������� ��� �������� ������ �����

	switch (N_block)
	{
	case 1:
		adr_block = adr_memN2_1;
		break;
	case 2:
		adr_block = adr_memN2_2;
		break;
	case 3:
		adr_block = adr_memN2_3;
		break;
	case 4:
		adr_block = adr_memN2_4;
		break;
	default:
		adr_block = adr_memN2_1;
		break;
	}
	myGLCD.print("O""\xA7\x9D""c""\xA4\x9F""a ""\xA4""a""\x96\xA0\x9D\xA6\xAB", CENTER, 10);                    // ������� �������
	for (int i=0;i<240;i++)
	{
		i2c_eeprom_write_byte(deviceaddress, adr_block+i, 0);                                                   // �������� ������� ����������
	}
	myGLCD.print("\x8B""op""\xA1\x9D""po""\x97""a""\xA2\x9D""e ""\xA4""a""\x96\xA0\x9D\xA6\xAB", CENTER, 10);   // ������������ �������

	i2c_eeprom_write_byte(deviceaddress, adr_block, _size_block);                                               // ��������� � "0" ������ ���������� ������� ������ 

	digitalWrite(Chanal_A, LOW);                                                                                // ���������� ����������� ������� �� ������������ U13,U17,U23
	delay(10);                                                                                                  // ����� �� ������������ ������ Chanal_A
	
	myGLCD.drawLine(80, 40, 80, 194);
	myGLCD.drawLine(122, 40, 122, 194);
	myGLCD.drawLine(164, 40, 164, 199);
	myGLCD.drawLine(208, 40, 208, 199);


	for (x_A = 1; x_A < _size_block + 1; x_A++)                                                                 // ���������������� ������ ��������� ��������.
	{
		//i2c_eeprom_write_byte(deviceaddress, adr_block + x_A, x_A);                                           // �������� ����� ������������ �������� � ������� � ������ �������� ������
		Serial.print(adr_block + x_A); Serial.print("\t"); Serial.print(x_A); Serial.print("\t");
	
		set_komm_mcp('A', x_A, 'O');                                                                            // ���������� ������� ���� �����������
		
		if (x_A < 10)
		{
			myGLCD.printNumI(x_A, x_p + 13, y_p);                                                                // ������������  ���������
			myGLCD.print("-", x_p + 30, y_p);
		}
		else
		{
			myGLCD.printNumI(x_A, x_p, y_p);                                                                     // ������������   ���������
			myGLCD.print("-", x_p + 30, y_p);
		}
																												// ��������� �� ������������ ��������� ���������
		int i_block = 1;																						// ��������������� ��������� ��� ������ ������� "�"
																												// ��������� ��� ������ ������� "�"
		for (x_B = 1; x_B < _size_block + 1; x_B++)                                                             // ���������������� ������ ��������� �������� "�" .
		{
			set_komm_mcp('B', x_B, 'O');                                                                        // ���������� ������� ���� �����������
		
			// ++++++++++++++++++++++++ �������� �� ���������� � - � +++++++++++++++++++++++++++++++++++
		
			if (digitalRead(Chanal_B) == LOW)                                                                   // ����� �� ����� ��� ���������� �������� 
			{                
				i2c_eeprom_write_byte(deviceaddress, adr_block + x_A + (_size_block*(i_block-1)), x_B);             // �������� ����� �������� � ������� � ������ �������� ������
				Serial.println(adr_block + x_A); Serial.print("\t"); Serial.print( x_A ); Serial.print("\t"); Serial.println(x_B);
			

				if (x_B < 10)
				{
					myGLCD.printNumI(x_B, (x_p + 13 + (42 * i_block)), y_p);                                     // ������������   ���������
				}
				else
				{
					myGLCD.printNumI(x_B, (x_p + 1 + (42 * i_block)), y_p);                                        // ������������  ���������
				}

				i_block++;
				if (i_block > 4) i_block = 4;
			}
			else
			{
				myGLCD.print("x", (x_p + 13 + (42 * i_block)), y_p);                                     // ������������   ���������

			}


		}
		y_p += 19;
	
		if (y_p > 185)                                            // ����� �� ����� ������� ������
		{
			myGLCD.setColor(0, 0, 0);
			myGLCD.fillRoundRect(1, 40, 319, 209);
			myGLCD.setColor(255, 255, 255);
			myGLCD.drawLine(80, 40, 80, 194);
			myGLCD.drawLine(122, 40, 122, 194);
			myGLCD.drawLine(164, 40, 164, 199);
			myGLCD.drawLine(208, 40, 208, 199);
			y_p = 40;
			delay(300);
		}
	}
	myGLCD.setColor(0, 0, 0);
	myGLCD.fillRoundRect(1, 40, 319, 209);
	myGLCD.setColor(0, 255, 0);
	myGLCD.print("\x85""ABEP""\x8E""EHO", CENTER, 110);                                               // ���������
	myGLCD.print("Ha""\x9B\xA1\x9D\xA4""e ""\xA2""a ""\x99\x9D""c""\xA3\xA0""e""\x9E", CENTER, 208);  // ������� �� �������
	myGLCD.print("\x99\xA0\xAF"" ""\x9C""a""\x97""ep""\xA8""e""\xA2\x9D\xAF", CENTER, 222);           // ��� ����������
	myGLCD.setColor(255, 255, 255);
	delay(2000);

	//do {
	//	if (myTouch.dataAvailable())
	//	{
	//		myTouch.read();
	//		x = myTouch.getX();
	//		y = myTouch.getY();
	//		if (((y >= 1) && (y <= 239)) && ((x >= 1) && (x <= 319)))         //����� �����
	//		{
	//			waitForIt(1, 1, 319, 239);
	//			break;
	//		}
	//	}
	//} while (true);
}

void test_cabel_soft(int cable_soft)
{
	int _cable_soft = cable_soft;
	myGLCD.clrScr();
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[60])));
	myGLCD.print(buffer,5, 1);                                                 // ���� ����� ������ N
	myGLCD.printNumI(_cable_soft, RIGHT, 1);                                   // ����� ������
	myGLCD.setColor(255, 255, 255);                                            // ����� ���������
	myGLCD.drawRoundRect(5, 200, 155, 239);
	myGLCD.drawRoundRect(160, 200, 315, 239);
	myGLCD.drawLine(10, 60, 310, 60);
	myGLCD.setColor(0, 0, 255);
	myGLCD.fillRoundRect(6, 201, 154, 238);
	myGLCD.fillRoundRect(161, 201, 314, 238);
	myGLCD.setColor(255, 255, 255);
	myGLCD.setBackColor(0, 0, 255);
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[21])));
	myGLCD.print(buffer, 10, 210);                                             //txt_test_repeat  ���������
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[20])));
	myGLCD.print(buffer, 168, 210);                                            //txt_test_end ���������
	myGLCD.setBackColor(0, 0, 0);                                              //
	mcp_Out2.digitalWrite(14, LOW);                                            // ��������� ���� +12v

	test_cabel_soft_run(_cable_soft);                                          // ��������� ��������
	while (true)                                                               // �������� ��������� �������
	{

		if (myTouch.dataAvailable())
		{
			myTouch.read();
			x = myTouch.getX();
			y = myTouch.getY();

			if (((y >= 200) && (y <= 239)) && ((x >= 5) && (x <= 155)))        //������ ������ "��������� ��������"
			{
				waitForIt(5, 200, 155, 239);
				myGLCD.setFont(BigFont);
				test_cabel_soft_run(_cable_soft);                              // ��������� ��������
			}
			if (((y >= 200) && (y <= 239)) && ((x >= 160) && (x <= 315))) //������ ������ "���������  ��������"
			{
				waitForIt(160, 200, 315, 239);
				myGLCD.setFont(BigFont);
				break;                                                    // ����� �� ���������
			}
		}

	}
}

void test_cabel_soft_run(int cable_soft_run)
{
	int _cable_soft_run = cable_soft_run;
	int _adr_mem_soft = 0;
	byte  _size_block = 0;
	switch (_cable_soft_run) {
	case 1:
		 _size_block = i2c_eeprom_read_byte(deviceaddress, adr_memN2_1);        // �������� ���������� ������� ������������ �������
		 _adr_mem_soft = adr_memN2_1;
		break;
	case 2:
		_size_block = i2c_eeprom_read_byte(deviceaddress, adr_memN2_2);        // �������� ���������� ������� ������������ �������
		_adr_mem_soft = adr_memN2_2;
		break;
	case 3:
		_size_block = i2c_eeprom_read_byte(deviceaddress, adr_memN2_3);        // �������� ���������� ������� ������������ �������
		_adr_mem_soft = adr_memN2_3;
		break;
	case 4:
		_size_block = i2c_eeprom_read_byte(deviceaddress, adr_memN2_4);        // �������� ���������� ������� ������������ �������
		_adr_mem_soft = adr_memN2_4;
		break;
	default:
		_size_block = i2c_eeprom_read_byte(deviceaddress, adr_memN2_1);        // �������� ���������� ������� ������������ �������
		_adr_mem_soft = adr_memN2_1;
		break;
	}

	
	pinMode(Chanal_A, INPUT);                                                    // ���������� �� ����  ����� ������������ U13,U17,U23 (������� ����� � �� ������ ������)
	pinMode(Chanal_B, INPUT);                                                    // ���������� �� ����  ����� ������������ U15,U18,U22 (������� ����� � �� �������� ������)
	digitalWrite(Chanal_A, HIGH);                                                // ���������� ������� ������� �� ������ Chanal_A (���������� ��������� PULL_UP)
	digitalWrite(Chanal_B, HIGH);                                                // ���������� ������� ������� �� ������ Chanal_B (���������� ��������� PULL_UP)
	myGLCD.print("                    ", 1, 40);                                 // �������� ������� ����������� ��������
	byte chanal_1 = 0;                                                           // ���������� �������� �1 ������ � ������
	byte chanal_2 = 0;                                                           // ���������� �������� �2 ������ � ������
	byte chanal_3 = 0;                                                           // ���������� �������� �3 ������ � ������

	unsigned int x_A = 1;                                                        // ���������� ������������ ������ �
	unsigned int x_B = 1;                                                        // ���������� ������������ ������ �
	int x_p = 1;                                                                 // ���������� ������ ������ ������ �� �
	int y_p = 82;                                                                // ���������� ������ ������ ������ �� �
	int count_error = 0;                                                         // ������� ���������� ������
	int ware_on = 0;                                                             // �������� ������ �� ���� ���������

	myGLCD.setColor(0, 0, 0);
	myGLCD.fillRoundRect(1, 82, 319, 199);
	myGLCD.setColor(255, 255, 255);

	myGLCD.print("Tec""\xA4"" ""\x9C""a""\xA1\xAB\x9F"". ""\xA2""a ""\x9C""e""\xA1\xA0\xAE", CENTER, 20);    //���� �����. �� �����
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[24])));                // txt_error_connect3 "������ ���"
	myGLCD.print(buffer, 50, 65);                                                // txt_error_connect3 "������ ���"
	                                                                             // ��������� �� ���������� ��������� �� �����  
	digitalWrite(Chanal_A, HIGH);                                                // ���������� ������� ������� �� ������������ U13,U17,U23
	delay(10);                                                                   // ����� �� ������������ ������ Chanal_A
	
	//++++++++++++++++++++++++++++++++++++++ �������� ��������� �� ����� +++++++++++++++++++++++++++++++++++++++++++++++++++
	for (x_A = 1; x_A < 49; x_A++)                                               // ���������������� ������ ��������� ��������.
	{
		set_komm_mcp('A', x_A, 'O');                                             // ���������� ������� ���� �����������
		set_komm_mcp('B', x_A, 'O');                                             // ���������� ������� ���� �����������
		if (x_A < 10)
		{
			myGLCD.printNumI(x_A, 30+12, 40);
		}
		else
		{
			myGLCD.printNumI(x_A, 30, 40);
		}
		myGLCD.print("<->", 66, 40);
		myGLCD.print("  ", 130, 40);
		//myGLCD.printNumI(canal_N, 130, 40);
		if (digitalRead(Chanal_A) == HIGH)                                       // ��������������� ��������� ��� ������ ������� "A"
		{
			myGLCD.print(" - Pass ", 170, 40);
		}
		else
		{
			myGLCD.print(" - Error", 170, 40);
			count_error++;
			strcpy_P(buffer, (char*)pgm_read_word(&(table_message[25])));
			myGLCD.print(buffer, 50, 65);                                       // txt_error_connect4
			myGLCD.printNumI(count_error, 190, 65);

			if (x_A < 10)
			{
				myGLCD.printNumI(x_A, x_p + 13, y_p);                          // ������������ ��������� ���������
				myGLCD.print(" - A", x_p + 29, y_p);
			}
			else
			{
				myGLCD.printNumI(x_A, x_p, y_p);                               // ������������ ��������� ���������
				myGLCD.print(" - A", x_p + 29, y_p);
			}
			y_p += 19;
			if (y_p > 190)                                                     // ����� �� ����� ������� ������
			{
				myGLCD.drawLine(x_p + 75, 85, x_p + 75, 190);
				x_p += 80;
				y_p = 82;
			}
		}

		if (digitalRead(Chanal_B) == HIGH)                                     // ��������������� ��������� ��� ������ ������� "�"
		{
			myGLCD.print(" - Pass ", 170, 40);
		}
		else
		{
			myGLCD.print(" - Error", 170, 40);
			count_error++;
			strcpy_P(buffer, (char*)pgm_read_word(&(table_message[25])));
			myGLCD.print(buffer, 50, 65);                                     // txt_error_connect4
			myGLCD.printNumI(count_error, 190, 65);
			if (x_A < 10)
			{
				myGLCD.printNumI(x_A, x_p + 13, y_p);                         // ������������ ��������� ���������
				myGLCD.print(" - B", x_p + 29, y_p);
			}
			else
			{
				myGLCD.printNumI(x_A, x_p, y_p);                              // ������������ ��������� ���������
				myGLCD.print(" - B", x_p + 29, y_p);
			}
			y_p += 19;
			if (y_p > 190)                                                    // ����� �� ����� ������� ������
			{
				myGLCD.drawLine(x_p + 75, 85, x_p + 75, 190);
				x_p += 80;
				y_p = 82;
			}
		}																        
  	}

	//------------------------------------------------------------------------------------------------------

//	while (!myTouch.dataAvailable()) {}                                         // �������� ��������� �������

	delay(1000);                                                                 // �������� ��������� ������
	myGLCD.print("                                ", CENTER, 20);                                                // 
	myGLCD.print("                   ", 1, 40);
	pinMode(Chanal_A, OUTPUT);                                                   // ���������� �� ����� ����� ������������ U13,U17,U23 (������� ����� � �� ������ ������)
	digitalWrite(Chanal_A, LOW);                                                 // ���������� ����������� ������� �� ������������ U13,U17,U23
	delay(10);                                                                   // ����� �� ������������ ������ Chanal_A
	
	
	for (x_A = 1; x_A < _size_block + 1; x_A++)                                  // ���������������� ������ ��������� ��������.
	{
		chanal_1 = i2c_eeprom_read_byte(deviceaddress, _adr_mem_soft + x_A);     // �������� �� ������� (���� 1) ������� ����������.
		chanal_2 = i2c_eeprom_read_byte(deviceaddress, _adr_mem_soft + x_A + _size_block);     // �������� �� ������� (���� 2) ������� ����������.
		chanal_3 = i2c_eeprom_read_byte(deviceaddress, _adr_mem_soft + x_A + (_size_block*2)); // �������� �� ������� (���� 3) ������� ����������.

		set_komm_mcp('A', x_A, 'O');                                             // ���������� ������� ���� �����������

		if (x_A < 10)
		{
			myGLCD.printNumI(x_A, 30 + 12, 40);
		}
		else
		{
			myGLCD.printNumI(x_A, 30, 40);
		}
		myGLCD.print("<->", 66, 40);

		delay(100);

		for (x_B = 1; x_B < _size_block + 1; x_B++)                      // ���������������� ������ ��������� �������� "�" .
		{
			set_komm_mcp('B', x_B, 'O');                                 // ���������� ������� ���� �����������
			if (chanal_1 < 10)
			{
				myGLCD.print(" ", 117, 40);
				myGLCD.printNumI(chanal_1, 133, 40);
			}
			else
			{
  				myGLCD.printNumI(chanal_1, 117, 40);
			}
			if (digitalRead(Chanal_B) == LOW)                            // ������ ���� ��������
			{
				if (x_B == chanal_1)
				{
					myGLCD.print(" - Pass ", 170, 40);
					//myGLCD.printNumI(chanal_1, 130, 40);
					/*
					if (x_A < 10)
					{
						myGLCD.printNumI(x_A, x_p + 13, y_p);            // ������������ ����������� ���������
						myGLCD.print("-", x_p + 29, y_p);
					}
					else
					{
						myGLCD.printNumI(x_A, x_p, y_p);                 // ������������  ����������� ���������
						myGLCD.print("-", x_p + 29, y_p);
					}
						myGLCD.print(" - Pass ", 170, 40);

					if (chanal_1 < 10)
					{
						myGLCD.printNumI(chanal_1, x_p + 32 + 26, y_p);   // ������������ ������������  ���������
					}
					else
					{
						myGLCD.printNumI(chanal_1, x_p + 32 + 10, y_p);   // ������������ ������������  ���������
					}
					*/
				}
				else if (x_B == chanal_2)
				{
					myGLCD.print(" - Pass ", 170, 40);
				}
				else if (x_B == chanal_3)
				{
					myGLCD.print(" - Pass ", 170, 40);
				}
				else
				{
					if (x_A < 10)
					{
						myGLCD.printNumI(x_A, x_p + 13, y_p);            // ������������ ��������� ���������
						myGLCD.print("-", x_p + 29, y_p);
					}
					else
					{
						myGLCD.printNumI(x_A, x_p, y_p);                 // ������������ ��������� ���������
						myGLCD.print("-", x_p + 29, y_p);
					}
					if (chanal_1 < 10)
					{
						myGLCD.printNumI(chanal_1, x_p + 32 + 26, y_p);   // ������������ ������������  ���������
					}
					else
					{
						myGLCD.printNumI(chanal_1, x_p + 32 + 10, y_p);   // ������������ ������������  ���������
					}
					myGLCD.print(" - Error", 170, 40);
					count_error++;
					myGLCD.print("   ", 164, 65);
					myGLCD.printNumI(count_error, 164, 65);
					y_p += 19;
					if (y_p > 190)                                          // ����� �� ����� ������� ������
					{
						myGLCD.drawLine(x_p + 75, 85, x_p + 75, 190);
						x_p += 80;
						y_p = 82;
						if (x_p > 320)                                                              // ���� ������� ������������ ���������.
						{
							if (x_A >= _size_block)        break;                                   // ���������� ��������� �������� ������ �������  
							while (!myTouch.dataAvailable()) {}                                      // �������� ��������� �������
							myGLCD.setColor(0, 0, 0);
							myGLCD.fillRoundRect(1, 82, 319, 199);
							myGLCD.setColor(255, 255, 255);
							x_p = 1;                                                                 // ���������� ������ ������ ������ �� �
							y_p = 82;                                                                // ���������� ������ ������ ������ �� �
						}
					}
				}
			}
			else
			{
				if (x_B == chanal_1 && chanal_1 != 0 )
				{
					if (x_A < 10)
					{
						myGLCD.printNumI(x_A, x_p + 13, y_p);            // ������������ ��������� ���������
						myGLCD.print("-", x_p + 29, y_p);
					}
					else
					{
						myGLCD.printNumI(x_A, x_p, y_p);                 // ������������ ��������� ���������
						myGLCD.print("-", x_p + 29, y_p);
					}
					if (chanal_1 < 10)
					{
						myGLCD.printNumI(chanal_1, x_p + 32 + 26, y_p);   // ������������ ������������  ���������
					}
					else
					{
						myGLCD.printNumI(chanal_1, x_p + 32 + 10, y_p);   // ������������ ������������  ���������
					}

					myGLCD.print(" - Error", 170, 40);
					count_error++;
					myGLCD.print("   ", 164, 65);
					myGLCD.printNumI(count_error, 164, 65);

					y_p += 19;
					if (y_p > 190)                                          // ����� �� ����� ������� ������
					{
						myGLCD.drawLine(x_p + 75, 85, x_p + 75, 190);
						x_p += 80;
						y_p = 82;
						if (x_p > 320)                                                              // ���� ������� ������������ ���������.
						{
							if (x_A >= _size_block)        break;                                   // ���������� ��������� �������� ������ �������  
							while (!myTouch.dataAvailable()) {}                                      // �������� ��������� �������
							myGLCD.setColor(0, 0, 0);
							myGLCD.fillRoundRect(1, 82, 319, 199);
							myGLCD.setColor(255, 255, 255);
							x_p = 1;                                                                 // ���������� ������ ������ ������ �� �
							y_p = 82;                                                                // ���������� ������ ������ ������ �� �
						}
					}
				}
				if (x_B == chanal_2 && chanal_2 != 0)
				{
					if (x_A < 10)
					{
						myGLCD.printNumI(x_A, x_p + 13, y_p);            // ������������ ��������� ���������
						myGLCD.print("-", x_p + 29, y_p);
					}
					else
					{
						myGLCD.printNumI(x_A, x_p, y_p);                 // ������������ ��������� ���������
						myGLCD.print("-", x_p + 29, y_p);
					}
					if (chanal_1 < 10)
					{
						myGLCD.printNumI(chanal_2, x_p + 32 + 26, y_p);   // ������������ ������������  ���������
					}
					else
					{
						myGLCD.printNumI(chanal_2, x_p + 32 + 10, y_p);   // ������������ ������������  ���������
					}

					myGLCD.print(" - Error", 170, 40);
					count_error++;
					myGLCD.print("   ", 164, 65);
					myGLCD.printNumI(count_error, 164, 65);

					y_p += 19;
					if (y_p > 190)                                          // ����� �� ����� ������� ������
					{
						myGLCD.drawLine(x_p + 75, 85, x_p + 75, 190);
						x_p += 80;
						y_p = 82;
						if (x_p > 320)                                                              // ���� ������� ������������ ���������.
						{
							if (x_A >= _size_block)        break;                                   // ���������� ��������� �������� ������ �������  
							while (!myTouch.dataAvailable()) {}                                      // �������� ��������� �������
							myGLCD.setColor(0, 0, 0);
							myGLCD.fillRoundRect(1, 82, 319, 199);
							myGLCD.setColor(255, 255, 255);
							x_p = 1;                                                                 // ���������� ������ ������ ������ �� �
							y_p = 82;                                                                // ���������� ������ ������ ������ �� �
						}
					}
				}
				if (x_B == chanal_3 && chanal_3 != 0)
				{
					if (x_A < 10)
					{
						myGLCD.printNumI(x_A, x_p + 13, y_p);            // ������������ ��������� ���������
						myGLCD.print("-", x_p + 29, y_p);
					}
					else
					{
						myGLCD.printNumI(x_A, x_p, y_p);                 // ������������ ��������� ���������
						myGLCD.print("-", x_p + 29, y_p);
					}
					if (chanal_1 < 10)
					{
						myGLCD.printNumI(chanal_3, x_p + 32 + 26, y_p);   // ������������ ������������  ���������
					}
					else
					{
						myGLCD.printNumI(chanal_3, x_p + 32 + 10, y_p);   // ������������ ������������  ���������
					}

					myGLCD.print(" - Error", 170, 40);
					count_error++;
					myGLCD.print("   ", 164, 65);
					myGLCD.printNumI(count_error, 164, 65);

					y_p += 19;
					if (y_p > 190)                                          // ����� �� ����� ������� ������
					{
						myGLCD.drawLine(x_p + 75, 85, x_p + 75, 190);
						x_p += 80;
						y_p = 82;
						if (x_p > 320)                                                              // ���� ������� ������������ ���������.
						{
							if (x_A >= _size_block)        break;                                   // ���������� ��������� �������� ������ �������  
							while (!myTouch.dataAvailable()) {}                                      // �������� ��������� �������
							myGLCD.setColor(0, 0, 0);
							myGLCD.fillRoundRect(1, 82, 319, 199);
							myGLCD.setColor(255, 255, 255);
							x_p = 1;                                                                 // ���������� ������ ������ ������ �� �
							y_p = 82;                                                                // ���������� ������ ������ ������ �� �
						}
					}
				}
			}
		}
	}
	
	strcpy_P(buffer, (char*)pgm_read_word(&(table_message[30])));
	myGLCD.print(buffer, CENTER, 20);                  // txt__test_end
}

void set_adr_EEPROM()
{
	adr_memN1_1 = 100;                       // ��������� ����� ������ ������� ������������ ��������� �������� �1�, �1�
	adr_memN1_2 = adr_memN1_1 + sizeof(connektN1_default) + 1;                   // ��������� ����� ������ ������� ������������ ��������� �������� �2�, �2�
	adr_memN1_3 = adr_memN1_2 + sizeof(connektN2_default) + 1;                   // ��������� ����� ������ ������� ������������ ��������� �������� �3�, �3�
	adr_memN1_4 = adr_memN1_3 + sizeof(connektN3_default) + 1;                   // ��������� ����� ������ ������� ������������ ��������� �������� �4�, �4�
	//++++++++++++++++++���� � 2 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	adr_memN1_5 = adr_memN1_4 + sizeof(connektN4_default) + 1;                   // ��������� ����� ������ ������� ������������ ��������� �������� �1�, �1�

	adr_memN2_1 = adr_memN1_5+sizeof(connekBVS_default)+1;                       // ��������� ����� ������ ������� ������������ ��������� �������� �2�, �2�
	adr_memN2_2 = adr_memN2_1+240+1;                                             // ��������� ����� ������ ������� ������������ ��������� �������� �2�, �2�
	adr_memN2_3 = adr_memN2_2+240+1;                                             // ��������� ����� ������ ������� ������������ ��������� �������� �3�, �3�
	adr_memN2_4 = adr_memN2_3+240+1;                                             // ��������� ����� ������ ������� ������������ ��������� �������� �4�, �4�
}
void setup_pin()
{
	pinMode(led_Red, OUTPUT);                             //
	pinMode(led_Green, OUTPUT);                           //
	digitalWrite(led_Red, HIGH);                          //
	digitalWrite(led_Green, LOW);                         //
	pinMode(Chanal_A, INPUT);                                   // ����� ������������ ����� �
	pinMode(Chanal_B, INPUT);                                   // ����� ������������ ����� �
	pinMode(Rele1 , OUTPUT);                             //
	pinMode(Rele2 , OUTPUT);                             //
	pinMode(Rele2 , OUTPUT);                             //
	digitalWrite(Rele1 , LOW);                         //
	digitalWrite(Rele2 , LOW);                         //
	digitalWrite(Rele3 , LOW);                         //
}
void setup_mcp()
{
  // ��������� ����������� ������

  mcp_Out1.begin(1);                               //  ����� (1) U6 �������  ����������� ������
  mcp_Out1.pinMode(0, OUTPUT);                     //  1A1
  mcp_Out1.pinMode(1, OUTPUT);                     //  1B1
  mcp_Out1.pinMode(2, OUTPUT);                     //  1C1
  mcp_Out1.pinMode(3, OUTPUT);                     //  1D1
  mcp_Out1.pinMode(4, OUTPUT);                     //  1A2
  mcp_Out1.pinMode(5, OUTPUT);                     //  1B2
  mcp_Out1.pinMode(6, OUTPUT);                     //  1C2
  mcp_Out1.pinMode(7, OUTPUT);                     //  1D2

  mcp_Out1.pinMode(8, OUTPUT);                     //  1E1   U13  ����� � in/out
  mcp_Out1.pinMode(9, OUTPUT);                     //  1E2   U17  ����� � in/out
  mcp_Out1.pinMode(10, OUTPUT);                    //  1E3   U23  ����� � in/out
  mcp_Out1.pinMode(11, OUTPUT);                    //  1E4   U14  ����� � GND
  mcp_Out1.pinMode(12, OUTPUT);                    //  1E5   U19  ����� � GND
  mcp_Out1.pinMode(13, OUTPUT);                    //  1E6   U21  ����� � GND
  mcp_Out1.pinMode(14, OUTPUT);                    //  1E7   ��������
  mcp_Out1.pinMode(15, OUTPUT);                    //  1E8   ��������

  mcp_Out2.begin(2);                               //  ����� (2) U9 �������  ����������� ������
  mcp_Out2.pinMode(0, OUTPUT);                     //  2A1
  mcp_Out2.pinMode(1, OUTPUT);                     //  2B1
  mcp_Out2.pinMode(2, OUTPUT);                     //  2C1
  mcp_Out2.pinMode(3, OUTPUT);                     //  2D1
  mcp_Out2.pinMode(4, OUTPUT);                     //  2A2
  mcp_Out2.pinMode(5, OUTPUT);                     //  2B2
  mcp_Out2.pinMode(6, OUTPUT);                     //  2C2
  mcp_Out2.pinMode(7, OUTPUT);                     //  2D2

  mcp_Out2.pinMode(8, OUTPUT);                     //  2E1   U15  ����� B in/out
  mcp_Out2.pinMode(9, OUTPUT);                     //  2E2   U18  ����� B in/out
  mcp_Out2.pinMode(10, OUTPUT);                    //  2E3   U22  ����� B in/out
  mcp_Out2.pinMode(11, OUTPUT);                    //  2E4   U16  ����� B GND
  mcp_Out2.pinMode(12, OUTPUT);                    //  2E5   U20  ����� B GND
  mcp_Out2.pinMode(13, OUTPUT);                    //  2E6   U24  ����� B GND
  mcp_Out2.pinMode(14, OUTPUT);                    //  2E7   ���� �1, �2
  mcp_Out2.pinMode(15, OUTPUT);                    //  2E8   ��������
  for (int i = 0; i < 16; i++)
  {
	mcp_Out1.digitalWrite(i, HIGH);
	mcp_Out2.digitalWrite(i, HIGH);
  }
  //mcp_Out2.digitalWrite(14, LOW);                 // ��������� ����
}

void setup_regModbus()
{
  regBank.setId(1);    // Slave ID 1

  regBank.add(1);      //
  regBank.add(2);      //
  regBank.add(3);      //
  regBank.add(4);      //
  regBank.add(5);      //
  regBank.add(6);      //
  regBank.add(7);      //
  regBank.add(8);      //

  regBank.add(10001);  //
  regBank.add(10002);  //
  regBank.add(10003);  //
  regBank.add(10004);  //
  regBank.add(10005);  //
  regBank.add(10006);  //
  regBank.add(10007);  //
  regBank.add(10008);  //

  regBank.add(30001);  //
  regBank.add(30002);  //
  regBank.add(30003);  //
  regBank.add(30004);  //
  regBank.add(30005);  //
  regBank.add(30006);  //
  regBank.add(30007);  //
  regBank.add(30008);  //

  regBank.add(40001);  //  ����� �������� ������� �� ����������
  regBank.add(40002);  //  ����� �������� ���� ������
  regBank.add(40003);  //  ����� �������� �������� ������� ���������� � 1
  regBank.add(40004);  //  ����� �������� �������� ������� ���������� � 2
  regBank.add(40005);  //  ����� ����� ��������� ��� �������� � �� ������.
  regBank.add(40006);  //  ����� ����� ������ ��� �������� � �� ������.
  regBank.add(40007);  //  ����� ����� ����� ������
  regBank.add(40008);  //  ����� ����� ������ �� ���������
  regBank.add(40009);  //

  regBank.add(40010);  //  �������� ���������� �������� ��� �������� �������
  regBank.add(40011);
  regBank.add(40012);
  regBank.add(40013);
  regBank.add(40014);
  regBank.add(40015);
  regBank.add(40016);
  regBank.add(40017);
  regBank.add(40018);
  regBank.add(40019);

  regBank.add(40020);
  regBank.add(40021);
  regBank.add(40022);
  regBank.add(40023);
  regBank.add(40024);
  regBank.add(40025);
  regBank.add(40026);
  regBank.add(40027);
  regBank.add(40028);
  regBank.add(40029);

  regBank.add(40030);
  regBank.add(40031);
  regBank.add(40032);
  regBank.add(40033);
  regBank.add(40034);
  regBank.add(40035);
  regBank.add(40036);
  regBank.add(40037);
  regBank.add(40038);
  regBank.add(40039);

  regBank.add(40040);
  regBank.add(40041);
  regBank.add(40042);
  regBank.add(40043);
  regBank.add(40044);
  regBank.add(40045);
  regBank.add(40046);
  regBank.add(40047);
  regBank.add(40048);
  regBank.add(40049);
  // ������� �����
  regBank.add(40050);  // ����� ���� ������ ����� �����������
  regBank.add(40051);  // ����� ����� ������ ����� �����������
  regBank.add(40052);  // ����� ��� ������ ����� �����������
  regBank.add(40053);  // ����� ��� ������ ����� �����������
  regBank.add(40054);  // ����� ������ ������ ����� �����������
  regBank.add(40055);  // ����� ������� ������ ����� �����������
  // ��������� ������� � �����������
  regBank.add(40056);  // ����� ����
  regBank.add(40057);  // ����� �����
  regBank.add(40058);  // ����� ���
  regBank.add(40059);  // ����� ���
  regBank.add(40060);  // ����� ������
  regBank.add(40061);  //
  regBank.add(40062);  //
  regBank.add(40063);  //
  slave._device = &regBank;
}

void setup()
{
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myTouch.InitTouch();
  delay(1000);
  //myTouch.setPrecision(PREC_MEDIUM);
  myTouch.setPrecision(PREC_HI);
  //myTouch.setPrecision(PREC_EXTREME);
  myButtons.setTextFont(BigFont);
  myButtons.setSymbolFont(Dingbats1_XL);
  Serial.begin(9600);                                    // ����������� � USB ��
  Serial1.begin(115200);                                 // ����������� �
  slave.setSerial(3, 57600);                             // ����������� � ��������� MODBUS ���������� Serial3
  Serial2.begin(115200);                                 // ����������� �
  setup_pin();
  Wire.begin();
  if (!RTC.begin())                                      // ��������� �����
  {
	Serial.println(F("RTC failed"));
	while (1);
  };
  //DateTime set_time = DateTime(16, 3, 15, 10, 19, 0);  // ������� ������ � ������� � ������ "set_time" ���, �����, �����, �����...
  //RTC.adjust(set_time);                                // �������� ����
  Serial.println(" ");
  Serial.println(F(" ***** Start system  *****"));
  Serial.println(" ");
  //set_time();
  serial_print_date();
  setup_mcp();                                          // ��������� ����� ����������
  //mcp_Out2.digitalWrite(15, LOW);                     // ��������� ���� ����������� "�����" �� ����� ��������//
  //mcp_Out2.digitalWrite(14, LOW);                     // ��������� ���� ������� +12�. �� ����� ��������
  MsTimer2::set(300, flash_time);                       // 300ms ������ ������� ���������
  setup_regModbus();

  Serial.print(F("FreeRam: "));
  Serial.println(FreeRam());
  wait_time_Old =  millis();
  digitalWrite(led_Green, HIGH);                          
  digitalWrite(led_Red, LOW);                           
  set_adr_EEPROM();
  Serial.println(" ");                                   
  Serial.println(F("System initialization OK!."));        // ���������� � ���������� ���������

}

void loop()
{
  draw_Glav_Menu();
  swichMenu();
}
