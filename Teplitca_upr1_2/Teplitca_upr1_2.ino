/*
J1 - J20 Разъемы управления окнами
J36 - Управление 16 клапанами полива через блок дополнительного реле. Уточнить реле ввключения насоса
J37 - Резервное управление 16 устройствами через блок дополнительного реле.
J53 - J66 Входа для подключения 1-Wire прибора DS2482S-800 и аналогового входа АЦП А0 - А13
Дисплей 7,0"  800x480

*/

#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <MCP23S17.h>
#include <UTouchCD.h>
#include <UTouch.h>
#include <UTFT_Buttons.h> 
#include <UTFT.h>
#include "Wire.h"
#include <OneWire.h>
#include <RTClib.h>
#include <MsTimer2.h> 
#include <modbus.h>
#include <modbusDevice.h>
#include <modbusRegBank.h>
#include <modbusSlave.h>
#include <dht11.h>
#include "Adafruit_MCP23017.h"
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <stdlib.h> // div, div_t
#include <HMC5883L.h>




//UTFT    myGLCD(ITDB32S,38,39,40,41);        // Дисплей 3,2" 320x240
UTFT    myGLCD(TFT01_70,38,39,40,41);       // Дисплей 7,0"  800x480

// Standard Arduino Mega/Due shield            : 6,5,4,3,2
UTouch        myTouch(6,5,4,3,2);
// Finally we set up UTFT_Buttons :)
UTFT_Buttons  myButtons(&myGLCD, &myTouch);

boolean default_colors = true;
uint8_t menu_redraw_required = 0;

// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t Dingbats1_XL[];
extern uint8_t SmallSymbolFont[];

//+++++++++++++++++++++++++++ Настройка часов +++++++++++++++++++++++++++++++
uint8_t second = 0;                      //Initialization time
uint8_t minute = 10;
uint8_t hour   = 10;
uint8_t dow    = 2;
uint8_t day    = 15;
uint8_t month  = 3;
uint16_t year  = 16;
RTC_DS1307 RTC;                         // define the Real Time Clock object

int clockCenterX               = 119;
int clockCenterY               = 119;
int oldsec                     = 0;
const char* str[]              = {"MON","TUE","WED","THU","FRI","SAT","SUN"};
const char* str1[]             = {"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};
const char* str_mon[]          = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
unsigned long wait_time        = 0;         // Время простоя прибора
unsigned long wait_time_Old    = 0;         // Время простоя прибора
int time_minute                = 5;         // Время простоя прибора

//-------------------------------------------------------------------------------
//+++++++++++++++++++++++++++++ Внешняя память +++++++++++++++++++++++++++++++++++++++
int deviceaddress        = 80;                      // Адрес микросхемы памяти
unsigned int eeaddress   =  0;                      // Адрес ячейки памяти
byte hi;                                            // Старший байт для преобразования числа
byte low;                                           // Младший байт для преобразования числа

//++++++++++++++++++++++++++++++ Объявление портов расширения +++++++++++++++++++++++++++++++++++

Adafruit_MCP23017 mcp_windows1;                   // 1 MCP23017 расширение портов, управление окнами
Adafruit_MCP23017 mcp_windows2;                   // 2 MCP23017 расширение портов, управление окнами 
Adafruit_MCP23017 mcp_poliv;                      // 4 MCP23017 расширение портов, управление поливом и насосами
Adafruit_MCP23017 mcp_Rele1;                      // 5 MCP23017 расширение портов, управление дополнительными реле
Adafruit_MCP23017 mcp_win_sensor1;                // 6 MCP23017 расширение портов, управление адресацией MCP

 
#define MCPS_CS    23                       // Выбор CS MCP23S17
#define  ledPin13  13   

MCP23S17 mcps_3(&SPI, MCPS_CS, 3);          // MC№3
MCP23S17 mcps_6(&SPI, MCPS_CS, 7);          // MC№6
MCP23S17 mcps_13(&SPI, MCPS_CS, 0);         // MC№13

bool keyValue = true;

#define Kl_Poliva1_1    0                   //
#define Kl_Poliva1_2    1                   //
#define Kl_Poliva1_3    2                   //
#define Kl_Poliva1_4    3                   //
#define Kl_Poliva1_5    4                   // 
#define Kl_Poliva1_6    5                   //
#define Kl_Poliva1_7    6                   //
#define Nasos_Poliv1    7                   //
#define Kl_Poliva2_1    8                   //
#define Kl_Poliva2_2    9                   //
#define Kl_Poliva2_3    10                  //
#define Kl_Poliva2_4    11                  //
#define Kl_Poliva2_5    12                  //
#define Kl_Poliva2_6    13                  //
#define Kl_Poliva2_7    14                  //
#define Nasos_Poliv2    15                  // 


//+++++++++++++++++++ MODBUS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

modbusDevice regBank;
//Create the modbus slave protocol handler
modbusSlave slave;




const unsigned int adr_control_command    PROGMEM       = 40001; // Адрес передачи комманд на выполнение 
const unsigned int adr_reg_count_err      PROGMEM       = 40002; // Адрес счетчика всех ошибок

//++++++++++++++++++++++++++++++++++ compass +++++++++++++++++++++++++++++++++++++++++


HMC5883L compass;

bool compass_enable1  = false;
bool compass_enable2  = false;
bool compass_enable3  = false;
bool compass_enable4  = false;
bool compass_enable5  = false;
bool compass_enable6  = false;
bool compass_enable7  = false;
bool compass_enable8  = false;
bool compass_enable9  = false;
bool compass_enable10 = false;
bool compass_enable11 = false;
bool compass_enable12 = false;
bool compass_enable13 = false;
bool compass_enable14 = false;

int minX = 0;
int maxX = 0;
int minY = 0;
int maxY = 0;
int offX = 0;
int offY = 0;


//++++++++++++++++++++++++++++ Переменные для цифровой клавиатуры +++++++++++++++++++++++++++++
int x, y, z;
char stCurrent[20]    ="";         // Переменная хранения введенной строки 
int stCurrentLen      =0;          // Переменная хранения длины введенной строки 
int stCurrentLen1     =0;          // Переменная временного хранения длины введенной строки  
char stLast[20]       ="";         // Данные в введенной строке строке.
int ret               = 0;         // Признак прерывания операции
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------------
//Назначение переменных для хранения № опций меню (клавиш)
int but1, but2, but3, but4, but5, but6, but7, but8, but9, but10, butX, butY, butA, butB, butC, butD, but_m1, but_m2, but_m3, but_m4, but_m5, pressed_button;
 //int kbut1, kbut2, kbut3, kbut4, kbut5, kbut6, kbut7, kbut8, kbut9, kbut0, kbut_save,kbut_clear, kbut_exit;
 //int kbutA, kbutB, kbutC, kbutD, kbutE, kbutF;
 int m2 = 1; // Переменная номера меню

 //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int sector = 1;
int teplitca = 1;
int teplitca_adr = 0;
bool avtomat = true;
bool avtomat1 = true;
bool avtomat2 = true;
bool on_off  = false;
bool on_off1  = false;
bool on_off2  = false;
bool err_save = true;
volatile int run_timer = 0;

// Теплица №1
int adr1_1kl_avto                    = 111;   //
int adr1_1kl_start_hour              = 112;   //
int adr1_1kl_start_min               = 113;   //
int adr1_1kl_stop_min                = 114;   //
int adr1_1kl_stop_sec                = 115;   //

int adr1_2kl_avto                    = 121;   //
int adr1_2kl_start_hour              = 122;   //
int adr1_2kl_start_min               = 123;   //
int adr1_2kl_stop_min                = 124;   //
int adr1_2kl_stop_sec                = 125;   //

int adr1_3kl_avto                    = 131;   //
int adr1_3kl_start_hour              = 132;   //
int adr1_3kl_start_min               = 133;   //
int adr1_3kl_stop_min                = 134;   //
int adr1_3kl_stop_sec                = 135;   //

int adr1_4kl_avto                    = 141;   //
int adr1_4kl_start_hour              = 142;   //
int adr1_4kl_start_min               = 143;   //
int adr1_4kl_stop_min                = 144;   //
int adr1_4kl_stop_sec                = 145;   //

int adr1_5kl_avto                    = 151;   //
int adr1_5kl_start_hour              = 152;   //
int adr1_5kl_start_min               = 153;   //
int adr1_5kl_stop_min                = 154;   //
int adr1_5kl_stop_sec                = 155;   //

int adr1_6kl_avto                    = 161;   //
int adr1_6kl_start_hour              = 162;   //
int adr1_6kl_start_min               = 163;   //
int adr1_6kl_stop_min                = 164;   //
int adr1_6kl_stop_sec                = 165;   //

int adr1_7kl_avto                    = 171;   //
int adr1_7kl_start_hour              = 172;   //
int adr1_7kl_start_min               = 173;   //
int adr1_7kl_stop_min                = 174;   //
int adr1_7kl_stop_sec                = 175;   //

// Теплица №2
int adr2_1kl_avto                    = 211;   //
int adr2_1kl_start_hour              = 212;   //
int adr2_1kl_start_min               = 213;   //
int adr2_1kl_stop_min                = 214;   //
int adr2_1kl_stop_sec                = 215;   //

int adr2_2kl_avto                    = 221;   //
int adr2_2kl_start_hour              = 222;   //
int adr2_2kl_start_min               = 223;   //
int adr2_2kl_stop_min                = 224;   //
int adr2_2kl_stop_sec                = 225;   //

int adr2_3kl_avto                    = 231;   //
int adr2_3kl_start_hour              = 232;   //
int adr2_3kl_start_min               = 233;   //
int adr2_3kl_stop_min                = 234;   //
int adr2_3kl_stop_sec                = 235;   //

int adr2_4kl_avto                    = 241;   //
int adr2_4kl_start_hour              = 242;   //
int adr2_4kl_start_min               = 243;   //
int adr2_4kl_stop_min                = 244;   //
int adr2_4kl_stop_sec                = 245;   //

int adr2_5kl_avto                    = 251;   //
int adr2_5kl_start_hour              = 252;   //
int adr2_5kl_start_min               = 253;   //
int adr2_5kl_stop_min                = 254;   //
int adr2_5kl_stop_sec                = 255;   //

int adr2_6kl_avto                    = 261;   //
int adr2_6kl_start_hour              = 262;   //
int adr2_6kl_start_min               = 263;   //
int adr2_6kl_stop_min                = 264;   //
int adr2_6kl_stop_sec                = 265;   //

int adr2_7kl_avto                    = 271;   //
int adr2_7kl_start_hour              = 272;   //
int adr2_7kl_start_min               = 273;   //
int adr2_7kl_stop_min                = 274;   //
int adr2_7kl_stop_sec                = 275;   //




 //------------------------------------------------------------------------------------------------------------------
 // Назначение переменных для хранения текстов

 char  txt_menu1_1[]       = "\x89""O""\x88\x86""B TE""\x89\x88\x86\x8C\x91"" N 1";                           // ПОЛИВ ТЕПЛИЦЫ N 1
 char  txt_menu1_2[]       = "\x89""O""\x88\x86""B TE""\x89\x88\x86\x8C\x91"" N 2";                           // ПОЛИВ ТЕПЛИЦЫ N 2
 char  txt_menu1_3[]       = "\x8A\x89""P""A""B""\x88""EH""\x86""E OKH""A""M""\x86"" N1";                     // УПРАВЛЕНИЕ ОКНАМИ N1
 char  txt_menu1_4[]       = "\x8A\x89""P""A""B""\x88""EH""\x86""E OKH""A""M""\x86"" N2";                     // УПРАВЛЕНИЕ ОКНАМИ N2
 char  txt_menu2_1[]       = "BPEM""\x95"" ""\x89""O""\x88\x86""B""A"" TE""\x89\x88\x86\x8C\x91"" N1";     // ВРЕМЯ ПОЛИВА ТЕПЛИЦЫ N1
 char  txt_menu2_2[]       = "BPEM""\x95"" ""\x89""O""\x88\x86""B""A"" TE""\x89\x88\x86\x8C\x91"" N2";     // ВРЕМЯ ПОЛИВА ТЕПЛИЦЫ N2
 char  txt_menu2_3[]       = "menu2_3";                                                //
 char  txt_menu2_4[]       = "menu2_4";                                                //
 char  txt_menu3_1[]       = "Ta""\x96\xA0\x9D\xA6""a coe""\x99"".";                   // Таблица соед.
 char  txt_menu3_2[]       = "Pe""\x99""a""\x9F\xA4"". ""\xA4""a""\x96\xA0\x9D\xA6";   // Редакт. таблиц
 char  txt_menu3_3[]       = "\x85""a""\x98""py""\x9C"". y""\xA1""o""\xA0\xA7"".";     // Загруз. умолч.
 char  txt_menu3_4[]       = "Bpe""\xA1\xAF"" ""\xA3""poc""\xA4""o""\xAF";             // Время простоя                   //
 char  txt_menu4_1[]       = "C\x9D\xA2yco\x9D\x99""a";                                // Синусоида
 char  txt_menu4_2[]       = "Tpey\x98o\xA0\xAC\xA2\xAB\x9E";                          // Треугольный
 char  txt_menu4_3[]       = "\x89\x9D\xA0oo\x96pa\x9C\xA2\xAB\x9E";                   // Пилообразный
 char  txt_menu4_4[]       = "\x89p\xAF\xA1oy\x98o\xA0\xAC\xA2\xAB\x9E";               // Прямоугольный
 char  txt_menu5_1[]       = " ";// 
 char  txt_menu5_2[]       = " ";//
 char  txt_menu5_3[]       = " ";// 
 char  txt_menu5_4[]       = " ";// 
 char  txt_pass_ok[]       = "Tec\xA4 Pass!";                                           // Тест Pass!
 char  txt_pass_no[]       = "Tec\xA4 NO!";                                             // Тест NO!
 char  txt_info1[]         = "\x81\x88""A""BHOE MEH""\x94";                             // ГЛАВНОЕ МЕНЮ
 char  txt_info2[]         = "H""A""CTPO""\x87""K""A"" ""\x89""O""\x88\x86""B""A";      // НАСТРОЙКА ПОЛИВА
 char  txt_info3[]         = "Hac\xA4po\x9E\x9F""a c\x9D""c\xA4""e\xA1\xAB";            // Настройка системы
 char  txt_info4[]         = "\x81""e\xA2""epa\xA4op c\x9D\x98\xA2""a\xA0o\x97";        // Генератор сигналов
 char  txt_info5[]         = "Oc\xA6\x9D\xA0\xA0o\x98pa\xA5";                           // Осциллограф
 char  txt_botton_clear[]  = "C\x96poc";                                                // Сброс
 char  txt_botton_otmena[] = "O""\xA4\xA1""e""\xA2""a";                                 // Отмена
 char  txt_system_clear1[] = "B\xA2\x9D\xA1""a\xA2\x9D""e!";                            // Внимание !  
 char  txt_system_clear2[] = "Bc\xAF \xA1\xA2\xA5op\xA1""a""\xA6\xA1\xAF \x96y\x99""e\xA4";  // Вся информация будет 
 char  txt_system_clear3[] = "\x8A\x82""A""\x88""EHA!";                                 // УДАЛЕНА 
 char  txt9[6]             = "B\x97o\x99";                                              // Ввод
 char  txt10[8]            = "O""\xA4\xA1""e""\xA2""a";                                 // "Отмена"
 char  txt_time_wait[]     = "\xA1\x9D\xA2"".""\x97""pe""\xA1\xAF"" ""\xA3""poc""\xA4""o""\xAF";      //  мин. время простоя
 char  txt_osc_menu1[]     = "Oc\xA6\x9D\xA0\xA0o\x98pa\xA5";                           //
 char  txt_osc_menu2[]     = "Oc\xA6\x9D\xA0\xA0.1-18\xA1\x9D\xA2";                     //
 char  txt_osc_menu3[]     = "O\xA8\x9d\x96\x9F\x9D";                                   //
 char  txt_osc_menu4[]     = "B\x91XO\x82";           
 char  txt_info29[]        = "Stop->PUSH Disp"; 
 char  txt_info30[]        = "\x89o\x97\xA4op."; 




//------------------------------------------------------------------------------------------------------

dht11 DHT11;


void dateTime(uint16_t* date, uint16_t* time)                  // Программа записи времени и даты файла
{
  DateTime now = RTC.now();

  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(now.year(), now.month(), now.day());

  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(now.hour(), now.minute(), now.second());
}

//++++++++++++++++++++++++++++++++ Программы часов ++++++++++++++++++++++++++++++++++++

void serial_print_date()                           // Печать даты и времени    
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
	  Serial.println(str1[now.dayOfWeek()-1]);
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
	if(day > 31)day = 1;
	month  = now.month();
	year   = now.year();
	DateTime set_time = DateTime(year, month, day, hour, minute, second); // Занести данные о времени в строку "set_time"
	RTC.adjust(set_time);             
}

