/*
Sun_Base.ino
Visual Studio 2015
VisualMicro

��������� ���������� ��������� ����������.
������:               - 1.0
�����:                - �������� �.�.
���� ������ �����:    - 16.04.2017�.
���� ��������� �����: - 00.00.2017�.
 
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
#include <OneWire.h>
#include <HMC5883L.h>
#include <DallasTemperature.h>




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

unsigned int adr_memN1_1 = 0;                           // ��������� ����� ������ ������� ������������ ��������� �������� �1�, �1�
unsigned int adr_memN1_2 = 0;                           // ��������� ����� ������ ������� ������������ ��������� �������� �2�, �2�

unsigned int adr_tempN1 = 10;                           // ����� ������ ������ ������� ����������� �1
unsigned int adr_tempN2 = 12;                           // ����� ������ ������ ������� ����������� �2
unsigned int adr_tempN3 = 14;                           // ����� ������ ������ ������� ����������� �3
unsigned int adr_tempN4 = 16;                           // ����� ������ ������ ������� ����������� �4
unsigned int adr_radius = 18;                           // ����� ������ ������ ������� �������
unsigned int adr_asimut = 20;                           // ����� ������ ������ ������� ���������


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

//++++++++++++++++++++++++ ��������� ����������� +++++++++++++++++++++++++++++++++
float temp_sun_in = 0.00;
float temp_sun_out = 0.00;
float temp_tube_in = 0.00;
float temp_tube_out = 0.00;
float temp_tank = 0.00;
float temp_out = 0.00;



#define ONE_WIRE_BUS_1 44
#define ONE_WIRE_BUS_2 45
#define ONE_WIRE_BUS_3 46
#define ONE_WIRE_BUS_4 47
#define ONE_WIRE_BUS_5 48
#define ONE_WIRE_BUS_6 49

OneWire oneWire_sun_in(ONE_WIRE_BUS_1);
OneWire oneWire_sun_out(ONE_WIRE_BUS_2);
OneWire oneWire_tube_in(ONE_WIRE_BUS_3);
OneWire oneWire_tube_out(ONE_WIRE_BUS_4);
OneWire oneWire_tank(ONE_WIRE_BUS_5);
OneWire oneWire_out(ONE_WIRE_BUS_6);

DallasTemperature sensor_sun_in(&oneWire_sun_in);
DallasTemperature sensor_sun_out(&oneWire_sun_out);
DallasTemperature sensor_tube_in(&oneWire_tube_in);
DallasTemperature sensor_tube_out(&oneWire_tube_out);
DallasTemperature sensor_tank(&oneWire_tank);
DallasTemperature sensor_outhouse(&oneWire_out);

//++++++++++++++++++++++++++++++++++ compass +++++++++++++++++++++++++++++++++++++++++


HMC5883L compass;
float headingDegrees = 0.00;
bool compass_enable1 = false;


int minX = 0;
int maxX = 0;
int minY = 0;
int maxY = 0;
int offX = 0;
int offY = 0;

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
const char* str[]              = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
const char* str1[]             = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
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

int m2 = 1; // ���������� ������ ����
int m3 = 0; // ���������� ������ ����

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
const char  txt_info1[]             PROGMEM  = "\x89""o""\x9F""a""\x9C""a""\xA2\x9D\xAF"" ""\x99""a""\xA4\xA7\x9D\x9F""o""\x97"; // ��������� ��������
const char  txt_info2[]             PROGMEM  = "=========";                                                              // ================  
const char  txt_info3[]             PROGMEM  = "Tec""\xA4"" o""\x96""y""\xA7"". ""\xA4""a""\x96\xA0\x9D\xA6""ax ";       // ���� �� ����. �������� 
const char  txt_info4[]             PROGMEM  = "Hac\xA4po\x9E\x9F""a c\x9D""c\xA4""e\xA1\xAB";                           // ��������� ������� 
const char  txt_info5[]             PROGMEM  = "Hac\xA4po\x9E\x9F""a c\x9D""c\xA4""e\xA1\xAB";                           // ��������� ������� 
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
  Serial.println(str1[now.dayOfWeek()]);
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
  myGLCD.print(str[dow], 256, 8);

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

	  if (((y >= 200) && (y <= 239)) && ((x >= 1) && (x <= 250))) //������� 
	  {
		//myGLCD.clrScr();
		myGLCD.setFont(BigFont);
		break;
	  }
	  if (((y >= 1) && (y <= 199)) && ((x >= 1) && (x <= 319))) //�������
	  {
		//myGLCD.clrScr();
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
	/*if (m2 == 1 && m3 == 0)
	{
		myGLCD.setColor(255, 255, 255);
		myGLCD.setBackColor(0, 0, 255);
		myGLCD.printNumI(100, 18, 54);
		myGLCD.printNumI(101, 96, 54);
		myGLCD.printNumI(102, 174, 54);
		myGLCD.printNumI(103, 252, 54);
		myGLCD.setBackColor(0, 0, 0);
	}*/
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
	1 -    
	2 -    
	3 -    
	4 -    
	5 -    
	6 -    
	7 -    
	8 -    
	9 -    
	10 -   
	11 -   
	12 -   
	13 -   
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
	
		break;
	  case 3:
	
		break;
	  case 4:
	
		break;
	  case 5:
		break;
	  case 6:
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
	 
		break;
	  case 13:
	 
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

