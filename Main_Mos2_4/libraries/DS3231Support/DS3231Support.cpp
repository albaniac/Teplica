/*
  DS3231.cpp - Arduino/chipKit library support for the DS3231 I2C Real-Time Clock
  Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
  
  This library has been made to easily interface and use the DS3231 RTC with
  an Arduino or chipKit.

  You can find the latest version of the library at 
  http://www.RinkyDinkElectronics.com/

  This library is free software; you can redistribute it and/or
  modify it under the terms of the CC BY-NC-SA 3.0 license.
  Please see the included documents for further information.

  Commercial use of this library requires you to buy a license that
  will allow commercial use. This includes using the library,
  modified or not, as a tool to sell products.

  The license applies to all part of the library including the 
  examples and tools supplied with the library.
*/
#include "DS3231Support.h"
#include <Arduino.h>
//#include "AbstractModule.h"

// Include hardware-specific functions for the correct MCU
#if defined(__AVR__)
	#include "hardware/avr/HW_AVR.h"
#elif defined(__PIC32MX__)
  #include "hardware/pic32/HW_PIC32.h"
#elif defined(__arm__)
	#include "hardware/arm/HW_ARM.h"
#endif

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
//DS3231Time DS3231Clock::getTime()
//{
//	DS3231Time t;
//
//	//Wire.beginTransmission(DS3231Address);
//	//DS3231_WIRE_WRITE(0); // �������, ��� �� ���������� ������ � �������� 0
//
//	//if (Wire.endTransmission() != 0) // ������
//	//	return t;
//
//	//if (Wire.requestFrom(DS3231Address, 7) == 7) // ������ 7 ����, ������� � �������� 0
//	//{
//	//	t.second = bcd2dec(DS3231_WIRE_READ() & 0x7F);
//	//	t.minute = bcd2dec(DS3231_WIRE_READ());
//	//	t.hour = bcd2dec(DS3231_WIRE_READ() & 0x3F);
//	//	t.dayOfWeek = bcd2dec(DS3231_WIRE_READ());
//	//	t.dayOfMonth = bcd2dec(DS3231_WIRE_READ());
//	//	t.month = bcd2dec(DS3231_WIRE_READ());
//	//	t.year = bcd2dec(DS3231_WIRE_READ());
//	//	t.year += 2000; // �������� ����� � ����������� �������
//	//} // if
//
//	return t;
//}






//DS3231Time::DS3231Time()
//{
//	this->year = 2014;
//	this->month = 1;
//	this->dayOfMonth = 1;
//	this->hour = 0;
//	this->minute = 0;
//	this->second = 0;
//	this->dayOfWeek = 3;
//}

DS3231Clock::DS3231Clock(uint8_t data_pin, uint8_t sclk_pin)
{
	_sda_pin = data_pin;
	_scl_pin = sclk_pin;
}

DS3231Time DS3231Clock::getTime()
{
	DS3231Time t;
	_burstRead();
	t.second = _decode(_burstArray[0]);
	t.minute = _decode(_burstArray[1]);
	t.hour	= _decodeH(_burstArray[2]);
	t.dayOfWeek = _burstArray[3];
	t.dayOfMonth = _decode(_burstArray[4]);
	t.month = _decode(_burstArray[5]);
	t.year	= _decodeY(_burstArray[6])+2000;
	return t;
}

void DS3231Clock::setTime(uint8_t hour, uint8_t min, uint8_t sec)
{
	if (((hour>=0) && (hour<24)) && ((min>=0) && (min<60)) && ((sec>=0) && (sec<60)))
	{
		_writeRegister(REG_HOUR, _encode(hour));
		_writeRegister(REG_MIN, _encode(min));
		_writeRegister(REG_SEC, _encode(sec));
	}
}

void DS3231Clock::setDate(uint8_t date, uint8_t mon, uint16_t year)
{
	if (((date>0) && (date<=31)) && ((mon>0) && (mon<=12)) && ((year>=2000) && (year<3000)))
	{
		year -= 2000;
		_writeRegister(REG_YEAR, _encode(year));
		_writeRegister(REG_MON, _encode(mon));
		_writeRegister(REG_DATE, _encode(date));
	}
}