void drawDisplay()
{
  // Clear screen
  myGLCD.clrScr();
  
  // Draw Clockface
  myGLCD.setColor(0, 0, 255);
  myGLCD.setBackColor(0, 0, 0);
  for (int i=0; i<5; i++)
  {
	myGLCD.drawCircle(clockCenterX, clockCenterY, 119-i);
  }
  for (int i=0; i<5; i++)
  {
	myGLCD.drawCircle(clockCenterX, clockCenterY, i);
  }
  
  myGLCD.setColor(192, 192, 255);
  myGLCD.print("3", clockCenterX+92, clockCenterY-8);
  myGLCD.print("6", clockCenterX-8, clockCenterY+95);
  myGLCD.print("9", clockCenterX-109, clockCenterY-8);
  myGLCD.print("12", clockCenterX-16, clockCenterY-109);
  for (int i=0; i<12; i++)
  {
	if ((i % 3)!=0)
	  drawMark(i);
  }  
  clock_read();
  drawMin(minute);
  drawHour(hour, minute);
  drawSec(second);
  oldsec=second;

  // Draw calendar
  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRoundRect(240, 0, 319, 85);
  myGLCD.setColor(0, 0, 0);
  for (int i=0; i<7; i++)
  {
	myGLCD.drawLine(249+(i*10), 0, 248+(i*10), 3);
	myGLCD.drawLine(250+(i*10), 0, 249+(i*10), 3);
	myGLCD.drawLine(251+(i*10), 0, 250+(i*10), 3);
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
  
  h=h*30;
  h=h+270;
  
  x1=110*cos(h*0.0175);
  y1=110*sin(h*0.0175);
  x2=100*cos(h*0.0175);
  y2=100*sin(h*0.0175);
  
  myGLCD.drawLine(x1+clockCenterX, y1+clockCenterY, x2+clockCenterX, y2+clockCenterY);
}
void drawSec(int s)
{
  float x1, y1, x2, y2;
  int ps = s-1;
  
  myGLCD.setColor(0, 0, 0);
  if (ps==-1)
  ps=59;
  ps=ps*6;
  ps=ps+270;
  
  x1=95*cos(ps*0.0175);
  y1=95*sin(ps*0.0175);
  x2=80*cos(ps*0.0175);
  y2=80*sin(ps*0.0175);
  
  myGLCD.drawLine(x1+clockCenterX, y1+clockCenterY, x2+clockCenterX, y2+clockCenterY);

  myGLCD.setColor(255, 0, 0);
  s=s*6;
  s=s+270;
  
  x1=95*cos(s*0.0175);
  y1=95*sin(s*0.0175);
  x2=80*cos(s*0.0175);
  y2=80*sin(s*0.0175);
  
  myGLCD.drawLine(x1+clockCenterX, y1+clockCenterY, x2+clockCenterX, y2+clockCenterY);
}
void drawMin(int m)
{
  float x1, y1, x2, y2, x3, y3, x4, y4;
  int pm = m-1;
  
  myGLCD.setColor(0, 0, 0);
  if (pm==-1)
  pm=59;
  pm=pm*6;
  pm=pm+270;
  
  x1=80*cos(pm*0.0175);
  y1=80*sin(pm*0.0175);
  x2=5*cos(pm*0.0175);
  y2=5*sin(pm*0.0175);
  x3=30*cos((pm+4)*0.0175);
  y3=30*sin((pm+4)*0.0175);
  x4=30*cos((pm-4)*0.0175);
  y4=30*sin((pm-4)*0.0175);
  
  myGLCD.drawLine(x1+clockCenterX, y1+clockCenterY, x3+clockCenterX, y3+clockCenterY);
  myGLCD.drawLine(x3+clockCenterX, y3+clockCenterY, x2+clockCenterX, y2+clockCenterY);
  myGLCD.drawLine(x2+clockCenterX, y2+clockCenterY, x4+clockCenterX, y4+clockCenterY);
  myGLCD.drawLine(x4+clockCenterX, y4+clockCenterY, x1+clockCenterX, y1+clockCenterY);

  myGLCD.setColor(0, 255, 0);
  m=m*6;
  m=m+270;
  
  x1=80*cos(m*0.0175);
  y1=80*sin(m*0.0175);
  x2=5*cos(m*0.0175);
  y2=5*sin(m*0.0175);
  x3=30*cos((m+4)*0.0175);
  y3=30*sin((m+4)*0.0175);
  x4=30*cos((m-4)*0.0175);
  y4=30*sin((m-4)*0.0175);
  
  myGLCD.drawLine(x1+clockCenterX, y1+clockCenterY, x3+clockCenterX, y3+clockCenterY);
  myGLCD.drawLine(x3+clockCenterX, y3+clockCenterY, x2+clockCenterX, y2+clockCenterY);
  myGLCD.drawLine(x2+clockCenterX, y2+clockCenterY, x4+clockCenterX, y4+clockCenterY);
  myGLCD.drawLine(x4+clockCenterX, y4+clockCenterY, x1+clockCenterX, y1+clockCenterY);
}
void drawHour(int h, int m)
{
  float x1, y1, x2, y2, x3, y3, x4, y4;
  int ph = h;
  
  myGLCD.setColor(0, 0, 0);
  if (m==0)
  {
	ph=((ph-1)*30)+((m+59)/2);
  }
  else
  {
	ph=(ph*30)+((m-1)/2);
  }
  ph=ph+270;
  
  x1=60*cos(ph*0.0175);
  y1=60*sin(ph*0.0175);
  x2=5*cos(ph*0.0175);
  y2=5*sin(ph*0.0175);
  x3=20*cos((ph+5)*0.0175);
  y3=20*sin((ph+5)*0.0175);
  x4=20*cos((ph-5)*0.0175);
  y4=20*sin((ph-5)*0.0175);
  
  myGLCD.drawLine(x1+clockCenterX, y1+clockCenterY, x3+clockCenterX, y3+clockCenterY);
  myGLCD.drawLine(x3+clockCenterX, y3+clockCenterY, x2+clockCenterX, y2+clockCenterY);
  myGLCD.drawLine(x2+clockCenterX, y2+clockCenterY, x4+clockCenterX, y4+clockCenterY);
  myGLCD.drawLine(x4+clockCenterX, y4+clockCenterY, x1+clockCenterX, y1+clockCenterY);

  myGLCD.setColor(255, 255, 0);
  h=(h*30)+(m/2);
  h=h+270;
  
  x1=60*cos(h*0.0175);
  y1=60*sin(h*0.0175);
  x2=5*cos(h*0.0175);
  y2=5*sin(h*0.0175);
  x3=20*cos((h+5)*0.0175);
  y3=20*sin((h+5)*0.0175);
  x4=20*cos((h-5)*0.0175);
  y4=20*sin((h-5)*0.0175);
  
  myGLCD.drawLine(x1+clockCenterX, y1+clockCenterY, x3+clockCenterX, y3+clockCenterY);
  myGLCD.drawLine(x3+clockCenterX, y3+clockCenterY, x2+clockCenterX, y2+clockCenterY);
  myGLCD.drawLine(x2+clockCenterX, y2+clockCenterY, x4+clockCenterX, y4+clockCenterY);
  myGLCD.drawLine(x4+clockCenterX, y4+clockCenterY, x1+clockCenterX, y1+clockCenterY);
}
void printDate()
{
  clock_read();
  myGLCD.setFont(BigFont);
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(255, 255, 255);
	
  myGLCD.print(str[dow-1], 256, 8);
  if (day<10)
	myGLCD.printNumI(day, 272, 28);
  else
	myGLCD.printNumI(day, 264, 28);

  myGLCD.print(str_mon[month-1], 256, 48);
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
		  if ((second == 0) and (minute == 0) and (hour == 0))
		  {
			clearDate();
			printDate();
		  }
		  if (second==0)
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
		  x=myTouch.getX();
		  y=myTouch.getY();
		  if (((y>=200) && (y<=239)) && ((x>=260) && (x<=319))) //установка часов
		  {
			myGLCD.setColor (255, 0, 0);
			myGLCD.drawRoundRect(260, 200, 319, 239);
			setClock();
		  }

		 //  if (((y>=140) && (y<=180)) && ((x>=260) && (x<=319))) //Возврат
		  if (((y>=1) && (y<=479)) && ((x>=1) && (x<=799))) //Возврат
		  {
			//myGLCD.setColor (255, 0, 0);
			//myGLCD.drawRoundRect(260, 140, 319, 180);
			myGLCD.clrScr();
			myGLCD.setFont(BigFont);
			break;
		  }
		 if (((y>=1) && (y<=180)) && ((x>=260) && (x<=319))) //Возврат
		  {
			//myGLCD.setColor (255, 0, 0);
			//myGLCD.drawRoundRect(260, 140, 319, 180);
			myGLCD.clrScr();
			myGLCD.setFont(BigFont);
			break;
		  }
		}
		delay(10);
		clock_read();
	  }
}
//------------------------------ Конец Программы часов --------------------------------
void flash_time()                                              // Программа обработчик прерывания 
{ 
	// PORTB = B00000000; // пин 12 переводим в состояние LOW
	slave.run();
	// PORTB = B01000000; // пин 12 переводим в состояние HIGH
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
		butX = myButtons.addButton(279, 199,  40,  40, "W", BUTTON_SYMBOL); // кнопка Часы 
		but_m1 = myButtons.addButton(  10, 199, 45,  40, "1");
		but_m2 = myButtons.addButton(  61, 199, 45,  40, "2");
		but_m3 = myButtons.addButton(  112, 199, 45,  40, "3");
		but_m4 = myButtons.addButton(  163, 199, 45,  40, "4");
		but_m5 = myButtons.addButton(  214, 199, 45,  40, "5");

}
void txt_pass_no_all()
{
		myGLCD.clrScr();
		myGLCD.setColor(255, 255, 255);
		myGLCD.setBackColor(0, 0, 0);
		myGLCD.print(txt_pass_no, RIGHT, 208);
		delay (1000);
}
void klav123() // ввод данных с цифровой клавиатуры
{
	ret = 0;

	while (true)
	  {
		if (myTouch.dataAvailable())
		{
			  myTouch.read();
			  x=myTouch.getX();
			  y=myTouch.getY();
	  
		if ((y>=10) && (y<=60))  // Upper row
		  {
			if ((x>=10) && (x<=60))  // Button: 1
			  {
				  waitForIt(10, 10, 60, 60);
				  updateStr('1');
			  }
			if ((x>=70) && (x<=120))  // Button: 2
			  {
				  waitForIt(70, 10, 120, 60);
				  updateStr('2');
			  }
			if ((x>=130) && (x<=180))  // Button: 3
			  {
				  waitForIt(130, 10, 180, 60);
				  updateStr('3');
			  }
			if ((x>=190) && (x<=240))  // Button: 4
			  {
				  waitForIt(190, 10, 240, 60);
				  updateStr('4');
			  }
			if ((x>=250) && (x<=300))  // Button: 5
			  {
				  waitForIt(250, 10, 300, 60);
				  updateStr('5');
			  }
		  }

		 if ((y>=70) && (y<=120))  // Center row
		   {
			 if ((x>=10) && (x<=60))  // Button: 6
				{
				  waitForIt(10, 70, 60, 120);
				  updateStr('6');
				}
			 if ((x>=70) && (x<=120))  // Button: 7
				{
				  waitForIt(70, 70, 120, 120);
				  updateStr('7');
				}
			 if ((x>=130) && (x<=180))  // Button: 8
				{
				  waitForIt(130, 70, 180, 120);
				  updateStr('8');
				}
			 if ((x>=190) && (x<=240))  // Button: 9
				{
				  waitForIt(190, 70, 240, 120);
				  updateStr('9');
				}
			 if ((x>=250) && (x<=300))  // Button: 0
				{
				  waitForIt(250, 70, 300, 120);
				  updateStr('0');
				}
			}
		  if ((y>=130) && (y<=180))  // Upper row
			 {
			 if ((x>=10) && (x<=130))  // Button: Clear
				{
				  waitForIt(10, 130, 120, 180);
				  stCurrent[0]='\0';
				  stCurrentLen=0;
				  myGLCD.setColor(0, 0, 0);
				  myGLCD.fillRect(0, 224, 319, 239);
				}
			 if ((x>=250) && (x<=300))  // Button: Exit
				{
				  waitForIt(250, 130, 300, 180);
				  myGLCD.clrScr();
				  myGLCD.setBackColor(VGA_BLACK);
				  ret = 1;
				  stCurrent[0]='\0';
				  stCurrentLen=0;
				  break;
				}
			 if ((x>=130) && (x<=240))  // Button: Enter
				{
				  waitForIt(130, 130, 240, 180);
				 if (stCurrentLen>0)
				   {
				   for (x=0; x<stCurrentLen+1; x++)
					 {
						stLast[x]=stCurrent[x];
					 }
						stCurrent[0]='\0';
						stLast[stCurrentLen+1]='\0';
						//i2c_eeprom_write_byte(deviceaddress,adr_stCurrentLen1,stCurrentLen);
						stCurrentLen1 = stCurrentLen;
						stCurrentLen=0;
						myGLCD.setColor(0, 0, 0);
						myGLCD.fillRect(0, 208, 319, 239);
						myGLCD.setColor(0, 255, 0);
						myGLCD.print(stLast, LEFT, 208);
						break;
					}
				  else
					{
						myGLCD.setColor(255, 0, 0);
						myGLCD.print("\x80\x8A\x8B\x8B""EP \x89\x8A""CTO\x87!", CENTER, 192);//"БУФФЕР ПУСТОЙ!"
						delay(500);
						myGLCD.print("                ", CENTER, 192);
						delay(500);
						myGLCD.print("\x80\x8A\x8B\x8B""EP \x89\x8A""CTO\x87!", CENTER, 192);//"БУФФЕР ПУСТОЙ!"
						delay(500);
						myGLCD.print("                ", CENTER, 192);
						myGLCD.setColor(0, 255, 0);
					}
				 }
			  }
		  }
	   } 
} 
void drawButtons1() // Отображение цифровой клавиатуры
{
// Draw the upper row of buttons
  for (x=0; x<5; x++)
  {
	myGLCD.setColor(0, 0, 255);
	myGLCD.fillRoundRect (10+(x*60), 10, 60+(x*60), 60);
	myGLCD.setColor(255, 255, 255);
	myGLCD.drawRoundRect (10+(x*60), 10, 60+(x*60), 60);
	myGLCD.printNumI(x+1, 27+(x*60), 27);
  }
// Draw the center row of buttons
  for (x=0; x<5; x++)
  {
	myGLCD.setColor(0, 0, 255);
	myGLCD.fillRoundRect (10+(x*60), 70, 60+(x*60), 120);
	myGLCD.setColor(255, 255, 255);
	myGLCD.drawRoundRect (10+(x*60), 70, 60+(x*60), 120);
	if (x<4)
	myGLCD.printNumI(x+6, 27+(x*60), 87);
  }

  myGLCD.print("0", 267, 87);
// Draw the lower row of buttons
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (10, 130, 120, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (10, 130, 120, 180);
  myGLCD.print(txt_botton_clear, 25, 147);     //"Сброс"


  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (130, 130, 240, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (130, 130, 240, 180);
  myGLCD.print("B\x97o\x99", 155, 147);       // "Ввод"
  

  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (250, 130, 300, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (250, 130, 300, 180);
  myGLCD.print("B""\xAB""x", 252, 147);       // Вых
  myGLCD.setBackColor (0, 0, 0);
}
void updateStr(int val)
{
  if (stCurrentLen<20)
  {
	stCurrent[stCurrentLen]=val;
	stCurrent[stCurrentLen+1]='\0';
	stCurrentLen++;
	myGLCD.setColor(0, 255, 0);
	myGLCD.print(stCurrent, LEFT, 224);
  }
  else
  {   // Вывод строки "ПЕРЕПОЛНЕНИЕ!"
	myGLCD.setColor(255, 0, 0);
	myGLCD.print("\x89""EPE""\x89O\x88HEH\x86""E!", CENTER, 224);// ПЕРЕПОЛНЕНИЕ!
	delay(500);
	myGLCD.print("              ", CENTER, 224);
	delay(500);
	myGLCD.print("\x89""EPE""\x89O\x88HEH\x86""E!", CENTER, 224);// ПЕРЕПОЛНЕНИЕ!
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
	Для вызова подпрограммы проверки необходимо записать номер проверки по адресу adr_control_command (40120) 
	Код проверки
	0 -   Выполнение команды окончено
	1 -   Программа проверки кабеля №1
	2 -   Программа проверки кабеля №2
	3 -   Программа проверки кабеля №3
	4 -   Программа проверки кабеля №4
	5 -   Программа проверки панели гарнитур
	6 -   Записать таблицу проверки №1 по умолчанию
	7 -   Записать таблицу проверки №2 по умолчанию
	8 -   Записать таблицу проверки №3 по умолчанию
	9 -   Записать таблицу проверки №4 по умолчанию
	10 -  Установить уровень сигнала резистором №1
	11 -  Установить уровень сигнала резистором №2
	12 -  Чтение таблиц из EEPROM для передачи в ПК
	13 -  Получить таблицу из ПK и записать в EEPROM
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


	int test_n = regBank.get(adr_control_command);   //адрес  40000
	if (test_n != 0)
	{
		if(test_n != 0) Serial.println(test_n);	
		switch (test_n)
		{
			case 1:
				           // Программа проверки кабеля №1
				 break;
			case 2:	
				             // Программа проверки кабеля №2
				 break;
			case 3:
				            // Программа проверки кабеля №3
				 break;
			case 4:	
				           // Программа проверки кабеля №4
				 break;
			case 5:
				         // Программа проверки панели гарнитур
				 break;
			case 6:	
				         // Записать таблицу проверки №1 по умолчанию
				 break;
			case 7:
				         // Записать таблицу проверки №2 по умолчанию
				 break;
			case 8:	
				       // Записать таблицу проверки №3 по умолчанию
				 break;
			case 9:
				         // Записать таблицу проверки №4 по умолчанию
				 break;
			case 10:
				          // Установить уровень сигнала резистором №1
				 break;
			case 11:
				            // Установить уровень сигнала резистором №1
				 break;
			case 12:
				     // Чтение таблиц из EEPROM для передачи в ПК
				 break;
			case 13:
				      // Получить таблицу из ПK и записать в EEPROM
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
				 regBank.set(adr_control_command,0);        // Установить резистором №1,№2  уровень сигнала
				 break;
		 }

	}
	else
	{
	   regBank.set(adr_control_command,0);
	}
}

void draw_Glav_Menu()
{

  but1   = myButtons.addButton( 10,  30,  400,  75, txt_menu1_1);
  but2   = myButtons.addButton( 10,  115, 400,  75, txt_menu1_2);
  but3   = myButtons.addButton( 10,  200, 400,  75, txt_menu1_3);
  but4   = myButtons.addButton( 10,  285, 400,  75, txt_menu1_4);
  butX   = myButtons.addButton( 510, 380, 90,  90, "W", BUTTON_SYMBOL); // кнопка Часы 
  but_m1 = myButtons.addButton( 10,  380, 90,  90, "1");
  but_m2 = myButtons.addButton( 110, 380, 90,  90, "2");
  but_m3 = myButtons.addButton( 210, 380, 90,  90, "3");
  but_m4 = myButtons.addButton( 310, 380, 90,  90, "4");
  but_m5 = myButtons.addButton( 410, 380, 90,  90, "5");
  myButtons.drawButtons(); // Восстановить кнопки
  myGLCD.setColor(VGA_BLACK);
  myGLCD.setBackColor(VGA_WHITE);
  myGLCD.setColor(0, 255, 0);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.print("                      ", CENTER, 0); 

  switch (m2) 
				   {
					case 1:
					      myGLCD.print(txt_info1, CENTER, 0);
					      break;
					 case 2:
                          myGLCD.print(txt_info2, CENTER, 0);
					      break;
					 case 3:
					      myGLCD.print(txt_info3, CENTER, 0);
					      break;
					 case 4:
					      myGLCD.print(txt_info4, CENTER, 0);
					      break;
					 case 5:
					      myGLCD.print(txt_info5, CENTER, 0);
					      break;
					 }
  myButtons.drawButtons();

}

void swichMenu() // Тексты меню в строках "txt....."
{
	 m2=1;                                                         // Устанивить первую странице меню
	 while(1) 
	   {
		 //  test_input_mcp();
		 // run_compass();
		  wait_time = millis();                                    // Программа вызова часов при простое 
		  if (wait_time - wait_time_Old > 60000 * time_minute)
		  {
				wait_time_Old =  millis();
				AnalogClock();
				myGLCD.clrScr();
				myButtons.drawButtons();                           // Восстановить кнопки
				print_up();                                        // Восстановить верхнюю строку
		  }

		  myButtons.setTextFont(BigFont);                          // Установить Большой шрифт кнопок  

			if (myTouch.dataAvailable() == true)                   // Проверить нажатие кнопок
			  {
			    pressed_button = myButtons.checkButtons();         // Если нажата - проверить что нажато
				wait_time_Old =  millis();

					 if (pressed_button==butX)                     // Нажата вызов часы
					      {  
							 AnalogClock();
							 myGLCD.clrScr();
							 myButtons.drawButtons();              // Восстановить кнопки
							 print_up();                           // Восстановить верхнюю строку
					      }
		 
					 if (pressed_button==but_m1)                   // Нажата 1 страница меню
						  {
							  myButtons.setButtonColors(VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_BLUE); // Голубой фон меню
							  myButtons.drawButtons();             // Восстановить кнопки
							  default_colors=true;
							  m2=1;                                // Устанивить первую странице меню
							  myButtons.relabelButton(but1, txt_menu1_1, m2 == 1);
							  myButtons.relabelButton(but2, txt_menu1_2, m2 == 1);
							  myButtons.relabelButton(but3, txt_menu1_3, m2 == 1);
							  myButtons.relabelButton(but4, txt_menu1_4, m2 == 1);
							  myGLCD.setColor(0, 255, 0);
							  myGLCD.setBackColor(0, 0, 0);
							  myGLCD.print("                      ", CENTER, 0); 
							  myGLCD.print(txt_info1, CENTER, 0);   // "Тест кабелей"
		
						  }
				    if (pressed_button==but_m2)
						  {
							  myButtons.setButtonColors(VGA_WHITE, VGA_RED, VGA_YELLOW, VGA_BLUE, VGA_TEAL);
							  myButtons.drawButtons();
							  default_colors=false;
							  m2=2;
							  myButtons.relabelButton(but1, txt_menu2_1 , m2 == 2);
							  myButtons.relabelButton(but2, txt_menu2_2 , m2 == 2);
							  myButtons.relabelButton(but3, txt_menu2_3 , m2 == 2);
							  myButtons.relabelButton(but4, txt_menu2_4 , m2 == 2);
							  myGLCD.setColor(0, 255, 0);
							  myGLCD.setBackColor(0, 0, 0);
							  myGLCD.print("                      ", CENTER, 0); 
							  myGLCD.print(txt_info2, CENTER, 0);     // Тест блока гарнитур
						 }

				   if (pressed_button==but_m3)
						 {
							  myButtons.setButtonColors(VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_GREEN);
							  myButtons.drawButtons();
							  default_colors=false;
							  m2=3;
							  myButtons.relabelButton(but1, txt_menu3_1 , m2 == 3);
							  myButtons.relabelButton(but2, txt_menu3_2 , m2 == 3);
							  myButtons.relabelButton(but3, txt_menu3_3 , m2 == 3);
							  myButtons.relabelButton(but4, txt_menu3_4 , m2 == 3);
							  myGLCD.setColor(0, 255, 0);
							  myGLCD.setBackColor(0, 0, 0);
							  myGLCD.print("                      ", CENTER, 0); 
							  myGLCD.print(txt_info3, CENTER, 0);      // Настройка системы
						}
				   if (pressed_button==but_m4)
						{
							  myButtons.setButtonColors(VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_RED);
							  myButtons.drawButtons();
							  default_colors=false;
							  m2=4;
							  myButtons.relabelButton(but1, txt_menu4_1 , m2 == 4);
							  myButtons.relabelButton(but2, txt_menu4_2 , m2 == 4);
							  myButtons.relabelButton(but3, txt_menu4_3 , m2 == 4);
							  myButtons.relabelButton(but4, txt_menu4_4 , m2 == 4);
							  myGLCD.setColor(0, 255, 0);
							  myGLCD.setBackColor(0, 0, 0);
							  myGLCD.print("                      ", CENTER, 0); 
							  myGLCD.print(txt_info4, CENTER, 0);     // Генератор сигналов
						}

				   if (pressed_button==but_m5)
						{
							  myButtons.setButtonColors(VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_NAVY);
							  myButtons.drawButtons();
							  default_colors=false;
							  m2=5;
							  myButtons.relabelButton(but1, txt_menu5_1 , m2 == 5);
							  myButtons.relabelButton(but2, txt_menu5_2 , m2 == 5);
							  myButtons.relabelButton(but3, txt_menu5_3 , m2 == 5);
							  myButtons.relabelButton(but4, txt_menu5_4 , m2 == 5);
							  myGLCD.setColor(0, 255, 0);
							  myGLCD.setBackColor(0, 0, 0);
							  myGLCD.print("                      ", CENTER, 0); 
							  myGLCD.print(txt_info5, CENTER, 0);     // Осциллограф
						}
	
	               //*****************  Меню №1  **************

		           if (pressed_button==but1 && m2 == 1)
			           {
						
								myGLCD.clrScr();   // Очистить экран
								myGLCD.print(txt_pass_ok, RIGHT, 208); 
								delay (500);
		    				    draw_Poliv_Menu();
								swich_Poliv_Menu();
			   				 myGLCD.clrScr();
							 myButtons.drawButtons();
							 print_up();
			           }
	  
		           if (pressed_button==but2 && m2 == 1)
					   {
						
								myGLCD.clrScr();   // Очистить экран
								myGLCD.print(txt_pass_ok, RIGHT, 208); 
								delay (500);
		    				//	gaz_save_start(); // если верно - выполнить пункт меню
					
			   				myGLCD.clrScr();
							myButtons.drawButtons();
							print_up();
					   }
	  
		           if (pressed_button==but3 && m2 == 1)
					   {
						
								myGLCD.clrScr();   // Очистить экран
								myGLCD.print(txt_pass_ok, RIGHT, 208); 
								delay (500);
		    				 //  colwater_save_start(); // если верно - выполнить пункт меню
					
			   				myGLCD.clrScr();
							myButtons.drawButtons();
							print_up();
					   }
		           if (pressed_button==but4 && m2 == 1)
					   {
					
								myGLCD.clrScr();   // Очистить экран
								myGLCD.print(txt_pass_ok, RIGHT, 208); 
								delay (500);
		    		// 
			   				myGLCD.clrScr();
							myButtons.drawButtons();
							print_up();
					   }

		         //*****************  Меню №2  **************


		           if (pressed_button==but1 && m2 == 2)
					  {
						//	print_info();
	        				myGLCD.clrScr();
							myButtons.drawButtons();
							print_up();
				      }

				  if (pressed_button==but2 && m2 == 2)
					  {
						//   info_nomer_user();
				  			myGLCD.clrScr();
							myButtons.drawButtons();
							print_up();
					  }
	  
				  if (pressed_button==but3 && m2 == 2)
					  {
					
						 // test_arRequestMod();
							myGLCD.clrScr();
							myButtons.drawButtons();
							print_up();
					  }
				  if (pressed_button==but4 && m2 == 2)
					  {
						 
						  //  testRemoteAtCommand();
						    myGLCD.clrScr();
							myButtons.drawButtons();
							print_up();
				      }
		
		        //*****************  Меню №3  **************
		           if (pressed_button==but1 && m2 == 3) // Первый пункт меню 3
					{
						    myGLCD.clrScr();   // Очистить экран
							myGLCD.print(txt_pass_ok, RIGHT, 208); 
							delay (500);
							//eeprom_clear == 0;
		    		//			system_clear_start(); // если верно - выполнить пункт меню
							 myGLCD.clrScr();
							 myButtons.drawButtons();
							 print_up();
					  }

			 //--------------------------------------------------------------
		           if (pressed_button==but2 && m2 == 3)  // Второй пункт меню 3
				      {
							myGLCD.clrScr();
							myGLCD.print(txt_pass_ok, RIGHT, 208);
						//	set_n_telef();
							myGLCD.clrScr();
							myButtons.drawButtons();
							print_up();
				   
					  }

			   //------------------------------------------------------------------

			       if (pressed_button==but3 && m2 == 3)  // Третий пункт меню 3
					  { 
							myGLCD.clrScr();
							myGLCD.print(txt_pass_ok, RIGHT, 208);
							delay (500);
		    	//
							myGLCD.clrScr();
							myButtons.drawButtons();
							print_up();
				      }

	 //------------------------------------------------------------------
				   if (pressed_button==but4 && m2 == 3)                 // Четвертый пункт меню 3
				      {
				
							myGLCD.clrScr();
							myGLCD.setFont(BigFont);
							myGLCD.setBackColor(0, 0, 255);
							myGLCD.clrScr();
							drawButtons1();                            // Нарисовать цифровую клавиатуру
							myGLCD.printNumI(time_minute, LEFT, 208);
							myGLCD.print(txt_time_wait, 35, 208);   //
							klav123();                                 // Считать информацию с клавиатуры
							if (ret == 1)                              // Если "Возврат" - закончить
								 {
									goto bailout41;                    // Перейти на окончание выполнения пункта меню
								 }
							else                                       // Иначе выполнить пункт меню
								 {
									 time_minute = atol(stLast);
								 }
						    bailout41:                                 // Восстановить пункты меню
						    myGLCD.clrScr();
						    myButtons.drawButtons();
						    print_up();
				      }

                   //*****************  Меню №4  **************

                   if (pressed_button==but1 && m2 == 4) // 
					  {
			
							myGLCD.clrScr();   // Очистить экран
							myGLCD.print(txt_pass_ok, RIGHT, 208); 
							delay (500);
							//butA = myButtons.addButton(279, 20,  40,  35, "W", BUTTON_SYMBOL); // Синусоида
							//if (myButtons.buttonEnabled(butB)) myButtons.deleteButton(butB);
							//if (myButtons.buttonEnabled(butC)) myButtons.deleteButton(butC);
							//if (myButtons.buttonEnabled(butD)) myButtons.deleteButton(butD);
							myButtons.drawButtons();
							print_up();
							//
				   
					  }

				   if (pressed_button==but2 && m2 == 4)
					  {
					
							myGLCD.clrScr();
							myGLCD.print(txt_pass_ok, RIGHT, 208);
							delay (500);
		    	//			butB = myButtons.addButton(279, 65, 40,  35, "W", BUTTON_SYMBOL); // Треугольный
							//if (myButtons.buttonEnabled(butA)) myButtons.deleteButton(butA);
							//if (myButtons.buttonEnabled(butC)) myButtons.deleteButton(butC);
							//if (myButtons.buttonEnabled(butD)) myButtons.deleteButton(butD);
							myButtons.drawButtons();
							print_up();
					  }

		           if (pressed_button==but3 && m2 == 4) // 
					  {
				
							myGLCD.clrScr();
							myGLCD.print(txt_pass_ok, RIGHT, 208);
							delay (500);
							//butC = myButtons.addButton(279, 110,  40,  35, "W", BUTTON_SYMBOL); // Пилообразный
							//if (myButtons.buttonEnabled(butA)) myButtons.deleteButton(butA);
							//if (myButtons.buttonEnabled(butB)) myButtons.deleteButton(butB);
							//if (myButtons.buttonEnabled(butD)) myButtons.deleteButton(butD);
							myButtons.drawButtons();
							print_up();
					  }
				   if (pressed_button==but4 && m2 == 4) //
					  {
							myGLCD.clrScr();
							myGLCD.print(txt_pass_ok, RIGHT, 208);
							delay (500);
							//butD = myButtons.addButton(279, 155,  40,  35, "W", BUTTON_SYMBOL); // Прямоугольный сигнал
							//if (myButtons.buttonEnabled(butB)) myButtons.deleteButton(butB);
							//if (myButtons.buttonEnabled(butC)) myButtons.deleteButton(butC);
							//if (myButtons.buttonEnabled(butA)) myButtons.deleteButton(butA);
							myButtons.drawButtons();
							print_up();
					  }
				    //*****************  Меню №5  **************

                   if (pressed_button==but1 && m2 == 5) // Сброс данных
					  {

							myGLCD.clrScr();
							myButtons.drawButtons();
							print_up();
					  }
				   if (pressed_button==but2 && m2 == 5)
					  {
							  myGLCD.clrScr();   // Очистить экран
							  myGLCD.print(txt_pass_ok, RIGHT, 208); 
							  delay (500);
		    				 // ZigBee_SetH(); // если верно - выполнить пункт меню
		
							myButtons.drawButtons();
							print_up();
					  }

				   if (pressed_button==but3 && m2 == 5) // Ввод пароля пользователя
					  {
							  myGLCD.clrScr();   // Очистить экран
							  myGLCD.print(txt_pass_ok, RIGHT, 208); 
							  delay (500);
		    				 // ZigBee_SetL(); // если верно - выполнить пункт меню
					
							myButtons.drawButtons();
							print_up();
					  }

			       if (pressed_button==but4 && m2 == 5) // 
			          {
				   
					
								myGLCD.clrScr();   // Очистить экран
								myGLCD.print(txt_pass_ok, RIGHT, 208); 
								delay (500);
		    			//		break;
						
							myButtons.drawButtons();
							print_up();
				      }
			
		           if (pressed_button==-1) 
					  {
						//  myGLCD.print("HET", 220, 220);
					  }
				  } 
       }
}
void print_up() // Печать верхней строчки над меню
{
	myGLCD.setColor(0, 255, 0);
	myGLCD.setBackColor(0, 0, 0);
	myGLCD.print("                      ", CENTER, 0); 
	switch (m2) 
	{
 		case 1:
			myGLCD.print(txt_info1, CENTER, 0);
			break;
		case 2:
            myGLCD.print(txt_info2, CENTER, 0);
			break;
		case 3:
			myGLCD.print(txt_info3, CENTER, 0);
			break;
		case 4:
			myGLCD.print(txt_info4, CENTER, 0);
			break;
        case 5:
			myGLCD.print(txt_info5, CENTER, 0);
			break;
    }
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
	Wire.requestFrom(deviceaddress,1);
	if (Wire.available()) rdata = Wire.read();
	return rdata;
}
void i2c_test1()
{
	
	for( int n = 0; n<250; n++)
	{
		//i2c_eeprom_write_byte(0x50, n, n);
		//i2c_eeprom_write_byte(deviceaddress, n, n);
	}


	for(unsigned int x=0;x<250;x++)
	{
		int  b = i2c_eeprom_read_byte(deviceaddress, x); //access an address from the memory
		Serial.print(x); //print content to serial port
		Serial.print(" - "); //print content to serial port
		Serial.println(b); //print content to serial port
	}
	
}
// +++++++++++++++++++++++++ Полив +++++++++++++++++++++++++++++++++++++++++
void draw_Poliv_Menu()  
{
	myGLCD.clrScr();
	myGLCD.setFont(BigFont);
	myGLCD.setColor(0, 0, 255);
	myGLCD.fillRoundRect (700, 50,  790, 132);     // Кнопка 
	myGLCD.fillRoundRect (700, 142, 790, 224);     // Кнопка 
	myGLCD.fillRoundRect (700, 234, 790, 316);     // 
	myGLCD.fillRoundRect (700, 326, 790, 410);     // Кнопка 

	myGLCD.setColor(255, 255, 255);
	myGLCD.drawRoundRect (700, 50,  790, 132);     // Кнопка 
	myGLCD.drawRoundRect (700, 142, 790, 224);     // Кнопка 
	myGLCD.drawRoundRect (700, 234, 790, 316);     // Кнопка 
	myGLCD.drawRoundRect (700, 326, 790, 410);     // Кнопка 

	myGLCD.drawRoundRect (10, 50, 200, 150);
	myGLCD.drawRoundRect (210, 50, 410, 150);
	myGLCD.drawRoundRect (420, 50, 600, 150);

	myGLCD.drawRoundRect (10, 160, 200, 300);
	myGLCD.drawRoundRect (210, 160, 410, 300);
	myGLCD.drawRoundRect (420, 160, 600, 300);

	myGLCD.drawRoundRect (10, 310, 200, 410);
	myGLCD.drawRoundRect (210, 310, 410, 410);
	myGLCD.drawRoundRect (420, 310, 600, 410);

	myGLCD.drawLine (210, 150, 410, 50);
	myGLCD.drawLine (210, 310, 410, 410);

	myGLCD.drawLine (620, 160, 640, 160);
	myGLCD.drawLine (620, 300, 640, 300);

	myGLCD.drawLine (620, 50, 620, 160);
	myGLCD.drawLine (640, 50, 640, 160);

	myGLCD.drawLine (620, 300, 620, 410);
	myGLCD.drawLine (640, 300, 640, 410);


	myGLCD.setBackColor(0, 0, 255);
	myGLCD.print("1",740, 85);                               // 
	myGLCD.print("2",740, 175);                              //
	myGLCD.print("ESC",720, 265);                            // "ESC"
	myGLCD.print("\x8D""ac\xAB",710, 355);                   // "Часы"
	myGLCD.print("BXO""\x82",610, 220);                      // ВХОД


	//myGLCD.setColor(VGA_YELLOW);
	//myGLCD.setBackColor(0, 0, 0);
	//myGLCD.print("1",10, 10+30);          
	//myGLCD.print("2",175, 10+30); 
	//myGLCD.print("3",10, 76+30);          
	//myGLCD.print("4",90, 76+30); 
	//myGLCD.print("5",175, 76+30); 
	//myGLCD.print("6",10, 180);          
	//myGLCD.print("7",175, 180); 
	//myGLCD.setFont(SmallFont);
	//myGLCD.print("=>",95, 15+30); 
	//myGLCD.print("<=",140, 40+30); 
	//myGLCD.print("=>",95, 210); 
	//myGLCD.print("<=",140, 187); 

	if(teplitca==1)
		{
			myGLCD.setFont(BigFont);
			myGLCD.print("Te\xA3\xA0\xA2\xA6""a N1",CENTER, 5);  
			myGLCD.setFont(SmallFont);
			if (i2c_eeprom_read_byte(deviceaddress, adr1_1kl_avto ) == 0 )  // 1
				{
					//myGLCD.print("Py\xA7\xA2",30, 14+30);   
					Serial.println(adr1_1kl_avto);
				}
			else
				{
					//myGLCD.print("A\x97\xA4o ",30, 14+30);  
					Serial.println(adr1_1kl_avto);
				}

			if (i2c_eeprom_read_byte(deviceaddress, adr1_2kl_avto ) == 0 )  // 2
				{
					//myGLCD.print("Py\xA7\xA2",195, 14+30);    
				}
			else
				{
					//myGLCD.print("A\x97\xA4o ",195, 14+30);  
				}


			if (i2c_eeprom_read_byte(deviceaddress, adr1_3kl_avto ) == 0 )  // 3
				{
					//myGLCD.print("Py\xA7\xA2",30, 80+30);    
				}
			else
				{
					//myGLCD.print("A\x97\xA4o ",30, 80+30);  
				}

			if (i2c_eeprom_read_byte(deviceaddress, adr1_4kl_avto ) == 0 ) // 4
				{
					//myGLCD.print("Py\xA7\xA2",110, 80+30);    
				}
			else
				{
					//myGLCD.print("A\x97\xA4o ",110, 80+30);  
				}

		   if (i2c_eeprom_read_byte(deviceaddress, adr1_5kl_avto ) == 0 )  // 5
				{
					//myGLCD.print("Py\xA7\xA2 ",195, 80+30);    
				}
			else
				{
					//myGLCD.print("A\x97\xA4o ",195, 80+30);  
				}

			if (i2c_eeprom_read_byte(deviceaddress, adr1_6kl_avto ) == 0 )  // 6
				{
					//myGLCD.print("Py\xA7\xA2",30, 184);    
				}
			else
				{
					//myGLCD.print("A\x97\xA4o ",30, 184);  
				}

			if (i2c_eeprom_read_byte(deviceaddress, adr1_7kl_avto ) == 0 )  // 7
				{
					//myGLCD.print("Py\xA7\xA2",195, 184);    
				}
			else
				{
					//myGLCD.print("A\x97\xA4o ",195, 184);  
				}
			myGLCD.setFont(BigFont);

			//myGLCD.print("  : ",6, 27+30);  // 1
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_1kl_start_hour ),10,27+30);
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_1kl_start_min ),48,27+30);
			//myGLCD.print("  : ",6, 45+30);  
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_1kl_stop_min ),10,45+30);
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_1kl_stop_sec ),48,45+30);

			//myGLCD.print("  : ",171, 27+30);  //2
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_2kl_start_hour ),175,27+30);
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_2kl_start_min ),216,27+30);
			//myGLCD.print("  : ",171, 45+30);  
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_2kl_stop_min ),175,45+30);
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_2kl_stop_sec ),216,45+30);

			//myGLCD.print("  : ",6, 93+30);  // 3
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_3kl_start_hour ),10,93+30);
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_3kl_start_min ),48,93+30);
			//myGLCD.print("  : ",6, 111+30);  
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_3kl_stop_min ),10,111+30);
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_3kl_stop_sec ),48,111+30);

			//myGLCD.print("  : ",86, 93+30);  //4
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_4kl_start_hour ),90,93+30);
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_4kl_start_min ),128,93+30);
			//myGLCD.print("  : ",86, 111+30);  
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_4kl_stop_min ),90,111+30);
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_4kl_stop_sec ),128,111+30);

			//myGLCD.print("  : ",171, 93+30);  // 5
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_5kl_start_hour ),175,93+30);
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_5kl_start_min ),216,93+30);
			//myGLCD.print("  : ",171, 111+30);  
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_5kl_stop_min ),175,111+30);
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_5kl_stop_sec ),216,111+30);

			//myGLCD.print("  : ",6, 197);  //6
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_6kl_start_hour ),10,197);
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_6kl_start_min ),48,197);
			//myGLCD.print("  : ",6, 215);  
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_6kl_stop_min ),10,215);
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_6kl_stop_sec ),48,215);

			//myGLCD.print("  : ",171, 197);  //7
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_7kl_start_hour ),175,197);
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_7kl_start_min ),216,197);
			//myGLCD.print("  : ",171, 215);  
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_7kl_stop_min ),175,215);
			//myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_7kl_stop_sec ),216,215);

		}

	if(teplitca==2)
		{
			myGLCD.setFont(BigFont);
			myGLCD.print("Te\xA3\xA0\xA2\xA6""a N2",40, 5);  
			myGLCD.setFont(SmallFont);
		if (i2c_eeprom_read_byte(deviceaddress, adr2_1kl_avto ) == 0 )  // 1
				{
					myGLCD.print("Py\xA7\xA2",30, 14+30);   
					Serial.println(adr1_1kl_avto);
				}
			else
				{
					myGLCD.print("A\x97\xA4o ",30, 14+30);  
					Serial.println(adr1_1kl_avto);
				}

			if (i2c_eeprom_read_byte(deviceaddress, adr2_2kl_avto ) == 0 )  // 2
				{
					myGLCD.print("Py\xA7\xA2",195, 14+30);    
				}
			else
				{
					myGLCD.print("A\x97\xA4o ",195, 14+30);  
				}


			if (i2c_eeprom_read_byte(deviceaddress, adr2_3kl_avto ) == 0 )  // 3
				{
					myGLCD.print("Py\xA7\xA2",30, 80+30);    
				}
			else
				{
					myGLCD.print("A\x97\xA4o ",30, 80+30);  
				}

			if (i2c_eeprom_read_byte(deviceaddress, adr2_4kl_avto ) == 0 ) // 4
				{
					myGLCD.print("Py\xA7\xA2",110, 80+30);    
				}
			else
				{
					myGLCD.print("A\x97\xA4o ",110, 80+30);  
				}

		   if (i2c_eeprom_read_byte(deviceaddress, adr2_5kl_avto ) == 0 )  // 5
				{
					myGLCD.print("Py\xA7\xA2 ",195, 80+30);    
				}
			else
				{
					myGLCD.print("A\x97\xA4o ",195, 80+30);  
				}

			if (i2c_eeprom_read_byte(deviceaddress, adr2_6kl_avto ) == 0 )  // 6
				{
					myGLCD.print("Py\xA7\xA2",30, 184);    
				}
			else
				{
					myGLCD.print("A\x97\xA4o ",30, 184);  
				}

			if (i2c_eeprom_read_byte(deviceaddress, adr2_7kl_avto ) == 0 )  // 7
				{
					myGLCD.print("Py\xA7\xA2",195, 184);    
				}
			else
				{
					myGLCD.print("A\x97\xA4o ",195, 184);  
				}
			myGLCD.setFont(BigFont);

			myGLCD.print("  : ",6, 27+30);  // 1
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_1kl_start_hour ),10,27+30);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_1kl_start_min ),48,27+30);
			myGLCD.print("  : ",6, 45+30);  
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_1kl_stop_min ),10,45+30);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_1kl_stop_sec ),48,45+30);

			myGLCD.print("  : ",171, 27+30);  //2
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_2kl_start_hour ),175,27+30);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_2kl_start_min ),216,27+30);
			myGLCD.print("  : ",171, 45+30);  
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_2kl_stop_min ),175,45+30);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_2kl_stop_sec ),216,45+30);

			myGLCD.print("  : ",6, 93+30);  // 3
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_3kl_start_hour ),10,93+30);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_3kl_start_min ),48,93+30);
			myGLCD.print("  : ",6, 111+30);  
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_3kl_stop_min ),10,111+30);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_3kl_stop_sec ),48,111+30);

			myGLCD.print("  : ",86, 93+30);  //4
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_4kl_start_hour ),90,93+30);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_4kl_start_min ),128,93+30);
			myGLCD.print("  : ",86, 111+30);  
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_4kl_stop_min ),90,111+30);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_4kl_stop_sec ),128,111+30);

			myGLCD.print("  : ",171, 93+30);  // 5
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_5kl_start_hour ),175,93+30);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_5kl_start_min ),216,93+30);
			myGLCD.print("  : ",171, 111+30);  
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_5kl_stop_min ),175,111+30);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_5kl_stop_sec ),216,111+30);

			myGLCD.print("  : ",6, 197);  //6
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_6kl_start_hour ),10,197);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_6kl_start_min ),48,197);
			myGLCD.print("  : ",6, 215);  
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_6kl_stop_min ),10,215);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_6kl_stop_sec ),48,215);

			myGLCD.print("  : ",171, 197);  //7
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_7kl_start_hour ),175,197);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_7kl_start_min ),216,197);
			myGLCD.print("  : ",171, 215);  
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_7kl_stop_min ),175,215);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr2_7kl_stop_sec ),216,215);
		}
}
void swich_Poliv_Menu()
{
	while (true)
		{
		if (myTouch.dataAvailable())
			{
				myTouch.read();
				int	x=myTouch.getX();
				int	y=myTouch.getY();

				if ((x>=260) && (x<=319))                 // 
					{
						 if ((y>=5) && (y<=55))         //  "1"
							{
								waitForIt(260, 5, 319, 55);
								poliv_ruchnoj();
							}

						else if ((y>=65) && (y<=115))         //  "2"
							{
								waitForIt(260, 65, 319, 115);
								// Программа 2
							}

						else if ((y>=125) && (y<=175))         //  "RET"
							{
								waitForIt(260, 125, 319, 175);
								break;
							}

						else if ((y>=185) && (y<=235))         // 
							{
								waitForIt(260, 185, 319, 235);
								AnalogClock();                   // Часы
								draw_Poliv_Menu();
							}

					}
				// ----------------------- Выбор секции --------------------------
				if ((y>=5+30) && (y<=65+30))                 // 
					{
						if ((x>=5) && (x<=80))               // 
							{
								waitForIt(5, 5+30, 80, 65+30);
								sector = 1;
								swich_menu_Sector();
							}
						else if ((x>=170) && (x<=250))         // 
							{
								waitForIt(170, 5+30, 250, 65+30);
								sector = 2;
								swich_menu_Sector();
							}
					}
				if ((y>=70+30) && (y<=170))                 // 
					{
						if ((x>=5) && (x<=80))         // 
							{
								waitForIt(5, 70+30, 80, 170);
								sector = 3;
								swich_menu_Sector();
							}
						else if ((x>=85) && (x<=165))         // 
							{
								waitForIt(85, 70+30, 165, 170);
								sector = 4;
								swich_menu_Sector();
							}
						else if ((x>=170) && (x<=250))         // 
							{
								waitForIt(170, 70+30, 250, 170);
								sector = 5;
								swich_menu_Sector();
							}
					}
				if ((y>=175) && (y<=235))                 // 
					{
						if ((x>=5) && (x<=80))         // 
							{
								waitForIt(5, 175, 80, 235);
								sector = 6;
								swich_menu_Sector();
							}
						else if ((x>=170) && (x<=250))         // 
							{
								waitForIt(170, 175, 250, 235);
								sector = 7;
								swich_menu_Sector();
							}
					}

		}
	}
}

