
#define DEBUG 0

#include "defines.h"


#include <UTouchCD.h>
#include <UTouch.h>
#include <UTFT.h>
#include "Wire.h"
#include <Time.h>
#include <DS1307RTC.h>
#include <OneWire.h>
#include <SPI.h>
#include <EEPROM.h>
#include <Adafruit_MCP23017.h>
#include <MsTimer2.h> 
 
//dht11 DHT11;


UTFT    myGLCD(ITDB32S,38,39,40,41);          // Дисплей 3,2"
//UTFT    myGLCD(TFT01_70,38,39,40,41);       // Дисплей 7,0"
extern uint8_t SmallFont[]; 
extern uint8_t SmallSymbolFont[];
extern uint8_t BigFont[];
UTouch      myTouch(6,5,4,3,2);

int x, y;
int  xi = 490;
int  yi = 200;
char stCurrent[20]="";
int stCurrentLen=0;
char stLast[20]="";

//+++++++++++++++++++++++++++ Настройка часов +++++++++++++++++++++++++++++++

tmElements_t tm;

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
    {'1','2','3','a'},
    {'4','5','6','b'},
    {'7','8','9','c'},
    {'*','0','#','d'}
};  

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


int clockCenterX     = 119;
int clockCenterY     = 119;
int oldsec=0;
const char* str[]          = {"","MON","TUE","WED","THU","FRI","SAT","SUN"};
const char* str_mon[]      = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
//-------------------------------------------------------------------------------
int but1, but2, but3, but4, but5, but6, but7, but8, but9, but10, butX, butY, but_m1, but_m2, but_m3, but_m4, but_m5, pressed_button;

Adafruit_MCP23017 mcp_Out1;             // U11 MCP23017 расширение портов (клапана полива)
Adafruit_MCP23017 mcp_Out2;             // U8  MCP23017 расширение портов ()

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


int deviceaddress                    = 80;                // Адрес EEPROM
unsigned int eeaddress               =  0;                // Адрес 

static int adr1_1kl_avto                    = 111;   // Вращение по горизонтали
static int adr1_1kl_start_hour              = 112;   //
static int adr1_1kl_start_min               = 113;   //
static int adr1_1kl_stop_min                = 114;   //
static int adr1_1kl_stop_sec                = 115;   //

static int adr1_2kl_avto                    = 121;   //
static int adr1_2kl_start_hour              = 122;   //
static int adr1_2kl_start_min               = 123;   //
static int adr1_2kl_stop_min                = 124;   //
static int adr1_2kl_stop_sec                = 125;   //

static int adr1_3kl_avto                    = 131;   //
static int adr1_3kl_start_hour              = 132;   //
static int adr1_3kl_start_min               = 133;   //
static int adr1_3kl_stop_min                = 134;   //
static int adr1_3kl_stop_sec                = 135;   //

static int adr1_4kl_avto                    = 141;   //
static int adr1_4kl_start_hour              = 142;   //
static int adr1_4kl_start_min               = 143;   //
static int adr1_4kl_stop_min                = 144;   //
static int adr1_4kl_stop_sec                = 145;   //

static int adr1_5kl_avto                    = 151;   //
static int adr1_5kl_start_hour              = 152;   //
static int adr1_5kl_start_min               = 153;   //
static int adr1_5kl_stop_min                = 154;   //
static int adr1_5kl_stop_sec                = 155;   //

static int adr1_6kl_avto                    = 161;   //
static int adr1_6kl_start_hour              = 162;   //
static int adr1_6kl_start_min               = 163;   //
static int adr1_6kl_stop_min                = 164;   //
static int adr1_6kl_stop_sec                = 165;   //

static int adr1_7kl_avto                    = 171;   //
static int adr1_7kl_start_hour              = 172;   //
static int adr1_7kl_start_min               = 173;   //
static int adr1_7kl_stop_min                = 174;   //
static int adr1_7kl_stop_sec                = 175;   //

int adr1_8kl_avto                    = 181;   //
int adr1_8kl_start_hour              = 182;   //
int adr1_8kl_start_min               = 183;   //
int adr1_8kl_stop_min                = 184;   //
int adr1_8kl_stop_sec                = 185;   //

