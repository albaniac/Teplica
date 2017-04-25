/*
Sun_Base.ino
Visual Studio 2015
VisualMicro

Программа управления солнечной установкой.
Версия:               - 1.0
Автор:                - Мосейчук А.В.
Дата начала работ:    - 16.04.2017г.
Дата окончания работ: - 00.00.2017г.
 

 /*

 DS18B20 №1  oneWire_in
 Черный - GND
 Белый - 8
 Красный - +5в

 DS18B20 №1  oneWire_in
 Черный - GND
 Белый - 9
 Красный - +5в

 DS18B20 №2  oneWire_out
 Черный - GND
 Белый - 10
 Красный - +5в

 DS18B20 №3  oneWire_sun
 Черный - GND
 Белый - 11
 Красный - +5в



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
#include <BH1750.h>




#define led_Green 12                                     // Светодиод на передней панели зеленый
#define led_Red   13                                     // Светодиод на передней панели красный

#define Chanal_A   A8                                    // Выход канала А блока коммутаторов
#define Chanal_B   A9                                    // Выход канала B блока коммутаторов


//#define Rele1       8                                    // Управление реле 1
//#define Rele2       9                                    // Управление реле 2
//#define Rele3      10                                    // Управление реле 3

MCP23017 mcp_Out1;                                       // Назначение портов расширения MCP23017  4 A - Out, B - Out
MCP23017 mcp_Out2;                                       // Назначение портов расширения MCP23017  6 A - Out, B - Out




#define  SW_West   9                    // Назначение концевик Запад  
#define  SW_East   8                    // Назначение концевик Восток 
#define  SW_High   10                    // Назначение концевик Верх
#define  SW_Down   11                    // Назначение концевик Низ
//#define  Rele1    10                    // Назначение Реле 1
//#define  Rele2    11                    // Назначение Реле 2  
#define  motor_West  0                 // Назначение  мотор Запад
#define  motor_East  1                 // Назначение  мотор Восток  Светодиод подсоединен к цифровому выводу 13 
#define  motor_High  2                 // Назначение  мотор Вверх
#define  motor_Down  3                 // Назначение  мотор Вниз 


//+++++++++++++++++++ MODBUS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

modbusDevice regBank;
modbusSlave slave;

//+++++++++++++++++++++++ Настройка электронного резистора +++++++++++++++++++++++++++++++++++++
byte resistance          = 0x00;                        // Сопротивление 0x00..0xFF - 0Ом..100кОм

//+++++++++++++++++++++++++++++ Внешняя память +++++++++++++++++++++++++++++++++++++++
int deviceaddress        = 80;                          // Адрес микросхемы памяти
unsigned int eeaddress   =  0;                          // Адрес ячейки памяти
byte hi;                                                // Старший байт для преобразования числа
byte low;                                               // Младший байт для преобразования числа

unsigned int adr_memN1_1 = 0;                           // Начальный адрес памяти таблицы соответствия контактов разъемов №1А, №1В
unsigned int adr_memN1_2 = 0;                           // Начальный адрес памяти таблицы соответствия контактов разъемов №2А, №2В

unsigned int adr_tempN1 = 10;                           // адрес памяти данных датчика температуры №1
unsigned int adr_tempN2 = 12;                           // адрес памяти данных датчика температуры №2
unsigned int adr_tempN3 = 14;                           // адрес памяти данных датчика температуры №3
unsigned int adr_tempN4 = 16;                           // адрес памяти данных датчика температуры №4
unsigned int adr_radius = 18;                           // адрес памяти данных датчика компаса
unsigned int adr_asimut = 20;                           // адрес памяти данных датчика вертикали


//********************* Настройка монитора ***********************************
UTFT          myGLCD(ITDB32S,38,39,40,41);                // Дисплей 3.2"
UTouch        myTouch(6, 5, 4, 3, 2);                   // Standard Arduino Mega/Due shield            : 6,5,4,3,2
UTFT_Buttons  myButtons(&myGLCD, &myTouch);             // Finally we set up UTFT_Buttons :)

boolean default_colors = true;                          //
uint8_t menu_redraw_required = 0;
// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t Dingbats1_XL[];
extern uint8_t SmallSymbolFont[];

//++++++++++++++++++++++++ Параметры температуры +++++++++++++++++++++++++++++++++


float temp_sun_in = 0.00;                // Результат измерения датчика температуры №1
float temp_sun_out = 0.00;               // Результат измерения датчика температуры №2
float temp_tube_in = 0.00;               // Результат измерения датчика температуры №3
float temp_tube_out = 0.00;              // Результат измерения датчика температуры №4
//float temp_tank = 0.00;
//float temp_out = 0.00;


#define ONE_WIRE_BUS_1 8                // Назначение DS1820 №1  
#define ONE_WIRE_BUS_2 9                // Назначение DS1820 №2  
#define ONE_WIRE_BUS_3 10               // Назначение DS1820 №3  
#define ONE_WIRE_BUS_4 11               // Назначение DS1820 №4  
//#define ONE_WIRE_BUS_5 48
//#define ONE_WIRE_BUS_6 49

OneWire oneWire_sun_in(ONE_WIRE_BUS_1);
OneWire oneWire_sun_out(ONE_WIRE_BUS_2);
OneWire oneWire_tube_in(ONE_WIRE_BUS_3);
OneWire oneWire_tube_out(ONE_WIRE_BUS_4);
//OneWire oneWire_tank(ONE_WIRE_BUS_5);
//OneWire oneWire_out(ONE_WIRE_BUS_6);

DallasTemperature sensor_sun_in(&oneWire_sun_in);
DallasTemperature sensor_sun_out(&oneWire_sun_out);
DallasTemperature sensor_tube_in(&oneWire_tube_in);
DallasTemperature sensor_tube_out(&oneWire_tube_out);
//DallasTemperature sensor_tank(&oneWire_tank);
//DallasTemperature sensor_outhouse(&oneWire_out);

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

int kompassCenterX = 220;
int kompassCenterY = 75;
int headingCenterX = 100;
int headingCenterY = 95;
int poz_min = 50;
int poz_max = 280;
int head_min = 14;
int head_max = 57;


int upr_head = 0;



float x1_temp, y1_temp, x2_temp, y2_temp, x3_temp, y3_temp, x4_temp, y4_temp;
float x1_tempH, y1_tempH, x2_tempH, y2_tempH, x3_tempH, y3_tempH, x4_tempH, y4_tempH;
float x1_tempC, y1_tempC, x2_tempC, y2_tempC, x3_tempC, y3_tempC, x4_tempC, y4_tempC;
int upr_motor = 0;              // Направление управления моторами

//+++++++++++++++++++++++++ Расчет положения солнца ++++++++++++++++++++++++++


//////////////////////////////////////////////////  
//PUT YOUR LATITUDE, LONGITUDE, AND TIME ZONE HERE
  float latitude = 55.59;
  float longitude = 37.12;
  float timezone = 3;
//////////////////////////////////////////////////  
  
//If you live in the southern hemisphere, it would probably be easier
//for you if you make north as the direction where the azimuth equals
//0 degrees. To do so, switch the 0 below with 180.  
  float northOrSouth = 180;

/////////////////////////////////////////////////////////// 
//MISC. VARIABLES
///////////////////////////////////////////////////////////  
  float pi = 3.14159265;
  float altitude;
  float azimuth;
  float delta;
  float h;
/////////////////////////////////////////////////////////// 




//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BH1750 lightMeter(0x23);

//+++++++++++++++++++++++++++ Настройка часов +++++++++++++++++++++++++++++++
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
unsigned long wait_time        = 0;                               // Время простоя прибора
unsigned long wait_time_Old    = 0;                               // Время простоя прибора
int time_minute                = 5;                               // Время простоя прибора
int pin_cable                  = 0;                               // Количество выводов кабеля
//------------------------------------------------------------------------------

const unsigned int adr_control_command    PROGMEM       = 40001;  // Адрес передачи комманд на выполнение
const unsigned int adr_reg_count_err      PROGMEM       = 40002;  // Адрес счетчика всех ошибок
//-------------------------------------------------------------------------------------------------------

//++++++++++++++++++++++++++++ Переменные для цифровой клавиатуры +++++++++++++++++++++++++++++
int x, y, z;
char stCurrent[20]    = "";                                       // Переменная хранения введенной строки
int stCurrentLen      = 0;                                        // Переменная хранения длины введенной строки
int stCurrentLen1     = 0;                                        // Переменная временного хранения длины введенной строки
char stLast[20]       = "";                                       // Данные в введенной строке строке.
int ret               = 0;                                        // Признак прерывания операции
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------
//Назначение переменных для хранения № опций меню (клавиш)
int but1, but2, but3, but4, but5, but6, but7, but8, but9, but10, butX, butY, butA, butB, butC, butD, but_m1, but_m2, but_m3, but_m4, but_m5, pressed_button;

int m2 = 1; // Переменная номера меню
int m3 = 0; // Переменная номера меню

//------------------------------------------------------------------------------------------------------------------
// Назначение переменных для хранения текстов

//char  txt_menu1_1[]            = " ";                                    //  
//char  txt_menu1_2[]            = " ";                                    //  
//char  txt_menu1_3[]            = " ";                                    //  
//char  txt_menu1_4[]            = " ";                                    //  
//char  txt_menu2_1[]            = " ";                                    //                                   
//char  txt_menu2_2[]            = "=========";                                                              // ================      
//char  txt_menu2_3[]            = "=========";                                                              // ================      
//char  txt_menu2_4[]            = "=========";                                                              // ================  
//char  txt_menu3_1[]            = " ";                   // 
//char  txt_menu3_2[]            = " ";                   //  
//char  txt_menu3_3[]            = " ";                   //  
//char  txt_menu3_4[]            = " ";                   // 
//char  txt_menu4_1[]            = " ";                                   // 
//char  txt_menu4_2[]            = " ";                        //  
//char  txt_menu4_3[]            = " ";                    //  
//char  txt_menu4_4[]            = " ";                             //  
char  txt_menu5_1[]              = "=========";                                                              // ================  
char  txt_menu5_2[]              = "=========";                                                              // ================  
char  txt_menu5_3[]              = "=========";                                                              // ================  
char  txt_menu5_4[]              = "Bpe""\xA1\xAF"" ""\xA3""poc""\xA4""o""\xAF";                             // Время простоя


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
  DateTime set_time = DateTime(year, month, day, hour, minute, second); // Занести данные о времени в строку "set_time"
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
	  if (((y >= 200) && (y <= 239)) && ((x >= 260) && (x <= 319))) //установка часов
	  {
		myGLCD.setColor (255, 0, 0);
		myGLCD.drawRoundRect(260, 200, 319, 239);
		setClock();
	  }

	  if (((y >= 200) && (y <= 239)) && ((x >= 1) && (x <= 250))) //Возврат 
	  {
		myGLCD.clrScr();
		myGLCD.setFont(BigFont);
		if (m2 == 2)
		{
			view_menuN2();
		}

		break;
	  }
	  if (((y >= 1) && (y <= 199)) && ((x >= 1) && (x <= 319))) //Возврат
	  {
		myGLCD.clrScr();
		myGLCD.setFont(BigFont);
		if (m2 == 2)
		{
			view_menuN2();
		}
		break;
	  }
	}
	delay(10);
	clock_read();
  }
}

void flash_time()                                              // Программа обработчик прерывания
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
  // PORTB = B00000000; // пин 12 переводим в состояние LOW
  slave.run();
  // PORTB = B01000000; // пин 12 переводим в состояние HIGH
}
void serialEvent3()
{
  control_command();
}

void klav123() // ввод данных с цифровой клавиатуры
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
  myGLCD.print("O""\xA4\xA1""e""\xA2""a", 20, 147);                                   // "Отмена"


  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (130, 130, 240, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (130, 130, 240, 180);
  myGLCD.print("B""\x97""o""\x99", 155, 147);                                  // "Ввод"


  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (250, 130, 300, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (250, 130, 300, 180);
  myGLCD.print("B""\xAB""x", 252, 147);                                  // Вых
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
  { // Вывод строки "ПЕРЕПОЛНЕНИЕ!"
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
void waitForIt_Upr(int x1, int y1, int x2, int y2, int upr)
{
	myGLCD.setColor(255, 0, 0);
	myGLCD.drawRoundRect(x1, y1, x2, y2);
	myGLCD.setColor(255, 255, 255);
  
	drawKompass0();
	while (myTouch.dataAvailable())
	{
		myGLCD.printNumI(upr_motor, 70, 130);
	
		switch (upr)
		{
		case 1:
			upr_head++;
			break;
		case 2:
			upr_head--;
			break;
		case 3:
			upr_motor++;
			break;
		case 4:
		
			upr_motor--;
			break;
		}

		delay(25);
		headingDegrees = upr_motor;
		if (headingDegrees >= poz_max) headingDegrees = poz_max;
		if (headingDegrees <= poz_min) headingDegrees = poz_min;
		if (upr_head >= head_max) upr_head = head_max;
		if (upr_head <= head_min) upr_head = head_min;


		drawKompass(headingDegrees);                          // Показания компаса
		draw_azimuth(azimuth);                                // Расчетное положение по горизонтали
		draw_header(upr_head);                                // Положение по вертикали
		draw_headerCalc(altitude);                            // Расчетное положение по вертикали

		myGLCD.printNumI(upr_head, 70, 5);
		myGLCD.printNumI(altitude, 70, 20);

	}
	myTouch.read();
	myGLCD.drawRoundRect(x1, y1, x2, y2);
}
void control_command()
{
  /*
	Для вызова подпрограммы проверки необходимо записать номер проверки по адресу adr_control_command (40120)
	Код проверки
	0 -   Выполнение команды окончено
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


  int test_n = regBank.get(adr_control_command);   //адрес  40000
  if (test_n != 0)
  {
	if (test_n != 0) Serial.println(test_n);
	switch (test_n)
	{
	  case 1:
	
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
	  case 20:                                 
		//
		break;
	  case 21:                      		  
		//
		break;
	  case 22:                                
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
		regBank.set(adr_control_command, 0);       //  
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

void draw_Glav_Menu()
{
	myGLCD.clrScr();
	Set_Down_Buttons();
	draw_measure();
}
void swichMenu()
{
	m2 = 1;
	while (1)
	{
		view_menuN1();

    	wait_time = millis();                                      // Программа вызова часов при простое
		if (wait_time - wait_time_Old > 6000 * 1)                  // if (wait_time - wait_time_Old > 60000 * time_minute)
		{
			wait_time_Old = millis();
			//sun_calc();
			// AnalogClock();
			// myGLCD.clrScr();
		}

		if (myTouch.dataAvailable() == true)                       // Проверить нажатие кнопок
		{

			myTouch.read();
			x = myTouch.getX();
			y = myTouch.getY();

			if ((y >= 189) && (y <= 239))                          //нажата кнопка 
			{
				myGLCD.setFont(BigFont);
				
				if ((x >= 10) && (x <= 60))                       //нажата кнопка "1"
				{
					waitForIt(10, 189, 60, 239);
					m2 = 1;
					myGLCD.clrScr();
					Set_Down_Buttons();
					draw_measure();                               // Отобразить результат измерения
				}

				if ((x >= 70) && (x <= 120))                      //нажата кнопка "2"
				{
					waitForIt(70, 189, 120, 239);
					m2 = 2;
					clear_display();                             // Очистить экран
					view_menuN2();
					//myGLCD.clrScr();
				}

				if ((x >= 130) && (x <= 180))                     //нажата кнопка "3"
				{
					waitForIt(130, 189, 180, 239);
					m2 = 3;
					clear_display();                             // Очистить экран
					view_menuN3();


				}

				if ((x >= 190) && (x <= 240))                    //нажата кнопка "4"
				{
					waitForIt(190, 189, 240, 239);
					m2 = 4;
					clear_display();                             // Очистить экран
					view_menuN4();


				}

				if ((x >= 250) && (x <= 300))                    //нажата кнопка "5"
				{
					waitForIt(250, 189, 300, 239);
					m3 = 1;
					AnalogClock();
				//	myGLCD.clrScr();
					Set_Down_Buttons();
					m3 = 0;
					if (m2 == 1)
					{
						draw_measure();                          // Отобразить результат измерения

					}
				}
			}

			if (m2 == 2)
			{

				if ((x >= 10) && (x <= 50))                          //нажата кнопка 
				{
					myGLCD.setFont(BigFont);

					if ((y >= 10) && (y <= 90))                       //нажата кнопка "Верх"
					{
						waitForIt_Upr(10, 10, 50, 90,1);

					}

					if ((y >= 100) && (y <= 180))                      //нажата кнопка "Вниз"
					{
						waitForIt_Upr(10, 100, 50, 180,2);
					}

				}
				if ((y >= 150) && (y <= 180))                          //нажата кнопка 
				{
					myGLCD.setFont(BigFont);

					if ((x >= 130) && (x <= 210))                       //нажата кнопка "Восток"
					{
						waitForIt_Upr(130, 150, 210, 180,4);

					}

					if ((x >= 220) && (x <= 300))                      //нажата кнопка "Запад"
					{
						waitForIt_Upr(220, 150, 300, 180,3);

					}
				}
			}
		}
	}
}

void view_menuN1()
{
		if (m2 == 1 && m3 == 0)
	{
		sun_calc();
		myGLCD.setColor(255, 255, 255);
		myGLCD.setBackColor(0, 0, 255);
		myGLCD.printNumF(temp_sun_in, 2, 40, 26);
		myGLCD.printNumF(temp_sun_out, 2, 196, 26);
		myGLCD.printNumF(temp_tube_in, 2, 40, 66);
		myGLCD.printNumF(temp_tube_out, 2, 196, 66);
		myGLCD.printNumF(azimuth, 2, 40, 106);
		myGLCD.printNumF(altitude, 2, 196, 106);
		myGLCD.printNumF(headingDegrees, 2, 40, 153);
		myGLCD.printNumF(10.00, 2, 196, 153);
		myGLCD.setBackColor(0, 0, 0);
		
	}
}
void view_menuN2()
{
	//myGLCD.clrScr();

	myGLCD.setColor(255, 255, 255);                                             // Белая окантовка
	myGLCD.setBackColor(0, 0, 0);
	myGLCD.drawRoundRect(10, 10, 50, 90);                              // Кнопка "Вверх"
	myGLCD.drawRoundRect(10, 100, 50, 180);                             // Кнопка "Вниз"
	myGLCD.drawRoundRect(130, 150, 210, 180);
	myGLCD.drawRoundRect(220, 150, 300, 180);

	for (int i = 0; i < 3; i++)                                      // Внешний круг компаса
	{    
		myGLCD.drawCircle(kompassCenterX, kompassCenterY, 65 - i);   
	}

	for (int i = 0; i < 5; i++)                                        // Центр компаса
	{
		myGLCD.drawCircle(kompassCenterX, kompassCenterY, i);
	}

	for (int i = 0; i < 5; i++)                                        // центр вертикального положения
	{
		myGLCD.drawCircle(headingCenterX, headingCenterY, i);
	}

	myGLCD.setColor(192, 192, 255);
	myGLCD.print("90", kompassCenterX + 60, kompassCenterY - 8);
	myGLCD.print("180", kompassCenterX - 20, kompassCenterY + 58);
	myGLCD.print("270", kompassCenterX - 109, kompassCenterY - 8);
	myGLCD.print("0", kompassCenterX - 6, kompassCenterY - 74);

	drawMaxMin(poz_min, poz_max);
	
	drawKompass(headingDegrees);
	draw_azimuth(azimuth);
	draw_header(headingDegrees);
	draw_headerCalc(altitude);

	myGLCD.setColor(0, 0, 255);

	myGLCD.fillRoundRect(11, 11, 49, 89);
	myGLCD.fillRoundRect(11, 101, 49, 179);
	myGLCD.fillRoundRect(131, 151, 209, 179);
	myGLCD.fillRoundRect(221, 151, 299, 179);
	
	myGLCD.setColor(255, 255, 255);
	myGLCD.setBackColor(0, 0, 255);

	myGLCD.print("East", 140, 157);
	myGLCD.print("West", 227, 157);
	
	myGLCD.print("Up", 40, 36,90);
	myGLCD.print("Down", 40, 110,90);

	myGLCD.setBackColor(0, 0, 0);



	//if (m2 == 1 && m3 == 0)
	//{
	//	sun_calc();
	//	myGLCD.setColor(255, 255, 255);
	//	myGLCD.setBackColor(0, 0, 255);
	//	myGLCD.printNumF(temp_sun_in, 2, 40, 26);
	//	myGLCD.printNumF(temp_sun_out, 2, 196, 26);
	//	myGLCD.printNumF(temp_tube_in, 2, 40, 66);
	//	myGLCD.printNumF(temp_tube_out, 2, 196, 66);
	//	myGLCD.printNumF(azimuth, 2, 40, 106);
	//	myGLCD.printNumF(altitude, 2, 196, 106);
	//	myGLCD.printNumF(headingDegrees, 2, 40, 153);
	//	myGLCD.printNumF(10.00, 2, 196, 153);
	//	myGLCD.setBackColor(0, 0, 0);

	//}

	
}


void drawKompass(int m)
{
	myGLCD.setColor(0, 255, 0);
	myGLCD.print("   ", 70, 150);
	if (m < 10)
	{
		myGLCD.printNumI(m, 70+24, 150);
	}
	else if (m < 100 && m > 9)
	{
		myGLCD.printNumI(m, 70+12, 150);
	}
	else if(m > 99)
	{
		myGLCD.printNumI(m, 70, 150);

	}

	drawKompass0();
	float x1, y1, x2, y2, x3, y3, x4, y4;
	myGLCD.setColor(0, 255, 0);
	m = m + 270;

	x1 = 58 * cos(m * 0.0175);
	y1 = 58 * sin(m * 0.0175);
	x2 = 5 * cos(m * 0.0175);
	y2 = 5 * sin(m * 0.0175);
	x3 = 20 * cos((m + 4) * 0.0175);
	y3 = 20 * sin((m + 4) * 0.0175);
	x4 = 20 * cos((m - 4) * 0.0175);
	y4 = 20 * sin((m - 4) * 0.0175);

	myGLCD.drawLine(x1 + kompassCenterX, y1 + kompassCenterY, x3 + kompassCenterX, y3 + kompassCenterY);
	myGLCD.drawLine(x3 + kompassCenterX, y3 + kompassCenterY, x2 + kompassCenterX, y2 + kompassCenterY);
	myGLCD.drawLine(x2 + kompassCenterX, y2 + kompassCenterY, x4 + kompassCenterX, y4 + kompassCenterY);
	myGLCD.drawLine(x4 + kompassCenterX, y4 + kompassCenterY, x1 + kompassCenterX, y1 + kompassCenterY);


	x1_temp = x1;
	y1_temp = y1;
	x2_temp = x2;
	y2_temp = y2;
	x3_temp = x3;
	y3_temp = y3;
	x4_temp = x4;
	y4_temp = y4;
}
void drawKompass0()
{
	float x1, y1, x2, y2, x3, y3, x4, y4;
	myGLCD.setColor(0, 0, 0);
	x1 = x1_temp;
	y1 = y1_temp;
	x2 = x2_temp;
	y2 = y2_temp;
	x3 = x3_temp;
	y3 = y3_temp;
	x4 = x4_temp;
	y4 = y4_temp;

	myGLCD.drawLine(x1 + kompassCenterX, y1 + kompassCenterY, x3 + kompassCenterX, y3 + kompassCenterY);
	myGLCD.drawLine(x3 + kompassCenterX, y3 + kompassCenterY, x2 + kompassCenterX, y2 + kompassCenterY);
	myGLCD.drawLine(x2 + kompassCenterX, y2 + kompassCenterY, x4 + kompassCenterX, y4 + kompassCenterY);
	myGLCD.drawLine(x4 + kompassCenterX, y4 + kompassCenterY, x1 + kompassCenterX, y1 + kompassCenterY);
}
void draw_azimuth(int m)
{
	myGLCD.setColor(255, 255, 0);
	myGLCD.print("   ", 70, 165);
	if (m < 10)
	{
		myGLCD.printNumI(m, 70 + 24, 165);
	}
	else if (m < 100 && m > 9)
	{
		myGLCD.printNumI(m, 70 + 12, 165);
	}
	else if (m > 99)
	{
		myGLCD.printNumI(m, 70, 165);

	}

	m = m/6;
	float x1, y1, x2, y2, x3, y3, x4, y4;
	int pm = m - 1;

	myGLCD.setColor(0, 0, 0);
	if (pm == -1)
		pm = 59;
	pm = pm * 6;
	pm = pm + 270;

	x1 = 58 * cos(pm * 0.0175);
	y1 = 58 * sin(pm * 0.0175);
	x2 = 5 * cos(pm * 0.0175);
	y2 = 5 * sin(pm * 0.0175);
	x3 = 20 * cos((pm + 4) * 0.0175);
	y3 = 20 * sin((pm + 4) * 0.0175);
	x4 = 20 * cos((pm - 4) * 0.0175);
	y4 = 20 * sin((pm - 4) * 0.0175);

	myGLCD.drawLine(x1 + kompassCenterX, y1 + kompassCenterY, x3 + kompassCenterX, y3 + kompassCenterY);
	myGLCD.drawLine(x3 + kompassCenterX, y3 + kompassCenterY, x2 + kompassCenterX, y2 + kompassCenterY);
	myGLCD.drawLine(x2 + kompassCenterX, y2 + kompassCenterY, x4 + kompassCenterX, y4 + kompassCenterY);
	myGLCD.drawLine(x4 + kompassCenterX, y4 + kompassCenterY, x1 + kompassCenterX, y1 + kompassCenterY);

	myGLCD.setColor(255, 255, 0);
	m = m * 6;
	m = m + 270;

	x1 = 58 * cos(m * 0.0175);
	y1 = 58 * sin(m * 0.0175);
	x2 = 5 * cos(m * 0.0175);
	y2 = 5 * sin(m * 0.0175);
	x3 = 20 * cos((m + 4) * 0.0175);
	y3 = 20 * sin((m + 4) * 0.0175);
	x4 = 20 * cos((m - 4) * 0.0175);
	y4 = 20 * sin((m - 4) * 0.0175);

	myGLCD.drawLine(x1 + kompassCenterX, y1 + kompassCenterY, x3 + kompassCenterX, y3 + kompassCenterY);
	myGLCD.drawLine(x3 + kompassCenterX, y3 + kompassCenterY, x2 + kompassCenterX, y2 + kompassCenterY);
	myGLCD.drawLine(x2 + kompassCenterX, y2 + kompassCenterY, x4 + kompassCenterX, y4 + kompassCenterY);
	myGLCD.drawLine(x4 + kompassCenterX, y4 + kompassCenterY, x1 + kompassCenterX, y1 + kompassCenterY);
}
void drawMaxMin(int min, int max)
{
	float x1, y1, x2, y2, x3, y3, x4, y4;

	myGLCD.setColor(255, 0, 0);
	min = min + 270-2;

	x1 = 60 * cos(min * 0.0175);
	y1 = 60 * sin(min * 0.0175);
	x2 = 5 * cos(min * 0.0175);
	y2 = 5 * sin(min * 0.0175);
	x3 = 20 * cos((min + 4) * 0.0175);
	y3 = 20 * sin((min + 4) * 0.0175);
	x4 = 20 * cos((min - 4) * 0.0175);
	y4 = 20 * sin((min - 4) * 0.0175);

	myGLCD.drawLine(x1 + kompassCenterX, y1 + kompassCenterY, x3 + kompassCenterX, y3 + kompassCenterY);
	myGLCD.drawLine(x3 + kompassCenterX, y3 + kompassCenterY, x2 + kompassCenterX, y2 + kompassCenterY);
	myGLCD.drawLine(x2 + kompassCenterX, y2 + kompassCenterY, x4 + kompassCenterX, y4 + kompassCenterY);
	myGLCD.drawLine(x4 + kompassCenterX, y4 + kompassCenterY, x1 + kompassCenterX, y1 + kompassCenterY);

	max = max + 270+2;

	x1 = 60 * cos(max * 0.0175);
	y1 = 60 * sin(max * 0.0175);
	x2 = 5 * cos(max * 0.0175);
	y2 = 5 * sin(max * 0.0175);
	x3 = 20 * cos((max + 4) * 0.0175);
	y3 = 20 * sin((max + 4) * 0.0175);
	x4 = 20 * cos((max - 4) * 0.0175);
	y4 = 20 * sin((max - 4) * 0.0175);

	myGLCD.drawLine(x1 + kompassCenterX, y1 + kompassCenterY, x3 + kompassCenterX, y3 + kompassCenterY);
	myGLCD.drawLine(x3 + kompassCenterX, y3 + kompassCenterY, x2 + kompassCenterX, y2 + kompassCenterY);
	myGLCD.drawLine(x2 + kompassCenterX, y2 + kompassCenterY, x4 + kompassCenterX, y4 + kompassCenterY);
	myGLCD.drawLine(x4 + kompassCenterX, y4 + kompassCenterY, x1 + kompassCenterX, y1 + kompassCenterY);


}
void draw_header(int m)
{
	float x1, y1, x2, y2, x3, y3, x4, y4;
	m = -m;
	m = m + 360;

	myGLCD.setColor(0, 0, 0);
	x1 = x1_tempH;
	y1 = y1_tempH;
	x2 = x2_tempH;
	y2 = y2_tempH;
	x3 = x3_tempH;
	y3 = y3_tempH;
	x4 = x4_tempH;
	y4 = y4_tempH;

	myGLCD.drawLine(x1 + headingCenterX, y1 + headingCenterY, x3 + headingCenterX, y3 + headingCenterY);
	myGLCD.drawLine(x3 + headingCenterX, y3 + headingCenterY, x2 + headingCenterX, y2 + headingCenterY);
	myGLCD.drawLine(x2 + headingCenterX, y2 + headingCenterY, x4 + headingCenterX, y4 + headingCenterY);
	myGLCD.drawLine(x4 + headingCenterX, y4 + headingCenterY, x1 + headingCenterX, y1 + headingCenterY);


	myGLCD.setColor(0, 255, 0);
	m = m + 270;

	x1 = 58 * cos(m * 0.0175);
	y1 = 58 * sin(m * 0.0175);
	x2 = 5 * cos(m * 0.0175);
	y2 = 5 * sin(m * 0.0175);
	x3 = 20 * cos((m + 4) * 0.0175);
	y3 = 20 * sin((m + 4) * 0.0175);
	x4 = 20 * cos((m - 4) * 0.0175);
	y4 = 20 * sin((m - 4) * 0.0175);

	myGLCD.drawLine(x1 + headingCenterX, y1 + headingCenterY, x3 + headingCenterX, y3 + headingCenterY);
	myGLCD.drawLine(x3 + headingCenterX, y3 + headingCenterY, x2 + headingCenterX, y2 + headingCenterY);
	myGLCD.drawLine(x2 + headingCenterX, y2 + headingCenterY, x4 + headingCenterX, y4 + headingCenterY);
	myGLCD.drawLine(x4 + headingCenterX, y4 + headingCenterY, x1 + headingCenterX, y1 + headingCenterY);


	x1_tempH = x1;
	y1_tempH = y1;
	x2_tempH = x2;
	y2_tempH = y2;
	x3_tempH = x3;
	y3_tempH = y3;
	x4_tempH = x4;
	y4_tempH = y4;

}
void draw_headerCalc(int m)
{
	float x1, y1, x2, y2, x3, y3, x4, y4;
	m = -m;
	m = m + 360;
	myGLCD.setColor(0, 0, 0);
	x1 = x1_tempC;
	y1 = y1_tempC;
	x2 = x2_tempC;
	y2 = y2_tempC;
	x3 = x3_tempC;
	y3 = y3_tempC;
	x4 = x4_tempC;
	y4 = y4_tempC;

	myGLCD.drawLine(x1 + headingCenterX, y1 + headingCenterY, x3 + headingCenterX, y3 + headingCenterY);
	myGLCD.drawLine(x3 + headingCenterX, y3 + headingCenterY, x2 + headingCenterX, y2 + headingCenterY);
	myGLCD.drawLine(x2 + headingCenterX, y2 + headingCenterY, x4 + headingCenterX, y4 + headingCenterY);
	myGLCD.drawLine(x4 + headingCenterX, y4 + headingCenterY, x1 + headingCenterX, y1 + headingCenterY);


	myGLCD.setColor(255, 255, 0);
	m = m + 270;

	x1 = 58 * cos(m * 0.0175);
	y1 = 58 * sin(m * 0.0175);
	x2 = 5 * cos(m * 0.0175);
	y2 = 5 * sin(m * 0.0175);
	x3 = 20 * cos((m + 4) * 0.0175);
	y3 = 20 * sin((m + 4) * 0.0175);
	x4 = 20 * cos((m - 4) * 0.0175);
	y4 = 20 * sin((m - 4) * 0.0175);

	myGLCD.drawLine(x1 + headingCenterX, y1 + headingCenterY, x3 + headingCenterX, y3 + headingCenterY);
	myGLCD.drawLine(x3 + headingCenterX, y3 + headingCenterY, x2 + headingCenterX, y2 + headingCenterY);
	myGLCD.drawLine(x2 + headingCenterX, y2 + headingCenterY, x4 + headingCenterX, y4 + headingCenterY);
	myGLCD.drawLine(x4 + headingCenterX, y4 + headingCenterY, x1 + headingCenterX, y1 + headingCenterY);


	x1_tempC = x1;
	y1_tempC = y1;
	x2_tempC = x2;
	y2_tempC = y2;
	x3_tempC = x3;
	y3_tempC = y3;
	x4_tempC = x4;
	y4_tempC = y4;

}
void view_menuN3()
{

}
void view_menuN4()
{

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
	// тут мы эти hi,low можем сохранить EEPROM
	i2c_eeprom_write_byte(deviceaddress, adr, hi);
	i2c_eeprom_write_byte(deviceaddress, adr + 1, low);
}

void clear_eeprom(int start, int long_mem)
{
	for (int i = start; i < long_mem; i++)                            // Очистить блока регистров в памяти.        
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
	//sensor_tank.requestTemperatures();
	//sensor_outhouse.requestTemperatures();

	temp_sun_in = sensor_sun_in.getTempCByIndex(0);
	temp_sun_out = sensor_sun_out.getTempCByIndex(0);
	temp_tube_in = sensor_tube_in.getTempCByIndex(0);
	temp_tube_out = sensor_tube_out.getTempCByIndex(0);
	//temp_tank = sensor_tank.getTempCByIndex(0);
	//temp_out = sensor_outhouse.getTempCByIndex(0);
	
	Serial.print("sensor_sun_in: ");
	Serial.println(temp_sun_in);

	Serial.print("sensor_sun_out: ");
	Serial.println(temp_sun_out);

	Serial.print("sensor_tube_in: ");
	Serial.println(temp_tube_in);

	Serial.print("sensor_tube_out: ");
	Serial.println(temp_tube_out);

	//Serial.print("sensor_tank: ");
	//Serial.println(temp_tank);

	//Serial.print("Outhouse: ");
	//Serial.println(temp_out);

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
void sun_calc()
{
	float month2;
	float day2;
	float hour2;
	float minute2;
	clock_read();
	//SET TIME AND DATE HERE//////////////
	month2 = month;
	day2 = day;
	hour2 = hour;//Use 24hr clock (ex: 1:00pm = 13:00) and don't use day3light saving time.
	minute2 = minute;
	//END SET TIME AND DATE /////////////


	//START OF THE CODE THAT CALCULATES THE POSITION OF THE SUN
	float n = daynum(month2) + day2;//NUMBER OF dayS SINCE THE START OF THE YEAR. 
	delta = .409279 * sin(2 * pi * ((284 + n) / 365.25));//SUN'S DECLINATION.
	day2 = dayToArrayNum(day2);//TAKES THE CURRENT day OF THE MONTH AND CHANGES IT TO A LOOK UP VALUE ON THE HOUR ANGLE TABLE.
	h = (FindH(day2, month2)) + longitude + (timezone * -1 * 15);//FINDS THE NOON HOUR ANGLE ON THE TABLE AND MODIFIES IT FOR THE USER'S OWN LOCATION AND TIME ZONE.
	h = ((((hour2 + minute2 / 60) - 12) * 15) + h)*pi / 180;//FURTHER MODIFIES THE NOON HOUR ANGLE OF THE CURRENT day AND TURNS IT INTO THE HOUR ANGLE FOR THE CURRENT HOUR AND MINUTE.
	altitude = (asin(sin(latitude) * sin(delta) + cos(latitude) * cos(delta) * cos(h))) * 180 / pi;//FINDS THE SUN'S ALTITUDE.
	azimuth = ((atan2((sin(h)), ((cos(h) * sin(latitude)) - tan(delta) * cos(latitude)))) + (northOrSouth*pi / 180)) * 180 / pi;//FINDS THE SUN'S AZIMUTH.
	
	Serial.print("month2 ");
	Serial.println(month2);
	Serial.print("day2 ");
	Serial.println(day);   
	Serial.print("hour2 ");
	Serial.println(hour2);
	Serial.print("minute2 ");
	Serial.println(minute2);   //END OF THE CODE THAT CALCULATES THE POSITION OF THE SUN

	Serial.println("Altitude");
	Serial.println(altitude);
	Serial.println("Azimuth");
	Serial.println(azimuth);

}

void clear_display()
{
	myGLCD.setColor(0, 0, 0);
	myGLCD.fillRoundRect(1, 1, 319, 188);
	myGLCD.setColor(255, 255, 255);
}

void print_up() // Печать верхней строчки над меню
{
  myGLCD.setColor(0, 255, 0);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.print("                      ", CENTER, 0);
  switch (m2)
  {
	case 1:
	/*  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[2])));
	  myGLCD.print(buffer, CENTER, 0);          */                       // txt_info1
	  break;
	case 2:
	/*  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[3])));
	  myGLCD.print(buffer, CENTER, 0);    */                             // txt_info2
	  break;
	case 3:
	/*  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[4])));
	  myGLCD.print(buffer, CENTER, 0);      */                           // txt_info3
	  break;
	case 4:
	/*  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[5])));
	  myGLCD.print(buffer, CENTER, 0);      */                           // txt_info4
	  break;
	case 5:
	/*  strcpy_P(buffer, (char*)pgm_read_word(&(table_message[6])));
	  myGLCD.print(buffer, CENTER, 0);         */                        // txt_info5
	  break;
  }
}
void draw_measure()
{
	myGLCD.setColor(255, 255, 255);                                             // Белая окантовка
	
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

void setup_pin()
{
	pinMode(led_Red, OUTPUT);                             //
	pinMode(led_Green, OUTPUT);                           //
	digitalWrite(led_Red, HIGH);                          //
	digitalWrite(led_Green, LOW);                         //
	//pinMode(Chanal_A, INPUT);                                   // Выход коммутаторов блока А
	//pinMode(Chanal_B, INPUT);                                   // Выход коммутаторов блока В
	//pinMode(Rele1 , OUTPUT);                             //
	//pinMode(Rele2 , OUTPUT);                             //
	//pinMode(Rele2 , OUTPUT);                             //
	//digitalWrite(Rele1 , LOW);                         //
	//digitalWrite(Rele2 , LOW);                         //
	//digitalWrite(Rele3 , LOW);                         //
}
void setup_mcp()
{
  // Настройка расширителя портов



  mcp_Out1.begin(1);                               //  Адрес (1) U6 первого  расширителя портов
  mcp_Out1.pinMode(motor_West, OUTPUT);            //  1A1 Назначение  мотор Запад
  mcp_Out1.pinMode(motor_East, OUTPUT);            //  1B1 Назначение  мотор Восток
  mcp_Out1.pinMode(motor_High, OUTPUT);            //  1C1 Назначение  мотор Вверх
  mcp_Out1.pinMode(motor_Down, OUTPUT);            //  1D1 Назначение  мотор Вниз
  mcp_Out1.pinMode(4, OUTPUT);                     //  1A2
  mcp_Out1.pinMode(5, OUTPUT);                     //  1B2
  mcp_Out1.pinMode(6, OUTPUT);                     //  1C2
  mcp_Out1.pinMode(7, OUTPUT);                     //  1D2


  mcp_Out1.pinMode(SW_East, INPUT);                //  1E1   Назначение концевик Восток 
  mcp_Out1.pinMode(SW_West, INPUT);                //  1E2  Назначение концевик Запад  
  mcp_Out1.pinMode(SW_High, INPUT);                //  1E3   Назначение концевик Верх
  mcp_Out1.pinMode(SW_Down, INPUT);                //  1E4   Назначение концевик Низ
  mcp_Out1.pinMode(12, INPUT);                    //  1E5   U19  порты А GND
  mcp_Out1.pinMode(13, INPUT);                    //  1E6   U21  порты А GND
  mcp_Out1.pinMode(14, INPUT);                    //  1E7   Свободен
  mcp_Out1.pinMode(15, INPUT);                    //  1E8   Свободен

  mcp_Out2.begin(2);                               //  Адрес (2) U9 второго  расширителя портов
  mcp_Out2.pinMode(0, OUTPUT);                     //  2A1
  mcp_Out2.pinMode(1, OUTPUT);                     //  2B1
  mcp_Out2.pinMode(2, OUTPUT);                     //  2C1
  mcp_Out2.pinMode(3, OUTPUT);                     //  2D1
  mcp_Out2.pinMode(4, OUTPUT);                     //  2A2
  mcp_Out2.pinMode(5, OUTPUT);                     //  2B2
  mcp_Out2.pinMode(6, OUTPUT);                     //  2C2
  mcp_Out2.pinMode(7, OUTPUT);                     //  2D2

  mcp_Out2.pinMode(8, OUTPUT);                     //  2E1   U15  порты B in/out
  mcp_Out2.pinMode(9, OUTPUT);                     //  2E2   U18  порты B in/out
  mcp_Out2.pinMode(10, OUTPUT);                    //  2E3   U22  порты B in/out
  mcp_Out2.pinMode(11, OUTPUT);                    //  2E4   U16  порты B GND
  mcp_Out2.pinMode(12, OUTPUT);                    //  2E5   U20  порты B GND
  mcp_Out2.pinMode(13, OUTPUT);                    //  2E6   U24  порты B GND
  mcp_Out2.pinMode(14, OUTPUT);                    //  2E7   Реле №1, №2
  mcp_Out2.pinMode(15, OUTPUT);                    //  2E8   Свободен

  for (int i = 0; i < 8; i++)
  {
	  mcp_Out1.digitalWrite(i, LOW);
  }
  for (int i = 0; i < 16; i++)
  {
	mcp_Out2.digitalWrite(i, HIGH);
  }
  //mcp_Out2.digitalWrite(14, LOW);                 // Отключить реле
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
  regBank.add(40008);  //  Номер блока таблиц по умолчанию
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
  Serial.begin(9600);                                    // Подключение к USB ПК
  //Serial1.begin(115200);                                 // Подключение к
  slave.setSerial(3, 57600);                             // Подключение к протоколу MODBUS компьютера Serial3
  Serial2.begin(115200);                                 // Подключение к
  setup_pin();
  Wire.begin();
  if (!RTC.begin())                                      // Настройка часов
  {
	Serial.println(F("RTC failed"));
	while (1);
  };
  //DateTime set_time = DateTime(16, 3, 15, 10, 19, 0);  // Занести данные о времени в строку "set_time" год, месяц, число, время...
  //RTC.adjust(set_time);                                // Записать дату
  Serial.println(" ");
  Serial.println(F(" ***** Start system  *****"));
  Serial.println(" ");
  //set_time();
  serial_print_date();
  setup_mcp();                                          // Настроить порты расширения
 
  MsTimer2::set(1000, flash_time);                       // 300ms период таймера прерывани

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
 

  read_Temperatures();
  upr_motor = poz_min;

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

  /*lightMeter.begin(BH1750_CONTINUOUS_HIGH_RES_MODE);
  Serial.println(F("BH1750 Test"));*/
  latitude = latitude * pi / 180;

  //MsTimer2::start();

  Serial.println(" ");
  Serial.println(F("System initialization OK!."));        // Информация о завершении настройки
}

void loop()
{
    draw_Glav_Menu();
    swichMenu();
}

//THIS CODE TURNS THE MONTH INTO THE NUMBER OF day3S SINCE JANUARY 1ST.
//ITS ONLY PURPOSE IS FOR CALCULATING DELTA (DECLINATION), AND IS NOT USED IN THE HOUR ANGLE TABLE OR ANYWHERE ELSE.
float daynum(float month) {
	float day3;
	if (month == 1) { day3 = 0; }
	if (month == 2) { day3 = 31; }
	if (month == 3) { day3 = 59; }
	if (month == 4) { day3 = 90; }
	if (month == 5) { day3 = 120; }
	if (month == 6) { day3 = 151; }
	if (month == 7) { day3 = 181; }
	if (month == 8) { day3 = 212; }
	if (month == 9) { day3 = 243; }
	if (month == 10) { day3 = 273; }
	if (month == 11) { day3 = 304; }
	if (month == 12) { day3 = 334; }
	return day3;
}

//THIS CODE TAKES THE day3 OF THE MONTH AND DOES ONE OF THREE THINGS: ADDS A day3, SUBTRACTS A day3, OR
//DOES NOTHING. THIS IS DONE SO THAT LESS VALUES ARE REQUIRED FOR THE NOON HOUR ANGLE TABLE BELOW.
int dayToArrayNum(int day3) {
	if ((day3 == 1) || (day3 == 2) || (day3 == 3)) { day3 = 0; }
	if ((day3 == 4) || (day3 == 5) || (day3 == 6)) { day3 = 1; }
	if ((day3 == 7) || (day3 == 8) || (day3 == 9)) { day3 = 2; }
	if ((day3 == 10) || (day3 == 11) || (day3 == 12)) { day3 = 3; }
	if ((day3 == 13) || (day3 == 14) || (day3 == 15)) { day3 = 4; }
	if ((day3 == 16) || (day3 == 17) || (day3 == 18)) { day3 = 5; }
	if ((day3 == 19) || (day3 == 20) || (day3 == 21)) { day3 = 6; }
	if ((day3 == 22) || (day3 == 23) || (day3 == 24)) { day3 = 7; }
	if ((day3 == 25) || (day3 == 26) || (day3 == 27)) { day3 = 8; }
	if ((day3 == 28) || (day3 == 29) || (day3 == 30) || (day3 == 31)) { day3 = 9; }
	return day3;
}

//////////////////////////////////////////////////////////////
//HERE IS THE TABLE OF NOON HOUR ANGLE VALUES. THESE VALUES GIVE THE HOUR ANGLE, IN DEGREES, OF THE SUN AT NOON (NOT SOLAR NOON)
//WHERE LONGITUDE = 0. dayS ARE SKIPPED TO SAVE SPACE, WHICH IS WHY THERE ARE NOT 365 NUMBERS IN THIS TABLE.
float FindH(int day3, int month) {
	float h;

	if (month == 1) {
		float h_Array[10] = {
			-1.038,-1.379,-1.703,-2.007,-2.289,-2.546,-2.776,-2.978,-3.151,-3.294, };
		h = h_Array[day3];
	}

	if (month == 2) {
		float h_Array[10] = {
			-3.437,-3.508,-3.55,-3.561,-3.545,-3.501,-3.43,-3.336,-3.219,-3.081, };
		h = h_Array[day3];
	}

	if (month == 3) {
		float h_Array[10] = {
			-2.924,-2.751,-2.563,-2.363,-2.153,-1.936,-1.713,-1.487,-1.26,-1.035, };
		h = h_Array[day3];
	}

	if (month == 4) {
		float h_Array[10] = {
			-0.74,-0.527,-0.322,-0.127,0.055,0.224,0.376,0.512,0.63,0.728, };
		h = h_Array[day3];
	}

	if (month == 5) {
		float h_Array[10] = {
			0.806,0.863,0.898,0.913,0.906,0.878,0.829,0.761,0.675,0.571, };
		h = h_Array[day3];
	}

	if (month == 6) {
		float h_Array[10] = {
			0.41,0.275,0.128,-0.026,-0.186,-0.349,-0.512,-0.673,-0.829,-0.977, };
		h = h_Array[day3];
	}

	if (month == 7) {
		float h_Array[10] = {
			-1.159,-1.281,-1.387,-1.477,-1.547,-1.598,-1.628,-1.636,-1.622,-1.585, };
		h = h_Array[day3];
	}

	if (month == 8) {
		float h_Array[10] = {
			-1.525,-1.442,-1.338,-1.212,-1.065,-0.9,-0.716,-0.515,-0.299,-0.07, };
		h = h_Array[day3];
	}

	if (month == 9) {
		float h_Array[10] = {
			0.253,0.506,0.766,1.03,1.298,1.565,1.831,2.092,2.347,2.593, };
		h = h_Array[day3];
	}

	if (month == 10) {
		float h_Array[10] = {
			2.828,3.05,3.256,3.444,3.613,3.759,3.882,3.979,4.049,4.091, };
		h = h_Array[day3];
	}

	if (month == 11) {
		float h_Array[10] = {
			4.1,4.071,4.01,3.918,3.794,3.638,3.452,3.236,2.992,2.722, };
		h = h_Array[day3];
	}

	if (month == 12) {
		float h_Array[10] = {
			2.325,2.004,1.665,1.312,0.948,0.578,0.205,-0.167,-0.534,-0.893, };
		h = h_Array[day3];
	}

	return h;
}
/////////////////////////////////////////////////////////////