void Draw_menu_Sector()
{
	myGLCD.clrScr();
	myGLCD.setFont(BigFont);
	myGLCD.setColor(0, 0, 255);
	myGLCD.fillRoundRect (260, 5, 319, 55);        // Кнопка 
	myGLCD.fillRoundRect (260, 65, 319, 115);      // Кнопка "ЗАП" 
	myGLCD.fillRoundRect (260, 125, 319, 175);     // "ESC"
	myGLCD.fillRoundRect (260, 185, 319, 235);     // Кнопка "Часы"
	myGLCD.setColor(255, 255, 255);
	myGLCD.drawRoundRect (260, 5, 319, 55);
	myGLCD.drawRoundRect (260, 65, 319, 115);
	myGLCD.drawRoundRect (260, 125, 319, 175);
	myGLCD.drawRoundRect (260, 185, 319, 235);
	myGLCD.drawRoundRect (5, 5, 250, 235);        // Обрамление поля экрана


	myGLCD.setBackColor(0, 0, 255);
	myGLCD.print("1",282, 22);                         // 
	myGLCD.print("\x85""A\x89",265, 82);               // "ЗАП"
	myGLCD.print("ESC",265, 142);                      // "ESC"
	myGLCD.print("\x8D""a",274, 195);                  // "Ча"
	myGLCD.print("c\xAB",274, 210);                    // "сы"

	myGLCD.setBackColor(0, 0, 0);
	myGLCD.print("Ce\x9F\xA6\x9D\xAF N",40, 15); 
	myGLCD.printNumI(sector,180, 15); 
	myGLCD.print("Pe\x9B\x9D\xA1 \xA3o\xA0\x9D\x97""a",25, 35); 

	drawUpButton(40, 97);
	drawUpButton(88, 97);
	drawUpButton(130, 97);
	drawUpButton(178, 97);

	drawDownButton(40, 148);
	drawDownButton(88, 148);
	drawDownButton(130, 148);
	drawDownButton(178, 148);

	print_set_avto();
	print_on_off();
}
void print_set_avto()
{
	int print_avtomat;
	if (teplitca == 1) teplitca_adr = 100;
	else if (teplitca == 2) teplitca_adr = 200;

	print_avtomat = i2c_eeprom_read_byte(deviceaddress, (sector * 10) + teplitca_adr + 1 );
	if (print_avtomat == 0)
		{
				myGLCD.setColor(0, 255, 0);
				myGLCD.fillRoundRect (130, 60, 210, 90);        // Кнопка 
				myGLCD.setColor(0, 0, 0);
				myGLCD.fillRoundRect (40, 60, 120, 90);         // Кнопка 
				myGLCD.setColor(255, 255, 255);
				myGLCD.drawRoundRect (130, 60, 210, 90);        //Клавиша "Ручн"
				myGLCD.drawRoundRect (40, 60, 120, 90);         //Клавиша "Авто"
				myGLCD.setBackColor(0, 255, 0);
				myGLCD.setColor(0, 0, 0);
				myGLCD.print("Py\xA7\xA2",140, 67); 
				myGLCD.setBackColor(0, 0, 0);
				myGLCD.setColor(255, 255, 255);
				myGLCD.print("A\x97\xA4o",48, 67); 

		}
	else
		{
				myGLCD.setColor(0, 255, 0);
				myGLCD.fillRoundRect (40, 60, 120, 90);         //Клавиша "Авто"
				myGLCD.setColor(0, 0, 0);
				myGLCD.fillRoundRect (130, 60, 210, 90);        // Кнопка 
				myGLCD.setColor(255, 255, 255);
				myGLCD.drawRoundRect (130, 60, 210, 90);        //Клавиша "Ручн"
				myGLCD.drawRoundRect (40, 60, 120, 90);         //Клавиша "Авто"
				myGLCD.setBackColor(0, 255, 0);
				myGLCD.setColor(0, 0, 0);
				myGLCD.print("A\x97\xA4o",48, 67); 
				myGLCD.setBackColor(0, 0, 0);
				myGLCD.setColor(255, 255, 255);
				myGLCD.print("Py\xA7\xA2",140, 67); 
		}
}
void print_on_off()
{
	if (on_off == false)
		{
				myGLCD.setColor(0, 255, 0);
				myGLCD.fillRoundRect (130, 180, 210, 230);        // Кнопка 
				myGLCD.setColor(0, 0, 0);
				myGLCD.fillRoundRect (40, 180, 120, 230);         // Кнопка 
				myGLCD.setColor(255, 255, 255);
				myGLCD.drawRoundRect (130, 180, 210, 230);        //Клавиша "Стоп"
				myGLCD.drawRoundRect (40, 180, 120, 230);         //Клавиша "Пуск"
				myGLCD.setBackColor(0, 255, 0);
				myGLCD.setColor(0, 0, 0);
				myGLCD.print("CTO\x89",140, 196);                 //Клавиша "Стоп"
				myGLCD.setBackColor(0, 0, 0);
				myGLCD.setColor(255, 255, 255);
				myGLCD.print("\x89\x8A""CK",48, 196);             //Клавиша "Пуск"

		}
	else
		{
				myGLCD.setColor(0, 255, 0);
				myGLCD.fillRoundRect (40, 180, 120, 230);         //Клавиша 
				myGLCD.setColor(0, 0, 0);
				myGLCD.fillRoundRect (130, 180, 210, 230);        // Кнопка 
				myGLCD.setColor(255, 255, 255);
				myGLCD.drawRoundRect (130, 180, 210, 230);        //Клавиша "Стоп"
				myGLCD.drawRoundRect (40, 180, 120, 230);         //Клавиша "Пуск"
				myGLCD.setBackColor(0, 255, 0);
				myGLCD.setColor(0, 0, 0);
				myGLCD.print("\x89\x8A""CK",48, 196);             //Клавиша "Пуск"
				myGLCD.setBackColor(0, 0, 0);
				myGLCD.setColor(255, 255, 255);                 
				myGLCD.print("CTO\x89",140, 196);                 //Клавиша "Стоп"
		}
}
void swich_menu_Sector()
{
	if (teplitca == 1) teplitca_adr = 100;
	else if (teplitca == 2) teplitca_adr = 200;

	Draw_menu_Sector();
	int t_temp_sec, t_temp_min, t_temp_hour, t_temp_min_run, t_temp_sec_run;

	t_temp_hour     = i2c_eeprom_read_byte(deviceaddress, (sector * 10)+teplitca_adr+2 );
	t_temp_min      = i2c_eeprom_read_byte(deviceaddress, (sector * 10)+teplitca_adr+3 );
	t_temp_min_run  = i2c_eeprom_read_byte(deviceaddress, (sector * 10)+teplitca_adr+4 );
	t_temp_sec_run  = i2c_eeprom_read_byte(deviceaddress, (sector * 10)+teplitca_adr+5 );

 
	int x, y;
	int res = 0;
	boolean ct=false;
	boolean cd=false;

   if (t_temp_hour<10)
	  {
		myGLCD.printNumI(0, 40, 127);
		myGLCD.printNumI(t_temp_hour, 56, 127);
	  }
  else
	  {
		myGLCD.printNumI(t_temp_hour, 40, 127);
	  }


  if (t_temp_min<10)
	  {
		myGLCD.printNumI(0, 90, 127);
		myGLCD.printNumI(t_temp_min, 106, 127);
	  }
  else
	  {
		myGLCD.printNumI(t_temp_min, 90, 127);
	  }

  if (t_temp_min_run<10)
	  {
		myGLCD.printNumI(0, 130, 127);
		myGLCD.printNumI(t_temp_min_run, 146, 127);
	  }
  else
	  {
		myGLCD.printNumI(t_temp_min_run, 130, 127);
	  }

 if (t_temp_sec_run<10)
	  {
		myGLCD.printNumI(0, 178, 127);
		myGLCD.printNumI(t_temp_sec_run, 194, 127);
	  }
 else
	  {
		myGLCD.printNumI(t_temp_sec_run, 178, 127);
	  }
   
 while (true)
	{
		if (myTouch.dataAvailable())
			{
				myTouch.read();
				int	x=myTouch.getX();
				int	y=myTouch.getY();
			

				if ((x>=260) && (x<=319))                                                          // 
					{
						  if ((y>=5) && (y<=55))                                                   //  "1"
							{
								waitForIt(260, 5, 319, 55);

							}

						if ((y>=65) && (y<=115))                                                   //  "ЗАП"
							{
								waitForIt(260, 65, 319, 115);
								save_Time(t_temp_hour,t_temp_min,t_temp_min_run,t_temp_sec_run );
							}

						if ((y>=125) && (y<=175))                                                  //  "ESC"
							{
								waitForIt(260, 125, 319, 175);
								break;
							}

						if ((y>=185) && (y<=235))                                                  // 
							{
								waitForIt(260, 185, 319, 235);
								AnalogClock();
								Draw_menu_Sector();
							}

					}

					if ((y>=60) && (y<=90))                                                        // 
						{

							if ((x>=40) && (x<=120))                                               // 
								{
									waitForIt(40, 60, 120, 90);                                    //  Клавиша "Авто"
									i2c_eeprom_write_byte(deviceaddress,(sector * 10)+teplitca_adr+1,1);
									delay(10);
									print_set_avto();
								}

							if ((x>=130) && (x<=210))                                              //  Клавиша "Ручн"
								{
									waitForIt(130, 60, 210, 90);
									i2c_eeprom_write_byte(deviceaddress,(sector * 10)+teplitca_adr+1,0);
									delay(10);
									print_set_avto();
								}

						}

					if ((y>=180) && (y<=230))                                                      // 
						{
							if ((x>=40) && (x<=120))                                               // 
								{
									waitForIt(40, 180, 120, 230);                                  //  Клавиша "Пуск"
									on_off = true;
									print_on_off();
									run_poliv_on_off(teplitca, sector, on_off);
								}

							if ((x>=130) && (x<=210))                                              //  Клавиша "Стоп"
								{
									waitForIt(130, 180, 210, 230);
									on_off = false;
									print_on_off();
									run_poliv_on_off(teplitca, sector, on_off);
								}

						}

//+++++++++++++++++++++++++ Установка времени полива +++++++++++++++++++++++++++++++++++++++

				if ((y>=97) && (y<=122)) // Buttons: Time UP Верхние клавиши установки времени
					  {
						if ((x>=40) && (x<=72)) // Час
							{
							  buttonWait(40, 97);
							  t_temp_hour+=1;
							  if (t_temp_hour==24) 	t_temp_hour=0;
							  if (t_temp_hour<10)
								  {
									myGLCD.printNumI(0, 40, 127);
									myGLCD.printNumI(t_temp_hour, 56, 127);
								  }
							  else
								  {
									  myGLCD.printNumI(t_temp_hour, 40, 127);
								  }
							  if (ct==false)
								  {
									ct=true;
								  }

							  test_Time(t_temp_hour,t_temp_min,t_temp_min_run,t_temp_sec_run );


							}
					  else if ((x>=88) && (x<=120))   // Минута
							{
								buttonWait(88, 97);
								t_temp_min+=1;
								if (t_temp_min==60) t_temp_min=0;
								if (t_temp_min<10)
									{
									myGLCD.printNumI(0, 90, 127);
									myGLCD.printNumI(t_temp_min, 106, 127);
									}
								else
									{
										myGLCD.printNumI(t_temp_min, 90, 127);
									}
								if (ct==false)
									{
										ct=true;
									}
								test_Time(t_temp_hour,t_temp_min,t_temp_min_run,t_temp_sec_run );
							}

					  else if ((x>=130) && (x<=162))  // Минута run
							{
							  buttonWait(130, 97);
							  t_temp_min_run+=1;
							  if (t_temp_min_run==60) t_temp_min_run=0;
							  if (t_temp_min_run<10)
								  {
									myGLCD.printNumI(0, 130, 127);
									myGLCD.printNumI(t_temp_min_run, 146, 127);
								  }
							  else
								  {
									 myGLCD.printNumI(t_temp_min_run, 130, 127);
								  }
							  if (ct==false)
								  {
									ct=true;
								  }
							  test_Time(t_temp_hour,t_temp_min,t_temp_min_run,t_temp_sec_run );
							}

					 else if ((x>=178) && (x<=210))  // sec_run
							{
							  buttonWait(178, 97);
							  t_temp_sec_run+=1;
							  if (t_temp_sec_run==60) 	t_temp_sec_run=0;
							  if (t_temp_sec_run<10)
								  {
									myGLCD.printNumI(0, 178, 127);
									myGLCD.printNumI(t_temp_sec_run, 194, 127);
								  }
							  else
								  {
									myGLCD.printNumI(t_temp_sec_run, 178, 127);
								  }
							  if (ct==false)
							  {
								ct=true;
							  }
							  test_Time(t_temp_hour,t_temp_min,t_temp_min_run,t_temp_sec_run );
							}
					  }
//----------------------------------------------------------------------------------------------
				 else if ((y>=148) && (y<=173))                       //Time DOWN Нижние  клавиши установки времени
				  {
					if ((x>=40) && (x<=72))
					{
					  buttonWait(40, 148);
					  t_temp_hour -= 1;
					  if (t_temp_hour<0)  t_temp_hour=23;
					  if (t_temp_hour<10)
					  {
						myGLCD.printNumI(0, 40, 127);
						myGLCD.printNumI(t_temp_hour, 56, 127);
					  }
					  else
					  {
						  myGLCD.printNumI(t_temp_hour, 40, 127);
					  }
					  if (ct==false)
					  {
						ct=true;
					   }
					  test_Time(t_temp_hour,t_temp_min,t_temp_min_run,t_temp_sec_run );
					}
				  else if ((x>=88) && (x<=120))
					{
					  buttonWait(88, 148);
					  t_temp_min-=1;
					  if (t_temp_min < 0) t_temp_min=59;
					  if (t_temp_min<10)
					  {
						myGLCD.printNumI(0, 90, 127);
						myGLCD.printNumI(t_temp_min, 106, 127);
					  }
					  else
					  {
						 myGLCD.printNumI(t_temp_min, 90, 127);
					  }
					  if (ct==false)
					  {
						ct=true;
					  }
					  test_Time(t_temp_hour,t_temp_min,t_temp_min_run,t_temp_sec_run );
					}


					else if ((x>=130) && (x<=162))
					{
					  buttonWait(130, 148);
					  t_temp_min_run-=1;
					  if (t_temp_min_run < 0) 	t_temp_min_run=59;
					  if (t_temp_min_run<10)
					  {
						myGLCD.printNumI(0, 130, 127);
						myGLCD.printNumI(t_temp_min_run, 146, 127);
					  }
					  else
					  {
						 myGLCD.printNumI(t_temp_min_run, 130, 127);
					  }
					  if (ct==false)
					  {
						ct=true;
					  }
					  test_Time(t_temp_hour,t_temp_min,t_temp_min_run,t_temp_sec_run );
					}
				 else if ((x>=178) && (x<=210))
					{
					  buttonWait(178, 148);
					  t_temp_sec_run-=1;
					  if (t_temp_sec_run<0)  t_temp_sec_run=59;

					  if (t_temp_sec_run<10)
					  {
						myGLCD.printNumI(0, 178, 127);
						myGLCD.printNumI(t_temp_sec_run, 194, 127);
					  }
					  else
					  {
						myGLCD.printNumI(t_temp_sec_run, 178, 127);
					  }
					  if (ct==false)
					  {
						ct=true;
					  }
					  test_Time(t_temp_hour,t_temp_min,t_temp_min_run,t_temp_sec_run );
					}
				  }
		   }
	}
	draw_Poliv_Menu();
}

