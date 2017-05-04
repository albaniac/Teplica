
#include <UTouchCD.h>
#include <UTouch.h>
#include <UTFT.h>
#include <RTClib.h>
#include "Wire.h"
#include <OneWire.h>
#include <SPI.h>
#include <EEPROM.h>
#include <Adafruit_MCP23017.h>
#include <MsTimer2.h> 
#include <dht11.h>

dht11 DHT11;


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
uint8_t sec     = 0;         //Initialization time
uint8_t min     = 0;
uint8_t hour    = 0;
uint8_t dow     = 1;
uint8_t date    = 1;
uint8_t mon     = 1;
uint16_t year   = 14;
RTC_DS1307 RTC;            // define the Real Time Clock object

int clockCenterX     = 119;
int clockCenterY     = 119;
int oldsec=0;
const char* str[]          = {"MON","TUE","WED","THU","FRI","SAT","SUN"};
const char* str_mon[]      = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
//-------------------------------------------------------------------------------
int but1, but2, but3, but4, but5, but6, but7, but8, but9, but10, butX, butY, but_m1, but_m2, but_m3, but_m4, but_m5, pressed_button;

Adafruit_MCP23017 mcp_Out1;             // U11 MCP23017 расширение портов (клапана полива)
Adafruit_MCP23017 mcp_Out2;             // U8  MCP23017 расширение портов ()
//mcp_Out1 U11
#define Kl_Poliva1_1    0                   //
#define Kl_Poliva1_2    1                   //
#define Kl_Poliva1_3    2                   //
#define Kl_Poliva1_4    3                   //
#define Kl_Poliva1_5    4                   // 
#define Kl_Poliva1_6    5                   //
#define Kl_Poliva1_7    6                   //
#define Nasos_Poliv1    7                   // Насос в теплице №1
#define Kl_Poliva2_1    8                   //
#define Kl_Poliva2_2    9                   //
#define Kl_Poliva2_3    10                  //
#define Kl_Poliva2_4    11                  //
#define Kl_Poliva2_5    12                  //
#define Kl_Poliva2_6    13                  //
#define Kl_Poliva2_7    14                  //
#define Nasos_Poliv2    15                  // Насос в теплице №2

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int sector                           = 1;                 // Номер сектора 
int count_sector                     = 0;                 // Счетчик политых секторов. Необходим для организации перерыва для заполнения бочки

unsigned long temp_time_pol          = 0;                 // Переменная для  хранения время старта полива.  
unsigned int  time_bochka            = 60*30;             // Время для промежуточного наполнения бочки (30 мин.)

int teplitca                         = 1;                 // Номер теплицы
int teplitca_adr                     = 0;                      

int time_poliv                       = 90;                // Время полива (сек.)       

bool on_off                          = false;
bool err_save                        = true;
volatile int run_timer               = 0;                 // переменная для отсчета времени полива
int deviceaddress                    = 80;                // Адрес EEPROM
unsigned int eeaddress               =  0;                // Адрес 


int adr1_1kl_avto                    = 111;   //Теплица №1
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


