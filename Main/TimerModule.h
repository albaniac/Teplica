#ifndef _TIMER_MODULE_H
#define _TIMER_MODULE_H

#include "AbstractModule.h"
//--------------------------------------------------------------------------------------------------------------------------------
#define NUM_TIMERS 4 // кол-во таймеров
//--------------------------------------------------------------------------------------------------------------------------------
// структура таймера
typedef struct
{
  byte DayMaskAndEnable; // младший бит - понедельник, старший бит - флаг активности таймера
  byte Pin; // номер пина, на который таймер будет выдавать сигнал
  uint16_t HoldOnTime; // сколько времени держать сигнал "включено", в секундах
  uint16_t HoldOffTime; // сколько времени держать сигнал "выключено", в секундах
  byte reserved[4]; // на будущее, знаем мы всякие хотелки :)
  
} PeriodicTimerSettings;
//--------------------------------------------------------------------------------------------------------------------------------
// класс таймера
class PeriodicTimer
{
 public:
  PeriodicTimer();
  
  PeriodicTimerSettings Settings; // настройки таймера

  void Update(uint16_t dt); // обновляет таймер

  bool IsActive(); // возвращает true, если таймер активен
  void Init(); // инициализирует таймер

  void On();
  void Off();

private:

  unsigned long tTimer; // таймер последнего обновления
  bool isHoldOnTimer; // если true - то ждём истечения периода включения, иначе - истечение периода выключения
  byte lastPinState; 
};
//--------------------------------------------------------------------------------------------------------------------------------
class TimerModule : public AbstractModule // модуль таймеров
{
  private:

  void LoadTimers();
  void SaveTimers();

  PeriodicTimer timers[NUM_TIMERS]; // наши таймеры
  
  public:
    TimerModule() : AbstractModule("TMR") {}

    bool ExecCommand(const Command& command, bool wantAnswer);
    void Setup();
    void Update(uint16_t dt);

};


#endif
