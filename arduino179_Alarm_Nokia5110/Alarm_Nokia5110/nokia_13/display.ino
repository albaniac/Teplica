// процедура вывода времени
void displaytoptime()
  {
    display.setTextColor(BLACK);   // цвет
    display.setCursor(0,0);   // 
    display.print(print2digits1(tm.Day));
    display.print('/');
    display.print(print2digits1(tm.Month));
    display.print('/');
    display.print(tmYearToCalendar(tm.Year)-2000);
    display.print(' ');
    display.print(print2digits1(tm.Hour));
    display.print(':');
    display.print(print2digits1(tm.Minute));
  }
// процедура вывода будильников
void displayalarms()
  {
  for(int i=1;i<4;i++)
    {
    if(i==tekalarm)  // белое на черном
      {
      display.fillRect(0,13+10*(i-1),84,10,1);
      display.setTextColor(WHITE);   // цвет
      }
    else
      {
      display.setTextColor(BLACK);   // цвет
      }
    if(alarmok[i-1]>0)  // будильник сработал
      {
      if(blink2==1)
         display.setTextColor(WHITE);
      else
         display.setTextColor(BLACK);
      display.setCursor(5,15+10*(i-1));   // 
      display.print("**:**:**");
      display.print(' ');
      display.print("****");
      }
   /*else if(alarms[i-1][0]==1 && alarms[i-1][4]==1)  // активный будильник и repeat
      {  
      display.setCursor(5,15+10*(i-1));   // 
      getdeltat(i-1);
      display.print(print2digits1(delta[2]));
      display.print(':');
      display.print(print2digits1(delta[1]));
      display.print(':');
      display.print(print2digits1(delta[0]));
      display.print(' ');
      display.print(print4digits1(delta[3]));
      }*/
    else if(alarms[i-1][0]==1 && alarmnotoverdue(i-1))  // активный будильник и не просрочен
      {  
      display.setCursor(5,15+10*(i-1));   // 
      getdeltat(i-1);
      display.print(print2digits1(delta[2]));
      display.print(':');
      display.print(print2digits1(delta[1]));
      display.print(':');
      display.print(print2digits1(delta[0]));
      display.print(' ');
      display.print(print4digits1(delta[3]));
      }
    else  // будильник отключен
      {  
      display.setCursor(5,15+10*(i-1));   // 
      display.print("--");
      display.print(':');
      display.print("--");
      display.print(':');
      display.print("--");
      display.print(' ');
      display.print("----");
      }
    delta[i-1]=24*60*60-(alarms[i-1][1]-tm.Hour)*60*60+(alarms[i-1][2]-tm.Minute)*60+(alarms[i-1][3]-tm.Second);
    if(DEBUG==1)
      {
      //Serial.print("delta");Serial.print(i);Serial.print("=");
      //Serial.println(delta[i]);
      }
    }
  }
// редактирование будильника
void displayeditalarm(int alarm)
  {
    // название будильника
    display.setTextColor(BLACK);
    display.setCursor(5,0);
    display.print("alarm");
    display.print(tekalarm);
    // вкл/выкл
    //display.setCursor(55,0);
    if(blink1==1 && editparam==0)
      display.drawBitmap(60, 0,  nullalarm, 8, 8, 1);
    else if(alarms[tekalarm-1][0]==1)
      display.drawBitmap(60, 0,  picalarm, 8, 8, 1);
    else if(alarms[tekalarm-1][0]==0)
      display.drawBitmap(60, 0,  noalarm, 8, 8, 1);
    // время
    display.setCursor(15,15);
    setcolortext(1);
    display.print(print2digits1(alarms[tekalarm-1][1]));
    display.setTextColor(BLACK);display.print(':');
    setcolortext(2);
    display.print(print2digits1(alarms[tekalarm-1][2]));
    display.setTextColor(BLACK);display.print(':');
    setcolortext(3);
    display.print(print2digits1(alarms[tekalarm-1][3]));
    // разовый/повтор
    display.setCursor(20,25);
    setcolortext(4);
    if(alarms[tekalarm-1][4]==0) display.print(" once ");
    else display.print("repeat");
    //
    if(alarms[tekalarm-1][4]==0)
      {
      display.setCursor(15,35);
      setcolortext(5);
      display.print(print2digits1(alarms[tekalarm-1][5]));
      display.setTextColor(BLACK);display.print('/');
      setcolortext(6);
      display.print(print2digits1(alarms[tekalarm-1][6]));
      display.setTextColor(BLACK);display.print('/');
      setcolortext(7);
      display.print(print2digits1(alarms[tekalarm-1][7]));
      }
    else
      {
      display.setCursor(5,35);
      for(int i=1;i<8;i++)
        {
         if(alarms[tekalarm-1][i+7]==1)
           {
            display.fillRect(4+(i-1)*12,34,8,9,1);
            display.setTextColor(WHITE);
           }
         else
            display.setTextColor(BLACK);
         setcolortext1(i+7);
         display.print(i);
         display.print(' ');
         }
      }
  }
