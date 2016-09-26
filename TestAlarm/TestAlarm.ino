
#define DEBUG 0

#include "defines.h"

// Подключение библиотеки
#include <EEPROM.h>
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <avr/pgmspace.h>

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
// pin 8 - Serial clock out (CLK)
// pin 9 - Serial data out (DIN)
// pin 10 - Data/Command select (DC)
// pin 11 - LCD chip select (CE)
// pin 12 - LCD reset (RST)
// GND - LCD  (GND)
// 3.3 V - LCD  (Vcc)
// 3.3 V - LCD  (BL)


Adafruit_PCD8544 display = Adafruit_PCD8544(8, 9, 10, 11, 12);
tmElements_t tm;

// клавиатура
#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
    {'1','2','3','a'},
    {'4','5','6','b'},
    {'7','8','9','c'},
    {'*','0','#','d'}
};  

byte rowPins[ROWS] = {17,16,15,14}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4,5,6,7}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
char key=' ';
/// для будильников
unsigned int edit[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned int minedit[15]={0,0,0,0,0,1,1,15,0,0,0,0,0,0,0};
unsigned int maxedit[15]={2,24,60,60,2,32,13,20,2,2,2,2,2,2,2};

// для установки времени
unsigned int edit1[7]={0,0,0,0,0,0,0};
unsigned int minedit1[7]={0,0,0,1,1,1,15};
unsigned int maxedit1[7]={24,60,60,8,32,13,20};
unsigned int stepedit1[7]={1,1,1,1,1,1,1};

unsigned int correcteditpos[2][16]={
  {0,1,2,3,4,5,6,7,0,0,0,0,0,0,7,0},
  {0,1,2,3,4,8,8,4,8,9,10,11,12,13,14,0}
  };
unsigned int alarms[3][15]={
    {1,8,34,45,1,12,3,16,0,0,0,0,0,0,0},
    {0,16,12,20,0,23,11,16,0,0,0,0,0,0,0},
    {1,23,5,10,1,10,10,16,1,1,1,0,1,0,1}
    };
int tektime[7]={0,0,0,0,0,0,0};
char weekdaystring[8][4]={"   ","Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
    
    
int daysformonth[5][13]={
  {0,31,28,31,30,31,30,31,31,30,31,30,31}, 
  {0,31,28,31,30,31,30,31,31,30,31,30,31}, 
  {0,31,28,31,30,31,30,31,31,30,31,30,31}, 
  {0,31,29,31,30,31,30,31,31,30,31,30,31}, 
  {0,31,28,31,30,31,30,31,31,30,31,30,31} 
  };      // 2015 - 2020
unsigned long millis1;


long delta[7]={0,0,0,0,0,0,0};
int tekalarm=0;
int mode=TABLO_MODE;
int editparam=0;
int blink1=0;
int blink2=0;
int alarmok[3]={0,0,0};
 
//
//static unsigned char PROGMEM picalarm[] =
//{ B0001000,
//  B0011100,
//  B0111110,
//  B0111110,
//  B1111111,
//  B0001000,
//  B0010000
//};
//static unsigned char PROGMEM noalarm[] =
//{ B1000001,
//  B0100010,
//  B0010100,
//  B0001000,
//  B0010100,
//  B0100010,
//  B1000001
//};
//static unsigned char PROGMEM nullalarm[] =
//{ B0000000,
//  B0000000,
//  B0000000,
//  B0000000,
//  B0000000,
//  B0000000,
//  B0000000
//};

static unsigned char  picalarm[] =
{ B0001000,
  B0011100,
  B0111110,
  B0111110,
  B1111111,
  B0001000,
  B0010000
};
static unsigned char  noalarm[] =
{ B1000001,
  B0100010,
  B0010100,
  B0001000,
  B0010100,
  B0100010,
  B1000001
};
static unsigned char  nullalarm[] =
{ B0000000,
  B0000000,
  B0000000,
  B0000000,
  B0000000,
  B0000000,
  B0000000
};


// динамик
int pinsound=3;
int freq=528;
int nfreqs[]={546,435,276,0,600,0};
int teknotes=0;
int nnotes=6;
boolean yesalarms=true;
//unsigned long nmillis=0;

void setup()
  {
  //if(DEBUG==1) 
    Serial.begin(9600);
  // инициализация настроек из EEPROM 
  if(EEPROM.read(0)==255)
     {
     clear_eeprom();
     ini_eeprom();
     }
  // инициализация настроек из EEPROM 
  for(int i=1;i<4;i++)
     get_prg_eeprom(i);

  display.begin();
  // установить контраст фона экрана
  display.setContrast(40);
  display.clearDisplay();       // очистить экран

  display.setTextSize(1);    // размер шрифта
  display.setTextColor(BLACK);   // цвет
  //  динамик
  pinMode(pinsound,OUTPUT);
  //tone(pinsound,freq,5000);
  }

 
void loop()
 {
  // проверка нажатия клавиши
  key = keypad.getKey();
  if(key)
    {
    if(DEBUG==1) {Serial.print("key=");Serial.println(key);}
    do_for_key(key);
    }  
 // обновление экрана  
 if(millis()-millis1>500)
  {
  blink1=1-blink1;
  blink2=1-blink2;
  millis1=millis();
  //tmElements_t tm;
  if (RTC.read(tm))   // получение времени
    {
    yesalarms=false;  
    for(int i=0;i<3;i++)
      {
     if(alarmok[i]>0)
        {
        alarmok[i]=alarmok[i]-1;
        yesalarms=true;  
        if(alarmok[i]==0 && alarms[i][4]==1)
          {
           tekalarm=i+1;
           set_next_data(); 
           put_prg_eeprom(tekalarm);
         }
        }
      } 
    for(int i=0;i<3;i++)
      {
      if(alarmgo(i))
        alarmok[i]=20;
      if(!yesalarms) 
        {teknotes=0;}
      }
    // проигрывание нот
    if(yesalarms)
       {tone(pinsound,nfreqs[teknotes],500);
        teknotes=(teknotes+1)%nnotes;
       }
    //  
    display.clearDisplay();       // очистить экран
    if(mode==TABLO_MODE) 
       {
       displaytoptime();
       displayalarms();
       }
    else if(mode==EDIT_MODE)
       {
       //displaytoptime();
       displayeditalarm(tekalarm);
       }
    else if(mode==SET_TIME)
       {
       //displaytoptime();
       displaysettime();
       }
   display.display();
    }
  else
    {
    if (RTC.chipPresent())
      {
      if(DEBUG==1) Serial.println("DS1307 is stopped");
      }
    else
      {
      if(DEBUG==1) Serial.println("DS1307 read error");
      }
    delay(9000);
    }

  }
}

// процедура вывода  с добавлением до двух цифр
String print2digits1(int number)
  {
  String out="";
  if (number >= 0 && number < 10)
    {out="0";}
  out=out+String(number);
  return out;
  }
// процедура вывода  с добавлением до трех цифр
String print4digits1(int number)
  {
  String out="";
  if (number >= 100 && number < 1000)
    {out+=" ";}
  if (number >= 10 && number < 100)
    {out+="  ";}
  if (number >=0 && number < 10)
    {out+="   ";}
  out=out+String(number);
  return out;
  }