void test_Time(int save_start_hour, int save_start_min, int save_stop_min, int save_stop_sec )
{
	//   !! Доработать ограничения
	int temp_start_hour = 0;
	int temp_start_min = 0;
	int temp_stop_min = 0;
	int temp_stop_sec = 0;

	if (teplitca == 1) teplitca_adr = 100;         // Определить диапазон адресов текущей теплицы
	else if (teplitca == 2) teplitca_adr = 200;

	err_save = true;
	unsigned long time_start = 0;
	unsigned long time_stop = 0;
	unsigned long s_time_start = (save_start_hour * 60 *60)+(save_start_min*60);              // Преобразовать в секунды планируемое время включения
	unsigned long s_time_stop = s_time_start + (save_stop_min * 60)+ save_stop_sec;           // Преобразовать в секунды планируемое время отключения

	Serial.print(s_time_start);
	Serial.print(" - ");
	Serial.println(s_time_stop);

	for (int i = 1; i<8; i++)                                                                 // Произвести проверку секторов на совпадение
	{
		temp_start_hour = i2c_eeprom_read_byte(deviceaddress, (sector * 10)+teplitca_adr+2);  // Получить текущее время включения
		temp_start_min =  i2c_eeprom_read_byte(deviceaddress, (sector * 10)+teplitca_adr+3);
		temp_stop_min  =  i2c_eeprom_read_byte(deviceaddress, (sector * 10)+teplitca_adr+4);  // Получить текущее время отключения
		temp_stop_sec  =  i2c_eeprom_read_byte(deviceaddress, (sector * 10)+teplitca_adr+5);

		time_start = (temp_start_hour *60*60)+(temp_start_min*60);                            // Преобразовать в секунды текущее время включения
		time_stop = time_start + (temp_stop_min*60)+temp_stop_sec;                            // Преобразовать в секунды текущее время отключения

		if(i != sector)                                                                       // Свой сектор не проверять
			{
				if (s_time_start < time_start)
					{
						if (s_time_stop < time_start )
							{
							   err_save = true;
							   myGLCD.setColor(VGA_LIME);
							   myGLCD.fillCircle(18,135,7);
							}
						else if (s_time_stop <= time_stop)
							{
								err_save = false;
								myGLCD.setColor(VGA_RED);
								myGLCD.fillCircle(18,135,7);
								Serial.println("false");
								break;
							}
					}
				else if (s_time_start >= time_start)
					{

						if(s_time_start >= time_stop )
							{
								err_save = true;
								myGLCD.setColor(VGA_LIME);
								myGLCD.fillCircle(18,135,7);
							}
						else 
							{
								err_save = false;
								myGLCD.setColor(VGA_RED);
								myGLCD.fillCircle(18,135,7);
								Serial.println("false");
								break;
							}

					}
			}
	}
}
void save_Time(int save_start_hour, int save_start_min, int save_stop_min, int save_stop_sec )
{
	byte  bsave_start_hour,  bsave_start_min,  bsave_stop_min,  bsave_stop_sec;
	bsave_start_hour = save_start_hour;
	bsave_start_min  = save_start_min;
	bsave_stop_min   = save_stop_min;
	bsave_stop_sec   = save_stop_sec;

	if (err_save == true)
	 {
		i2c_eeprom_write_byte(deviceaddress,((sector * 10)+teplitca_adr+2 ),save_start_hour);
		i2c_eeprom_write_byte(deviceaddress,((sector * 10)+teplitca_adr+3 ),save_start_min);
		i2c_eeprom_write_byte(deviceaddress,((sector * 10)+teplitca_adr+4 ),save_stop_min);
		i2c_eeprom_write_byte(deviceaddress,((sector * 10)+teplitca_adr+5 ),save_stop_sec);
	 }
}

