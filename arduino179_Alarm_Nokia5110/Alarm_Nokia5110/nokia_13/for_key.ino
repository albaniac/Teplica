// отработка получения кодов клавиш
void do_for_key(char kod)
 {
 if(mode==TABLO_MODE)
    do_for_key1(kod);
 else if(mode==EDIT_MODE)
    do_for_key2(kod);
 else if(mode==SET_TIME)
    do_for_key3(kod);
 else
    ;
 } 
// режим табло
void do_for_key1(char kod)
 {
 switch(kod)
   {
   case btn_SETTIME:   // установка времени
        mode=SET_TIME;
        tekalarm=0;//editparam=0;
        tektime[0]=tm.Hour;tektime[1]=tm.Minute;tektime[2]=tm.Second;
        tektime[3]=tm.Wday;
        Serial.print(tm.Wday);//Serial.println(tektime[3]);
        tektime[4]=tm.Day;tektime[5]=tm.Month;
        tektime[6]=tmYearToCalendar(tm.Year)-2000;       
        break;  
   case btn_ALARM1:   // выбор будильника 1
        tekalarm=1;
        break;  
   case btn_ALARM2:   // выбор будильника 2
        tekalarm=2;
        break;  
   case btn_ALARM3:   // выбор будильника 3
        tekalarm=3;
        break;  
   case btn_UP:   // предыдущий 
        tekalarm=tekalarm-1;
        if(tekalarm<0) tekalarm=1;
        else if(tekalarm<1) tekalarm=3;
        break;  
   case btn_DOWN:   // следующий
        tekalarm=tekalarm+1;
        if(tekalarm==4) tekalarm=1;
        break;  
   case btn_ESC:   // отмена
        tekalarm=0;
        break;  
   case btn_EDIT:   // редактировать
        if(tekalarm>0) mode=EDIT_MODE;
        editparam=0;
        break;
   case btn_ON:   // включить будильник
        if(tekalarm>0)
          {
          alarms[tekalarm-1][0]=1;
          put_prg_eeprom(tekalarm);
          }
        break;  
   case btn_OFF:   // выключить будильник
        if(tekalarm>0)
          {
          alarms[tekalarm-1][0]=0;
          put_prg_eeprom(tekalarm);
          }
        break;  
   default:
        break;  
   }
 } 
// режим редактирования будильников
void do_for_key2(char kod)
 {
 switch(kod)
   {
   case btn_ALARM1:   // выбор будильника 1
        get_prg_eeprom(tekalarm);
        tekalarm=1;
        break;  
   case btn_ALARM2:   // выбор будильника 2
        get_prg_eeprom(tekalarm);
        tekalarm=2;
        break;  
   case btn_ALARM3:   // выбор будильника 3
        get_prg_eeprom(tekalarm);
        tekalarm=3;
        break;  
   case btn_LEFT:   // предыдущий параметр
        if(editparam==0)
           editparam=correcteditpos[alarms[tekalarm-1][4]][14];
        else   
          editparam=correcteditpos[alarms[tekalarm-1][4]][editparam-1];
        break;  
   case btn_RIGHT:   // следующий параметр
         editparam=correcteditpos[alarms[tekalarm-1][4]][editparam+1];
         break;  
   case btn_UP:   // увеличить значение параметра
        alarms[tekalarm-1][editparam]=max((alarms[tekalarm-1][editparam]+1)%maxedit[editparam],minedit[editparam]);
         control_data();
         break;  
   case btn_DOWN:   // уменьшить значение параметра
        if(alarms[tekalarm-1][editparam]==minedit[editparam])
           alarms[tekalarm-1][editparam]=maxedit[editparam]-1;
        else
          alarms[tekalarm-1][editparam]=alarms[tekalarm-1][editparam]-1;
         control_data();
         break;  
   case btn_OK:   // 
        mode=TABLO_MODE;
        set_next_data();
        put_prg_eeprom(tekalarm);
        break;  
   case btn_ESC:   // 
        get_prg_eeprom(tekalarm);
        mode=TABLO_MODE;
        break;  
   case btn_EDIT:   // 
        get_prg_eeprom(tekalarm);
        mode=TABLO_MODE;
        break;  
   case btn_ON:   // включить будильник
        if(tekalarm>0)
          {
          alarms[tekalarm-1][0]=1;
          }
        break;  
   case btn_OFF:   // выключить будильник
        if(tekalarm>0)
          {
          alarms[tekalarm-1][0]=0;
          }
        break;  
   default:
        break;  
   }
 } 