void flash_time()                              // Программа обработчик прерывания 
{ 
  run_timer++;                                 // Отсчет времени полива 1 сек
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
  clock_read();
  drawMin(min);
  drawHour(hour, min);
  drawSec(sec);
  oldsec=sec;

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
  
  myGLCD.setColor(64, 64, 128);
  myGLCD.fillRoundRect(260, 140, 319, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect(260, 140, 319, 180);
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
  clock_read();
  myGLCD.setFont(BigFont);
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(255, 255, 255);
	
  myGLCD.print(str[dow-1], 256, 8);
  if (date<10)
	myGLCD.printNumI(date, 272, 28);
  else
	myGLCD.printNumI(date, 264, 28);

  myGLCD.print(str_mon[mon-1], 256, 48);
  myGLCD.printNumI(year, 248, 65);
}
void clearDate()
{
  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRect(248, 8, 312, 81);
}
void clock_read()
{
		Wire.beginTransmission(0x68);//Send the address of DS1307
		Wire.write(0);//Sending address	
		Wire.endTransmission();//The end of the IIC communication
		Wire.requestFrom(0x68, 7);//IIC communication is started, you can continue to access another address (address auto increase) and the number of visits
		sec = bcd2bin(Wire.read());//read time
		min = bcd2bin(Wire.read());
		hour = bcd2bin(Wire.read());
		dow = Wire.read();
		date = bcd2bin(Wire.read());
		mon = bcd2bin(Wire.read());
		year = bcd2bin(Wire.read()) + 2000;
		delay(10);
	//  Wire.endTransmission();//The end of the IIC communication
}
int bcd2bin(int temp)//BCD  to decimal
{
	int a,b,c;
	a=temp;
	b=0;
	if(a>=16)
	{
		while(a>=16)
		{
			a=a-16;
			b=b+10;
			c=a+b;
			temp=c;
		}
	}
	return temp;
}
void AnalogClock()
{
	int x, y;

	drawDisplay();
	printDate();
    clock_read();
  
  while (true)
  {
	if (oldsec!=sec)
	{
	  if ((sec== 0) && (min==0) && (hour==0))
	  {
		clearDate();
		printDate();
	  }
	  if (sec==0)
	  {
		drawMin(min);
		drawHour(hour, min);
	  }
	  drawSec(sec);
	  oldsec=sec;
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
	  if (((y>=140) && (y<=180)) && ((x>=260) && (x<=319))) //Возврат
	  {
		myGLCD.setColor (255, 0, 0);
		myGLCD.drawRoundRect(260, 140, 319, 180);
		myGLCD.clrScr();
		myGLCD.setFont(BigFont);
		break;
	  }
	}
	delay(10);
	clock_read();
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

	  if ((y>=10) && (y<=60))                   // Upper row
	  {
		if ((x>= xi+10) && (x<= xi+60))         // Button: 1
		{
		  waitForIt( xi+10, 10,  xi+60, 60);
		  updateStr('1');
		}
		if ((x>= xi+70) && (x<= xi+120))        // Button: 2
		{
		  waitForIt( xi+70, 10,  xi+120, 60);
		  updateStr('2');
		}
		if ((x>= xi+130) && (x<= xi+180))       // Button: 3
		{
		  waitForIt( xi+130, 10,  xi+180, 60);
		  updateStr('3');
		}
		if ((x>= xi+190) && (x<= xi+240))       // Button: 4
		{
		  waitForIt( xi+190, 10,  xi+240, 60);
		  updateStr('4');
		}
		if ((x>= xi+250) && (x<= xi+300))       // Button: 5
		{
		  waitForIt( xi+250, 10,  xi+300, 60);
		  updateStr('5');
		}
	  }

	  if ((y>=70) && (y<=120))                 // Center row
	  {
		if ((x>= xi+10) && (x<= xi+60))        // Button: 6
		{
		  waitForIt( xi+10, 70,  xi+60, 120);
		  updateStr('6');
		}
		if ((x>= xi+70) && (x<= xi+120))       // Button: 7
		{
		  waitForIt( xi+70, 70,  xi+120, 120);
		  updateStr('7');
		}
		if ((x>= xi+130) && (x<= xi+180))     // Button: 8
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
	                                                         // Обрамление кнопок
	myGLCD.fillRoundRect (1, 1, 240, 117);                   // Кнопка  зоны Теплица №1
	myGLCD.fillRoundRect (1, 122, 240, 239);                 // Кнопка  зоны Теплица №2
	myGLCD.fillRoundRect (245, 1, 319, 117);                 // Кнопка "ПОЛИВ"
	myGLCD.fillRoundRect (245, 122, 319, 239);               // Кнопка "ОКНА"
	myGLCD.setColor(0, 0, 0);                          
	myGLCD.fillRoundRect (222, 10, 235, 112);                // Шкала уровня воды теплицы №1 
	myGLCD.fillRoundRect (222, 132, 235, 234);               // Шкала уровня воды теплицы №2

	                                                         // Заполнение кнопок
	myGLCD.setColor(255, 255, 255);
	myGLCD.drawRoundRect (1, 1, 240,117);                    // Кнопка  зоны Теплица №1
	myGLCD.drawRoundRect (1, 122, 240, 239);                 // Кнопка  зоны Теплица №2
	myGLCD.drawRoundRect (245, 1, 319, 117);                 // Кнопка "ПОЛИВ"
	myGLCD.drawRoundRect (245, 122, 319, 239);               // Кнопка "ОКНА"
	myGLCD.drawRoundRect (222, 10, 235, 112);                // Шкала уровня воды теплицы №1 
	myGLCD.drawRoundRect (222, 132, 235, 234);               // Шкала уровня воды теплицы №2

	myGLCD.setFont(BigFont);
	//myGLCD.setColor(VGA_YELLOW);
	myGLCD.setColor(255,255,255);
	myGLCD.setBackColor(0, 0, 255);
	myGLCD.print("Te\xA3\xA0\xA2\xA6""a N1",40, 5);         // Теплица №1
	myGLCD.print("Te\xA3\xA0\xA2\xA6""a N2",40, 127);       // Теплица №2

	myGLCD.print("Te\xA1\xA3""epa\xA4ypa",10, 25);          // Температура
	myGLCD.print("\x97o\x9C\x99yxa",10, 40);                // воздуха
	myGLCD.print("\x97o\x99\xAB",10, 55);                   // воды
	myGLCD.print("\x98py\xA2\xA4""a",10, 70);               // грунта
	myGLCD.print("B\xA0""a\x9B\xA2oc\xA4\xAC",10, 85);      // Влажность
	myGLCD.print("\x80o\xA7\x9F""a",10, 100);               // Бочка

	myGLCD.print("Te\xA1\xA3""epa\xA4ypa",10, 25+120);      // Температура    
	myGLCD.print("\x97o\x9C\x99yxa",10, 40+120);            // воздуха
	myGLCD.print("\x97o\x99\xAB",10, 55+120);               // воды
	myGLCD.print("\x98py\xA2\xA4""a",10, 70+120);           // грунта
	myGLCD.print("B\xA0""a\x9B\xA2oc\xA4\xAC",10, 85+120);  // Влажность
	myGLCD.print("\x80o\xA7\x9F""a",10, 100+120);           // Бочка

	myGLCD.print("\x89",275, 15);                           // П
	myGLCD.print("O",275, 32);                              // О
	myGLCD.print("\x88",275, 49);                           // Л
	myGLCD.print("\x86",275, 66);                           // И
	myGLCD.print("B",275, 83);                              // В

	myGLCD.print("O",275, 15+130);                          // О
	myGLCD.print("K",275, 32+130);                          // К
	myGLCD.print("H",275, 49+130);                          // Н
	myGLCD.print("A",275, 66+130);                          // А

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

				if ((x>= 1) && (x<=240))                                              // кнопка Выбор номера теплицы
					{
						 if ((y>=1) && (y<=117))                                      // Выбрана  Теплица № "1"
							{
								waitForIt(1, 1, 240, 117);
								teplitca = 1;                                         // Выбрана  Теплица № "1"                        
								myGLCD.setColor(VGA_RED);                             // Установлен красный цвет надписи
								myGLCD.setBackColor(0, 0, 255);                       // Установлен синий цвет фона
								myGLCD.print("Te\xA3\xA0\xA2\xA6""a N1",40, 5);       // Теплица №1
								myGLCD.setColor(255, 255, 255);                       // Установлен белый цвет надписи
								myGLCD.print("Te\xA3\xA0\xA2\xA6""a N2",40, 127);     // Теплица №2


							}

						else if ((y>=122) && (y<=239))                                // Выбрана  Теплица № "2"
							{
								waitForIt(1, 122, 240, 239);
								teplitca = 2;                                         // Выбрана  Теплица № "2"   
								myGLCD.setColor(255, 255, 255);                       // Установлен белый цвет надписи
								myGLCD.setBackColor(0, 0, 255);                       // Установлен синий цвет фона
								myGLCD.print("Te\xA3\xA0\xA2\xA6""a N1",40, 5);       // Теплица №1 
								myGLCD.setColor(VGA_RED);                             // Установлен красный цвет надписи
								myGLCD.print("Te\xA3\xA0\xA2\xA6""a N2",40, 127);     // Теплица №2
							}

					}
				if ((x>=245) && (x<=319))                                             // кнопка Управление поливом или окнами
					{
						 if ((y>=1) && (y<=117))                                      // выбран Полив
							{
								waitForIt(245, 1, 319, 117);
								draw_Poliv_Menu();                                    // нарисовать Меню полива
								swich_Poliv_Menu();                                   // вызов меню полива
								draw_Glav_menu();                                     // возврат в главное меню
							}

						else if ((y>=122) && (y<=239))                                // Управление окнами
							{
								waitForIt(245, 122, 319, 239);
								// Программа  управления окнами
								draw_Glav_menu();                                     // возврат в главное меню
							}

					}
		  }
	}
}

void draw_Poliv_Menu()  
{
	myGLCD.clrScr();
	myGLCD.setFont(BigFont);
	myGLCD.setColor(0, 0, 255);
	myGLCD.fillRoundRect (260, 5, 319, 55);        // Кнопка 
	myGLCD.fillRoundRect (260, 65, 319, 115);      // Кнопка 
	myGLCD.fillRoundRect (260, 125, 319, 175);     // 
	myGLCD.fillRoundRect (260, 185, 319, 235);     // Кнопка 

	myGLCD.setColor(255, 255, 255);
	myGLCD.drawRoundRect (260, 5, 319, 55);        //  
	myGLCD.drawRoundRect (260, 65, 319, 115);
	myGLCD.drawRoundRect (260, 125, 319, 175);
	myGLCD.drawRoundRect (260, 185, 319, 235);

	myGLCD.drawRoundRect (5, 5+30, 80, 65+30);
	myGLCD.drawRoundRect (85, 5+30, 165, 65+30);
	myGLCD.drawRoundRect (170, 5+30, 250, 65+30);
	myGLCD.drawRoundRect (5, 70+30, 80, 170);
	myGLCD.drawRoundRect (85, 70+30, 165, 170);
	myGLCD.drawRoundRect (170, 70+30, 250, 170);
	myGLCD.drawRoundRect (5, 175, 80, 235);
	myGLCD.drawRoundRect (85, 175, 165, 235);
	myGLCD.drawRoundRect (170, 175, 250, 235);
	myGLCD.drawLine (85, 65+30, 165, 5+30);
	myGLCD.drawLine (85, 175, 165, 235);

	myGLCD.setBackColor(0, 0, 255);
	myGLCD.print("BCE",265, 13);                               // 
	myGLCD.print("BK""\x88",265, 32);                          // 
	myGLCD.print("BCE",265, 73);                               //
	myGLCD.print("OTK",265, 92);                         //
	myGLCD.print("B""\x91""X",265, 142);                            // "ESC"
	myGLCD.print("\x8D""a",274, 195);                        // "Ча"
	myGLCD.print("c\xAB",274, 210);                          // "сы"
	myGLCD.setColor(VGA_YELLOW);
	myGLCD.setBackColor(0, 0, 0);
	myGLCD.print("1",10, 10+30);          
	myGLCD.print("2",175, 10+30); 
	myGLCD.print("3",10, 76+30);          
	myGLCD.print("4",90, 76+30); 
	myGLCD.print("5",175, 76+30); 
	myGLCD.print("6",10, 180);          
	myGLCD.print("7",175, 180); 
	myGLCD.setFont(SmallFont);
	myGLCD.print("=>",95, 15+30); 
	myGLCD.print("<=",140, 40+30); 
	myGLCD.print("=>",95, 210); 
	myGLCD.print("<=",140, 187); 

	if(teplitca==1)
		{
			myGLCD.setFont(BigFont);
			myGLCD.print("Te\xA3\xA0\xA2\xA6""a N1",40, 5);  
			myGLCD.setFont(SmallFont);
			if (i2c_eeprom_read_byte(deviceaddress, adr1_1kl_avto ) == 0 )  // 1
				{
					myGLCD.print("Py\xA7\xA2",30, 14+30);   
					//Serial.println(adr1_1kl_avto);
				}
			else
				{
					myGLCD.print("A\x97\xA4o ",30, 14+30);  
					//Serial.println(adr1_1kl_avto);
				}

			if (i2c_eeprom_read_byte(deviceaddress, adr1_2kl_avto ) == 0 )  // 2
				{
					myGLCD.print("Py\xA7\xA2",195, 14+30);    
				}
			else
				{
					myGLCD.print("A\x97\xA4o ",195, 14+30);  
				}


			if (i2c_eeprom_read_byte(deviceaddress, adr1_3kl_avto ) == 0 )  // 3
				{
					myGLCD.print("Py\xA7\xA2",30, 80+30);    
				}
			else
				{
					myGLCD.print("A\x97\xA4o ",30, 80+30);  
				}

			if (i2c_eeprom_read_byte(deviceaddress, adr1_4kl_avto ) == 0 ) // 4
				{
					myGLCD.print("Py\xA7\xA2",110, 80+30);    
				}
			else
				{
					myGLCD.print("A\x97\xA4o ",110, 80+30);  
				}

		   if (i2c_eeprom_read_byte(deviceaddress, adr1_5kl_avto ) == 0 )  // 5
				{
					myGLCD.print("Py\xA7\xA2 ",195, 80+30);    
				}
			else
				{
					myGLCD.print("A\x97\xA4o ",195, 80+30);  
				}

			if (i2c_eeprom_read_byte(deviceaddress, adr1_6kl_avto ) == 0 )  // 6
				{
					myGLCD.print("Py\xA7\xA2",30, 184);    
				}
			else
				{
					myGLCD.print("A\x97\xA4o ",30, 184);  
				}

			if (i2c_eeprom_read_byte(deviceaddress, adr1_7kl_avto ) == 0 )  // 7
				{
					myGLCD.print("Py\xA7\xA2",195, 184);    
				}
			else
				{
					myGLCD.print("A\x97\xA4o ",195, 184);  
				}
			myGLCD.setFont(BigFont);

			myGLCD.print("  : ",6, 27+30);  // 1
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_1kl_start_hour ),10,27+30);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_1kl_start_min ),48,27+30);
			myGLCD.print("  : ",6, 45+30);  
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_1kl_stop_min ),10,45+30);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_1kl_stop_sec ),48,45+30);

			myGLCD.print("  : ",171, 27+30);  //2
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_2kl_start_hour ),175,27+30);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_2kl_start_min ),216,27+30);
			myGLCD.print("  : ",171, 45+30);  
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_2kl_stop_min ),175,45+30);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_2kl_stop_sec ),216,45+30);

			myGLCD.print("  : ",6, 93+30);  // 3
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_3kl_start_hour ),10,93+30);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_3kl_start_min ),48,93+30);
			myGLCD.print("  : ",6, 111+30);  
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_3kl_stop_min ),10,111+30);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_3kl_stop_sec ),48,111+30);

			myGLCD.print("  : ",86, 93+30);  //4
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_4kl_start_hour ),90,93+30);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_4kl_start_min ),128,93+30);
			myGLCD.print("  : ",86, 111+30);  
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_4kl_stop_min ),90,111+30);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_4kl_stop_sec ),128,111+30);

			myGLCD.print("  : ",171, 93+30);  // 5
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_5kl_start_hour ),175,93+30);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_5kl_start_min ),216,93+30);
			myGLCD.print("  : ",171, 111+30);  
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_5kl_stop_min ),175,111+30);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_5kl_stop_sec ),216,111+30);

			myGLCD.print("  : ",6, 197);  //6
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_6kl_start_hour ),10,197);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_6kl_start_min ),48,197);
			myGLCD.print("  : ",6, 215);  
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_6kl_stop_min ),10,215);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_6kl_stop_sec ),48,215);

			myGLCD.print("  : ",171, 197);  //7
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_7kl_start_hour ),175,197);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_7kl_start_min ),216,197);
			myGLCD.print("  : ",171, 215);  
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_7kl_stop_min ),175,215);
			myGLCD.printNumI(i2c_eeprom_read_byte(deviceaddress, adr1_7kl_stop_sec ),216,215);

		}

	if(teplitca==2)
		{
			myGLCD.setFont(BigFont);
			myGLCD.print("Te\xA3\xA0\xA2\xA6""a N2",40, 5);  
			myGLCD.setFont(SmallFont);
		if (i2c_eeprom_read_byte(deviceaddress, adr2_1kl_avto ) == 0 )  // 1
				{
					myGLCD.print("Py\xA7\xA2",30, 14+30);   
					//Serial.println(adr1_1kl_avto);
				}
			else
				{
					myGLCD.print("A\x97\xA4o ",30, 14+30);  
					//Serial.println(adr1_1kl_avto);
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

				if ((x>=260) && (x<=319))                               // 
					{
						 if ((y>=5) && (y<=55))                         //  "1"
							{
								waitForIt(260, 5, 319, 55);             //   
								myGLCD.setColor(0, 255, 0);
								myGLCD.setBackColor(0, 255, 0);
	                            myGLCD.fillRoundRect (260, 5, 319, 55);        // Кнопка 
								myGLCD.setColor(255, 255, 255);
	                            myGLCD.drawRoundRect (260, 5, 319, 55);        //
								myGLCD.setColor(255, 255, 255);
								myGLCD.print("BCE",265, 13);                               // 
	                            myGLCD.print("BK""\x88",265, 32);                          // 
							    count_sector=0;
								poliv_ruchnoj();  
							
								myGLCD.setColor(0, 0, 255);
								myGLCD.setBackColor(0, 0, 255);
								myGLCD.fillRoundRect (260, 5, 319, 55);        // Кнопка 
								myGLCD.setColor(255, 255, 255);
								myGLCD.drawRoundRect (260, 5, 319, 55);        //
								myGLCD.setColor(255, 255, 255);
								myGLCD.print("BCE",265, 13);                               // 
								myGLCD.print("BK""\x88",265, 32);    
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

	if (teplitca == 1) teplitca_adr = 100;                                                   // Определить диапазон адресов текущей теплицы
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
			mcp_Out1.digitalWrite(temp_sector-1, LOW);  
			delay(500);
			mcp_Out1.digitalWrite(7, LOW);  
			Serial.print(temp_teplitca);
			Serial.print("  ");
			Serial.print(temp_sector);
			Serial.print("  ");
			Serial.println("On");

			switch (temp_sector) 
			{
			case 1:
				myGLCD.setColor(255, 0, 0);
				myGLCD.drawRoundRect (5, 5+30, 80, 65+30);
				myGLCD.drawLine (85, 65+30, 165, 5+30);

				myGLCD.drawLine (85, 5+30, 165, 5+30);
                myGLCD.drawLine (85, 65+30, 85, 5+30);
				break;
			case 2:
				myGLCD.setColor(255, 0, 0);
				myGLCD.drawRoundRect (170, 5+30, 250, 65+30);
				myGLCD.drawLine (85, 65+30, 165, 5+30);
					
				myGLCD.drawLine (85, 65+30, 165, 65+30);
                myGLCD.drawLine (165, 65+30, 165, 5+30);

				break;
 			case 3:
				myGLCD.setColor(255, 0, 0);
				myGLCD.drawRoundRect (5, 70+30, 80, 170);
				break;
			case 4:
				myGLCD.setColor(255, 0, 0);
				myGLCD.drawRoundRect (85, 70+30, 165, 170);
				break;
			case 5:
				myGLCD.setColor(255, 0, 0);
				myGLCD.drawRoundRect (170, 70+30, 250, 170);
				break;
			case 6:
				myGLCD.setColor(255, 0, 0);
				myGLCD.drawRoundRect (5, 175, 80, 235);
				myGLCD.drawLine (85, 175, 165, 235);
					
				myGLCD.drawLine (85, 235, 165, 235);
                myGLCD.drawLine (85, 175, 85, 235);
				break;
			case 7:
				myGLCD.setColor(255, 0, 0);
				myGLCD.drawRoundRect (170, 175, 250, 235);
				myGLCD.drawLine (85, 175, 165, 235);
					
				myGLCD.drawLine (165, 175, 165, 235);
                myGLCD.drawLine (85, 175, 165, 175);
            	break;
			}
		}
		else if(temp_on_off == false)
		{
			mcp_Out1.digitalWrite(temp_sector-1, HIGH);  
			mcp_Out1.digitalWrite(7, HIGH);  
			Serial.print(temp_teplitca);
			Serial.print("  ");
			Serial.print(temp_sector);
			Serial.print("  ");
			Serial.println("Off");
			switch (temp_sector) 
			{
			case 1:
				myGLCD.setColor(255, 255, 255);
				myGLCD.drawRoundRect (5, 5+30, 80, 65+30);
				myGLCD.drawLine (85, 65+30, 165, 5+30);
				myGLCD.drawLine (85, 5+30, 165, 5+30);
                myGLCD.drawLine (85, 65+30, 85, 5+30);
				break;
			case 2:
				myGLCD.setColor(255, 255, 255);
				myGLCD.drawRoundRect (170, 5+30, 250, 65+30);
				myGLCD.drawLine (85, 65+30, 165, 5+30);
				myGLCD.drawLine (85, 65+30, 165, 65+30);
                myGLCD.drawLine (165, 65+30, 165, 5+30);
				break;
 			case 3:
				myGLCD.setColor(255, 255, 255);
				myGLCD.drawRoundRect (5, 70+30, 80, 170);
				break;
			case 4:
				myGLCD.setColor(255, 255, 255);
				myGLCD.drawRoundRect (85, 70+30, 165, 170);
				break;
			case 5:
				myGLCD.setColor(255, 255, 255);
				myGLCD.drawRoundRect (170, 70+30, 250, 170);
				break;
			case 6:
				myGLCD.setColor(255, 255, 255);
				myGLCD.drawRoundRect (5, 175, 80, 235);
				myGLCD.drawLine (85, 175, 165, 235);
				myGLCD.drawLine (85, 235, 165, 235);
                myGLCD.drawLine (85, 175, 85, 235);
				break;
			case 7:
				myGLCD.setColor(255, 255, 255);
				myGLCD.drawRoundRect (170, 175, 250, 235);
				myGLCD.drawLine (85, 175, 165, 235);
				myGLCD.drawLine (165, 175, 165, 235);
                myGLCD.drawLine (85, 175, 165, 175);


				break;
			}
		}
 	}
	else if(temp_teplitca == 2)
	{
		if(temp_on_off == true)
		{
			mcp_Out1.digitalWrite(temp_sector-1+8, LOW);  
			delay(500);
			mcp_Out1.digitalWrite(7+8, LOW); 
			Serial.print(temp_teplitca);
			Serial.print("  ");
			Serial.print(temp_sector);
			Serial.print("  ");
			Serial.println("On");
			  
			switch (temp_sector) 
			{
			case 1:
				myGLCD.setColor(255, 0, 0);
				myGLCD.drawRoundRect (5, 5+30, 80, 65+30);
				myGLCD.drawLine (85, 65+30, 165, 5+30);

				myGLCD.drawLine (85, 5+30, 165, 5+30);
                myGLCD.drawLine (85, 65+30, 85, 5+30);
				break;
			case 2:
				myGLCD.setColor(255, 0, 0);
				myGLCD.drawRoundRect (170, 5+30, 250, 65+30);
				myGLCD.drawLine (85, 65+30, 165, 5+30);
					
				myGLCD.drawLine (85, 65+30, 165, 65+30);
                myGLCD.drawLine (165, 65+30, 165, 5+30);

				break;
 			case 3:
				myGLCD.setColor(255, 0, 0);
				myGLCD.drawRoundRect (5, 70+30, 80, 170);
				break;
			case 4:
				myGLCD.setColor(255, 0, 0);
				myGLCD.drawRoundRect (85, 70+30, 165, 170);
				break;
			case 5:
				myGLCD.setColor(255, 0, 0);
				myGLCD.drawRoundRect (170, 70+30, 250, 170);
				break;
			case 6:
				myGLCD.setColor(255, 0, 0);
				myGLCD.drawRoundRect (5, 175, 80, 235);
				myGLCD.drawLine (85, 175, 165, 235);
					
				myGLCD.drawLine (85, 235, 165, 235);
                myGLCD.drawLine (85, 175, 85, 235);
				break;
			case 7:
				myGLCD.setColor(255, 0, 0);
				myGLCD.drawRoundRect (170, 175, 250, 235);
				myGLCD.drawLine (85, 175, 165, 235);
					
				myGLCD.drawLine (165, 175, 165, 235);
                myGLCD.drawLine (85, 175, 165, 175);
            	break;
			}

		}
		else if(temp_on_off == false)
		{
			mcp_Out1.digitalWrite(temp_sector-1+8, HIGH);  
			mcp_Out1.digitalWrite(7+8, HIGH);  
			Serial.print(temp_teplitca);
			Serial.print("  ");
			Serial.print(temp_sector);
			Serial.print("  ");
			Serial.println("Off");

			switch (temp_sector) 
			{
			case 1:
				myGLCD.setColor(255, 255, 255);
				myGLCD.drawRoundRect (5, 5+30, 80, 65+30);
				myGLCD.drawLine (85, 65+30, 165, 5+30);
				myGLCD.drawLine (85, 5+30, 165, 5+30);
                myGLCD.drawLine (85, 65+30, 85, 5+30);
				break;
			case 2:
				myGLCD.setColor(255, 255, 255);
				myGLCD.drawRoundRect (170, 5+30, 250, 65+30);
				myGLCD.drawLine (85, 65+30, 165, 5+30);
				myGLCD.drawLine (85, 65+30, 165, 65+30);
                myGLCD.drawLine (165, 65+30, 165, 5+30);
				break;
 			case 3:
				myGLCD.setColor(255, 255, 255);
				myGLCD.drawRoundRect (5, 70+30, 80, 170);
				break;
			case 4:
				myGLCD.setColor(255, 255, 255);
				myGLCD.drawRoundRect (85, 70+30, 165, 170);
				break;
			case 5:
				myGLCD.setColor(255, 255, 255);
				myGLCD.drawRoundRect (170, 70+30, 250, 170);
				break;
			case 6:
				myGLCD.setColor(255, 255, 255);
				myGLCD.drawRoundRect (5, 175, 80, 235);
				myGLCD.drawLine (85, 175, 165, 235);
				myGLCD.drawLine (85, 235, 165, 235);
                myGLCD.drawLine (85, 175, 85, 235);
				break;
			case 7:
				myGLCD.setColor(255, 255, 255);
				myGLCD.drawRoundRect (170, 175, 250, 235);
				myGLCD.drawLine (85, 175, 165, 235);
				myGLCD.drawLine (165, 175, 165, 235);
                myGLCD.drawLine (85, 175, 165, 175);
				break;
			}
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
					myGLCD.setColor(0, 0, 255);
					myGLCD.setBackColor(0, 0, 255);
					myGLCD.fillRoundRect (260, 5, 319, 55);        // Кнопка 
					myGLCD.setColor(255, 255, 255);
					myGLCD.drawRoundRect (260, 5, 319, 55);        //
					myGLCD.setColor(255, 255, 255);
					myGLCD.print("BCE",265, 13);                               // 
					myGLCD.print("BK""\x88",265, 32);    
					rtemp_on_off = false;
					run_poliv_on_off(teplitca ,i ,rtemp_on_off);
					break;
				}
			}
		}

		print_avtomat = i2c_eeprom_read_byte(deviceaddress, (i * 10) + teplitca_adr + 1 );

		if (print_avtomat == true)                                   // Включить если режим полива "Автомат"
		{
			rtemp_on_off = true;
			if (t_run == true)
			{
				run_poliv_on_off(teplitca , i ,rtemp_on_off);
				t_run = false;

				temp_time_pol = millis();                         // Записать время старта полива
				// MsTimer2::start();
			}
				  
			if (millis()- temp_time_pol > time_poliv * 1000)     // Проверить время окончания полива.
			{
				//  MsTimer2::stop();
				rtemp_on_off = false;
				run_poliv_on_off(teplitca , i ,rtemp_on_off);
				i++;
				count_sector++;
				t_run = true;
				// run_timer =0;
			}

		}
		else
		{
			i++;
		}

		if (count_sector >=3)
		{
			Serial.println("voda in");

            for (int i=0;i<time_bochka;i++)
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
									myGLCD.setColor(0, 0, 255);
									myGLCD.setBackColor(0, 0, 255);
									myGLCD.fillRoundRect (260, 5, 319, 55);        // Кнопка 
									myGLCD.setColor(255, 255, 255);
									myGLCD.drawRoundRect (260, 5, 319, 55);        //
									myGLCD.setColor(255, 255, 255);
									myGLCD.print("BCE",265, 13);                               // 
									myGLCD.print("BK""\x88",265, 32);    
									rtemp_on_off = false;
									run_poliv_on_off(teplitca ,i ,rtemp_on_off);
									break;
								}
						}
				}

			delay(1000);       // Время наполнения бочки.  1 sec * 
			}
            count_sector=0;
			Serial.println("voda out");
		}
	} while (i<8);
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
void test_MCP1()
 {
	 for(int i =0; i<16; i++)
	 {
	   mcp_Out1.digitalWrite(i, LOW);       
	   delay(100);
	   mcp_Out1.digitalWrite(i,HIGH);       
	   delay(100);
	   mcp_Out1.digitalWrite(i, LOW);       
	   delay(100);
	   mcp_Out1.digitalWrite(i,HIGH);       
	   delay(100);
	   mcp_Out1.digitalWrite(i, LOW);       
	   delay(100);
	   mcp_Out1.digitalWrite(i,HIGH);       
	   delay(100);
	   mcp_Out1.digitalWrite(i, LOW);       
	   delay(100);
	   mcp_Out1.digitalWrite(i,HIGH);       
	   delay(100);
	 }
 }