void run_poliv_on_off(int temp_teplitca, int temp_sector, bool temp_on_off)
{
	  if(temp_teplitca == 1)
	   {
		   if(temp_on_off == true)
		   {
			  mcp_poliv.digitalWrite(temp_sector-1, LOW);  
			  delay(500);
			  mcp_poliv.digitalWrite(7, LOW);  
			  Serial.print(temp_teplitca);
			  Serial.print("  ");
			  Serial.print(temp_sector);
			  Serial.print("  ");
			  Serial.println("On");
		   }
		   else if(temp_on_off == false)
		   {
			  mcp_poliv.digitalWrite(temp_sector-1, HIGH);  
			  mcp_poliv.digitalWrite(7, HIGH);  
			  Serial.print(temp_teplitca);
			  Serial.print("  ");
			  Serial.print(temp_sector);
			  Serial.print("  ");
			  Serial.println("Off");
		   }
	   }
   else if(temp_teplitca == 2)
	   {
		  if(temp_on_off == true)
		   {
			  mcp_poliv.digitalWrite(temp_sector-1+8, LOW);  
			  delay(500);
			  mcp_poliv.digitalWrite(7+8, LOW); 
			  Serial.print(temp_teplitca);
			  Serial.print("  ");
			  Serial.print(temp_sector);
			  Serial.print("  ");
			  Serial.println("On");
		   }
		   else if(temp_on_off == false)
		   {
			  mcp_poliv.digitalWrite(temp_sector-1+8, HIGH);  
			  mcp_poliv.digitalWrite(7+8, HIGH);  
			  Serial.print(temp_teplitca);
			  Serial.print("  ");
			  Serial.print(temp_sector);
			  Serial.print("  ");
			  Serial.println("Off");
		   }
	   }
}
void poliv_ruchnoj()
{
	int print_avtomat;
	bool rtemp_on_off;
	if (teplitca == 1) teplitca_adr = 100;                        // Определить диапазон адресов текущей теплицы
	if (teplitca == 2) teplitca_adr = 200;
	uint32_t tPct = millis();
	int i = 1;
	bool t_run = true;

    do                                                            // Произвести проверку секторов на совпадение
		{
			if (myTouch.dataAvailable())
				{
					myTouch.read();
					int	x=myTouch.getX();
					int	y=myTouch.getY();

					if ((x>=260) && (x<=319))                     // Отключить полив
						{
							if ((y>=65) && (y<=115))              //  "2"
								{
									waitForIt(260, 65, 319, 115);
									rtemp_on_off = false;
									run_poliv_on_off(teplitca ,i ,rtemp_on_off);
									break;
								}
						}
				}

			print_avtomat = i2c_eeprom_read_byte(deviceaddress, (i * 10) + teplitca_adr + 1 );
			  Serial.print((i * 10) + teplitca_adr + 1);
			  Serial.print("  ");
		      Serial.println(print_avtomat);
			if (print_avtomat == true)
				{
				   rtemp_on_off = true;
				   if (t_run == true)
				   {
					//  Serial.println(i);
					  run_poliv_on_off(teplitca , i ,rtemp_on_off);
					  t_run =false;
					  MsTimer2::start();
				   }
				  
				   if (run_timer > 120) 
					   {
						   MsTimer2::stop();
						   rtemp_on_off = false;
						   run_poliv_on_off(teplitca , i ,rtemp_on_off);
						   i++;
						   t_run = true;
						   run_timer =0;
					   }
				}
			else
				{
					i++;
				}

		} while (i<8);
}

