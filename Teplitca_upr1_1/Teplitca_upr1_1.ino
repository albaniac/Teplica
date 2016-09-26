
#include <UTouchCD.h>
#include <UTouch.h>
#include <UTFT.h>
#include <RTClib.h>
#include "Wire.h"
#include <OneWire.h>
#include <SPI.h>
#include <Adafruit_MCP23017.h>
#include <modbus.h>
#include <modbusDevice.h>
#include <modbusRegBank.h>
#include <modbusSlave.h>


//UTFT    myGLCD(ITDB32S,38,39,40,41);        // Дисплей 3,2"
UTFT    myGLCD(TFT01_70,38,39,40,41);       // Дисплей 7,0"
extern uint8_t BigFont[];
extern uint8_t SmallFont[];
UTouch      myTouch(6,5,4,3,2);

int x, y;
int  xi = 490;
int  yi = 200;
char stCurrent[20]="";
int stCurrentLen=0;
char stLast[20]="";

//+++++++++++++++++++++++++++ Настройка часов +++++++++++++++++++++++++++++++
uint8_t sec    = 0;         //Initialization time
uint8_t min    = 0;
uint8_t hour   = 0;
uint8_t dow    = 1;
uint8_t date   = 1;
uint8_t mon    = 1;
uint16_t year  = 14;
RTC_DS1307 RTC;            // define the Real Time Clock object

int clockCenterX     = 119;
int clockCenterY     = 119;
int oldsec=0;
const char* str[]          = {"MON","TUE","WED","THU","FRI","SAT","SUN"};
const char* str_mon[]      = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
//-------------------------------------------------------------------------------


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
  myGLCD.print("RET", 266, 150);
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
	  if ((sec==0) and (min==0) and (hour==0))
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
//------------------------------ Конец Программы часов --------------------------------

//++++++++++++++++++++++++++++++ Объявление портов расширения +++++++++++++++++++++++++++++++++++
Adafruit_MCP23017 mcp_windows1;             //  MCP23017 расширение портов, управление окнами
Adafruit_MCP23017 mcp_windows2;             //  MCP23017 расширение портов, управление окнами 
Adafruit_MCP23017 mcp_windows3;             //  MCP23017 расширение портов, управление окнами 
Adafruit_MCP23017 mcp_poliv;                //  MCP23017 расширение портов, управление поливом и насосами
Adafruit_MCP23017 mcp_Rele1;                //  MCP23017 расширение портов, , управление дополнительными реле
Adafruit_MCP23017 mcp_sda_ext;              //  MCP23017 расширение портов, управление адресацией SDA
Adafruit_MCP23017 mcp_win_sensor1;          //  MCP23017 расширение портов, управление адресацией MCP
Adafruit_MCP23017 mcp_upr_adr1;             //  MCP23017 расширение портов, управление адресацией MCP
//------------------------------------------------------------------------------------------------------



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