void DS3231Clock::setDOW()
{
	int dow;
	byte mArr[12] = {6,2,2,5,0,3,5,1,4,6,2,4};
	DS3231Time _t = getTime();
  
	dow = (_t.year % 100);
	dow = dow*1.25;
	dow += _t.dayOfMonth;
	dow += mArr[_t.month -1];
	if (((_t.year % 4)==0) && (_t.month<3))
		dow -= 1;
	while (dow>7)
		dow -= 7;
	_writeRegister(REG_DOW, dow);
}

void DS3231Clock::setDOW(uint8_t dow)
{
	if ((dow>0) && (dow<8))
		_writeRegister(REG_DOW, dow);
}



const char* DS3231Clock::getTimeStr(const DS3231Time& t)
{
	char* writePtr = workBuff;

	if (t.hour < 10)
		*writePtr++ = '0';
	else
		*writePtr++ = (t.hour / 10) + '0';

	*writePtr++ = (t.hour % 10) + '0';

	*writePtr++ = ':';

	if (t.minute < 10)
		*writePtr++ = '0';
	else
		*writePtr++ = (t.minute / 10) + '0';

	*writePtr++ = (t.minute % 10) + '0';

	*writePtr++ = ':';

	if (t.second < 10)
		*writePtr++ = '0';
	else
		*writePtr++ = (t.second / 10) + '0';

	*writePtr++ = (t.second % 10) + '0';

	*writePtr = 0;

	return workBuff;
}


const char* DS3231Clock::getDateStr(const DS3231Time& t)
{
	char* writePtr = workBuff;
	if (t.dayOfMonth < 10)
		*writePtr++ = '0';
	else
		*writePtr++ = (t.dayOfMonth / 10) + '0';
	*writePtr++ = (t.dayOfMonth % 10) + '0';

	*writePtr++ = '.';

	if (t.month < 10)
		*writePtr++ = '0';
	else
		*writePtr++ = (t.month / 10) + '0';
	*writePtr++ = (t.month % 10) + '0';

	*writePtr++ = '.';

	*writePtr++ = (t.year / 1000) + '0';
	*writePtr++ = (t.year % 1000) / 100 + '0';
	*writePtr++ = (t.year % 100) / 10 + '0';
	*writePtr++ = (t.year % 10) + '0';

	*writePtr = 0;

	return workBuff;
}

const char* DS3231Clock::getDayOfWeekStr(const DS3231Time& t)
{
	static const char* dow[] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
	return dow[t.dayOfWeek - 1];
}






//
//char *DS3231Clock::getTimeStr(uint8_t format)
//{
//	static char output[] = "xxxxxxxx";
//	DS3231Time t;
//	t=getTime();
//	if (t.hour<10)
//		output[0]=48;
//	else
//		output[0]=char((t.hour / 10)+48);
//	output[1]=char((t.hour % 10)+48);
//	output[2]=58;
//	if (t.minute<10)
//		output[3]=48;
//	else
//		output[3]=char((t.minute / 10)+48);
//	output[4]=char((t.minute % 10)+48);
//	output[5]=58;
//	if (format==FORMAT_SHORT)
//		output[5]=0;
//	else
//	{
//	if (t.sec<10)
//		output[6]=48;
//	else
//		output[6]=char((t.sec / 10)+48);
//	output[7]=char((t.sec % 10)+48);
//	output[8]=0;
//	}
//	return (char*)&output;
//}