int adr1_9kl_avto                    = 191;   //
int adr1_9kl_start_hour              = 192;   //
int adr1_9kl_start_min               = 193;   //
int adr1_9kl_stop_min                = 194;   //
int adr1_9kl_stop_sec                = 195;   //

int adr1_10kl_avto                   = 101;   //
int adr1_10kl_start_hour             = 102;   //
int adr1_10kl_start_min              = 103;   //
int adr1_10kl_stop_min               = 104;   //
int adr1_10kl_stop_sec               = 105;   //


int adr2_1kl_avto                    = 211;   // Вращение по вертиткали
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





void flash_time()                              // Программа обработчик прерывания 
{ 
 
}

//++++++++++++++++++++++++++++++++ Программы часов ++++++++++++++++++++++++++++++++++++
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
  RTC.read(tm);
  drawMin(tm.Minute);
  drawHour(tm.Hour, tm.Minute);
  drawSec(tm.Second);
  oldsec=tm.Second;

  // Draw calendar
  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRoundRect(240, 0, 317, 85);
  myGLCD.setColor(0, 0, 0);
  for (int i=0; i<7; i++)
  {
	myGLCD.drawLine(249+(i*10), 0, 248+(i*10), 3);
	myGLCD.drawLine(250+(i*10), 0, 249+(i*10), 3);
	myGLCD.drawLine(251+(i*10), 0, 250+(i*10), 3);
  }

  // Draw SET button
  myGLCD.setColor(64, 64, 128);
  myGLCD.fillRoundRect(260, 200, 317, 239);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect(260, 200, 317, 239);
  myGLCD.setBackColor(64, 64, 128);
  myGLCD.print("SET", 266, 212);
  myGLCD.setBackColor(0, 0, 0);
  
  myGLCD.setColor(64, 64, 128);
  myGLCD.fillRoundRect(260, 140, 317, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect(260, 140, 317, 180);
  myGLCD.setBackColor(64, 64, 128);
  myGLCD.print("ESC", 266, 152);
  myGLCD.setBackColor(0, 0, 0);
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
  RTC.read(tm);
  myGLCD.setFont(BigFont);
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(255, 255, 255);
  myGLCD.print(str[tm.Wday], 256, 8);
  if (tm.Day<10)
	myGLCD.printNumI(tm.Day, 272, 28);
  else
	myGLCD.printNumI(tm.Day, 264, 28);

  myGLCD.print(str_mon[tm.Month-1], 256, 48);
  myGLCD.printNumI(tmYearToCalendar(tm.Year), 248, 65);
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
	//RTC.read(tm);
	printDate();
	//RTC.read(tm);
  
	while (true)
	{
	RTC.read(tm);
	if (oldsec!=tm.Second)
	{
		if ((tm.Second==0) and (tm.Minute==0) and (tm.Hour==0))
		{
			clearDate();
			printDate();
		}
		if (tm.Second==0)
		{
			drawMin(tm.Minute);
			drawHour(tm.Hour, tm.Minute);
		}
		drawSec(tm.Second);
		oldsec=tm.Second;
	}

	if (myTouch.dataAvailable())
	{
		myTouch.read();
		x=myTouch.getX();
		y=myTouch.getY();
		if (((y>=200) && (y<=239)) && ((x>=260) && (x<=317))) //установка часов
		{
			myGLCD.setColor (255, 0, 0);
			myGLCD.drawRoundRect(260, 200, 317, 239);
			setClock();
		}

		if (((y>=140) && (y<=180)) && ((x>=260) && (x<=317))) //Возврат
		{
			myGLCD.setColor (255, 0, 0);
			myGLCD.drawRoundRect(260, 140, 317, 180);
			myGLCD.clrScr();
			myGLCD.setFont(BigFont);
			break;
		}
	}
	delay(10);
  }
}
//------------------------------------------------------------------------------------------------
void drawButtons()
{
  // Draw the upper row of buttons
  for (x=0; x<5; x++)
  {
	myGLCD.setColor(0, 0, 255);
	myGLCD.fillRoundRect (xi+10+(x*60), 10, xi+60+(x*60), 60);
	myGLCD.setColor(255, 255, 255);
	myGLCD.drawRoundRect (xi+10+(x*60), 10, xi+60+(x*60), 60);
	myGLCD.printNumI(x+1,  xi+27+(x*60), 27);
  }
  // Draw the center row of buttons
  for (x=0; x<5; x++)
  {
	myGLCD.setColor(0, 0, 255);
	myGLCD.fillRoundRect (xi+10+(x*60), 70, xi+60+(x*60), 120);
	myGLCD.setColor(255, 255, 255);
	myGLCD.drawRoundRect (xi+10+(x*60), 70, xi+60+(x*60), 120);
	if (x<4)
	  myGLCD.printNumI(x+6,  xi+27+(x*60), 87);
  }
  myGLCD.print("0",  xi+267, 87);
// Draw the lower row of buttons
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (xi+10, 130, xi+150, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (xi+10, 130, xi+150, 180);
  myGLCD.print("Clear", xi+ 40, 147);
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (xi+160, 130, xi+300, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (xi+160, 130, xi+300, 180);
  myGLCD.print("Enter",  xi+190, 147);
  myGLCD.setBackColor (0, 0, 0);
}
void klav_vvod()
{
  while (true)
  {
	if (myTouch.dataAvailable())
	{
	  myTouch.read();
	  x=myTouch.getX();
	  y=myTouch.getY();

	  if ((y>=10) && (y<=60))                 // Upper row
	  {
		if ((x>= xi+10) && (x<= xi+60))       // Button: 1
		{
		  waitForIt( xi+10, 10,  xi+60, 60);
		  updateStr('1');
		}
		if ((x>= xi+70) && (x<= xi+120))  // Button: 2
		{
		  waitForIt( xi+70, 10,  xi+120, 60);
		  updateStr('2');
		}
		if ((x>= xi+130) && (x<= xi+180))  // Button: 3
		{
		  waitForIt( xi+130, 10,  xi+180, 60);
		  updateStr('3');
		}
		if ((x>= xi+190) && (x<= xi+240))  // Button: 4
		{
		  waitForIt( xi+190, 10,  xi+240, 60);
		  updateStr('4');
		}
		if ((x>= xi+250) && (x<= xi+300))  // Button: 5
		{
		  waitForIt( xi+250, 10,  xi+300, 60);
		  updateStr('5');
		}
	  }

	  if ((y>=70) && (y<=120))  // Center row
	  {
		if ((x>= xi+10) && (x<= xi+60))  // Button: 6
		{
		  waitForIt( xi+10, 70,  xi+60, 120);
		  updateStr('6');
		}
		if ((x>= xi+70) && (x<= xi+120))  // Button: 7
		{
		  waitForIt( xi+70, 70,  xi+120, 120);
		  updateStr('7');
		}
		if ((x>= xi+130) && (x<= xi+180))  // Button: 8
		{
		  waitForIt( xi+130, 70,  xi+180, 120);
		  updateStr('8');
		}
		if ((x>= xi+190) && (x<= xi+240))  // Button: 9
		{
		  waitForIt( xi+190, 70,  xi+240, 120);
		  updateStr('9');
		}
		if ((x>= xi+250) && (x<= xi+300))  // Button: 0
		{
		  waitForIt( xi+250, 70,  xi+300, 120);
		  updateStr('0');
		}
	  }

	  if ((y>=130) && (y<=180))  // Upper row
	  {
		if ((x>= xi+10) && (x<= xi+150))  // Button: Clear
		{
		  waitForIt( xi+10, 130,  xi+150, 180);
		  stCurrent[0]='\0';
		  stCurrentLen=0;
		  myGLCD.setColor(0, 0, 0);
		  myGLCD.fillRect( 0, 224,  319, 239);
		}
		if ((x>= xi+160) && (x<= xi+300))  // Button: Enter
		{
		  waitForIt( xi+160, 130,  xi+300, 180);
		  if (stCurrentLen>0)
		  {
			for (x=0; x<stCurrentLen+1; x++)
			{
			  stLast[x]=stCurrent[x];
			}
			stCurrent[0]='\0';
			stCurrentLen=0;
			myGLCD.setColor(0, 0, 0);
			myGLCD.fillRect( xi+0, 208,  xi+319, 239);
			myGLCD.setColor(0, 255, 0);
			myGLCD.print(stLast,  LEFT, 208);
		  }
		  else
		  {
			myGLCD.setColor(255, 0, 0);
			myGLCD.print("BUFFER EMPTY", CENTER, 192);
			delay(500);
			myGLCD.print("            ", CENTER, 192);
			delay(500);
			myGLCD.print("BUFFER EMPTY", CENTER, 192);
			delay(500);
			myGLCD.print("            ", CENTER, 192);
			myGLCD.setColor(0, 255, 0);
		  }
		}
	  }
	}
  }
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
  {
	myGLCD.setColor(255, 0, 0);
	myGLCD.print("BUFFER FULL!", CENTER, 192);
	delay(500);
	myGLCD.print("            ", CENTER, 192);
	delay(500);
	myGLCD.print("BUFFER FULL!", CENTER, 192);
	delay(500);
	myGLCD.print("            ", CENTER, 192);
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

void scanI2CBus(byte from_addr, byte to_addr, void(*callback)(byte address, byte result) ) 
{
	/*
  byte rc;
  byte data1 = 0; // not used, just an address to feed to twi_writeTo()
  for( byte addr = from_addr; addr <= to_addr; addr++ )
  {
	rc = twi_writeTo(addr, &data1,0, 0, 1);
	if(rc==0) callback( addr, rc );
  }
  */
}
void i2c_eeprom_write_byte( int deviceaddress, unsigned int eeaddress, byte data )
{
	int rdata = data;
	Wire.beginTransmission(deviceaddress);
	Wire.write((int)(eeaddress >> 8)); // MSB
	Wire.write((int)(eeaddress & 0xFF)); // LSB
	Wire.write(rdata);
	Wire.endTransmission();
	delay(20);
}
void i2c_eeprom_write_page( int deviceaddress, unsigned int eeaddresspage, byte* data, byte length ) 
{
	/*
	Wire.beginTransmission(deviceaddress);
	Wire.write((int)(eeaddresspage >> 8)); // MSB
	Wire.write((int)(eeaddresspage & 0xFF)); // LSB
	byte c;
	for ( c = 0; c < length; c++)
	Wire.write(data[c]);
	Wire.endTransmission();
	*/
}
byte i2c_eeprom_read_byte( int deviceaddress, unsigned int eeaddress )
{

	byte rdata = 0xFF;
	Wire.beginTransmission(deviceaddress);
	Wire.write((int)(eeaddress >> 8)); // MSB
	Wire.write((int)(eeaddress & 0xFF)); // LSB
	Wire.endTransmission();
	Wire.requestFrom(deviceaddress,1);
	if (Wire.available()) rdata = Wire.read();
	//delay(10);
	return rdata;
}
void i2c_eeprom_read_buffer( int deviceaddress, unsigned int eeaddress, byte *buffer, int length )
{
	
	Wire.beginTransmission(deviceaddress);
	Wire.write((int)(eeaddress >> 8)); // MSB
	Wire.write((int)(eeaddress & 0xFF)); // LSB
	Wire.endTransmission();
	Wire.requestFrom(deviceaddress,length);
	int c = 0;
	for ( c = 0; c < length; c++ )
	if (Wire.available()) buffer[c] = Wire.read();
}

void i2c_test1()
{
	for( int n = 0; n<100; n++)
	{
		i2c_eeprom_write_byte(deviceaddress, n,12);
		
	}

	for(unsigned int x=0;x<100;x++)
	{
		int  b = i2c_eeprom_read_byte(deviceaddress, x); //access an address from the memory
		delay(10);
		Serial.print(x); //print content to serial port
		Serial.print(" - "); //print content to serial port
		Serial.println(b); //print content to serial port
	}
}
void draw_Glav_menu()
{
	myGLCD.clrScr();
	myGLCD.setFont(BigFont);
	myGLCD.setColor(0, 0, 255);

	// Заполнение кнопок                                                    
	myGLCD.fillRoundRect (10,  20,  110, 120);                  // Кнопка "Запад"
	myGLCD.fillRoundRect (120, 20,  220, 120);                  // Кнопка "Восток"
	myGLCD.fillRoundRect (10,  130, 110, 230);                  // Кнопка "Вверх" 
	myGLCD.fillRoundRect (120, 130, 220, 230);                  // Кнопка "Вниз"
	myGLCD.fillRoundRect (230, 130, 310, 175);                  // Кнопка "Меню1" 
	myGLCD.fillRoundRect (230, 185, 310, 230);                  // Кнопка "Меню2"

	// Обрамление кнопок                                  
	myGLCD.setColor(255, 255, 255);
	myGLCD.drawRoundRect (10, 20, 110,120);                    // Кнопка "Запад"
	myGLCD.drawRoundRect (120, 20, 220, 120);                  // Кнопка "Восток"
	myGLCD.drawRoundRect (10, 130, 110, 230);                  // Кнопка "Вверх" 
	myGLCD.drawRoundRect (120, 130,220, 230);                  // Кнопка "Вниз"
	myGLCD.drawRoundRect (230, 130, 310, 175);                 // Кнопка "Меню1" 
	myGLCD.drawRoundRect (230, 185, 310, 230);                 // Кнопка "Меню2"
	myGLCD.drawRoundRect (230, 20, 310, 120);                  // Зона информации

	myGLCD.setFont( SmallFont);
	myGLCD.setColor(255,255,255);
	myGLCD.setBackColor(0, 0, 0);
	myGLCD.print("C""\x86""CTEMA C""\x88""E""\x84""EH""\x86\x95"" ""\x85""A CO""\x88""H""\x8C""EM",CENTER, 1);           // Система слежения за солнцем
	myGLCD.setBackColor(0, 0, 255);
    myGLCD.setFont(BigFont);
	myGLCD.print("BPA""\x8F""EH""\x86""E",48, 25);             // ВРАЩЕНИЕ
	
	myGLCD.print("\x85""A""\x89""A""\x82",20, 50);             // ЗАПАД
	myGLCD.print("<===",25, 80);                               // ЗАПАД
	myGLCD.print("BOCTOK",123, 50);                            // ВОСТОК
	myGLCD.print("===>",140, 80);                              // ВОСТОК
	myGLCD.print("BBEPX",20, 146);                             // ВВЕРХ
	myGLCD.print("<==",48, 163,45);                            // ВВЕРХ
	myGLCD.print("BH""\x86\x85",138, 146);                     // ВНИЗ
	myGLCD.print("==>",155, 163,45);                           // ВНИЗ
	myGLCD.print("MEH""\x94",235, 146);                        // МЕНЮ
	myGLCD.print("\x8D""AC""\x91" ,237, 200);                  // ЧАСЫ
}
void swich_Glav_Menu()
{
	while (true)
	 {                      	
		if (myTouch.dataAvailable())
			{
				myTouch.read();
				int	x=myTouch.getX();
				int	y=myTouch.getY();

				if ((y>= 20) && (y<=120))                                              // кнопка направления запад-восток
					{
						 if ((x>=10) && (x<=110))                                      // Выбрана кнопка "Запад"
							{
								waitForIt(10, 20, 110, 120);

								rotor_runWest();


						//		teplitca = 1;                                         // Выбрана                       
								//myGLCD.setColor(VGA_RED);                             // Установлен красный цвет надписи
								//myGLCD.setBackColor(0, 0, 255);                       // Установлен синий цвет фона
								//myGLCD.print("Te\xA3\xA0\xA2\xA6""a N1",40, 5);       // Теплица №1
								//myGLCD.setColor(255, 255, 255);                       // Установлен белый цвет надписи
								//myGLCD.print("Te\xA3\xA0\xA2\xA6""a N2",40, 127);     // Теплица №2
							}

						else if ((x>=120) && (x<=220))                                // Выбрана  кнопка восток
							{
								waitForIt(120, 20, 220, 120);
								rotor_runEast();
			//					teplitca = 2;                                         // Выбрана   
								//myGLCD.setColor(255, 255, 255);                       // Установлен белый цвет надписи
								//myGLCD.setBackColor(0, 0, 255);                       // Установлен синий цвет фона
								//myGLCD.print("Te\xA3\xA0\xA2\xA6""a N1",40, 5);       // Теплица №1 
								//myGLCD.setColor(VGA_RED);                             // Установлен красный цвет надписи
								//myGLCD.print("Te\xA3\xA0\xA2\xA6""a N2",40, 127);     // Теплица №2
							}

					}
				if ((y>=130) && (y<=230))                                             // кнопка Управление "Вверх - вниз
					{
						 if ((x>=10) && (x<=110))                                      // выбран "Вверх"
							{
								waitForIt(10, 130, 110, 230);
								rotor_runUp();
								//draw_Poliv_Menu();                                    //  
								//swich_Poliv_Menu();                                   //  
							    // draw_Glav_menu();                                     // возврат в главное меню
							}

						else if ((x>=120) && (x<=220))                                // Выбран "Вниз"
							{
								waitForIt(120, 130, 220, 230);
								rotor_runDown();
								//draw_Glav_menu();                                     // возврат в главное меню
							}
	
				if ((x>=230) && (x<=310))                                             // кнопка выбора Меню1-2
					{
						 if ((y>=130) && (y<=175))                                      // выбран Меню
							{
								waitForIt(230, 130, 310, 175);
								//draw_Poliv_Menu();                                    //  
								//swich_Poliv_Menu();                                   //  
								//draw_Glav_menu();                                     // возврат в главное меню
							}

						else if ((y>=185) && (y<=230))                                //  выбран Часы
							{
								waitForIt(230, 185, 310, 230);
								AnalogClock();
								draw_Glav_menu();                                     // возврат в главное меню
							}
					}
		      }
		}
	}
}

void rotor_runWest()
{
	myGLCD.setColor(255,0, 0);                                 // Установлен красный цвет надписи
	myGLCD.drawRoundRect (10, 20, 110,120);                    // Кнопка "Запад"
	int i=0;
	delay(100);
	while (true)
	{
		// Здесь проверить на отключение по концевику. Отключен = break;
		// Здесь включить
		if(i==0) myGLCD.print("<  =",25, 80);  
		if(i==1) myGLCD.print("< = ",25, 80); 
		if(i==2) myGLCD.print("<=  ",25, 80); 
		if(i==3) myGLCD.print("<   ",25, 80); 
		delay(200);
		i++;
		if(i>3) i=0;

		if (myTouch.dataAvailable())
		{
			myTouch.read();
			x=myTouch.getX();
			y=myTouch.getY();

			if (((y>=20) && (y<=120)) && ((x>=10) && (x<=110))) //Возврат
			{
				waitForIt(10, 20, 110, 120);
				break;
			}
			else if (((y>=20) && (y<=120)) && ((x>=120) && (x<=220)))     //Возврат
			{
				break;
			}
			else if (((y>=130) && (y<=230)) && ((x>=10) && (x<=110))) //Возврат
			{
				break;
			}
			else if (((y>=130) && (y<=230)) && ((x>=120) && (x<=220))) //Возврат
			{
				break;
			}
		}
	}
	// Здесь отключить
	myGLCD.setColor(255, 255, 255);
	myGLCD.drawRoundRect (10, 20, 110,120);                    // Кнопка "Запад"
	myGLCD.print("<===",25, 80);      
	delay(500);
}
void rotor_runEast()
{
	myGLCD.setColor(255,0, 0);                       // Установлен красный цвет надписи
	myGLCD.drawRoundRect (120, 20, 220, 120);                  // Кнопка "Восток"
	int i=0;
	delay(100);
	while (true)
	{
		// Здесь проверить на отключение по концевику. Отключен = break;
		// Здесь включить
		if(i==0) 	myGLCD.print("=  >",140, 80);                              // ВОСТОК
		if(i==1) 	myGLCD.print(" = >",140, 80);                              // ВОСТОК
		if(i==2) 	myGLCD.print("  =>",140, 80);                              // ВОСТОК
		if(i==3) 	myGLCD.print("   >",140, 80);                              // ВОСТОК
		delay(200);
		i++;
		if(i>3) i=0;
		if (myTouch.dataAvailable())
		{
			myTouch.read();
			x=myTouch.getX();
			y=myTouch.getY();

				if (((y>=20) && (y<=120)) && ((x>=10) && (x<=110))) //Возврат
			{
				break;
			}
			else if (((y>=20) && (y<=120)) && ((x>=120) && (x<=220)))     //Возврат
			{
				waitForIt(120, 20, 220, 120);
				break;
			}
			else if (((y>=130) && (y<=230)) && ((x>=10) && (x<=110))) //Возврат
			{
				break;
			}
			else if (((y>=130) && (y<=230)) && ((x>=120) && (x<=220))) //Возврат
			{
				break;
			}
		}
	}
	// Здесь отключить
	myGLCD.setColor(255, 255, 255);
	myGLCD.drawRoundRect (120, 20, 220, 120);                  // Кнопка "Восток"
	myGLCD.print("===>",140, 80);                              // ВОСТОК    
	delay(500);
}
void rotor_runUp()
{
	myGLCD.setColor(255,0, 0);                       // Установлен красный цвет надписи
	myGLCD.drawRoundRect (10, 130, 110, 230);                  // Кнопка "Вверх" 
	int i=0;
	delay(100);
	while (true)
	{
		// Здесь проверить на отключение по концевику. Отключен = break;
		// Здесь включить
		if(i==0) myGLCD.print("< =",48, 163,45);                            // ВВЕРХ 
		if(i==1) myGLCD.print("<= ",48, 163,45);                            // ВВЕРХ
		if(i==2) myGLCD.print("<  ",48, 163,45);                            // ВВЕРХ

		delay(200);
		i++;
		if(i>2) i=0;
		if (myTouch.dataAvailable())
		{
			myTouch.read();
			x=myTouch.getX();
			y=myTouch.getY();

			if (((y>=20) && (y<=120)) && ((x>=10) && (x<=110))) //Возврат
			{
				break;
			}
			else if (((y>=20) && (y<=120)) && ((x>=120) && (x<=220)))     //Возврат
			{
				break;
			}
			else if (((y>=130) && (y<=230)) && ((x>=10) && (x<=110))) //Возврат
			{
				waitForIt(10, 130, 110, 230);
				break;
			}
			else if (((y>=130) && (y<=230)) && ((x>=120) && (x<=220))) //Возврат
			{
				break;
			}
		}
	}
	// Здесь отключить
	myGLCD.setColor(255, 255, 255);
	myGLCD.drawRoundRect (10, 130, 110, 230);                  // Кнопка "Вверх" 
	myGLCD.print("<==",48, 163,45);                            // ВВЕРХ   
	delay(500);
}
void rotor_runDown()
{
	myGLCD.setColor(255,0, 0);                       // Установлен красный цвет надписи
	myGLCD.drawRoundRect (120, 130,220, 230);                  // Кнопка "Вниз"
	int i=0;
	delay(100);
	while (true)
	{
		// Здесь проверить на отключение по концевику. Отключен = break;
		// Здесь включить
		if(i==0) myGLCD.print("= >",155, 163,45);                           // ВНИЗ
		if(i==1) myGLCD.print(" =>",155, 163,45);                           // ВНИЗ
		if(i==2) myGLCD.print("  >",155, 163,45);                           // ВНИЗ

		delay(200);
		i++;
		if(i>2) i=0;
		if (myTouch.dataAvailable())
		{
			myTouch.read();
			x=myTouch.getX();
			y=myTouch.getY();

			if (((y>=20) && (y<=120)) && ((x>=10) && (x<=110))) //Возврат
			{
				break;
			}
			else if (((y>=20) && (y<=120)) && ((x>=120) && (x<=220)))     //Возврат
			{
				break;
			}
			else if (((y>=130) && (y<=230)) && ((x>=10) && (x<=110))) //Возврат
			{
				break;
			}
			else if (((y>=130) && (y<=230)) && ((x>=120) && (x<=220))) //Возврат
			{
				waitForIt(120, 130, 220, 230);
				break;
			}
		}
	}
	// Здесь отключить
	myGLCD.setColor(255, 255, 255);
	myGLCD.drawRoundRect (120, 130,220, 230);                  // Кнопка "Вниз"
	myGLCD.print("==>",155, 163,45);                           // ВНИЗ
	delay(500);
}

void setup_mcp()
{
		mcp_Out1.begin(1);                      //  U11
		mcp_Out1.pinMode(0, OUTPUT);            //  
		mcp_Out1.pinMode(1, OUTPUT);            //  
		mcp_Out1.pinMode(2, OUTPUT);            //  
		mcp_Out1.pinMode(3, OUTPUT);            //  
		mcp_Out1.pinMode(4, OUTPUT);            //  
		mcp_Out1.pinMode(5, OUTPUT);            //  
		mcp_Out1.pinMode(6, OUTPUT);            //  
		mcp_Out1.pinMode(7, OUTPUT);            //    
		mcp_Out1.pinMode(8, OUTPUT);            //  
		mcp_Out1.pinMode(9, OUTPUT);            //  
		mcp_Out1.pinMode(10, OUTPUT);           //  
		mcp_Out1.pinMode(11, OUTPUT);           //    
		mcp_Out1.pinMode(12, OUTPUT);           //     
		mcp_Out1.pinMode(13, OUTPUT);           //   
		mcp_Out1.pinMode(14, OUTPUT);           //       
		mcp_Out1.pinMode(15, OUTPUT);           //        
	
		mcp_Out2.begin(2);                      // (2)   
		mcp_Out2.pinMode(0, OUTPUT);            // 
		mcp_Out2.pinMode(1, OUTPUT);            // 
		mcp_Out2.pinMode(2, OUTPUT);            // 
		mcp_Out2.pinMode(3, OUTPUT);            // 
		mcp_Out2.pinMode(4, OUTPUT);            //   
		mcp_Out2.pinMode(5, OUTPUT);            // 
		mcp_Out2.pinMode(6, OUTPUT);            // 
		mcp_Out2.pinMode(7, OUTPUT);            // 
		mcp_Out2.pinMode(8, OUTPUT);            // 
		mcp_Out2.pinMode(9, OUTPUT);            // 
		mcp_Out2.pinMode(10, OUTPUT);           // 
		mcp_Out2.pinMode(11, OUTPUT);           // 
		mcp_Out2.pinMode(12, OUTPUT);           // 
		mcp_Out2.pinMode(13, OUTPUT);           //   
		mcp_Out2.pinMode(14, OUTPUT);           //    
		mcp_Out2.pinMode(15, OUTPUT);           //  

		for(int i =0; i<16; i++)
		 {
		   mcp_Out1.digitalWrite(i,HIGH);       
		   mcp_Out2.digitalWrite(i,HIGH);       
		   delay(10);
		 }

		for(int i =0; i<16; i++)
		 {
		   mcp_Out1.digitalWrite(i,HIGH);       
		   mcp_Out2.digitalWrite(i,HIGH);       
		   delay(10);
		 }
}

void setup()  
{  
	Serial.begin(9600); 
	Wire.begin();
	myGLCD.InitLCD();
	myGLCD.clrScr();
	myGLCD.setFont(BigFont);
	myTouch.InitTouch();
	myTouch.setPrecision(PREC_HI);

	if(EEPROM.read(0)==255)
     {
		 clear_eeprom();
		 ini_eeprom();
     }
	 for(int i=1;i<4;i++)
	 {
        get_prg_eeprom(i);
     }
	draw_Glav_menu();

	Serial.println("Setup Ok");
}  
void loop() 
{    
 	swich_Glav_Menu();
	delay(10);
} 