void test_MCP2()
 {
	 for(int i =0; i<16; i++)
	 {
	   mcp_Out2.digitalWrite(i, LOW);       
	   delay(100);
	   mcp_Out2.digitalWrite(i,HIGH);       
	   delay(100);
	   mcp_Out2.digitalWrite(i, LOW);       
	   delay(100);
	   mcp_Out2.digitalWrite(i,HIGH);       
	   delay(100);
	   mcp_Out2.digitalWrite(i, LOW);       
	   delay(100);
	   mcp_Out2.digitalWrite(i,HIGH);       
	   delay(100);
	   mcp_Out2.digitalWrite(i, LOW);       
	   delay(100);
	   mcp_Out2.digitalWrite(i,HIGH);       
	   delay(100);
	 }
 }
void dh11_temperature() 
{
  Serial.println("\n");

  int chk = DHT11.read();

  Serial.print("Read sensor: ");
  switch (chk)
  {
    case  0: Serial.println("OK"); break;
    case -1: Serial.println("Checksum error"); break;
    case -2: Serial.println("Time out error"); break;
    default: Serial.println("Unknown error"); break;
  }

  Serial.print("Humidity (%): ");
  Serial.println((float)DHT11.humidity, DEC);

  Serial.print("Temperature (°C): ");
  Serial.println((float)DHT11.temperature, DEC);

  Serial.print("Temperature (°F): ");
  Serial.println(DHT11.fahrenheit(), DEC);

  Serial.print("Temperature (°K): ");
  Serial.println(DHT11.kelvin(), DEC);

  Serial.print("Dew Point (°C): ");
  Serial.println(DHT11.dewPoint(), DEC);

  Serial.print("Dew PointFast (°C): ");
  Serial.println(DHT11.dewPointFast(), DEC);

}

void setup()  
{  
	Serial.begin(115200); 
	Wire.begin();
	if (!RTC.begin())
		{
			Serial.println("RTC failed");
			while(1);
		}; 
	myGLCD.InitLCD();
	myGLCD.clrScr();
	myGLCD.setFont(BigFont);
	myTouch.InitTouch();
	myTouch.setPrecision(PREC_HI);
	//myTouch.setPrecision(PREC_MEDIUM);
	//i2c_test1();
	setup_mcp();
	//draw_Glav_menu();

	MsTimer2::set(1000, flash_time);                // 1 сек. период таймера прерывания
	DHT11.attach(12);                               // Уточнить
	Serial.println("Setup Ok");

}  
void loop() 
{    
    draw_Glav_menu();
	//swich_Poliv_Menu();
	swich_Glav_Menu();
	 //test_MCP1();
	 //delay(100);



}  