void Set_Down_Buttons()
{
	for (x = 0; x < 5; x++)
	{
		myGLCD.setColor(0, 0, 255);
		myGLCD.fillRoundRect(10 + (x * 60), 189, 60 + (x * 60), 239);
		myGLCD.setColor(255, 255, 255);
		myGLCD.drawRoundRect(10 + (x * 60), 189, 60 + (x * 60), 239);
		myGLCD.setBackColor(0, 0, 255);
		myGLCD.printNumI(x + 1, 27 + (x * 60), 206);
	}
	myGLCD.setBackColor(0, 0, 0);
}


void draw_Glav_Menu1()
{
	myGLCD.clrScr();
	Set_Down_Buttons();
	draw_measure();
}


void draw_Glav_Menu()
{
  myGLCD.clrScr();
  myButtons.deleteAllButtons();
  Set_Down_Buttons();
  myButtons.drawButtons(); // ������������ ������
  myGLCD.setColor(VGA_BLACK);
  myGLCD.setBackColor(VGA_WHITE);
  myGLCD.setColor(0, 255, 0);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.print("                      ", CENTER, 0);
 // draw_measure();
  if (m2 == 1)
  {
	  draw_measure();

  }
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

void swichMenu1()
{
	m2 = 1;
	while (1)
	{
		measure_view();

    	wait_time = millis();                                      // ��������� ������ ����� ��� �������
		if (wait_time - wait_time_Old > 60000 * time_minute)
		{
			wait_time_Old = millis();
			// AnalogClock();
			// myGLCD.clrScr();
		}

		if (myTouch.dataAvailable() == true)                       // ��������� ������� ������
		{

			myTouch.read();
			x = myTouch.getX();
			y = myTouch.getY();

			if ((y >= 189) && (y <= 239))                          //������ ������ 
			{
				myGLCD.setFont(BigFont);
				
				if ((x >= 10) && (x <= 60))                       //������ ������ "1"
				{
					waitForIt(10, 189, 60, 239);
					m2 = 1;
					draw_measure();                               // ���������� ��������� ���������



				}

				if ((x >= 70) && (x <= 120))                      //������ ������ "2"
				{
					waitForIt(70, 189, 120, 239);
					m2 = 2;
					clear_display();                             // �������� �����



				}

				if ((x >= 130) && (x <= 180))                     //������ ������ "3"
				{
					waitForIt(130, 189, 180, 239);
					m2 = 3;
					clear_display();                             // �������� �����



				}

				if ((x >= 190) && (x <= 240))                    //������ ������ "4"
				{
					waitForIt(190, 189, 240, 239);
					m2 = 4;
					clear_display();                             // �������� �����



				}

				if ((x >= 250) && (x <= 300))                    //������ ������ "5"
				{
					waitForIt(250, 189, 300, 239);
					m3 = 1;
					AnalogClock();
					myGLCD.clrScr();
					Set_Down_Buttons();
					m3 = 0;
					if (m2 == 1)
					{
						draw_measure();                          // ���������� ��������� ���������

					}
				}
			}
		}
	}
}


void measure_view()
{
	
	if (m2 == 1 && m3 == 0)
	{
		myGLCD.setColor(255, 255, 255);
		myGLCD.setBackColor(0, 0, 255);
		myGLCD.printNumF(temp_sun_in, 2, 40, 26);
		myGLCD.printNumF(temp_sun_out, 2, 196, 26);
		myGLCD.printNumF(temp_tube_in, 2, 40, 66);
		myGLCD.printNumF(temp_tube_out, 2, 196, 66);
		myGLCD.printNumF(temp_tank, 2, 40, 106);
		myGLCD.printNumF(temp_out, 2, 196, 106);
		myGLCD.printNumF(headingDegrees, 2, 40, 153);
		myGLCD.printNumF(temp_out, 2, 196, 153);
		myGLCD.setBackColor(0, 0, 0);
		
	}
}



int read_int_eeprom(unsigned int adr)
{
	unsigned int res_eeprom;
	hi = i2c_eeprom_read_byte(deviceaddress, adr);                // 
	low = i2c_eeprom_read_byte(deviceaddress, adr + 1);
	res_eeprom = (hi << 8) | low;
	return res_eeprom;
}

void save_int_eeprom(unsigned int adr, unsigned int res)
{
	hi = highByte(res);
	low = lowByte(res);
	// ��� �� ��� hi,low ����� ��������� EEPROM
	i2c_eeprom_write_byte(deviceaddress, adr, hi);
	i2c_eeprom_write_byte(deviceaddress, adr + 1, low);
}

void clear_eeprom(int start, int long_mem)
{
	for (int i = start; i < long_mem; i++)                            // �������� ����� ��������� � ������.        
	{
		i2c_eeprom_write_byte(deviceaddress, i, 0);
	}

}

void read_Temperatures()
{

	sensor_sun_in.requestTemperatures();
	sensor_sun_out.requestTemperatures();
	sensor_tube_in.requestTemperatures();
	sensor_tube_out.requestTemperatures();
	sensor_tank.requestTemperatures();
	sensor_outhouse.requestTemperatures();

	temp_sun_in = sensor_sun_in.getTempCByIndex(0);
	temp_sun_out = sensor_sun_out.getTempCByIndex(0);
	temp_tube_in = sensor_tube_in.getTempCByIndex(0);
	temp_tube_out = sensor_tube_out.getTempCByIndex(0);
	temp_tank = sensor_tank.getTempCByIndex(0);
	temp_out = sensor_outhouse.getTempCByIndex(0);
	
	Serial.print("sensor_sun_in: ");
	Serial.println(temp_sun_in);

	Serial.print("sensor_sun_out: ");
	Serial.println(temp_sun_out);

	Serial.print("sensor_tube_in: ");
	Serial.println(temp_tube_in);

	Serial.print("sensor_tube_out: ");
	Serial.println(temp_tube_out);

	Serial.print("sensor_tank: ");
	Serial.println(temp_tank);

	Serial.print("Outhouse: ");
	Serial.println(temp_out);

}

void read_compass()
{
	Vector norm = compass.readNormalize();

	// Calculate heading
	float heading = atan2(norm.YAxis, norm.XAxis);

	// Set declination angle on your location and fix heading
	// You can find your declination on: http://magnetic-declination.com/
	// (+) Positive or (-) for negative
	// For Bytom / Poland declination angle is 4'26E (positive)
	// Formula: (deg + (min / 60.0)) / (180 / M_PI);
	float declinationAngle = (4.0 + (26.0 / 60.0)) / (180 / M_PI);
	heading += declinationAngle;

	// Correct for heading < 0deg and heading > 360deg
	if (heading < 0)
	{
		heading += 2 * PI;
	}

	if (heading > 2 * PI)
	{
		heading -= 2 * PI;
	}

	// Convert to degrees
	headingDegrees = heading * 180 / M_PI;

	// Output
	Serial.print(" Heading = ");
	Serial.print(heading);
	Serial.print(" Degress = ");
	Serial.print(headingDegrees);
	Serial.println();
}

void clear_display()
{
	myGLCD.setColor(0, 0, 0);
	myGLCD.fillRoundRect(1, 1, 319, 188);
	myGLCD.setColor(255, 255, 255);
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
	 // AnalogClock();
	  myGLCD.clrScr();
	  myButtons.drawButtons();                               // ������������ ������
	  print_up();                                            // ������������ ������� ������
	}
	
	myButtons.setTextFont(BigFont);                          // ���������� ������� ����� ������

	if (myTouch.dataAvailable() == true)                     // ��������� ������� ������
	{
	  pressed_button = myButtons.checkButtons();             // ���� ������ - ��������� ��� ������
	  delay(100);
	  wait_time_Old =  millis();

	  if (pressed_button == butX)                            // ������ - ����� ����
	  {
		m3 = 1;
		AnalogClock();
		myGLCD.clrScr();
		myButtons.drawButtons();                             // ������������ ������
		print_up();                                          // ������������ ������� ������
		m3 = 0;
		if (m2 == 1)
		{
			draw_measure();

		}
	  }

	  if (pressed_button == but_m1)                          // ������ 1 �������� ����
	  {
		m2 = 1;                                              // ���������� ������ �������� ����
		myGLCD.clrScr();
		myButtons.setButtonColors(VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_BLUE); // ������� ��� ����
		myButtons.deleteAllButtons();
		Set_Down_Buttons();
		myButtons.drawButtons();                             // ������������ ������
		default_colors = true;
		myGLCD.setColor(0, 255, 0);
		myGLCD.setBackColor(0, 0, 0);
		myGLCD.print("                      ", CENTER, 0);
		strcpy_P(buffer, (char*)pgm_read_word(&(table_message[2])));
		myGLCD.print(buffer, CENTER, 0);                     // txt_info1 "���� �������"

		draw_measure();

	  }
	  else if (pressed_button == but_m2)
	  {
		m2 = 2;
	    myGLCD.clrScr();
		myButtons.deleteAllButtons();
		myButtons.setButtonColors(VGA_WHITE, VGA_RED, VGA_YELLOW, VGA_BLUE, VGA_TEAL);
		but1   = myButtons.addButton( 10,  20, 250,  35, txt_menu2_1);
		but2   = myButtons.addButton( 10,  65, 250,  35, txt_menu2_2);
		but3   = myButtons.addButton( 10, 110, 250,  35, txt_menu2_3);
		but4   = myButtons.addButton( 10, 155, 250,  35, txt_menu2_4);
		Set_Down_Buttons();
		myButtons.drawButtons();
		default_colors = false;
		myGLCD.setColor(0, 255, 0);
		myGLCD.setBackColor(0, 0, 0);
		myGLCD.print("                      ", CENTER, 0);
		strcpy_P(buffer, (char*)pgm_read_word(&(table_message[3])));
		myGLCD.print(buffer, CENTER, 0);                     // txt_info2 ���� ����� ��������
	  }

	  else if (pressed_button == but_m3)
	  {
		m2 = 3;
		myGLCD.clrScr();
		myButtons.deleteAllButtons();
		myButtons.setButtonColors(VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_GREEN);
		but1 = myButtons.addButton(10, 20, 250, 35, txt_menu3_1);
		but2 = myButtons.addButton(10, 65, 250, 35, txt_menu3_2);
		but3 = myButtons.addButton(10, 110, 250, 35, txt_menu3_3);
		but4 = myButtons.addButton(10, 155, 250, 35, txt_menu3_4);
		Set_Down_Buttons();
		myButtons.drawButtons();
		default_colors = false;
		myGLCD.setColor(0, 255, 0);
		myGLCD.setBackColor(0, 0, 0);
		myGLCD.print("                      ", CENTER, 0);
		strcpy_P(buffer, (char*)pgm_read_word(&(table_message[4])));
		myGLCD.print(buffer, CENTER, 0);                     // txt_info3 ��������� �������
	  }
	  else if (pressed_button == but_m4)
	  {
	    m2 = 4;
		myGLCD.clrScr();
		myButtons.deleteAllButtons();
		myButtons.setButtonColors(VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_RED);
		but1 = myButtons.addButton(10, 20, 250, 35, txt_menu4_1);
		but2 = myButtons.addButton(10, 65, 250, 35, txt_menu4_2);
		but3 = myButtons.addButton(10, 110, 250, 35, txt_menu4_3);
		but4 = myButtons.addButton(10, 155, 250, 35, txt_menu4_4);
		Set_Down_Buttons();
		myButtons.drawButtons();
		default_colors = false;
		myGLCD.setColor(0, 255, 0);
		myGLCD.setBackColor(0, 0, 0);
		myGLCD.print("                      ", CENTER, 0);
		strcpy_P(buffer, (char*)pgm_read_word(&(table_message[5])));
		myGLCD.print(buffer, CENTER, 0);                     // txt_info4 
	  }

	  else if (pressed_button == but_m5)
	  {
		m2 = 5;
		myGLCD.clrScr();
		myButtons.deleteAllButtons();
		myButtons.setButtonColors(VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_NAVY);
		but1 = myButtons.addButton(10, 20, 250, 35, txt_menu5_1);
		but2 = myButtons.addButton(10, 65, 250, 35, txt_menu5_2);
		but3 = myButtons.addButton(10, 110, 250, 35, txt_menu5_3);
		but4 = myButtons.addButton(10, 155, 250, 35, txt_menu5_4);
		Set_Down_Buttons();
		myButtons.drawButtons();
		default_colors = false;
		myGLCD.setColor(0, 255, 0);
		myGLCD.setBackColor(0, 0, 0);
		myGLCD.print("                      ", CENTER, 0);
		strcpy_P(buffer, (char*)pgm_read_word(&(table_message[6])));
		myGLCD.print(buffer, CENTER, 0);                     // txt_info5  �����������
	  }

	//  //*****************  ���� �1  **************

	//  if (pressed_button == but1 && m2 == 1)
	//  {
	// 
	//	myGLCD.clrScr();
	//	myButtons.drawButtons();
	//	print_up();
	//  }

	//  if (pressed_button == but2 && m2 == 1)
	//  {
	// 
	//	myGLCD.clrScr();
	//	myButtons.drawButtons();
	//	print_up();
	//  }

	//  if (pressed_button == but3 && m2 == 1)
	//  {
	// 
	//	myGLCD.clrScr();
	//	myButtons.drawButtons();
	//	print_up();
	//  }
	//  if (pressed_button == but4 && m2 == 1)
	//  {
	// 
	//	myGLCD.clrScr();
	//	myButtons.drawButtons();
	//	print_up();
	//  }

	//  //*****************  ���� �2  **************


	//  if (pressed_button == but1 && m2 == 2)
	//  {
	// 
	//	myGLCD.clrScr();
	//	myButtons.drawButtons();
	//	print_up();
	//  }

	//  if (pressed_button == but2 && m2 == 2)
	//  {
	// 
	//	myGLCD.clrScr();
	//	myButtons.drawButtons();
	//	print_up();
	//  }

	//  if (pressed_button == but3 && m2 == 2)
	//  {
 //
	//	myGLCD.clrScr();
	//	myButtons.drawButtons();
	//	print_up();
	//  }
	//  if (pressed_button == but4 && m2 == 2)
	//  {

	//	myGLCD.clrScr();
	//	myButtons.drawButtons();
	//	print_up();
	//  }

	//  //*****************  ���� �3  **************
	//  if (pressed_button == but1 && m2 == 3)                 // ������ ����� ���� 3
	//  {
	// 
	//	 myGLCD.clrScr();                                    // �������� �����
	//	 myButtons.drawButtons();
	//	 print_up();
	//  }

	//  //--------------------------------------------------------------
	//  if (pressed_button == but2 && m2 == 3)                 // ������ ����� ���� 3
	//  {
	// 
	//	myGLCD.clrScr();
	//	myButtons.drawButtons();
	//	print_up();
	//  }

	//  //------------------------------------------------------------------

	//  if (pressed_button == but3 && m2 == 3)                 // ������ ����� ���� 3
	//  {
	// 
	//	myGLCD.clrScr();
	//	myButtons.drawButtons();
	//	print_up();
	//  }

	//  //------------------------------------------------------------------
	//  if (pressed_button == but4 && m2 == 3)                 // ��������� ����� ���� 3
	//  {
	// 
	//	myGLCD.clrScr();
	//	myButtons.drawButtons();
	//	print_up();
	//  }

	//  //*****************  ���� �4  **************

	//  if (pressed_button == but1 && m2 == 4) //
	//  {
	// 
	//	  myGLCD.clrScr();                                    // �������� �����
	//	  myButtons.drawButtons();
	//	  print_up();
	//  }

	//  if (pressed_button == but2 && m2 == 4)
	//  {
	// 
	//	  myGLCD.clrScr();                                     // �������� �����
	//	  myButtons.drawButtons();
	//	  print_up();;
	//  }

	//  if (pressed_button == but3 && m2 == 4) //
	//  {
	// 
	//	  myGLCD.clrScr();
	//	  myButtons.drawButtons();
	//	  print_up();
	//  }
	//  if (pressed_button == but4 && m2 == 4) //
	//  {
	// 
	//	  myGLCD.clrScr();
	//	  myButtons.drawButtons();
	//	  print_up();
	//  }
	  //*****************  ���� �5  **************

	//  if (pressed_button == but1 && m2 == 5)                 // ����� ������
	//  {
	// 

	///*	myGLCD.clrScr();
	//	myButtons.drawButtons();
	//	print_up();*/
	//  }
	//  if (pressed_button == but2 && m2 == 5)
	//  {
	// 

	//	//myGLCD.clrScr();                                     // �������� �����
	// //  	myButtons.drawButtons();
	//	//print_up();
	//  }

	//  if (pressed_button == but3 && m2 == 5)                 // ���� 
	//  {
	///*	  myGLCD.clrScr();
	//	  myButtons.drawButtons();
	//	  print_up();*/
	//  }

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
		 // myGLCD.print("HET", 220, 220);
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
 // if (search_cabel(40) == 1)                                                 // ������ ������ �1
 // {
	//test_cabel_N1_run();                                                   // ��������� ��������
	//while (true)                                                           // �������� ��������� �������
	//{

	//  if (myTouch.dataAvailable())
	//  {
	//	myTouch.read();
	//	x = myTouch.getX();
	//	y = myTouch.getY();

	//	if (((y >= 200) && (y <= 239)) && ((x >= 5) && (x <= 155)))   //������ ������ "��������� ��������"
	//	{
	//	  waitForIt(5, 200, 155, 239);
	//	  myGLCD.setFont(BigFont);
	//	  test_cabel_N1_run();                                  // ��������� ��������� ��������
	//	}
	//	if (((y >= 200) && (y <= 239)) && ((x >= 160) && (x <= 315))) //������ ������ "���������  ��������"
	//	{
	//	  waitForIt(160, 200, 315, 239);
	//	  myGLCD.setFont(BigFont);
	//	  break;                                                // ����� �� ���������
	//	}
	//  }

	//}
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
														  
  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[24])));
  myGLCD.print(buffer, 50, 65);                                                // txt_error_connect3 "������ ���"
 // if (search_cabel(40) == 1)                                                   // ��������� ������������ ����������� ������ �1
 // {
	//digitalWrite(Chanal_A, LOW);                                                     // ���������� ����������� ������� �� ������������ U13,U17,U23
	//delay(10);                                                                 // ����� �� ������������ ������ Chanal_A
	//for (x_A = 1; x_A < _size_block + 1; x_A++)                                // ���������������� ������ ��������� ��������.
	//{
	//  canal_N = i2c_eeprom_read_byte(deviceaddress, adr_memN1_1 + x_A);        // �������� � ������ �� EEPROM
	//  ware_on = i2c_eeprom_read_byte(deviceaddress, adr_memN1_1 + x_A + (_size_block * 4)); // �������� �� ������� ������� ����������.
	//  if (canal_N == 1)                                                        // 40 ����� ��� �������� ������ ������������ �������
	//  {
	//	set_komm_mcp('A', 40, 'O');                                            // ���������� ���� ����������� �� ����������� 40 �����
	//  }
	//  else
	//  {
	//	set_komm_mcp('A', canal_N, 'O');                                 // ���������� ������� ���� �����������
	//  }
	//  // ��������������� ��������� ��� ������ ������� "�"
	//  // ��������� ��� ������ ������� "�"
	//  for (x_B = 1; x_B < _size_block + 1; x_B++)                          // ���������������� ������ ��������� �������� "�" .
	//  {
	//	canal_N = i2c_eeprom_read_byte(deviceaddress, adr_memN1_1 + x_B + _size_block); // �������� �� ������� ����� ����� �����������.

	//	if (canal_N == 1)                                                // 40 ����� ��� �������� ������ ������������ �������
	//	{
	//	  set_komm_mcp('B', 40, 'O');                                     // ���������� ����������� ���� �����������
	//	}
	//	else
	//	{
	//	  set_komm_mcp('B', canal_N, 'O');                                // ���������� ������� ���� �����������
	//	}
	//	// ++++++++++++++++++++++++ �������� �� ���������� � - � +++++++++++++++++++++++++++++++++++
	//	if (x_A == x_B)
	//	{
	//	  myGLCD.printNumI(x_A, 30, 40);
	//	  if (ware_on == 1)myGLCD.print("<->", 66, 40);
	//	  else myGLCD.print("<X>", 66, 40);
	//	  myGLCD.print("  ",130, 40);
	//	  myGLCD.printNumI(canal_N, 130, 40);
	//	  if (digitalRead(Chanal_B) == LOW && ware_on == 1)
	//	  {
	//		myGLCD.print(" - Pass", 170, 40);
	//	  }
	//	  else
	//	  {
	//		if (digitalRead(Chanal_B) != LOW && ware_on == 0)                  // ������ ���� ��������
	//		{
	//		  myGLCD.print(" - Pass", 170, 40);
	//		}
	//		else
	//		{
	//		  count_error++;
	//		  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[25])));
	//		  myGLCD.print(buffer, 50, 65);                            // txt_error_connect4
	//		  myGLCD.printNumI(count_error, 190, 65);

	//		  if ( ware_on == 1)
	//		  {
	//			if (x_A < 10)
	//			{
	//			  myGLCD.printNumI(x_A, x_p + 13, y_p);            // ������������ ��������� ���������
	//			  myGLCD.print("-", x_p + 29, y_p);
	//			}
	//			else
	//			{
	//			  myGLCD.printNumI(x_A, x_p, y_p);                 // ������������ ��������� ���������
	//			  myGLCD.print("-", x_p + 29, y_p);
	//			}
	//			if (canal_N < 10)
	//			{
	//			  myGLCD.printNumI(canal_N, x_p + 32 + 26, y_p);   // ������������ ��������� ���������
	//			}
	//			else
	//			{
	//			  myGLCD.printNumI(canal_N, x_p + 32 + 10, y_p);   // ������������ ��������� ���������
	//			}
	//		  }
	//		  else
	//		  {
	//			if (x_A < 10)
	//			{
	//			  myGLCD.printNumI(x_A, x_p + 13, y_p);            // ������������ ��������� ���������
	//			  myGLCD.print("+", x_p + 29, y_p);
	//			}
	//			else
	//			{
	//			  myGLCD.printNumI(x_A, x_p, y_p);                 // ������������ ��������� ���������
	//			  myGLCD.print("+", x_p + 29, y_p);
	//			}
	//			if (canal_N < 10)
	//			{
	//			  myGLCD.printNumI(canal_N, x_p + 32 + 26, y_p);   // ������������ ��������� ���������
	//			}
	//			else
	//			{
	//			  myGLCD.printNumI(canal_N, x_p + 32 + 10, y_p);   // ������������ ��������� ���������
	//			}
	//		  }
	//		  y_p += 19;
	//		  if ( y_p > 190)                                          // ����� �� ����� ������� ������
	//		  {
	//			myGLCD.drawLine( x_p + 75, 85, x_p + 75, 190);
	//			x_p += 80;
	//			y_p = 82;
	//		  }
	//		}
	//	  }
	//	}

	//	//------------------------ ����� �������� �� ���������� ---------------------------------------

	//	//++++++++++++++++++++++++ �������� ��������� �������� �� ��������� ---------------------------
	//	if (x_A != x_B)                                                      //����������� ������� �� �� ������ ���� ���������
	//	{
	//	  if (digitalRead(Chanal_B) == LOW)                                      // ��� ���� ��������
	//	  {
	//		// �������� �������������� 3 �������, �������� ������ ����� ����������
	//		int canal_N_err = i2c_eeprom_read_byte(deviceaddress, adr_memN1_1 + x_A + (_size_block * 2)); // �������� �� ������� ����� ����� �����������.
	//		if (x_B != canal_N_err)                                      // ����������� ���������� �� �������� � �������
	//		{
	//		  // �������� �������������� 4 �������
	//		  int canal_N_err = i2c_eeprom_read_byte(deviceaddress, adr_memN1_1 + x_A + (_size_block * 3)); // �������� �� ������� ����� ����� �����������.
	//		  if (x_B != canal_N_err)                                  // ����������� ���������� �� �������� � �������
	//		  {
	//			count_error++;
	//			strcpy_P(buffer, (char*)pgm_read_word(&(table_message[25])));
	//			myGLCD.print(buffer, 50, 65);                        // txt_error_connect4
	//			myGLCD.printNumI(count_error, 190, 65);
	//			if (x_A < 10)
	//			{
	//			  myGLCD.printNumI(x_A, x_p + 13, y_p);            // ������������ ��������� ���������
	//			  myGLCD.print("+", x_p + 29, y_p);
	//			}
	//			else
	//			{
	//			  myGLCD.printNumI(x_A, x_p, y_p);                 // ������������ ��������� ���������
	//			  myGLCD.print("+", x_p + 29, y_p);
	//			}
	//			if (canal_N < 10)
	//			{
	//			  myGLCD.printNumI(canal_N, x_p + 32 + 26, y_p);   // ������������ ��������� ���������
	//			}
	//			else
	//			{
	//			  myGLCD.printNumI(canal_N, x_p + 32 + 10, y_p);   // ������������ ��������� ���������
	//			}
	//			y_p += 19;
	//			if ( y_p > 190)                                      // ����� �� ����� ������� ������
	//			{
	//			  myGLCD.drawLine( x_p + 75, 85, x_p + 75, 190);
	//			  x_p += 80;
	//			  y_p = 82;
	//			}
	//		  }
	//		}
	//	  }
	//	} 	//----------------------- ����� �������� �� ��������� -----------------------------------------
	//  }
	//}
	//strcpy_P(buffer, (char*)pgm_read_word(&(table_message[30])));
	//if (count_error == 0) myGLCD.print(buffer, CENTER, 120);               // txt__test_end
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

