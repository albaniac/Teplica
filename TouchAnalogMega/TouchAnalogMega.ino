
#include <UTouchCD.h>
#include <UTouch.h>
#include <UTFT.h>

UTFT    myGLCD(TFT01_70,38,39,40,41);
extern uint8_t BigFont[];

extern uint8_t SmallFont[];
UTouch      myTouch(6,5,4,3,2);

int x, y, xi, yi;
char stCurrent[20]="";
int stCurrentLen=0;
char stLast[20]="";

/*************************
**   Custom functions   **
*************************/

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

// Draw a red frame while a button is touched
void waitForIt(int x1, int y1, int x2, int y2)
{
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  while (myTouch.dataAvailable())
    myTouch.read();
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
}

/*************************
**  Required functions  **
*************************/







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


void setup()  
{  
  Serial.begin(115200);  
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);
  xi = 490;
  yi = 200;
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(0, 0, 255);
  drawButtons();  
  test_display();
 }  
   
void loop() 
{    
	 while (true)
  {
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();

	  myGLCD.setColor(255, 255, 255);
      myGLCD.setBackColor(0, 0, 0);
      myGLCD.print("            ", CENTER, 310);
      myGLCD.printNumI(x, CENTER, 310);
      myGLCD.print("            ", CENTER, 330);
      myGLCD.printNumI(y, CENTER, 330); 




      
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