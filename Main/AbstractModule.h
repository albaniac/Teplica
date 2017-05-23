#ifndef _ABSTRACT_MODULE_H
#define _ABSTRACT_MODULE_H

#include <WString.h>

class ModuleController; // forward declaration

#include "Globals.h"
#include "CommandParser.h"
#include "TinyVector.h"
#include "MCP23017.h"


// структура для публикации
struct PublishStruct
{
  bool Status;                  // Статус ответа на запрос: false - ошибка, true - нет ошибки
  String Text;                  // текстовое сообщение о публикации, общий для всех буфер
  bool AddModuleIDToAnswer;     // добавлять ли имя модуля в ответ?
  void* Data;                   // любая информация, в зависимости от типа модуля
  bool Busy;                    // флаг, что структура занята для записи

  void Reset()
  {
    Status = false;
    Text = F("");
    AddModuleIDToAnswer = true;
    Data = NULL;
    Busy = false;
  }

  PublishStruct& operator=(const String& src);
  PublishStruct& operator=(const char* src);
  PublishStruct& operator=(char src);
  PublishStruct& operator=(const __FlashStringHelper *src);
  PublishStruct& operator=(unsigned long src);
  PublishStruct& operator=(int src);
  PublishStruct& operator=(long src);

  PublishStruct& operator<<(const String& src);
  PublishStruct& operator<<(const char* src);
  PublishStruct& operator<<(char src);
  PublishStruct& operator<<(const __FlashStringHelper *src);
  PublishStruct& operator<<(unsigned long src);
  PublishStruct& operator<<(unsigned int src);
  PublishStruct& operator<<(int src);
  PublishStruct& operator<<(long src);
  
};

struct Temperature // структура показаний с датчика температуры
{
  int8_t Value; // значение градусов (-128 - 127)
  uint8_t Fract; // сотые доли градуса (значение после запятой)

  bool operator!=(const Temperature& rhs)
  {
    return !(Value == rhs.Value && Fract == rhs.Fract);
  }

  bool operator==(const Temperature& rhs)
  {
    return (Value == rhs.Value && Fract == rhs.Fract);
  }

  bool HasData() // есть ли данные с датчика
  {
    return Value != NO_TEMPERATURE_DATA;
  }

  operator String() const; // возвращаем значение температуры как строку

  Temperature(const Temperature& rhs)
  {
    Value = rhs.Value;
    Fract = rhs.Fract;
  }

  Temperature& operator=(const Temperature& rhs)
  {
    if(this == &rhs)
      return *this;
      
    Value = rhs.Value;
    Fract = rhs.Fract;
    return *this;
  }

  friend Temperature operator-(const Temperature& left, const Temperature& right); // оператор получения дельты температур

  Temperature();
  Temperature(int8_t v,uint8_t f) : Value(v), Fract(f) {}
};

// влажность у нас может храниться так же, как и температура, поэтому
// незачем плодить вторую структуру - просто делаем typedef.
typedef struct Temperature Humidity; 


typedef enum
{
  StateUnknown = 0, // неизвестное состояние
  StateTemperature = 1, // есть температурные датчики
 // StateDummyState = 2, // запас :)
  StateLuminosity = 4, // есть датчики освещенности
  StateHumidity = 8, // есть датчики влажности
  StateWaterFlowInstant = 16, // есть датчик мгновенного расхода воды
  StateWaterFlowIncremental = 32, // есть датчик постоянного расхода воды
  StateSoilMoisture = 64, // есть датчик влажности почвы
  StatePH = 128 // есть датчики pH

} ModuleStates; // вид состояния

struct TemperaturePair
{
  Temperature Prev;
  Temperature Current;  
  TemperaturePair(const Temperature& p, const Temperature& c) : Prev(p), Current(c) {}

  private:
      TemperaturePair();
      TemperaturePair operator=(const TemperaturePair&);

};
struct HumidityPair
{
  const Humidity Prev;
  const Humidity Current;
  