void draw_measure()
{
	myGLCD.setColor(255, 255, 255);                                             // ����� ���������
	
	myGLCD.drawRoundRect(8, 20, 156, 50);
	myGLCD.drawRoundRect(164, 20, 312, 50);
	myGLCD.drawRoundRect(8, 60, 156, 90);
	myGLCD.drawRoundRect(164, 60, 312, 90);
	myGLCD.drawRoundRect(8, 100, 156, 130);
	myGLCD.drawRoundRect(164, 100, 312, 130);

	myGLCD.drawRoundRect(8, 145, 156, 175);
	myGLCD.drawRoundRect(164, 145, 312, 175);

	myGLCD.setColor(0, 0, 255);

	myGLCD.fillRoundRect(9, 21, 155, 49);
	myGLCD.fillRoundRect(165, 21, 311, 49);
	myGLCD.fillRoundRect(9, 61, 155, 89);
	myGLCD.fillRoundRect(165, 61, 311, 89);
	myGLCD.fillRoundRect(9, 101, 155, 129);
	myGLCD.fillRoundRect(165, 101, 311, 129);

	myGLCD.fillRoundRect(9, 146, 155, 174);
	myGLCD.fillRoundRect(165, 146, 311, 174);

	myGLCD.setColor(255, 255, 255);
	myGLCD.setBackColor(0, 0, 0);
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
  MsTimer2::set(1000, flash_time);                       // 300ms ������ ������� ���������

  setup_regModbus();

  Serial.print(F("FreeRam: "));
  Serial.println(FreeRam());
  wait_time_Old =  millis();
  digitalWrite(led_Green, HIGH);                          
  digitalWrite(led_Red, LOW);      

  sensor_sun_in.begin();
  sensor_sun_out.begin();
  sensor_tube_in.begin();
  sensor_tube_out.begin();
  sensor_tank.begin();
  sensor_outhouse.begin();

  Serial.println(" ");                                   
  Serial.println(F("System initialization OK!."));        // ���������� � ���������� ���������

  read_Temperatures();


  // Initialize Initialize HMC5883L
  Serial.println("Initialize HMC5883L");
 

 /* while (!compass.begin())
  {
	  Serial.println("Could not find a valid HMC5883L sensor, check wiring!");
	  delay(500);
  }

  // Set measurement range
  compass.setRange(HMC5883L_RANGE_1_3GA);

  // Set measurement mode
  compass.setMeasurementMode(HMC5883L_CONTINOUS);

  // Set data rate
  compass.setDataRate(HMC5883L_DATARATE_30HZ);

  // Set number of samples averaged
  compass.setSamples(HMC5883L_SAMPLES_8);

  // Set calibration offset. See HMC5883L_calibration.ino
  compass.setOffset(0, 0);

 // read_compass();
  */
  //MsTimer2::start();
}

void loop()
{
  draw_Glav_Menu1();
  swichMenu1();
}