//char *DS3231Clock::getDateStr(uint8_t slformat, uint8_t eformat, char divider)
//{
//	static char output[] = "xxxxxxxxxx";
//	int yr, offset;
//	DS3231Time t;
//	t=getTime();
//	switch (eformat)
//	{
//		case FORMAT_LITTLEENDIAN:
//			if (t.date<10)
//				output[0]=48;
//			else
//				output[0]=char((t.date / 10)+48);
//			output[1]=char((t.date % 10)+48);
//			output[2]=divider;
//			if (t.mon<10)
//				output[3]=48;
//			else
//				output[3]=char((t.mon / 10)+48);
//			output[4]=char((t.mon % 10)+48);
//			output[5]=divider;
//			if (slformat==FORMAT_SHORT)
//			{
//				yr=t.year-2000;
//				if (yr<10)
//					output[6]=48;
//				else
//					output[6]=char((yr / 10)+48);
//				output[7]=char((yr % 10)+48);
//				output[8]=0;
//			}
//			else
//			{
//				yr=t.year;
//				output[6]=char((yr / 1000)+48);
//				output[7]=char(((yr % 1000) / 100)+48);
//				output[8]=char(((yr % 100) / 10)+48);
//				output[9]=char((yr % 10)+48);
//				output[10]=0;
//			}
//			break;
//		case FORMAT_BIGENDIAN:
//			if (slformat==FORMAT_SHORT)
//				offset=0;
//			else
//				offset=2;
//			if (slformat==FORMAT_SHORT)
//			{
//				yr=t.year-2000;
//				if (yr<10)
//					output[0]=48;
//				else
//					output[0]=char((yr / 10)+48);
//				output[1]=char((yr % 10)+48);
//				output[2]=divider;
//			}
//			else
//			{
//				yr=t.year;
//				output[0]=char((yr / 1000)+48);
//				output[1]=char(((yr % 1000) / 100)+48);
//				output[2]=char(((yr % 100) / 10)+48);
//				output[3]=char((yr % 10)+48);
//				output[4]=divider;
//			}
//			if (t.mon<10)
//				output[3+offset]=48;
//			else
//				output[3+offset]=char((t.mon / 10)+48);
//			output[4+offset]=char((t.mon % 10)+48);
//			output[5+offset]=divider;
//			if (t.date<10)
//				output[6+offset]=48;
//			else
//				output[6+offset]=char((t.date / 10)+48);
//			output[7+offset]=char((t.date % 10)+48);
//			output[8+offset]=0;
//			break;
//		case FORMAT_MIDDLEENDIAN:
//			if (t.mon<10)
//				output[0]=48;
//			else
//				output[0]=char((t.mon / 10)+48);
//			output[1]=char((t.mon % 10)+48);
//			output[2]=divider;
//			if (t.date<10)
//				output[3]=48;
//			else
//				output[3]=char((t.date / 10)+48);
//			output[4]=char((t.date % 10)+48);
//			output[5]=divider;
//			if (slformat==FORMAT_SHORT)
//			{
//				yr=t.year-2000;
//				if (yr<10)
//					output[6]=48;
//				else
//					output[6]=char((yr / 10)+48);
//				output[7]=char((yr % 10)+48);
//				output[8]=0;
//			}
//			else
//			{
//				yr=t.year;
//				output[6]=char((yr / 1000)+48);
//				output[7]=char(((yr % 1000) / 100)+48);
//				output[8]=char(((yr % 100) / 10)+48);
//				output[9]=char((yr % 10)+48);
//				output[10]=0;
//			}
//			break;
//	}
//	return (char*)&output;
//}
//
//char *DS3231Clock::getDOWStr(uint8_t format)
//{
//	char *output = "xxxxxxxxxx";
//	char *daysLong[]  = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
//	char *daysShort[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
//	DS3231Time t;
//	t=getTime();
//	if (format == FORMAT_SHORT)
//		output = daysShort[t.dayOfWeek -1];
//	else
//		output = daysLong[t.dayOfWeek -1];
//	return output;
//}

