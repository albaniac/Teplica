#pragma once
//--------------------------------------------------------------------------------------------------------------------------------------
#ifdef USE_INTERNAL_CLOCK

  #pragma message "Use internal clock..."
  // Includes Atmel CMSIS
  #include <chip.h>
  
  #define SUPC_KEY     0xA5u
  #define RESET_VALUE  0x01210720
  
  #define RC           0
  #define  XTAL         1

#else
#include "Arduino.h"
#include "HW_ARM_defines.h"
//
//  #pragma message "Use DS3231 clock..."
//#include <Wire.h>
#endif
#include "AbstractModule.h"
//--------------------------------------------------------------------------------------------------------------------------------------

#define DS3231_ADDR_R	0xD1
#define DS3231_ADDR_W	0xD0
#define DS3231_ADDR		0x68

#define FORMAT_SHORT	1
#define FORMAT_LONG		2

#define FORMAT_LITTLEENDIAN	1
#define FORMAT_BIGENDIAN	2
#define FORMAT_MIDDLEENDIAN	3

#define MONDAY		1
#define TUESDAY		2
#define WEDNESDAY	3
#define THURSDAY	4
#define FRIDAY		5
#define SATURDAY	6
#define SUNDAY		7

#define SQW_RATE_1		0
#define SQW_RATE_1K		1
#define SQW_RATE_4K		2
#define SQW_RATE_8K		3

#define OUTPUT_SQW		0
#define OUTPUT_INT		1


//--------------------------------------------------------------------------------------------------------------------------------------

struct RTCTime // данные по текущему времени
{
  uint8_t second; // секунда (0-59)
  uint8_t minute; // минута (0-59)
  uint8_t hour; // час (0-23)
  uint8_t dayOfWeek; // день недели (1 - понедельник и т.д.)
  uint8_t dayOfMonth; // день месяца (0-31)
  uint8_t month; // месяц(1-12)
  uint16_t year; // формат - ХХХХ
 // RTCTime();
}; 

//struct RTCTime // данные по текущему времени
//{
//	uint8_t second; // секунда (0-59)
//	uint8_t minute; // минута (0-59)
//	uint8_t hour; // час (0-23)
//	uint8_t dayOfWeek; // день недели (1 - понедельник и т.д.)
//	uint8_t dayOfMonth; // день месяца (0-31)
//	uint8_t month; // месяц(1-12)
//	uint16_t year; // формат - ХХХХ
//};
//


//
//class Time
//{
//public:
//	uint8_t		hour;
//	uint8_t		min;
//	uint8_t		sec;
//	uint8_t		date;
//	uint8_t		mon;
//	uint16_t	year;
//	uint8_t		dow;
//
//	Time();
//};

//--------------------------------------------------------------------------------------------------------------------------------------
#ifdef USE_INTERNAL_CLOCK
//--------------------------------------------------------------------------------------------------------------------------------------
class RealtimeClock
{
  public:
    RealtimeClock();
    void setTime(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek, uint8_t dayOfMonth, uint8_t month, uint16_t year);
    void setTime(const RTCTime& time);


    const char* getDayOfWeekStr(const RTCTime& t);
    const char* getTimeStr(const RTCTime& t);
    const char* getDateStr(const RTCTime& t);

    RTCTime getTime();

    Temperature getTemperature();
 
    void begin(uint8_t src);
        
  private:
    static char workBuff[12]; // буфер под дату/время
    
  
};
//--------------------------------------------------------------------------------------------------------------------------------------
#else
//--------------------------------------------------------------------------------------------------------------------------------------
enum { DS3231Address = 0x68 }; // адрес датчика
//--------------------------------------------------------------------------------------------------------------------------------------
class RealtimeClock
{
public:
	RealtimeClock();
	//void	begin();
	//Time	getTime();
	//void	setTime(uint8_t hour, uint8_t min, uint8_t sec);
	void	setDate(uint8_t date, uint8_t mon, uint16_t year);
	void	setDOW();
	void	setDOW(uint8_t dow);

	char	*getTimeStr(uint8_t format = FORMAT_LONG);
	char	*getDateStr(uint8_t slformat = FORMAT_LONG, uint8_t eformat = FORMAT_LITTLEENDIAN, char divider = '.');
	char	*getDOWStr(uint8_t format = FORMAT_LONG);
	char	*getMonthStr(uint8_t format = FORMAT_LONG);
	long	getUnixTime(RTCTime t);

	void	enable32KHz(bool enable);
	void	setOutput(byte enable);
	void	setSQWRate(int rate);
	float	getTemp();

	//  RealtimeClock();

    void setTime(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek, uint8_t dayOfMonth, uint8_t month, uint16_t year);
    void setTime(const RTCTime& time);

    const char* getDayOfWeekStr(const RTCTime& t);
    const char* getTimeStr(const RTCTime& t);
    const char* getDateStr(const RTCTime& t);

    RTCTime getTime();

    Temperature getTemperature();

    void begin(uint8_t wireNumber);
	

private:
	uint8_t _scl_pin;
	uint8_t _sda_pin;
	uint8_t _burstArray[7];
	boolean	_use_hw;

	void	_sendStart(byte addr);
	void	_sendStop();
	void	_sendAck();
	void	_sendNack();
	void	_waitForAck();
	uint8_t	_readByte();
	void	_writeByte(uint8_t value);
	void	_burstRead();
	uint8_t	_readRegister(uint8_t reg);
	void 	_writeRegister(uint8_t reg, uint8_t value);
	uint8_t	_decode(uint8_t value);
	uint8_t	_decodeH(uint8_t value);
	uint8_t	_decodeY(uint8_t value);
	uint8_t	_encode(uint8_t vaule);

	Twi		*twi;
	static char workBuff[12]; // буфер под дату/время

  //private:

  //  uint8_t dec2bcd(uint8_t val);
  //  uint8_t bcd2dec(uint8_t val);

  //  static char workBuff[12]; // буфер под дату/время

  //  TwoWire* wireInterface;
  //
  //public:
  //  RealtimeClock();

  //  void setTime(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek, uint8_t dayOfMonth, uint8_t month, uint16_t year);
  //  void setTime(const RTCTime& time);

  //  const char* getDayOfWeekStr(const RTCTime& t);
  //  const char* getTimeStr(const RTCTime& t);
  //  const char* getDateStr(const RTCTime& t);

  //  RTCTime getTime();

  //  Temperature getTemperature();
 
//    void begin(uint8_t wireNumber);
};
//--------------------------------------------------------------------------------------------------------------------------------------
#endif