// -------------------------------------------------------------------------
void setup_compass()
{
	mcps_6.digitalWrite(0, HIGH);
	
	compass.begin();
  // Initialize Initialize HMC5883L
 /* while (!compass.begin())
  {
    delay(500);
  }
*/
  // Set measurement range
  compass.setRange(HMC5883L_RANGE_1_3GA);

  // Set measurement mode
  compass.setMeasurementMode(HMC5883L_CONTINOUS);

  // Set data rate
  compass.setDataRate(HMC5883L_DATARATE_30HZ);

  // Set number of samples averaged
  compass.setSamples(HMC5883L_SAMPLES_8);


	/*
		 // Initialize Initialize HMC5883L
	  Serial.println("Initialize HMC5883L");
	  while (!compass.begin())
	  {
		Serial.println("Could not find a valid HMC5883L sensor, check wiring!");
		delay(500);
	  }
	  
	  // Set measurement range
	  compass.setRange(HMC5883L_RANGE_1_3GA);

	  // Set measurement mode
	  compass.setMeasurementMode(HMC5883L_CONTINOUS);

	  // Set data rate
	
    // compass.setDataRate(HMC5883L_DATARATE_1_5HZ);
	  compass.setDataRate(HMC5883L_DATARATE_30HZ);

	  // Set number of samples averaged
	  compass.setSamples(HMC5883L_SAMPLES_8);

	  // Set calibration offset. See HMC5883L_calibration.ino
	  compass.setOffset(0, 0);
	  
	  */
	mcps_6.digitalWrite(0, LOW);
}
void run_compass()
{
  mcps_6.digitalWrite(0, HIGH);

    Vector mag = compass.readRaw();

  // Determine Min / Max values
  //if (mag.XAxis < minX) minX = mag.XAxis;
  //if (mag.XAxis > maxX) maxX = mag.XAxis;
  //if (mag.YAxis < minY) minY = mag.YAxis;
  //if (mag.YAxis > maxY) maxY = mag.YAxis;

  //// Calculate offsets
  //offX = (maxX + minX)/2;
  //offY = (maxY + minY)/2;

  Serial.print(mag.XAxis);
  Serial.print("    ");
  Serial.print(mag.YAxis);
 /* Serial.print(":");
  Serial.print(minX);
  Serial.print(":");
  Serial.print(maxX);
  Serial.print(":");
  Serial.print(minY);
  Serial.print(":");
  Serial.print(maxY);
  Serial.print(":");
  Serial.print(offX);
  Serial.print(":");
  Serial.print(offY);*/
  Serial.print("\n");



  /*
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
  float headingDegrees = heading * 180/M_PI; 

  // Output
  Serial.print(" Heading = ");
  Serial.print(heading);
  Serial.print(" Degress = ");
  Serial.print(headingDegrees);
  Serial.println();
  
  delay(50);
  */
 mcps_6.digitalWrite(0, LOW);
}
void test_display()
	{
		myGLCD.setColor(255, 255, 255);
		myGLCD.setBackColor(0, 0, 0);
		myGLCD.drawRoundRect (0, 0, 799, 479);
		myGLCD.drawRoundRect (10, 10, 799-10, 479-10);
		myGLCD.drawRoundRect (20, 20, 799-20, 479-20);
		myGLCD.drawRoundRect (30, 30, 799-30, 479-30);
		myGLCD.drawRoundRect (40, 40, 799-40, 479-40);
		myGLCD.drawRoundRect (200, 200, 799-200, 479-200);
		myGLCD.drawLine(400, 0, 400, 479);
		myGLCD.drawLine(100, 0, 100, 479);
		myGLCD.drawLine(200, 0, 200, 479);
		myGLCD.drawLine(300, 0, 300, 479);
		myGLCD.drawLine(0, 240, 799, 240);
	}