  HumidityPair(const Humidity& p, const Humidity& c) : Prev(p), Current(c) {}

  private:
    HumidityPair();
    HumidityPair& operator=(const HumidityPair&);
};

struct LuminosityPair
{
  long Prev;
  long Current;

  LuminosityPair(long p, long c) : Prev(p), Current(c) {}

  private:
    LuminosityPair();
    LuminosityPair& operator=(const LuminosityPair&);
};

struct WaterFlowPair
{
  unsigned long Prev;
  unsigned long Current;

  WaterFlowPair(unsigned long p, unsigned long c) : Prev(p), Current(c) {}

  private:
    WaterFlowPair();
    WaterFlowPair& operator=(const WaterFlowPair&);
};

class OneState
{
    ModuleStates Type; // тип состояния (температура, освещенность, каналы реле)
    
    uint8_t Index; // индекс (например, датчика температуры)
    void* Data; // данные с датчика
    void* PreviousData; // предыдущие данные с датчика

    public:

    static ModuleStates GetType(const String& stringType);
    static ModuleStates GetType(const char* stringType);
    static String GetStringType(ModuleStates type);

    String GetUnit(); // возвращает единицы измерения состояния в виде строки

    uint8_t GetIndex() {return Index;}
    ModuleStates GetType() {return Type;}
    
    void Update(void* newData); // обновляет состояние
    bool IsChanged(); // тестирует, есть ли изменения
    bool HasData(); // проверяет, есть ли данные от датчика
    uint8_t GetRawData(byte* outBuffer); // копирует сырые данные в выходной буфер, возвращает размер скопированных данных 

    OneState& operator=(const OneState& rhs); // копирует состояние из одной структуры в другую, если структуры одинаковых типов, индексы при этом остаются нетронутыми

    friend OneState operator-(const OneState& left, const OneState& right); // оператор получения дельты состояний, индексы игнорируются, типы - должны быть одинаковыми

    operator String(); // для удобства вывода информации
    operator TemperaturePair(); // получает температуру в виде пары предыдущее/текущее изменение
    operator HumidityPair(); // получает влажность в виде пары предыдущее/текущее изменение
    operator LuminosityPair(); // получает состояние освещенности в виде пары предыдущее/текущее изменение
    operator WaterFlowPair(); // получает значения расхода воды в виде пары предыдущее/текущее изменение
    
    OneState(ModuleStates s, uint8_t idx)
    {
      Init(s,idx);
    }
    ~OneState();

    private:

    OneState();
    OneState(const OneState& rhs);
    void Init(ModuleStates type, uint8_t idx); // инициализирует состояние
    
};

typedef Vector<OneState*> StateVec;

class ModuleState
{
 uint8_t supportedStates; // какие состояния поддерживаем?
 StateVec states; // какие состояния поддерживаем?

public:
  ModuleState();

  bool HasState(ModuleStates state); // проверяет, поддерживаются ли такие состояния?
  bool HasChanges(); // проверяет, есть ли изменения во внутреннем состоянии модуля?
  
  OneState* AddState(ModuleStates state, uint8_t sensorIndex); // добавляем датчик и привязываем его к индексу
  void UpdateState(ModuleStates state, uint8_t sensorIndex, void* newData); // обновляем состояние модуля (например, показания с температурных датчиков);
  
  uint8_t GetStateCount(ModuleStates state); // возвращает кол-во датчиков определённого вида (не даёт информации об индексах датчиков!)
  OneState* GetState(ModuleStates state, uint8_t sensorIndex); // возвращает состояние определённого вида по индексу датчика
  OneState* GetStateByOrder(ModuleStates state, uint8_t orderNum); // возвращает состояние определённого вида по номеру его хранения в массиве
  
  void RemoveState(ModuleStates state, uint8_t sensorIndex); // удаляет состояние по индексу датчика

 
};


class AbstractModule
{
  private:
    const char* moduleID;    
    
protected:

public:

