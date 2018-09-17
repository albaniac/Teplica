 #include "RTCSupport.h"
 #include <Arduino.h>
 #include "AbstractModule.h"
#include "HW_ARM.h"
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
#define REG_SEC		0x00
#define REG_MIN		0x01
#define REG_HOUR	0x02
#define REG_DOW		0x03
#define REG_DATE	0x04
#define REG_MON		0x05
#define REG_YEAR	0x06
#define REG_CON		0x0e
#define REG_STATUS	0x0f
#define REG_AGING	0x10
#define REG_TEMPM	0x11
#define REG_TEMPL	0x12

#define SEC_1970_TO_2000 946684800

static const uint8_t dim[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

/* Public */
//
//RTCTime::Time()
//{
//	this->year = 2014;
//	this->mon = 1;
//	this->date = 1;
//	this->hour = 0;
//	this->min = 0;
//	this->sec = 0;
//	this->dow = 3;
//}

//--------------------------------------------------------------------------------------------------------------------------------------


RealtimeClock::RealtimeClock() //RealtimeClock()
{
	//_sda_pin = data_pin;
	//_scl_pin = sclk_pin;
    //wireInterface = &Wire;
}
//--------------------------------------------------------------------------------------------------------------------------------------



//
////--------------------------------------------------------------------------------------------------------------------------------------
//uint8_t RealtimeClock::dec2bcd(uint8_t val)
//{
//  return( (val/10*16) + (val%10) );
//}
////--------------------------------------------------------------------------------------------------------------------------------------
//uint8_t RealtimeClock::bcd2dec(uint8_t val)
//{
//  return( (val/16*10) + (val%16) );
//}
////--------------------------------------------------------------------------------------------------------------------------------------
void RealtimeClock::setTime(const RTCTime& time)
{
  setTime(time.second, time.minute, time.hour, time.dayOfWeek, time.dayOfMonth, time.month,time.year);
}
//--------------------------------------------------------------------------------------------------------------------------------------
void RealtimeClock::setTime(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek, uint8_t dayOfMonth, uint8_t month, uint16_t year)
{

  //while(year > 100) // приводим к диапазону 0-99
  //  year -= 100;
  

  if (((hour >= 0) && (hour < 24)) && ((minute >= 0) && (minute < 60)) && ((second >= 0) && (second < 60))&& ((dayOfMonth > 0) && (dayOfMonth <= 31)) && ((month > 0) && (month <= 12)) && ((year >= 2000) && (year < 3000)))
  {
	  year -= 2000;
	  _writeRegister(REG_HOUR, _encode(hour));
	  _writeRegister(REG_MIN, _encode(minute));
	  _writeRegister(REG_SEC, _encode(second));
	  _writeRegister(REG_YEAR, _encode(year));
	  _writeRegister(REG_MON, _encode(month));
	  _writeRegister(REG_DATE, _encode(dayOfMonth));
	  setDOW(dayOfWeek);
  }

    
  //wireInterface->beginTransmission(DS3231Address);
  //
  //wireInterface->write(0); // указываем, что начинаем писать с регистра секунд
  //wireInterface->write(dec2bcd(second)); // пишем секунды
  //wireInterface->write(dec2bcd(minute)); // пишем минуты
  //wireInterface->write(dec2bcd(hour)); // пишем часы
  //wireInterface->write(dec2bcd(dayOfWeek)); // пишем день недели
  //wireInterface->write(dec2bcd(dayOfMonth)); // пишем дату
  //wireInterface->write(dec2bcd(month)); // пишем месяц
  //wireInterface->write(dec2bcd(year)); // пишем год
  //
  //wireInterface->endTransmission(); 

  delay(10); // немного подождём для надёжности
}
//--------------------------------------------------------------------------------------------------------------------------------------
void RealtimeClock::setDate(uint8_t date, uint8_t mon, uint16_t year)
{
	if (((date > 0) && (date <= 31)) && ((mon > 0) && (mon <= 12)) && ((year >= 2000) && (year < 3000)))
	{
		year -= 2000;
		_writeRegister(REG_YEAR, _encode(year));
		_writeRegister(REG_MON, _encode(mon));
		_writeRegister(REG_DATE, _encode(date));
	}
}


void RealtimeClock::setDOW()
{
	int dow;
	byte mArr[12] = { 6,2,2,5,0,3,5,1,4,6,2,4 };
	RTCTime _t = getTime();

	dow = (_t.year % 100);
	dow = dow * 1.25;
	dow += _t.dayOfMonth;
	dow += mArr[_t.month - 1];
	if (((_t.year % 4) == 0) && (_t.month < 3))
		dow -= 1;
	while (dow > 7)
		dow -= 7;
	_writeRegister(REG_DOW, dow);
}

void RealtimeClock::setDOW(uint8_t dow)
{
	if ((dow > 0) && (dow < 8))
		_writeRegister(REG_DOW, dow);
}


Temperature RealtimeClock::getTemperature()
{
 Temperature res;

 //uint8_t _msb = _readRegister(REG_TEMPM);
 //uint8_t _lsb = _readRegister(REG_TEMPL);

 union int16_byte {
      int i;
      byte b[2];
  } rtcTemp;
    

   rtcTemp.b[1] = _readRegister(REG_TEMPM);
   rtcTemp.b[0] = _readRegister(REG_TEMPL);

    long tempC100 = (rtcTemp.i >> 6) * 25;

    res.Value = tempC100/100;
    res.Fract = abs(tempC100 % 100);



 //return (float)_msb + ((_lsb >> 6) * 0.25f);
  
 //union int16_byte {
 //      int i;
 //      byte b[2];
 //  } rtcTemp;
 //    
 // wireInterface->beginTransmission(DS3231Address);
 // wireInterface->write(0x11);
 // if(wireInterface->endTransmission() != 0) // ошибка
 //   return res;

 // if(wireInterface->requestFrom(DS3231Address, 2) == 2)
 // {
 //   rtcTemp.b[1] = wireInterface->read();
 //   rtcTemp.b[0] = wireInterface->read();

 //   long tempC100 = (rtcTemp.i >> 6) * 25;

 //   res.Value = tempC100/100;
 //   res.Fract = abs(tempC100 % 100);
 //   
 // }
 // 
  return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------
RTCTime RealtimeClock::getTime()
{
  RTCTime t;
    
  _burstRead();
  t.second = _decode(_burstArray[0]);
  t.minute = _decode(_burstArray[1]);
  t.hour = _decodeH(_burstArray[2]);
  t.dayOfWeek = _burstArray[3];
  t.dayOfMonth = _decode(_burstArray[4]);
  t.month = _decode(_burstArray[5]);
  t.year = _decodeY(_burstArray[6]) + 2000;
  return t;




  //wireInterface->beginTransmission(DS3231Address);
  //wireInterface->write(0); // говорим, что мы собираемся читать с регистра 0
  //
  //if(wireInterface->endTransmission() != 0) // ошибка
  //  return t;
  //
  //if(wireInterface->requestFrom(DS3231Address, 7) == 7) // читаем 7 байт, начиная с регистра 0
  //{
  //    t.second = bcd2dec(wireInterface->read() & 0x7F);
  //    t.minute = bcd2dec(wireInterface->read());
  //    t.hour = bcd2dec(wireInterface->read() & 0x3F);
  //    t.dayOfWeek = bcd2dec(wireInterface->read());
  //    t.dayOfMonth = bcd2dec(wireInterface->read());
  //    t.month = bcd2dec(wireInterface->read());
  //    t.year = bcd2dec(wireInterface->read());     
  //    t.year += 2000; // приводим время к нормальному формату
  //} // if
  //
 // return t;
}
//--------------------------------------------------------------------------------------------------------------------------------------
const char* RealtimeClock::getDayOfWeekStr(const RTCTime& t)
{
	//char *output = "xxxxxxxxxx";
	//char *daysLong[] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
	//char *daysShort[] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
	//RTCTime t;
	//t = getTime();
	//if (format == FORMAT_SHORT)
	//	output = daysShort[t.dow - 1];
	//else
	//	output = daysLong[t.dow - 1];
	//return output;

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
//void RealtimeClock::begin(uint8_t wireNumber)
//{
//  #if TARGET_BOARD == MEGA_BOARD
//    wireInterface = &Wire;
//  #else
//	if (wireNumber == 1)
//	{
//		_sda_pin = SDA1;// , SCLdata_pin;
//		_scl_pin = SCL1;// sclk_pin;
//	}
//	else
//	{
//		_sda_pin = SDA;// , SCLdata_pin;
//		_scl_pin = SCL;// sclk_pin;
//	}
//  #endif
//}
//--------------------------------------------------------------------------------------------------------------------------------------

/* Private */

void RealtimeClock::_sendStart(byte addr)
{
	pinMode(_sda_pin, OUTPUT);
	digitalWrite(_sda_pin, HIGH);
	digitalWrite(_scl_pin, HIGH);
	digitalWrite(_sda_pin, LOW);
	digitalWrite(_scl_pin, LOW);
	shiftOut(_sda_pin, _scl_pin, MSBFIRST, addr);
}

void RealtimeClock::_sendStop()
{
	pinMode(_sda_pin, OUTPUT);
	digitalWrite(_sda_pin, LOW);
	digitalWrite(_scl_pin, HIGH);
	digitalWrite(_sda_pin, HIGH);
	pinMode(_sda_pin, INPUT);
}

void RealtimeClock::_sendNack()
{
	pinMode(_sda_pin, OUTPUT);
	digitalWrite(_scl_pin, LOW);
	digitalWrite(_sda_pin, HIGH);
	digitalWrite(_scl_pin, HIGH);
	digitalWrite(_scl_pin, LOW);
	pinMode(_sda_pin, INPUT);
}

void RealtimeClock::_sendAck()
{
	pinMode(_sda_pin, OUTPUT);
	digitalWrite(_scl_pin, LOW);
	digitalWrite(_sda_pin, LOW);
	digitalWrite(_scl_pin, HIGH);
	digitalWrite(_scl_pin, LOW);
	pinMode(_sda_pin, INPUT);
}

void RealtimeClock::_waitForAck()
{
	pinMode(_sda_pin, INPUT);
	digitalWrite(_scl_pin, HIGH);
	while (digitalRead(_sda_pin) == HIGH) {}
	digitalWrite(_scl_pin, LOW);
}

uint8_t RealtimeClock::_readByte()
{
	pinMode(_sda_pin, INPUT);

	uint8_t value = 0;
	uint8_t currentBit = 0;

	for (int i = 0; i < 8; ++i)
	{
		digitalWrite(_scl_pin, HIGH);
		currentBit = digitalRead(_sda_pin);
		value |= (currentBit << 7 - i);
		delayMicroseconds(1);
		digitalWrite(_scl_pin, LOW);
	}
	return value;
}

void RealtimeClock::_writeByte(uint8_t value)
{
	pinMode(_sda_pin, OUTPUT);
	shiftOut(_sda_pin, _scl_pin, MSBFIRST, value);
}

uint8_t	RealtimeClock::_decode(uint8_t value)
{
	uint8_t decoded = value & 127;
	decoded = (decoded & 15) + 10 * ((decoded & (15 << 4)) >> 4);
	return decoded;
}

uint8_t RealtimeClock::_decodeH(uint8_t value)
{
	if (value & 128)
		value = (value & 15) + (12 * ((value & 32) >> 5));
	else
		value = (value & 15) + (10 * ((value & 48) >> 4));
	return value;
}

uint8_t	RealtimeClock::_decodeY(uint8_t value)
{
	uint8_t decoded = (value & 15) + 10 * ((value & (15 << 4)) >> 4);
	return decoded;
}

uint8_t RealtimeClock::_encode(uint8_t value)
{
	uint8_t encoded = ((value / 10) << 4) + (value % 10);
	return encoded;
}


#endif