// установка времени
void displaysettime()
  {
    // название будильника
    display.setTextColor(BLACK);
    display.setCursor(15,0);
    display.print("set time");
    // время
    display.setCursor(15,15);
    setcolortext(0);
    display.print(print2digits1(tektime[0]));
    display.setTextColor(BLACK);display.print(':');
    setcolortext(1);
    display.print(print2digits1(tektime[1]));
    display.setTextColor(BLACK);display.print(':');
    setcolortext(2);
    display.print(print2digits1(tektime[2]));
    // день недели
    display.setCursor(28,25);
    setcolortext(3);
    display.print(weekdaystring[tektime[3]]);
    // дата
    display.setCursor(15,35);
    setcolortext(4);
    display.print(print2digits1(tektime[4]));
    display.setTextColor(BLACK);display.print('/');
    setcolortext(5);
    display.print(print2digits1(tektime[5]));
    display.setTextColor(BLACK);display.print('/');
    setcolortext(6);
    display.print(print2digits1(tektime[6]));
  }
 // установка цвета букв
void setcolortext(int param)
  {
    if(param==editparam && blink1==1)
      display.setTextColor(WHITE);
    else 
      display.setTextColor(BLACK);
  }
  // установка цвета букв
void setcolortext1(int param)
  {
   if(param==editparam && blink1==1)
      display.setTextColor(WHITE);
   else if(param==editparam && blink1==0)
       display.setTextColor(BLACK);
  }
 // проверка не просрочен ли будильник
 boolean alarmnotoverdue(int j)
   {
   if((tmYearToCalendar(tm.Year)-2000)<alarms[j][7])
     return true;
   if((tmYearToCalendar(tm.Year)-2000)<alarms[j][7])
     return false;    
   if(tm.Month<alarms[j][6])
     return true;
   if(tm.Month>alarms[j][6])
     return false;
   if(tm.Day<alarms[j][5])
     return true;
   if(tm.Day>alarms[j][5])
     return false;
   if(tm.Hour<alarms[j][1])
     return true;
   if(tm.Hour>alarms[j][1])
     return false;
   if(tm.Minute<alarms[j][2])
     return true;
   if(tm.Minute>alarms[j][2])
     return false;
   if(tm.Second<alarms[j][3])
     return true;
   return false;       
   }
  // проверка наступил ли будильник
 boolean alarmgo(int j)
   {
   if((tmYearToCalendar(tm.Year)-2000)<alarms[j][7])
     return false;
   if((tmYearToCalendar(tm.Year)-2000)<alarms[j][7])
     return false;    
   if(tm.Month<alarms[j][6])
     return false;
   if(tm.Month>alarms[j][6])
     return false;
   if(tm.Day<alarms[j][5])
     return false;
   if(tm.Day>alarms[j][5])
     return false;
   if(tm.Hour<alarms[j][1])
     return false;
   if(tm.Hour>alarms[j][1])
     return false;
   if(tm.Minute<alarms[j][2])
     return false;
   if(tm.Minute>alarms[j][2])
     return false;
   if(tm.Second<alarms[j][3])
     return false;
   if(tm.Second>alarms[j][3])
     return false;
   return true;
   }
 // получить delta T
 void getdeltat(int j)
   {
   int d1,d2;
   int yeart=tmYearToCalendar(tm.Year)-2000;
   for(int i=0;i<7;i++)
     delta[i]=0;
   if(alarms[j][3]<tm.Second) 
     delta[1]=-1;
   delta[0]=(60+alarms[j][3]-tm.Second)%60;
   if(alarms[j][2]<tm.Minute) 
     delta[2]=-1;
   delta[1]=(delta[1]+60+alarms[j][2]-tm.Minute)%60;
   if(alarms[j][1]<tm.Hour) 
     delta[3]=-1;
   delta[2]=(delta[2]+24+alarms[j][1]-tm.Hour)%24;
   // вычисление дней
   d1=(yeart-15)*13+tm.Month;d2=(alarms[j][7]-15)*13+alarms[j][6];
   //Serial.print(d1);Serial.print(" ");Serial.println(10/12);
   for(int i1=d1;i1<=d2;i1++)
     {
     delta[3]=delta[3]+daysformonth[i1/13][i1%13];
     //Serial.print(delta[3]);Serial.print(" ");Serial.println(daysformonth[i1/12][i1%12]);
     }
   delta[3]=delta[3]-tm.Day;  
   delta[3]=delta[3]-(daysformonth[alarms[j][7]-15][alarms[j][6]]-alarms[j][5]);
   }