// режим установки времени
void do_for_key3(char kod)
 {
 switch(kod)
   {
   case btn_SETTIME:   // переход в режим табло
        mode=TABLO_MODE;
        break;  
   case btn_LEFT:   // предыдущий параметр
        if(editparam==0)
           editparam=6;
        else   
          editparam=editparam-1;
        break;  
   case btn_RIGHT:   // следующий параметр
         editparam=(editparam+1)%7;
         break;  
   case btn_UP:   // увеличить значение параметра
        tektime[editparam]=max((tektime[editparam]+1)%maxedit1[editparam],minedit1[editparam]);
        if(DEBUG==1)
          {Serial.print("param=");Serial.println(alarms[tekalarm-1][editparam]);}
         //control_data();
         break;  
   case btn_DOWN:   // уменьшить значение параметра
        if(tektime[editparam]==minedit1[editparam])
           tektime[editparam]=maxedit1[editparam]-1;
        else
          tektime[editparam]=tektime[editparam]-1;
         //control_data();
         break;  
   case btn_OK:   // 
        mode=TABLO_MODE;
        tm.Day=tektime[4];tm.Month=tektime[5];;
        tm.Year=CalendarYrToTm(tektime[6]+2000);
        tm.Hour=tektime[0];tm.Minute=tektime[1];tm.Second=tektime[2]; 
        tm.Wday=tektime[3];
        RTC.write(tm);           
        break;  
   case btn_ESC:   // 
        mode=TABLO_MODE;
        break;  
   default:
        break;  
   }
 } 
//  проверка дата>текущей
void control_data()
  {
  // режим repeat  
  //if(alarms[tekalarm-1][4]==1)
  //  return;
  // 
  int yeart=tmYearToCalendar(tm.Year)-2000;
  if(DEBUG==1)
    {
    //Serial.print("yeart=");Serial.println(yeart);
    //Serial.print("days=");Serial.println(daysformonth[yeart-15][alarms[tekalarm-1][6]]);
    }
  alarms[tekalarm-1][5]=min(daysformonth[yeart-15][alarms[tekalarm-1][6]],alarms[tekalarm-1][5]);
   
  }  
//  установка даты срабатывания будильника
void set_next_data()
  {
  // режим once 
  if(alarms[tekalarm-1][4]==0)
    return;
  // 
  int nextday=0;int countday=0;boolean ok=false;int i=tm.Wday;
  while(ok==false)
      {
      if(alarms[tekalarm-1][8+i%7]==1 && i==tm.Wday && alarm_tek_day())
        {
        ok=true;countday=i-tm.Wday;nextday=i%7+1;
        set_next_day(countday);
        }
      else if(alarms[tekalarm-1][8+i%7]==1 && i>tm.Wday)
        {
        ok=true;countday=i-tm.Wday;nextday=i%7+1;
        set_next_day(countday);
        }
      i++;
      }
  //if(DEBUG==1)
    //{
    //Serial.print("tekweek=");Serial.println(tm.Wday);
    //Serial.print("countday=");Serial.println(countday);
    //Serial.print("nextday=");Serial.println(nextday);
    //}
  }  
// установка следующей даты срабатывания будильника
void set_next_day(int inc)
  {
  alarms[tekalarm-1][5]=tm.Day;
  alarms[tekalarm-1][6]=tm.Month;
  alarms[tekalarm-1][7]=tmYearToCalendar(tm.Year)-2000;
  
  alarms[tekalarm-1][5]=alarms[tekalarm-1][5]+inc;
  
  int yeart=tmYearToCalendar(tm.Year)-2000;
  if(alarms[tekalarm-1][5]<=daysformonth[yeart-15][alarms[tekalarm-1][6]])
    return;
  alarms[tekalarm-1][5]=alarms[tekalarm-1][5]%daysformonth[yeart-15][alarms[tekalarm-1][6]];
  alarms[tekalarm-1][6]=alarms[tekalarm-1][6]+1;
  if(alarms[tekalarm-1][6]<=12)
    return;
  alarms[tekalarm-1][6]=1;
  alarms[tekalarm-1][7]=alarms[tekalarm-1][7]+1;
  
  }
// проверка  установкио срабатывания будильника в текущий день
boolean alarm_tek_day()
  {
  if(alarms[tekalarm-1][1]<tm.Hour)
     return false;
  if(alarms[tekalarm-1][1]==tm.Hour && alarms[tekalarm-1][2]<tm.Minute)
     return false;
  if(alarms[tekalarm-1][1]==tm.Hour && alarms[tekalarm-1][2]==tm.Minute && alarms[tekalarm-1][3]==tm.Second)
     return false;
  return true;
  }