void test_mcp()
{
		//mcp_win_sensor1.init(6);                   //  U12 Концевые датчики состояния окон
		mcp_win_sensor1.pinMode(0, OUTPUT);          //  Окно №1 датчик концевой
		mcp_win_sensor1.pinMode(1, OUTPUT);          //  Окно №2 датчик концевой  
		mcp_win_sensor1.pinMode(2, OUTPUT);          //  
		mcp_win_sensor1.pinMode(3, OUTPUT);          //  Окно №3 датчик концевой  
		mcp_win_sensor1.pinMode(4, OUTPUT);          //  Окно №4 датчик концевой  
		mcp_win_sensor1.pinMode(5, OUTPUT);          //  Окно №5 датчик концевой  
		mcp_win_sensor1.pinMode(6, OUTPUT);          //  Окно №6 датчик концевой  
		mcp_win_sensor1.pinMode(7, OUTPUT);          //  Окно №7 датчик концевой  

		mcp_win_sensor1.pinMode(8, OUTPUT);          //  Окно №8 датчик концевой  
		mcp_win_sensor1.pinMode(9, OUTPUT);          //  Окно №9 датчик концевой  
		mcp_win_sensor1.pinMode(10, OUTPUT);         //  Окно №10 датчик концевой  
		mcp_win_sensor1.pinMode(11, OUTPUT);         //  Окно №11 датчик концевой  
		mcp_win_sensor1.pinMode(12, OUTPUT);         //  Окно №12 датчик концевой 
		mcp_win_sensor1.pinMode(13, OUTPUT);         //  Окно №13 датчик концевой   
		mcp_win_sensor1.pinMode(14, OUTPUT);         //  Окно №14 датчик концевой    
		mcp_win_sensor1.pinMode(15, OUTPUT);         //  Окно №15 датчик концевой  



		/*
		mcps_13.init(MCPS_CS,0);                     //  U13  
	    mcps_13.pinMode(0, OUTPUT);                  //  Окно №16 датчик концевой  
		mcps_13.pinMode(1, OUTPUT);                  //  Окно №17 датчик концевой  
		mcps_13.pinMode(2, OUTPUT);                  //  Окно №18 датчик концевой
		mcps_13.pinMode(3, OUTPUT);                  //  Окно №19 датчик концевой  
		mcps_13.pinMode(4, OUTPUT);                  //  Окно №20 датчик концевой  
		mcps_13.pinMode(5, OUTPUT);                  //   
		mcps_13.pinMode(6, OUTPUT);                  //   
		mcps_13.pinMode(7, OUTPUT);                  //   
	 
 	    mcps_13.pinMode(8, OUTPUT);                  //   
		mcps_13.pinMode(9, OUTPUT);                  //    
		mcps_13.pinMode(10, OUTPUT);                  //   
		mcps_13.pinMode(11, OUTPUT);                  //    
		mcps_13.pinMode(12, OUTPUT);                  //   
		mcps_13.pinMode(13, OUTPUT);                  //   
		mcps_13.pinMode(14, OUTPUT);                  //   
		mcps_13.pinMode(15, OUTPUT);                  //   
		
		*/
		/*
		mcps_13.pinMode(0, LOW);                  //  Окно №16 датчик концевой  
		mcps_13.pinMode(1, LOW);                  //  Окно №17 датчик концевой  
		mcps_13.pinMode(2, LOW);                  //  Окно №18 датчик концевой
		mcps_13.pinMode(3, LOW);                  //  Окно №19 датчик концевой  
		mcps_13.pinMode(4, LOW);                  //  Окно №20 датчик концевой  
		mcps_13.pinMode(5, LOW);                  //   
		mcps_13.pinMode(6, LOW);                  //   
		mcps_13.pinMode(7, LOW);                  //   
	 
 	    mcps_13.pinMode(8, LOW);                  //   
		mcps_13.pinMode(9, LOW);                  //    
		mcps_13.pinMode(10, LOW);                  //   
		mcps_13.pinMode(11, LOW);                  //    
		mcps_13.pinMode(12, LOW);                  //   
		mcps_13.pinMode(13, LOW);                  //   
		mcps_13.pinMode(14, LOW);                  //   
		mcps_13.pinMode(15, LOW);                  //   
		*/

    for (int i = 0; i<16; i++)
	{
		//mcp_windows1.digitalWrite(i, HIGH);
		//delay(10);
		//mcp_windows1.digitalWrite(i, LOW);
		//delay(10);
  //  	mcp_windows2.digitalWrite(i, HIGH);
		//delay(10);
		//mcp_windows2.digitalWrite(i, LOW);
		//delay(10);
		//mcp_poliv.digitalWrite(i, HIGH);
		//delay(10);
		//mcp_poliv.digitalWrite(i, LOW);
		//delay(10); 
		//mcp_Rele1.digitalWrite(i, HIGH);
		//delay(10);
		//mcp_Rele1.digitalWrite(i, LOW);
		//delay(10);
		//mcp_win_sensor1.digitalWrite(i, HIGH);
		//delay(10);
		//mcp_win_sensor1.digitalWrite(i, LOW);
		//delay(10);
		mcps_13.pinMode(i, OUTPUT);
		//mcps_3.digitalWrite(i, HIGH); 
		//delay(10);
		//mcps_3.digitalWrite(i, LOW);
		//delay(10);
		//mcps_6.digitalWrite(i, HIGH);
		//delay(10);
		//mcps_6.digitalWrite(i, LOW);
		//delay(10);
		mcps_13.digitalWrite(i, HIGH);
		delay(10);
		mcps_13.digitalWrite(i, LOW);
		delay(10);
	}
	
}
void test_input_mcp()
{
	int In_sensor=0;
	bool In_sensor_win = true;
	
	for (int i = 0; i<16; i++)
		{
			if (i == 0 )	In_sensor = i+1;
			if (i == 1 )	In_sensor = i+1;
			if (i > 2)In_sensor = i;
			In_sensor_win = mcp_win_sensor1.digitalRead(i);   //  Окно №1 датчик концевой
			if (In_sensor_win == false && i!= 2)
				{
					Serial.println(In_sensor);
				}
		}
	
	for (int i = 0; i<5; i++)
		{   
			In_sensor = i+16;
 			In_sensor_win = mcps_13.digitalRead(i);      //  Окно №16 датчик концевой
			if (In_sensor_win == false)
				{
					Serial.println(In_sensor);
				}
			delay(50);
		}

}
void setup_mcp() 
{
	    mcp_windows1.begin(1);                      //  U1  Управление окнами
		mcp_windows1.pinMode(0, OUTPUT);            //  Окно №1 открыть
		mcp_windows1.pinMode(1, OUTPUT);            //  Окно №1 закрыть
		mcp_windows1.pinMode(2, OUTPUT);            //  Окно №2 открыть
		mcp_windows1.pinMode(3, OUTPUT);            //  Окно №2 закрыть
		mcp_windows1.pinMode(4, OUTPUT);            //  Окно №3 открыть
		mcp_windows1.pinMode(5, OUTPUT);            //  Окно №3 закрыть
		mcp_windows1.pinMode(6, OUTPUT);            //  Окно №4 открыть
		mcp_windows1.pinMode(7, OUTPUT);            //  Окно №4 закрыть 
		
		mcp_windows1.pinMode(8, OUTPUT);            //  Окно №5 открыть
		mcp_windows1.pinMode(9, OUTPUT);            //  Окно №5 закрыть
		mcp_windows1.pinMode(10, OUTPUT);           //  Окно №6 открыть
		mcp_windows1.pinMode(11, OUTPUT);           //  Окно №6 закрыть
		mcp_windows1.pinMode(12, OUTPUT);           //  Окно №7 открыть
		mcp_windows1.pinMode(13, OUTPUT);           //  Окно №7 закрыть
		mcp_windows1.pinMode(14, OUTPUT);           //  Окно №8 открыть
		mcp_windows1.pinMode(15, OUTPUT);           //  Окно №8 закрыть
		
	
		mcp_windows2.begin(2);                       //  U2  Управление окнами
		mcp_windows2.pinMode(0, OUTPUT);            //  Окно №9 открыть
		mcp_windows2.pinMode(1, OUTPUT);            //  Окно №9 закрыть 
		mcp_windows2.pinMode(2, OUTPUT);            //  Окно №10 открыть
		mcp_windows2.pinMode(3, OUTPUT);            //  Окно №10 закрыть
		mcp_windows2.pinMode(4, OUTPUT);            //  Окно №11 открыть
		mcp_windows2.pinMode(5, OUTPUT);            //  Окно №11 закрыть  
		mcp_windows2.pinMode(6, OUTPUT);            //  Окно №12 открыть
		mcp_windows2.pinMode(7, OUTPUT);            //  Окно №12 закрыть  
 
		mcp_windows2.pinMode(8, OUTPUT);            //  Окно №13 открыть
		mcp_windows2.pinMode(9, OUTPUT);            //  Окно №13 закрыть
		mcp_windows2.pinMode(10, OUTPUT);           //  Окно №14 открыть
		mcp_windows2.pinMode(11, OUTPUT);           //  Окно №14 закрыть 
		mcp_windows2.pinMode(12, OUTPUT);           //  Окно №15 открыть
		mcp_windows2.pinMode(13, OUTPUT);           //  Окно №15 закрыть   
		mcp_windows2.pinMode(14, OUTPUT);           //  Окно №16 открыть  
		mcp_windows2.pinMode(15, OUTPUT);           //  Окно №16 закрыть  

	
		mcp_poliv.begin(4);                          //  U4 Управление поливом
		mcp_poliv.pinMode(0, OUTPUT);               //  Клапан полива № 1-1
		mcp_poliv.pinMode(1, OUTPUT);               //  Клапан полива № 1-2 
		mcp_poliv.pinMode(2, OUTPUT);               //  Клапан полива № 1-3 
		mcp_poliv.pinMode(3, OUTPUT);               //  Клапан полива № 1-4   
		mcp_poliv.pinMode(4, OUTPUT);               //  Клапан полива № 1-5   
		mcp_poliv.pinMode(5, OUTPUT);               //  Клапан полива № 1-6   
		mcp_poliv.pinMode(6, OUTPUT);               //  Клапан полива № 1-7   
		mcp_poliv.pinMode(7, OUTPUT);               //  Насос полива 1

		mcp_poliv.pinMode(8, OUTPUT);               //  Клапан полива № 2-1   
		mcp_poliv.pinMode(9, OUTPUT);               //  Клапан полива № 2-2   
		mcp_poliv.pinMode(10, OUTPUT);              //  Клапан полива № 2-3   
		mcp_poliv.pinMode(11, OUTPUT);              //  Клапан полива № 2-4   
		mcp_poliv.pinMode(12, OUTPUT);              //  Клапан полива № 2-5  
		mcp_poliv.pinMode(13, OUTPUT);              //  Клапан полива № 2-6     
		mcp_poliv.pinMode(14, OUTPUT);              //  Клапан полива № 2-7   
		mcp_poliv.pinMode(15, OUTPUT);              //  Насос полива2
		
		
		mcp_Rele1.begin(5);                          //  U5  Резервные реле
		mcp_Rele1.pinMode(0, OUTPUT);               //  Реле № 1
		mcp_Rele1.pinMode(1, OUTPUT);               //  Реле № 2  
		mcp_Rele1.pinMode(2, OUTPUT);               //  Реле № 3  
		mcp_Rele1.pinMode(3, OUTPUT);               //  Реле № 4  
		mcp_Rele1.pinMode(4, OUTPUT);               //  Реле № 5  
		mcp_Rele1.pinMode(5, OUTPUT);               //  Реле № 6  
		mcp_Rele1.pinMode(6, OUTPUT);               //  Реле № 7  
		mcp_Rele1.pinMode(7, OUTPUT);               //  Реле № 8  
 
		mcp_Rele1.pinMode(8, OUTPUT);               //  Реле № 9 
		mcp_Rele1.pinMode(9, OUTPUT);               //  Реле № 10  
		mcp_Rele1.pinMode(10, OUTPUT);              //  Реле № 11 
		mcp_Rele1.pinMode(11, OUTPUT);              //  Реле № 12 
		mcp_Rele1.pinMode(12, OUTPUT);              //  Реле № 13 
		mcp_Rele1.pinMode(13, OUTPUT);              //  Реле № 14   
		mcp_Rele1.pinMode(14, OUTPUT);              //  Реле № 15    
		mcp_Rele1.pinMode(15, OUTPUT);              //  Реле № 16  
		
		
		mcp_win_sensor1.begin(6);                   //  U12 Концевые датчики состояния окон
		mcp_win_sensor1.pinMode(0, INPUT);          //  Окно №1 датчик концевой
		mcp_win_sensor1.pinMode(1, INPUT);          //  Окно №2 датчик концевой  
		mcp_win_sensor1.pinMode(2, INPUT);          //  Пропущен, не подключен
		mcp_win_sensor1.pinMode(3, INPUT);          //  Окно №3 датчик концевой  
		mcp_win_sensor1.pinMode(4, INPUT);          //  Окно №4 датчик концевой  
		mcp_win_sensor1.pinMode(5, INPUT);          //  Окно №5 датчик концевой  
		mcp_win_sensor1.pinMode(6, INPUT);          //  Окно №6 датчик концевой  
		mcp_win_sensor1.pinMode(7, INPUT);          //  Окно №7 датчик концевой  

		mcp_win_sensor1.pinMode(8, INPUT);          //  Окно №8 датчик концевой  
		mcp_win_sensor1.pinMode(9, INPUT);          //  Окно №9 датчик концевой  
		mcp_win_sensor1.pinMode(10, INPUT);         //  Окно №10 датчик концевой  
		mcp_win_sensor1.pinMode(11, INPUT);         //  Окно №11 датчик концевой  
		mcp_win_sensor1.pinMode(12, INPUT);         //  Окно №12 датчик концевой 
		mcp_win_sensor1.pinMode(13, INPUT);         //  Окно №13 датчик концевой   
		mcp_win_sensor1.pinMode(14, INPUT);         //  Окно №14 датчик концевой    
		mcp_win_sensor1.pinMode(15, INPUT);         //  Окно №15 датчик концевой  
		

		for (int i = 0; i<16; i++)
			{
				mcp_win_sensor1.pullUp(i, HIGH);  
			}
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

	regBank.add(40001);  //  Адрес передачи комманд на выполнение 
	regBank.add(40002);  //  Адрес счетчика всех ошибок
	regBank.add(40003);  //  Адрес хранения величины сигнала резистором № 1
	regBank.add(40004);  //  Адрес хранения величины сигнала резистором № 2
	regBank.add(40005);  //  Адрес блока регистров для передачи в ПК таблиц.
	regBank.add(40006);  //  Адрес блока памяти для передачи в ПК таблиц.
	regBank.add(40007);  //  Адрес длины блока таблиц
	regBank.add(40008);  //  
	regBank.add(40009);  //  

	regBank.add(40010);  //  Регистры временного хранения для передачи таблицы
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


						 // Текущее время 
	regBank.add(40050);  // адрес день модуля часов контроллера
	regBank.add(40051);  // адрес месяц модуля часов контроллера
	regBank.add(40052);  // адрес год модуля часов контроллера
	regBank.add(40053);  // адрес час модуля часов контроллера
	regBank.add(40054);  // адрес минута модуля часов контроллера
	regBank.add(40055);  // адрес секунда модуля часов контроллера
 
						 // Установка времени в контроллере
	regBank.add(40056);  // адрес день
	regBank.add(40057);  // адрес месяц
	regBank.add(40058);  // адрес год
	regBank.add(40059);  // адрес час
	regBank.add(40060);  // адрес минута
	regBank.add(40061);  // 
	regBank.add(40062);  // 
	regBank.add(40063);  // 

}
void setup()  
{  
	Serial.begin(9600); 
	Serial1.begin(115200);
	Serial.println("Start setup");
	SPI.begin();

	pinMode(MCPS_CS, OUTPUT);
	digitalWrite(MCPS_CS, HIGH);
	mcps_3.begin();                                // MC№3
	mcps_6.begin();                                // MC№6
    mcps_13.begin();                               //  U13  
	
		for (int i = 0; i < 16; i++)
		 {
			 mcps_3.pinMode(i, OUTPUT);
			 mcps_3.digitalWrite(i, LOW);
			 mcps_6.pinMode(i,  OUTPUT);
			 mcps_6.digitalWrite(i,LOW);
			 mcps_13.pinMode(i, INPUT_PULLUP);  
		 }  
		
	Wire.begin();
	setup_mcp();

	if (!RTC.begin())
		{
			Serial.println("RTC failed");
			while(1);
		}; 
	//DateTime set_time = DateTime(16, 3, 15, 10, 19, 0);      // Занести данные о времени в строку "set_time" год, месяц, число, время...
	//RTC.adjust(set_time);                                    // Записать дату

	Serial.println(" ");
	//set_time();
	serial_print_date();
//	MsTimer2::set(300, flash_time);                       // 300ms период таймера прерывани
    pinMode(ledPin13, OUTPUT);  
	setup_regModbus();
	myGLCD.InitLCD();
	myGLCD.clrScr();
	myGLCD.setFont(BigFont);
	myTouch.InitTouch();
	// myTouch.setPrecision(PREC_MEDIUM);
	myTouch.setPrecision(PREC_HI);
	myButtons.setTextFont(BigFont);
	myButtons.setSymbolFont(Dingbats1_XL);
	//setup_compass();
	draw_Glav_Menu();
	wait_time_Old =  millis();

	Serial.println("Setup end");
}  
   
void loop() 
{    
    // AnalogClock();
	swichMenu();
    // test_mcp();
	// print_serial_date();
	// test_input_mcp();
	// i2c_test1();
	// delay(500);

  delay(50);
}  