char *DS3231Clock::getMonthStr(uint8_t format)
{
	char *output= "xxxxxxxxx";
	char *monthLong[]  = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
	char *monthShort[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	DS3231Time t;
	t=getTime();
	if (format == FORMAT_SHORT)
		output = monthShort[t.month -1];
	else
		output = monthLong[t.month -1];
	return output;
}

long DS3231Clock::getUnixTime(DS3231Time t)
{
	uint16_t	dc;

	dc = t.dayOfMonth; // ���� ������ (0-31);
	for (uint8_t i = 0; i<(t.month -1); i++)
		dc += dim[i];
	if ((t.month > 2) && (((t.year-2000) % 4) == 0))
		++dc;
	dc = dc + (365 * (t.year-2000)) + (((t.year-2000) + 3) / 4) - 1;

	return ((((((dc * 24L) + t.hour) * 60) + t.minute) * 60) + t.second) + SEC_1970_TO_2000;

}

void DS3231Clock::enable32KHz(bool enable)
{
  uint8_t _reg = _readRegister(REG_STATUS);
  _reg &= ~(1 << 3);
  _reg |= (enable << 3);
  _writeRegister(REG_STATUS, _reg);
}

void DS3231Clock::setOutput(byte enable)
{
  uint8_t _reg = _readRegister(REG_CON);
  _reg &= ~(1 << 2);
  _reg |= (enable << 2);
  _writeRegister(REG_CON, _reg);
}

void DS3231Clock::setSQWRate(int rate)
{
  uint8_t _reg = _readRegister(REG_CON);
  _reg &= ~(3 << 3);
  _reg |= (rate << 3);
  _writeRegister(REG_CON, _reg);
}

float DS3231Clock::getTemp()
{
	uint8_t _msb = _readRegister(REG_TEMPM);
	uint8_t _lsb = _readRegister(REG_TEMPL);
	return (float)_msb + ((_lsb >> 6) * 0.25f);
}

/* Private */

void DS3231Clock::_sendStart(byte addr)
{
	pinMode(_sda_pin, OUTPUT);
	digitalWrite(_sda_pin, HIGH);
	digitalWrite(_scl_pin, HIGH);
	digitalWrite(_sda_pin, LOW);
	digitalWrite(_scl_pin, LOW);
	shiftOut(_sda_pin, _scl_pin, MSBFIRST, addr);
}

void DS3231Clock::_sendStop()
{
	pinMode(_sda_pin, OUTPUT);
	digitalWrite(_sda_pin, LOW);
	digitalWrite(_scl_pin, HIGH);
	digitalWrite(_sda_pin, HIGH);
	pinMode(_sda_pin, INPUT);
}

void	DS3231Clock::_sendNack()
{
	pinMode(_sda_pin, OUTPUT);
	digitalWrite(_scl_pin, LOW);
	digitalWrite(_sda_pin, HIGH);
	digitalWrite(_scl_pin, HIGH);
	digitalWrite(_scl_pin, LOW);
	pinMode(_sda_pin, INPUT);
}

void DS3231Clock::_sendAck()
{
	pinMode(_sda_pin, OUTPUT);
	digitalWrite(_scl_pin, LOW);
	digitalWrite(_sda_pin, LOW);
	digitalWrite(_scl_pin, HIGH);
	digitalWrite(_scl_pin, LOW);
	pinMode(_sda_pin, INPUT);
}

void	DS3231Clock::_waitForAck()
{
	pinMode(_sda_pin, INPUT);
	digitalWrite(_scl_pin, HIGH);
	while (digitalRead(_sda_pin)==HIGH) {}
	digitalWrite(_scl_pin, LOW);
}

uint8_t DS3231Clock::_readByte()
{
	pinMode(_sda_pin, INPUT);

	uint8_t value = 0;
	uint8_t currentBit = 0;

	for (int i = 0; i < 8; ++i)
	{
		digitalWrite(_scl_pin, HIGH);
		currentBit = digitalRead(_sda_pin);
		value |= (currentBit << 7-i);
		delayMicroseconds(1);
		digitalWrite(_scl_pin, LOW);
	}
	return value;
}

void DS3231Clock::_writeByte(uint8_t value)
{
	pinMode(_sda_pin, OUTPUT);
	shiftOut(_sda_pin, _scl_pin, MSBFIRST, value);
}

uint8_t	DS3231Clock::_decode(uint8_t value)
{
	uint8_t decoded = value & 127;
	decoded = (decoded & 15) + 10 * ((decoded & (15 << 4)) >> 4);
	return decoded;
}

uint8_t DS3231Clock::_decodeH(uint8_t value)
{
  if (value & 128)
    value = (value & 15) + (12 * ((value & 32) >> 5));
  else
    value = (value & 15) + (10 * ((value & 48) >> 4));
  return value;
}

uint8_t	DS3231Clock::_decodeY(uint8_t value)
{
	uint8_t decoded = (value & 15) + 10 * ((value & (15 << 4)) >> 4);
	return decoded;
}

uint8_t DS3231Clock::_encode(uint8_t value)
{
	uint8_t encoded = ((value / 10) << 4) + (value % 10);
	return encoded;
}

//void DS3231Clock::begin()
//{
//
//
//}