  AbstractModule(const char* id) : moduleID(id)
  { 

  }

  ModuleState State; // текущее состояние модуля
 
  const char* GetID() {return moduleID;}
  void SetID(const char* id) {moduleID = id;}

  // функции, перегружаемые наследниками
  virtual bool ExecCommand(const Command& command, bool wantAnswer) = 0; // вызывается при приходе текстовой команды для модуля (wantAnswer - ждут ли от нас текстового ответа) 
  virtual void Setup() = 0; // вызывается для настроек модуля
  virtual void Update(uint16_t dt) = 0; // обновляет состояние модуля (для поддержки состояния периферии, например, включение диода)
  
};

// абстрактный класс резервирования датчиков
class ReservationResolver
{
  public:
    // возвращает первое попавшееся состояние с данными, основываясь на списках резервирования для указанного типа
    // датчиков. Параметр sourceModule - содержит модуль с датчиком, с которого нет показаний.
    // В списках резервирования ищется датчик, привязанный к этому модулю и с индексом sensorIndex, соответствующий
    // типу sensorType. Если такой датчик найден, то из списка резервирования возвращается первое состояние,
    // для которого есть данные.
    virtual OneState* GetReservedState(AbstractModule* sourceModule, ModuleStates sensorType, uint8_t sensorIndex) = 0;
};

typedef struct
{
  unsigned long WindowsState; // состояние каналов окон, 4 байта = 32 бита = 16 окон)
  byte WaterChannelsState; // состояние каналов полива, 1 байт, (8 каналов)
  byte LightChannelsState; // состояние каналов досветки, 1 байт (8 каналов)
  byte PinsState[16]; // состояние пинов, 16 байт, 128 пинов
  
} ControllerState; // состояние контроллера

#define PINS_MAP_SIZE 10 // размер поля информации о пинах

typedef struct  
{
  byte PinsUsed[PINS_MAP_SIZE]; // N бит информации по занятости пина. Если бит с номером пина выставлен в 1 - этот пин используется
  byte PinsMode[PINS_MAP_SIZE]; // если в бите с номером пина 1 - то пин настроен на выход, иначе - на вход. Имеет значение только, если соответствующий пин используется
   
} UsedPinsInfo; // состояние занятости пинов

class WorkStatus
{
  uint8_t statuses[STATUSES_BYTES];
  uint8_t lastStatuses[STATUSES_BYTES];

  void CopyStatusModes();
  void CopyStatusMode(uint8_t bitNum);
  bool IsStatusModeChanged(uint8_t bitNum);

  static byte MakeNum(char symbol);

  ControllerState State;

  public:
  
    void SetStatus(uint8_t bitNum, bool bOn);
    void WriteStatus(Stream* pStream, bool bAsTextHex);
    bool GetStatus(uint8_t bitNum);
    bool IsModeChanged();
    void SetModeUnchanged();
    WorkStatus();
	void setup_mcp();

  static const char* ToHex(int i);
  static byte FromHex(const char* buff);

  UsedPinsInfo UsedPins; // массив информации о занятых пинах

  // устанавливает режим работы пина, попутно записывая в карту занятых пинов.
  // если последний парааметр равен false - то с пином ничего не делается, просто его режим копируется в карту занятости.
  void PinMode(byte pinNumber,byte mode, bool setMode=true); 
  void PinWrite(byte pin, byte level); // пишет в пин состояние, заодно копируя его в слепок состояния контроллера
 // void PinWrite(byte pin, byte level); // !!! Переделать для записи в MCP23017


  void SaveWindowState(byte channel, byte state);
  void SaveWaterChannelState(byte channel, byte state);
  void SaveLightChannelState(byte channel, byte state);

  ControllerState& GetState()
  {
    return State;
  }
  
}; // структура статусов работы 

extern WorkStatus WORK_STATUS; // статус состояния

extern char SD_BUFFER[SD_BUFFER_LENGTH];

#endif