//   myGLCD.setColor(255, 255, 255);
//   myGLCD.setBackColor(0, 0, 0);
//   myGLCD.print("            ", CENTER, 310);
//   myGLCD.printNumI(x, CENTER, 310);
//   myGLCD.print("            ", CENTER, 330);
//   myGLCD.printNumI(y, CENTER, 330); 


	  if ((y>=10) && (y<=60))  // Upper row
	  {
		if ((x>= xi+10) && (x<= xi+60))  // Button: 1
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





 void setup_mcp()
{
		mcp_upr_adr1.begin(1);                     //  U13  
		mcp_upr_adr1.pinMode(0, INPUT);             //  Окно №16 датчик концевой   
		mcp_upr_adr1.pinMode(1, INPUT);             //  Окно №17 датчик концевой    
		mcp_upr_adr1.pinMode(2, INPUT);             //  Окно №18 датчик концевой    
		mcp_upr_adr1.pinMode(3, INPUT);             //  Окно №19 датчик концевой    
		mcp_upr_adr1.pinMode(4, INPUT);             //  Окно №20 датчик концевой  
		mcp_upr_adr1.pinMode(5, OUTPUT);            //  U14  A0  Адрес Коммутатора 1Ware
		mcp_upr_adr1.pinMode(6, OUTPUT);            //  U15  A0  Адрес Коммутатора 1Ware
		mcp_upr_adr1.pinMode(7, OUTPUT);            //  

		mcp_upr_adr1.pinMode(8, OUTPUT);            //  U1 A0  Адрес MCP1
		mcp_upr_adr1.pinMode(9, OUTPUT);            //  U2 A0  Адрес MCP1 
		mcp_upr_adr1.pinMode(10, OUTPUT);           //  U3 A0  Адрес MCP1  
		mcp_upr_adr1.pinMode(11, OUTPUT);           //  U4 A0  Адрес MCP1  
		mcp_upr_adr1.pinMode(12, OUTPUT);           //  U5 A0  Адрес MCP1 
		mcp_upr_adr1.pinMode(13, OUTPUT);           //  U6 A0  Адрес MCP1   
		mcp_upr_adr1.pinMode(14, OUTPUT);           //  U12 A0 Адрес MCP1    
		mcp_upr_adr1.pinMode(15, OUTPUT);           //  

		mcp_upr_adr1.digitalWrite(5, LOW);            //  U14  A0  Адрес Коммутатора 1Ware
		mcp_upr_adr1.digitalWrite(6, LOW);            //  U15  A0  Адрес Коммутатора 1Ware

		mcp_upr_adr1.digitalWrite(8, HIGH );            //  U1 A0  Адрес MCP1
		mcp_upr_adr1.digitalWrite(9, LOW);            //  U2 A0  Адрес MCP1 
		mcp_upr_adr1.digitalWrite(10, LOW);           //  U3 A0  Адрес MCP1  
		mcp_upr_adr1.digitalWrite(11, LOW);           //  U4 A0  Адрес MCP1  
		mcp_upr_adr1.digitalWrite(12, LOW);           //  U5 A0  Адрес MCP1 
		mcp_upr_adr1.digitalWrite(13, LOW);           //  U6 A0  Адрес MCP1   
		mcp_upr_adr1.digitalWrite(14, LOW);           //  U12 A0 Адрес MCP1    
		mcp_upr_adr1.digitalWrite(15, LOW);           //  

		mcp_upr_adr1.digitalWrite(8, HIGH);            //  U1 A0  Адрес MCP1
		mcp_windows1.begin(7);   
		for(int i=0;i<16;i++)
		{
			mcp_windows1.pinMode(i, OUTPUT);            //  Окно №1 открыть
			mcp_windows1.digitalWrite(i, LOW);    
		}
		mcp_upr_adr1.digitalWrite(8, LOW);            //  U1 A0  Адрес MCP1

		mcp_upr_adr1.digitalWrite(9, HIGH);            //  U1 A0  Адрес MCP1
		mcp_windows2.begin(7);   
		for(int i=0;i<16;i++)
		{
			mcp_windows2.pinMode(i, OUTPUT);            //  Окно №1 открыть
			mcp_windows2.digitalWrite(i, LOW);    
		}
		mcp_upr_adr1.digitalWrite(9, LOW);              //  U1 A0  Адрес MCP1

		mcp_upr_adr1.digitalWrite(10, HIGH);            //  U1 A0  Адрес MCP1
		mcp_windows3.begin(7);   
		for(int i=0;i<16;i++)
		{
			mcp_windows3.pinMode(i, OUTPUT);            //  Окно №1 открыть
			mcp_windows3.digitalWrite(i, LOW);    
		}
		mcp_upr_adr1.digitalWrite(10, LOW);             //  U1 A0  Адрес MCP1

		mcp_upr_adr1.digitalWrite(11, HIGH);            //  U1 
		mcp_poliv.begin(7);                             //  U4 Управление поливом
		for(int i=0;i<16;i++)
		{
			mcp_poliv.pinMode(i, OUTPUT);            //  Окно №1 открыть
			mcp_poliv.digitalWrite(i, HIGH);    
		}
		mcp_upr_adr1.digitalWrite(11, LOW);             //  U1 A0  Адрес MCP1

		mcp_upr_adr1.digitalWrite(12, HIGH);            //  U1 
		mcp_Rele1.begin(7);                             //  U5  Резервные реле
		for(int i=0;i<16;i++)
		{
			mcp_Rele1.pinMode(i, OUTPUT);               //   
			mcp_Rele1.digitalWrite(i, HIGH);    
		}
		mcp_upr_adr1.digitalWrite(12, LOW);             //  U1 A0  Адрес MCP1

		mcp_upr_adr1.digitalWrite(13, HIGH);            //  U1 A0  Адрес MCP1
		mcp_sda_ext.begin(7);   
		for(int i=0;i<16;i++)
		{
			mcp_sda_ext.pinMode(i, OUTPUT);            //  Окно №1 открыть
			mcp_sda_ext.digitalWrite(i, LOW);    
		}
		mcp_upr_adr1.digitalWrite(13, LOW);             //  U1 A0  Адрес MCP1

		mcp_upr_adr1.digitalWrite(14, HIGH);            //  U1 A0  Адрес MCP1
		mcp_win_sensor1.begin(7);   
		for(int i=0;i<16;i++)
		{
			mcp_win_sensor1.pinMode(i, INPUT);            //  Окно №1 открыть
		}
		mcp_upr_adr1.digitalWrite(14, LOW);             //  U1 A0  Адрес MCP1
/*

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
	
		mcp_windows2.begin(2);                      //  U2  Управление окнами
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


		mcp_windows3.begin(3);                      //  U3  Управление окнами
		mcp_windows3.pinMode(0, OUTPUT);            //  Окно №17 открыть
		mcp_windows3.pinMode(1, OUTPUT);            //  Окно №17 закрыть 
		mcp_windows3.pinMode(2, OUTPUT);            //  Окно №18 открыть
		mcp_windows3.pinMode(3, OUTPUT);            //  Окно №18 закрыть 
		mcp_windows3.pinMode(4, OUTPUT);            //  Окно №19 открыть
		mcp_windows3.pinMode(5, OUTPUT);            //  Окно №19 закрыть
		mcp_windows3.pinMode(6, OUTPUT);            //  Окно №20 открыть
		mcp_windows3.pinMode(7, OUTPUT);            //  Окно №20 закрыть 
 
		mcp_windows3.pinMode(8, OUTPUT);            //  U11 EN1 i2c 1-4
		mcp_windows3.pinMode(9, OUTPUT);            //  U11 EN2 i2c 1-4 
		mcp_windows3.pinMode(10, OUTPUT);           //  U11 EN3 i2c 1-4 
		mcp_windows3.pinMode(11, OUTPUT);           //  U11 EN4 i2c 1-4 
		mcp_windows3.pinMode(12, OUTPUT);           // 
		mcp_windows3.pinMode(13, OUTPUT);           //   
		mcp_windows3.pinMode(14, OUTPUT);           //    
		mcp_windows3.pinMode(15, OUTPUT);           //  

		mcp_poliv.begin(4);                         //  U4 Управление поливом
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

		mcp_Rele1.begin(5);                         //  U5  Резервные реле
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

		mcp_sda_ext.begin(6);                       //  U6  Управление расширением i2c
		mcp_sda_ext.pinMode(0, OUTPUT);             //  U7 EN1 i2c 1-4  
		mcp_sda_ext.pinMode(1, OUTPUT);             //  U7 EN2 i2c 1-4    
		mcp_sda_ext.pinMode(2, OUTPUT);             //  U7 EN3 i2c 1-4    
		mcp_sda_ext.pinMode(3, OUTPUT);             //  U7 EN4 i2c 1-4    
		mcp_sda_ext.pinMode(4, OUTPUT);             //  U8 EN1 i2c 1-4   
		mcp_sda_ext.pinMode(5, OUTPUT);             //  U8 EN2 i2c 1-4    
		mcp_sda_ext.pinMode(6, OUTPUT);             //  U8 EN3 i2c 1-4    
		mcp_sda_ext.pinMode(7, OUTPUT);             //  U8 EN4 i2c 1-4    
  
		mcp_sda_ext.pinMode(8, OUTPUT);             //  U9 EN1 i2c 1-4    
		mcp_sda_ext.pinMode(9, OUTPUT);             //  U9 EN2 i2c 1-4    
		mcp_sda_ext.pinMode(10, OUTPUT);            //  U9 EN3 i2c 1-4    
		mcp_sda_ext.pinMode(11, OUTPUT);            //  U9 EN4 i2c 1-4    
		mcp_sda_ext.pinMode(12, OUTPUT);            //  U10 EN1 i2c 1-4   
		mcp_sda_ext.pinMode(13, OUTPUT);            //  U10 EN2 i2c 1-4      
		mcp_sda_ext.pinMode(14, OUTPUT);            //  U10 EN3 i2c 1-4       
		mcp_sda_ext.pinMode(15, OUTPUT);            //  U10 EN4 i2c 1-4     

		mcp_win_sensor1.begin(12);                  //  U12 Концевые датчики состояния окон
		mcp_win_sensor1.pinMode(0, INPUT);          //  Окно №1 датчик концевой
		mcp_win_sensor1.pinMode(1, INPUT);          //  Окно №2 датчик концевой  
		mcp_win_sensor1.pinMode(2, INPUT);          //  
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
		*/

}



void setup()  
{  
	Serial.begin(115200); 
//	setup_mcp();
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
	myGLCD.setColor(255, 255, 255);
	myGLCD.setBackColor(0, 0, 0);
}  
   
void loop() 
{    
 AnalogClock();
}  