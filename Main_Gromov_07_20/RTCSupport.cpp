 #include "RTCSupport.h"
 #include <Arduino.h>
 #include "AbstractModule.h"
//--------------------------------------------------------------------------------------------------------------------------------------
#ifdef USE_INTERNAL_CLOCK
//--------------------------------------------------------------------------------------------------------------------------------------
char RealtimeClock::workBuff[12] = {0};
//--------------------------------------------------------------------------------------------------------------------------------------
RealtimeClock::RealtimeClock()
{
  
}
//--------------------------------------------------------------------------------------------------------------------------------------
void RealtimeClock::begin(uint8_t src)
{
  if (src) 
  {
    pmc_switch_sclk_to_32kxtal(0);
  
  while (!pmc_osc_is_ready_32kxtal());
  }

  RTC_SetHourMode(RTC, 0);
  
  NVIC_DisableIRQ(RTC_IRQn);
  NVIC_ClearPendingIRQ(RTC_IRQn);
  NVIC_SetPriority(RTC_IRQn, 0);
    
}
//--------------------------------------------------------------------------------------------------------------------------------------
RTCTime RealtimeClock::getTime()
{
  RTCTime result;
  
  RTC_GetTime(RTC, (uint8_t*)&(result.hour), (uint8_t*)&(result.minute), (uint8_t*)&(result.second));
  RTC_GetDate(RTC, (uint16_t*)&(result.year), (uint8_t*)&(result.month), (uint8_t*)&(result.dayOfMonth), (uint8_t*)&(result.dayOfWeek));

  return result;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void RealtimeClock::setTime(const RTCTime& time)
{
  setTime(time.second, time.minute, time.hour, time.dayOfWeek, time.dayOfMonth, time.month,time.year);
}
//--------------------------------------------------------------------------------------------------------------------------------------
void RealtimeClock::setTime(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek, uint8_t dayOfMonth, uint8_t month, uint16_t year)
{
  RTC_SetTime (RTC, hour, minute, second);  
  RTC_SetDate (RTC, year, month, dayOfMonth, dayOfWeek);  
}
//--------------------------------------------------------------------------------------------------------------------------------------
Temperature RealtimeClock::getTemperature()
{
 Temperature res;
 
 return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------
const char* RealtimeClock::getDayOfWeekStr(const RTCTime& t)
{
  static const char* dow[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
  return dow[t.dayOfWeek-1];
}
//--------------------------------------------------------------------------------------------------------------------------------------
const char* RealtimeClock::getTimeStr(const RTCTime& t)
{
  char* writePtr = workBuff;
  
  if(t.hour < 10)
    *writePtr++ = '0';
  else
    *writePtr++ = (t.hour/10) + '0';

  *writePtr++ = (t.hour % 10) + '0';

 *writePtr++ = ':';

 if(t.minute < 10)
  *writePtr++ = '0';
 else
  *writePtr++ = (t.minute/10) + '0';

 *writePtr++ = (t.minute % 10) + '0';

 *writePtr++ = ':';

 if(t.second < 10)
  *writePtr++ = '0';
 else
  *writePtr++ = (t.second/10) + '0';

 *writePtr++ = (t.second % 10) + '0';

 *writePtr = 0;

 return workBuff;
}
//--------------------------------------------------------------------------------------------------------------------------------------
const char* RealtimeClock::getDateStr(const RTCTime& t)
{
  char* writePtr = workBuff;
  if(t.dayOfMonth < 10)
    *writePtr++ = '0';
  else
    *writePtr++ = (t.dayOfMonth/10) + '0';
  *writePtr++ = (t.dayOfMonth % 10) + '0';

  *writePtr++ = '.';

  if(t.month < 10)
    *writePtr++ = '0';
  else
    *writePtr++ = (t.month/10) + '0';
  *writePtr++ = (t.month % 10) + '0';

  *writePtr++ = '.';

  *writePtr++ = (t.year/1000) + '0';
  *writePtr++ = (t.year % 1000)/100 + '0';
  *writePtr++ = (t.year % 100)/10 + '0';
  *writePtr++ = (t.year % 10) + '0';  

  *writePtr = 0;

  return workBuff;
}
//--------------------------------------------------------------------------------------------------------------------------------------
#else
//--------------------------------------------------------------------------------------------------------------------------------------
char RealtimeClock::workBuff[12] = {0};
//--------------------------------------------------------------------------------------------------------------------------------------
RealtimeClock::RealtimeClock()
{
    wireInterface = &Wire;
}
//--------------------------------------------------------------------------------------------------------------------------------------
uint8_t RealtimeClock::dec2bcd(uint8_t val)
{
  return( (val/10*16) + (val%10) );
}
//--------------------------------------------------------------------------------------------------------------------------------------
uint8_t RealtimeClock::bcd2dec(uint8_t val)
{
  return( (val/16*10) + (val%16) );
}
//--------------------------------------------------------------------------------------------------------------------------------------
void RealtimeClock::setTime(const RTCTime& time)
{
  setTime(time.second, time.minute, time.hour, time.dayOfWeek, time.dayOfMonth, time.month,time.year);
}
//--------------------------------------------------------------------------------------------------------------------------------------
void RealtimeClock::setTime(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek, uint8_t dayOfMonth, uint8_t month, uint16_t year)
{

  while(year > 100) // приводим к диапазону 0-99
    year -= 100;
 
  wireInterface->beginTransmission(DS3231Address);
  
  wireInterface->write(0); // указываем, что начинаем писать с регистра секунд
  wireInterface->write(dec2bcd(second)); // пишем секунды
  wireInterface->write(dec2bcd(minute)); // пишем минуты
  wireInterface->write(dec2bcd(hour)); // пишем часы
  wireInterface->write(dec2bcd(dayOfWeek)); // пишем день недели
  wireInterface->write(dec2bcd(dayOfMonth)); // пишем дату
  wireInterface->write(dec2bcd(month)); // пишем месяц
  wireInterface->write(dec2bcd(year)); // пишем год
  
  wireInterface->endTransmission();

  delay(10); // немного подождём для надёжности
}
//--------------------------------------------------------------------------------------------------------------------------------------
Temperature RealtimeClock::getTemperature()
{
 Temperature res;
  
 union int16_byte {
       int i;
       byte b[2];
   } rtcTemp;
     
  wireInterface->beginTransmission(DS3231Address);
  wireInterface->write(0x11);
  if(wireInterface->endTransmission() != 0) // ошибка
    return res;

  if(wireInterface->requestFrom(DS3231Address, 2) == 2)
  {
    rtcTemp.b[1] = wireInterface->read();
    rtcTemp.b[0] = wireInterface->read();

    long tempC100 = (rtcTemp.i >> 6) * 25;

    res.Value = tempC100/100;
    res.Fract = abs(tempC100 % 100);
    
  }
  
  return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------
RTCTime RealtimeClock::getTime()
{
  RTCTime t;

  wireInterface->beginTransmission(DS3231Address);
  wireInterface->write(0); // говорим, что мы собираемся читать с регистра 0
  
  if(wireInterface->endTransmission() != 0) // ошибка
    return t;
  
  if(wireInterface->requestFrom(DS3231Address, 7) == 7) // читаем 7 байт, начиная с регистра 0
  {
      t.second = bcd2dec(wireInterface->read() & 0x7F);
      t.minute = bcd2dec(wireInterface->read());
      t.hour = bcd2dec(wireInterface->read() & 0x3F);
      t.dayOfWeek = bcd2dec(wireInterface->read());
      t.dayOfMonth = bcd2dec(wireInterface->read());
      t.month = bcd2dec(wireInterface->read());
      t.year = bcd2dec(wireInterface->read());     
      t.year += 2000; // приводим время к нормальному формату
  } // if
  
  return t;
}
//--------------------------------------------------------------------------------------------------------------------------------------
const char* RealtimeClock::getDayOfWeekStr(const RTCTime& t)
{
  static const char* dow[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
  return dow[t.dayOfWeek-1];
}
/*
//--------------------------------------------------------------------------------------------------------------------------------------
char* RealtimeClock::getMonthStr(const RTCTime& t)
{
  static const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  return months[t.month-1];
}
*/
//--------------------------------------------------------------------------------------------------------------------------------------
const char* RealtimeClock::getTimeStr(const RTCTime& t)
{
  char* writePtr = workBuff;
  
  if(t.hour < 10)
    *writePtr++ = '0';
  else
    *writePtr++ = (t.hour/10) + '0';

  *writePtr++ = (t.hour % 10) + '0';

 *writePtr++ = ':';

 if(t.minute < 10)
  *writePtr++ = '0';
 else
  *writePtr++ = (t.minute/10) + '0';

 *writePtr++ = (t.minute % 10) + '0';

 *writePtr++ = ':';

 if(t.second < 10)
  *writePtr++ = '0';
 else
  *writePtr++ = (t.second/10) + '0';

 *writePtr++ = (t.second % 10) + '0';

 *writePtr = 0;

 return workBuff;
}
//--------------------------------------------------------------------------------------------------------------------------------------
const char* RealtimeClock::getDateStr(const RTCTime& t)
{
  char* writePtr = workBuff;
  if(t.dayOfMonth < 10)
    *writePtr++ = '0';
  else
    *writePtr++ = (t.dayOfMonth/10) + '0';
  *writePtr++ = (t.dayOfMonth % 10) + '0';

  *writePtr++ = '.';

  if(t.month < 10)
    *writePtr++ = '0';
  else
    *writePtr++ = (t.month/10) + '0';
  *writePtr++ = (t.month % 10) + '0';

  *writePtr++ = '.';

  *writePtr++ = (t.year/1000) + '0';
  *writePtr++ = (t.year % 1000)/100 + '0';
  *writePtr++ = (t.year % 100)/10 + '0';
  *writePtr++ = (t.year % 10) + '0';  

  *writePtr = 0;

  return workBuff;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void RealtimeClock::begin(uint8_t wireNumber)
{
  #if TARGET_BOARD == MEGA_BOARD
    wireInterface = &Wire;
  #else
    if(wireNumber == 1)
      wireInterface = &Wire1;
    else
      wireInterface = &Wire;
  #endif
}
//--------------------------------------------------------------------------------------------------------------------------------------
#endif
