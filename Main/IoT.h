#ifndef _IOT_H
#define _IOT_H

#include <Arduino.h>
#include <WString.h>
#include "TinyVector.h"
#include "Globals.h"


// классы  и структуры для доступа во внешние IoT-хранилища, типа ThingSpeak

// тип сервиса, для понимания - через какой шлюз на какой сервис отослали
typedef enum
{
  iotThingSpeak // пока поддерживаем только ThingSpeak
  
} IoTService;

typedef Vector<IoTService> IoTServices;

typedef struct // структура результатов вызова
{
  bool success; // отправлены ли данные ?
  IoTService service; // и через какой сервис
  
} IoTCallResult;

// интерфейсы обратного вызова
typedef void (*IOT_OnWriteToStream)(Stream* writeTo); // вызывается, когда в поток можно писать, в этом обработчике можно писать в поток данные длиной dataLength
typedef void (*IOT_OnSendDataDone)(const IoTCallResult& result); // вызывается по окончанию отправки данных на внешний шлюз

// интерфейс доступа к шлюзу
struct IoTGate
{
  // функция отсыла данных во внешний шлюз
  // dataLength - длина отправляемых данных, в байтах ( МАКСИМУМ 2048 байт!!!! )
  // writer вызывается, когда в поток можно писать, в этом обработчике вызвавшая сторона
  // пишет в переданный поток.
  // onDone вызывается, когда обработка завершена, в параметр обработчика приходят результаты вызова
  virtual void SendData(IoTService service,uint16_t dataLength, IOT_OnWriteToStream writer, IOT_OnSendDataDone onDone) = 0;
};

typedef Vector<IoTGate*> IoTGates;

class IoTListClass // список шлюзов в IoT
{
private:

    IoTGates gates; // наши шлюзы, которые регистрируют себя при старте контроллера здесь

public:

   IoTListClass();
  //virtual 
  ~IoTListClass();


   void RegisterGate(IoTGate* gate);
   size_t GetGatesCount();
   IoTGate* GetGate(size_t index);
  
};

#ifdef USE_IOT_MODULE
extern IoTListClass IoTList;
#endif

#endif

