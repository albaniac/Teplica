#pragma once
//--------------------------------------------------------------------------------------------------------------------------------------
#ifdef USE_INTERNAL_CLOCK

//  #pragma message "Use internal clock..."
  // Includes Atmel CMSIS
  #include <chip.h>
  
  #define SUPC_KEY     0xA5u
  #define RESET_VALUE  0x01210720
  
  #define RC           0
  #define  XTAL         1

#else
  //#pragma message "Use DS3231 clock..."
#include <Wire.h>
#endif
#include "AbstractModule.h"
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
}; 
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

  private:

    uint8_t dec2bcd(uint8_t val);
    uint8_t bcd2dec(uint8_t val);

    static char workBuff[12]; // буфер под дату/время

    TwoWire* wireInterface;
  
  public:
    RealtimeClock();

    void setTime(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek, uint8_t dayOfMonth, uint8_t month, uint16_t year);
    void setTime(const RTCTime& time);

    const char* getDayOfWeekStr(const RTCTime& t);
    const char* getTimeStr(const RTCTime& t);
    const char* getDateStr(const RTCTime& t);

    RTCTime getTime();

    Temperature getTemperature();
 
    void begin(uint8_t wireNumber);
};
//--------------------------------------------------------------------------------------------------------------------------------------
#endif

