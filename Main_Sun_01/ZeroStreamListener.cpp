#include "ZeroStreamListener.h"
#include "ModuleController.h"
#ifdef USE_REMOTE_MODULES
#include "RemoteModule.h"
#endif

#include "UniversalSensors.h"
#include "InteropStream.h"
#include "Memory.h"
//-------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef USE_UNIVERSAL_MODULES

  #ifdef USE_UNI_REGISTRATION_LINE
    UniRegistrationLine uniRegistrator(UNI_REGISTRATION_PIN);
  #endif // USE_UNI_REGISTRATION_LINE
  
  #if UNI_WIRED_MODULES_COUNT > 0
    UniPermanentLine uniWiredModules[UNI_WIRED_MODULES_COUNT] = { UNI_WIRED_MODULES };
  #endif

#endif // USE_UNIVERSAL_MODULES

#ifdef USE_NRF_GATE
  UniNRFGate nrfGate;
#endif

#ifdef USE_LORA_GATE
  UniLoRaGate loraGate;
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------------
void(* resetFunc) (void) = 0;
//-------------------------------------------------------------------------------------------------------------------------------------------------------
void ZeroStreamListener::Setup()
{
  // настройка модуля тут
  #ifdef USE_DS3231_REALTIME_CLOCK
    // добавляем температуру часов
    State.AddState(StateTemperature,0);
  #endif

  #ifdef USE_RS485_GATE
    RS485.Setup();
  #endif

#ifdef USE_NRF_GATE
  nrfGate.Setup();
#endif  

#ifdef USE_LORA_GATE
  loraGate.Setup();
#endif  
 }
//-------------------------------------------------------------------------------------------------------------------------------------------------------
void ZeroStreamListener::Update(uint16_t dt)
{
#ifdef USE_UNIVERSAL_MODULES


  #if UNI_WIRED_MODULES_COUNT > 0
    for(uint8_t i=0;i<UNI_WIRED_MODULES_COUNT;i++)
    {
      uniWiredModules[i].Update(dt);
      yield(); // вызываем критически важные операции
    }
  #endif
  
#endif // USE_UNIVERSAL_MODULES

  UNUSED(dt);

 #ifdef USE_DS3231_REALTIME_CLOCK

  // читать чаще, чем раз в 20 секунд - быссмысленно, 
  // внутренняя конверсия температуры у DS3231 происходит
  // каждые 64 секунды.
  static uint16_t dsTimer = 20000;
  dsTimer += dt;
  if(dsTimer > 20000)
  {
    dsTimer = 0;
  // получаем температуру модуля реального времени
    RealtimeClock rtc = MainController->GetClock();
    Temperature t = rtc.getTemperature();

    // convert to Fahrenheit if needed
    #ifdef MEASURE_TEMPERATURES_IN_FAHRENHEIT
     t = Temperature::ConvertToFahrenheit(t);
    #endif      
          
    State.UpdateState(StateTemperature,0,(void*)&t);
    
  }
  #endif 


  #ifdef USE_RS485_GATE
    RS485.Update(dt);
  #endif  

#ifdef USE_NRF_GATE
  nrfGate.Update(dt);
#endif    

#ifdef USE_LORA_GATE
  loraGate.Update(dt);
#endif    

}
//-------------------------------------------------------------------------------------------------------------------------------------------------------
void ZeroStreamListener::PrintSensorsValues(uint8_t totalCount,ModuleStates wantedState,AbstractModule* module, Stream* outStream)
{
  if(!totalCount) // нечего писать
    return;

  // буфер под сырые данные, у нас максимум 4 байта на показание с датчика
  static uint8_t raw_data[sizeof(unsigned long)] = {0};
  const char* noDataByte = "FF"; // байт - нет данных с датчика

  // пишем количество датчиков
  outStream->write(WorkStatus::ToHex(totalCount));

  for(uint8_t cntr=0;cntr<totalCount;cntr++)
  {
    yield(); // немного даём поработать другим модулям
    
    // получаем нужное состояние
    OneState* os = module->State.GetStateByOrder(wantedState,cntr);
    
    // потом идут пакеты данных, каждый пакет состоит из:
    // 1 байт - индекс датчика
    outStream->write(WorkStatus::ToHex(os->GetIndex()));

    // N байт - его показания, мы пишем любые показания, даже если датчика нет на линии

    // копируем сырые данные
    uint8_t rawDataSize = os->GetRawData(raw_data);

    // сырые данные идут от младшего байта к старшему, но их надо слать
    // старшим байтом вперёд.
    
      if(os->HasData())
      {
        do
        {
          rawDataSize--;

          outStream->write(WorkStatus::ToHex(raw_data[rawDataSize]));
          
        } while(rawDataSize > 0);
        
      } // if
      else
      {
        // датчика нет на линии, пишем FF столько раз, сколько байт сырых данных мы получили
        for(uint8_t i=0;i<rawDataSize;i++)
          outStream->write(noDataByte);

      }    
  } // for
  
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------
String ZeroStreamListener::GetGUID(const char* passedGuid)
{
    uint16_t address = GUID_ADDRESS;
    
    byte header1 = MemRead(address++);
    byte header2 = MemRead(address++);

    if(header1 == SETT_HEADER1 && header2 == SETT_HEADER2)
    {
      // есть сохранённые настройки
      String result;
      for(byte i=0;i<32;i++)
        result += (char) MemRead(address++);

        return result;
    }
    else
    {
      // нет сохранённых настроек, пишем GUID в EEPROM и возвращаем его
      address = GUID_ADDRESS;
      MemWrite(address++,SETT_HEADER1);
      MemWrite(address++,SETT_HEADER2);

      const char* ptr = passedGuid;

      for(byte i=0;i<32;i++)
      {
          if(*ptr)
            MemWrite(address++,*ptr++);
          else
            MemWrite(address++,'\0');
      }
      return passedGuid;
    }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------
bool  ZeroStreamListener::ExecCommand(const Command& command, bool wantAnswer)
{
  if(wantAnswer) PublishSingleton = UNKNOWN_COMMAND;

  bool canPublish = true; // флаг, что можем публиковать

   size_t argsCnt = command.GetArgsCount();
  
  if(command.GetType() == ctGET) 
  {
     PublishSingleton = NOT_SUPPORTED;      
    if(!argsCnt) // нет аргументов
    {
      PublishSingleton = PARAMS_MISSED;
    }
    else
    {
      if(argsCnt < 1)
      {
        // мало параметров
        PublishSingleton = PARAMS_MISSED;
        
      } // if
      else
      {
        String t = command.GetArg(0); // получили команду
        t.toUpperCase();
        if(t == PING_COMMAND) // пинг
        {
          PublishSingleton.Flags.Status = true;
          PublishSingleton = PONG;
          PublishSingleton.Flags.AddModuleIDToAnswer = false;
        } // if
        else if(t == F("LIMITS"))
        {
          // получить настойки ограничений
            PublishSingleton.Flags.Status = true;
            PublishSingleton.Flags.AddModuleIDToAnswer = false;
            PublishSingleton = t;
            PublishSingleton << PARAM_DELIMITER;
            PublishSingleton << MAX_ALERT_RULES;
            PublishSingleton << PARAM_DELIMITER;
            PublishSingleton << MAX_DELTAS;
        }
        else if(t == F("TUNIT")) // unit for temperatures ( For query use "CTGET=0|TUNIT", answer is "OK=TUNIT|C" for Celsius, "OK=TUNIT|F" for Fahrenheit )
        {
            PublishSingleton.Flags.Status = true;
            PublishSingleton.Flags.AddModuleIDToAnswer = false;
            PublishSingleton = t;
            PublishSingleton << PARAM_DELIMITER;
            #ifdef MEASURE_TEMPERATURES_IN_FAHRENHEIT
              PublishSingleton << F("F");
            #else
              PublishSingleton << F("C");
            #endif
            
        }
        else if(t == F("PSTATE")) // информация о состоянии пинов
        {
           PublishSingleton.Flags.Status = true;
           PublishSingleton = "";
           PublishSingleton.Flags.AddModuleIDToAnswer = false;

           ControllerState st = WORK_STATUS.GetState();
           for(size_t i=0;i<sizeof(st.PinsState);i++)
           {
              PublishSingleton << WorkStatus::ToHex(st.PinsState[i]);
           }
          
        }
        else if(t == F("GUID"))
        {
           // получить уникальный ID контроллера. Параметром приходит сгенерённый конфигуратором GUID, в формате 32 символа подряд.
           // если сохранённого GUID нет - надо вернуть переданный и сохранить в EEPROM, иначе - вернуть сохранённый.
           if(argsCnt < 2)
           {
              PublishSingleton = PARAMS_MISSED;
           }
           else
           {
              PublishSingleton.Flags.Status = true;
              PublishSingleton = GetGUID(command.GetArg(1));
              PublishSingleton.Flags.AddModuleIDToAnswer = false;
           }
           
        }
         else 
         if (t == F("RFSCAN")) // сканировать канал на занятость
         {
            byte ch = atoi(command.GetArg(1));
            
            #ifdef USE_NRF_GATE
              
              int level = nrfGate.ScanChannel(ch);
              PublishSingleton.Flags.Status = true;
              PublishSingleton = t; 
              PublishSingleton << PARAM_DELIMITER << ch << PARAM_DELIMITER << level;
              
            #else
              PublishSingleton = t; 
              PublishSingleton << PARAM_DELIMITER << ch << PARAM_DELIMITER << NOT_SUPPORTED;
            #endif
         }        
        else
        if(t == UNI_RF_CHANNEL_COMMAND)
        {
          PublishSingleton.Flags.Status = true;
          PublishSingleton = UNI_RF_CHANNEL_COMMAND;
          PublishSingleton << PARAM_DELIMITER;
          PublishSingleton << UniDispatcher.GetRFChannel();
          PublishSingleton.Flags.AddModuleIDToAnswer = false;          
        }
        else if(t == PINS_COMMAND) {
          // получить информацию по пинам
          PublishSingleton.Flags.Status = true;
          PublishSingleton = PINS_COMMAND;
          PublishSingleton << PARAM_DELIMITER;
          PublishSingleton << PINS_MAP_SIZE;
          PublishSingleton << PARAM_DELIMITER;

          for(byte i=0;i<PINS_MAP_SIZE;i++) {
            PublishSingleton << WorkStatus::ToHex(WORK_STATUS.UsedPins.PinsUsed[i]);
          }

          PublishSingleton << PARAM_DELIMITER;

          for(byte i=0;i<PINS_MAP_SIZE;i++) {
            PublishSingleton << WorkStatus::ToHex(WORK_STATUS.UsedPins.PinsMode[i]);
          }          
          
        }
        #if defined(USE_UNIVERSAL_MODULES) && defined(USE_UNI_REGISTRATION_LINE)
        else
        if(t == UNI_SEARCH) // поиск универсального модуля на линии регистрации
        {
          PublishSingleton.Flags.AddModuleIDToAnswer = false;
          
          if(uniRegistrator.IsModulePresent())
          {
            // датчик найден, отправляем его внутреннее состояние
            PublishSingleton.Flags.Status = true;

            UniRawScratchpad scratch;
            uniRegistrator.CopyScratchpad(&scratch);
            byte* raw = (byte*) &scratch;
            
            PublishSingleton = "";
            
            // теперь пишем весь скратчпад вызывающему, пущай сам разбирается, как с ним быть
            for(byte i=0;i<sizeof(UniRawScratchpad);i++)
            {
              PublishSingleton << WorkStatus::ToHex(raw[i]);
            } // for

          } // if
          else
          {
            // датчика нету
            PublishSingleton = UNI_NOT_FOUND;
          } // else
        }
        #endif // USE_UNI_REGISTRATION_LINE
        else
        if(t == ID_COMMAND)
        {
          PublishSingleton.Flags.Status = true;
          PublishSingleton.Flags.AddModuleIDToAnswer = false;
          PublishSingleton = ID_COMMAND; 
          PublishSingleton << PARAM_DELIMITER << MainController->GetSettings()->GetControllerID();
        }
        else
        if(t == WIRED_COMMAND) // получить количество жёстко указанных в прошивке обычных датчиков
        {
          PublishSingleton.Flags.Status = true;
          PublishSingleton.Flags.AddModuleIDToAnswer = false;
          PublishSingleton = WIRED_COMMAND;

          PublishSingleton << PARAM_DELIMITER << UniDispatcher.GetHardCodedSensorsCount(uniTemp);
          PublishSingleton << PARAM_DELIMITER << UniDispatcher.GetHardCodedSensorsCount(uniHumidity);
          PublishSingleton << PARAM_DELIMITER << UniDispatcher.GetHardCodedSensorsCount(uniLuminosity);
          PublishSingleton << PARAM_DELIMITER << UniDispatcher.GetHardCodedSensorsCount(uniSoilMoisture);
          PublishSingleton << PARAM_DELIMITER << UniDispatcher.GetHardCodedSensorsCount(uniPH);
          //TODO: Тут остальные типы датчиков указывать !!!
                     
        }
        else
        if(t == UNI_COUNT_COMMAND) // получить количество зарегистрированных универсальных датчиков
        {
          PublishSingleton.Flags.Status = true;
          PublishSingleton.Flags.AddModuleIDToAnswer = false;
          PublishSingleton = UNI_COUNT_COMMAND;

          PublishSingleton << PARAM_DELIMITER << UniDispatcher.GetUniSensorsCount(uniTemp);
          PublishSingleton << PARAM_DELIMITER << UniDispatcher.GetUniSensorsCount(uniHumidity);
          PublishSingleton << PARAM_DELIMITER << UniDispatcher.GetUniSensorsCount(uniLuminosity);
          PublishSingleton << PARAM_DELIMITER << UniDispatcher.GetUniSensorsCount(uniSoilMoisture);
          PublishSingleton << PARAM_DELIMITER << UniDispatcher.GetUniSensorsCount(uniPH);
          //TODO: Тут остальные типы датчиков указывать !!!
                     
        }

        
        else
        if(t == SMS_NUMBER_COMMAND) // номер телефона для управления по СМС
        {
          PublishSingleton.Flags.Status = true;
          PublishSingleton.Flags.AddModuleIDToAnswer = false;
          PublishSingleton = SMS_NUMBER_COMMAND; 
          PublishSingleton << PARAM_DELIMITER << MainController->GetSettings()->GetSmsPhoneNumber();
        }
  
        else if(t == STATUS_COMMAND) // получить статус всего железного добра
        {
          if(wantAnswer)
          {
            // входящий поток установлен, значит, можем писать прямо в него
            canPublish = false; // скажем, что мы не хотим публиковать через контроллер - будем писать в поток сами
            Stream* pStream = command.GetIncomingStream();
            pStream->print(OK_ANSWER);
            pStream->print(COMMAND_DELIMITER);

            WORK_STATUS.WriteStatus(pStream,true); // просим записать статус

            // тут можем писать остальные статусы, типа показаний датчиков и т.п.:

            size_t modulesCount = MainController->GetModulesCount(); // получаем кол-во зарегистрированных модулей

            // пробегаем по всем модулям
            String moduleName;
            moduleName.reserve(20);
            
            for(size_t i=0;i<modulesCount;i++)
            {
              yield(); // немного даём поработать другим модулям

              AbstractModule* mod = MainController->GetModule(i);

              // проверяем, не пустой ли модуль. для этого смотрим, сколько у него датчиков вообще
              uint8_t tempCount = mod->State.GetStateCount(StateTemperature);
              uint8_t humCount = mod->State.GetStateCount(StateHumidity);
              uint8_t lightCount = mod->State.GetStateCount(StateLuminosity);
              uint8_t waterflowCountInstant = mod->State.GetStateCount(StateWaterFlowInstant);
              uint8_t waterflowCount = mod->State.GetStateCount(StateWaterFlowIncremental);
              uint8_t soilMoistureCount = mod->State.GetStateCount(StateSoilMoisture); 
              uint8_t phCount = mod->State.GetStateCount(StatePH); 
              
              //TODO: тут другие типы датчиков!!!

              if((tempCount + humCount + lightCount + waterflowCountInstant + waterflowCount + soilMoistureCount + phCount) < 1) // пустой модуль, без интересующих нас датчиков
                continue;

              uint8_t flags = 0;
              if(tempCount) flags |= StateTemperature;
              if(humCount) flags |= StateHumidity;
              if(lightCount) flags |= StateLuminosity;
              if(waterflowCountInstant) flags |= StateWaterFlowInstant;
              if(waterflowCount) flags |= StateWaterFlowIncremental;
              if(soilMoistureCount) flags |= StateSoilMoisture;
              if(phCount) flags |= StatePH;
              //TODO: Тут другие типы датчиков!!!

            // показание каждого модуля идут так:
            
            // 1 байт - флаги о том, какие датчики есть
             pStream->write(WorkStatus::ToHex(flags));
             yield(); // немного даём поработать другим модулям
            
            // 1 байт - длина ID модуля
              moduleName = mod->GetID();
              uint8_t mnamelen = moduleName.length();
              pStream->write(WorkStatus::ToHex(mnamelen));
              yield(); // немного даём поработать другим модулям
             // далее идёт имя модуля
              pStream->write(moduleName.c_str());
              yield(); // немного даём поработать другим модулям

            
              // затем идут данные из модуля, сначала - показания температуры, если они есть
              PrintSensorsValues(tempCount,StateTemperature,mod,pStream);
              yield(); // немного даём поработать другим модулям
              // затем идёт кол-во датчиков влажности, если они есть
              PrintSensorsValues(humCount,StateHumidity,mod,pStream);
              yield(); // немного даём поработать другим модулям
              // затем идут показания датчиков освещенности, если они есть
              PrintSensorsValues(lightCount,StateLuminosity,mod,pStream);
              yield(); // немного даём поработать другим модулям
              // затем идут моментальные показания датчиков расхода воды, если они есть
              PrintSensorsValues(waterflowCountInstant,StateWaterFlowInstant,mod,pStream);
              yield(); // немного даём поработать другим модулям
              // затем идут накопительные показания датчиков расхода воды, если они есть
              PrintSensorsValues(waterflowCount,StateWaterFlowIncremental,mod,pStream);
              yield(); // немного даём поработать другим модулям
              // затем идут датчики влажности почвы, если они есть
              PrintSensorsValues(soilMoistureCount,StateSoilMoisture,mod,pStream);
              yield(); // немного даём поработать другим модулям
              // затем идут датчики pH, если они есть
              PrintSensorsValues(phCount,StatePH,mod,pStream);
            
              //TODO: тут другие типы датчиков!!!

            } // for
            

            pStream->print(NEWLINE); // пишем перевод строки
            
          } // wantAnswer
          
        } // STATUS_COMMAND     
        else if(t == REGISTERED_MODULES_COMMAND) // пролистать зарегистрированные модули
        {
          PublishSingleton.Flags.AddModuleIDToAnswer = false;
          PublishSingleton.Flags.Status = true;
          PublishSingleton = F("");
          size_t cnt = MainController->GetModulesCount();
          for(size_t i=0;i<cnt;i++)
          {
            AbstractModule* mod = MainController->GetModule(i);

            if(mod != this)
            {
              if(PublishSingleton.Text.length())
                PublishSingleton << PARAM_DELIMITER;
              
              PublishSingleton << mod->GetID();
             
            }// if
              
          } // for
        }
        else
        {
            // неизвестная команда
        } // else
        
          
      } // else
    } // elsse
    
  } // ctGET
  else
  if(command.GetType() == ctSET)
  {

    if(!argsCnt) // нет аргументов
    {
      PublishSingleton = PARAMS_MISSED;
    }
    else
    {
      if(argsCnt < 2)
      {
        // мало параметров
        PublishSingleton = PARAMS_MISSED;
        String t = command.GetArg(0);    

        if(t == RESET_COMMAND)
        {
          #if TARGET_BOARD == DUE_BOARD
            const int RSTC_KEY = 0xA5;
            RSTC->RSTC_CR = RSTC_CR_KEY(RSTC_KEY) | RSTC_CR_PROCRST | RSTC_CR_PERRST;
            while (true);
          #else
          resetFunc(); // ресетимся, писать в ответ ничего не надо
          #endif
          
        } // RESET_COMMAND
        else
        if(t == F("AUTO")) // CTSET=0|AUTO - перевести в автоматический режим
        {
          // очищаем общий буфер ответов
          PublishSingleton = "";

          // выполняем команды
          ModuleInterop.QueryCommand(ctSET, F("STATE|MODE|AUTO"),false);
          ModuleInterop.QueryCommand(ctSET, F("WATER|MODE|AUTO"),false);
          ModuleInterop.QueryCommand(ctSET, F("LIGHT|MODE|AUTO"),false);

          // говорим, что выполнили
          PublishSingleton = REG_SUCC;
          PublishSingleton.Flags.Status = true;
        
        } // AUTO
                
      } // if
      else
      {
        String t = command.GetArg(0); // получили команду
        
      #ifdef USE_REMOTE_MODULES 
      if(t == ADD_COMMAND) // запросили регистрацию нового модуля
       {
          // ищем уже зарегистрированный
          String reqID = command.GetArg(1);
          AbstractModule* mod = c->GetModuleByID(reqID);
          if(mod)
          {
            // модуль уже зарегистрирован
            PublishSingleton = REG_ERR; 
            PublishSingleton << PARAM_DELIMITER << reqID;
          } // if
          else
          {
            // регистрируем новый модуль
            RemoteModule* remMod = new RemoteModule(reqID); 
            c->RegisterModule(remMod);
            PublishSingleton.Flags.Status = true;
            PublishSingleton = REG_SUCC; 
            PublishSingleton << PARAM_DELIMITER << reqID;

          } // else
       }
       
       else 
       #endif
       if(t == SMS_NUMBER_COMMAND) // номер телефона для управления по SMS
       {
          GlobalSettings* sett = MainController->GetSettings();
          sett->SetSmsPhoneNumber(command.GetArg(1));
//          sett->Save();
          PublishSingleton.Flags.Status = true;
          PublishSingleton = SMS_NUMBER_COMMAND; 
          PublishSingleton << PARAM_DELIMITER << REG_SUCC;
          
       }
       else
       if(t == UNI_RF_CHANNEL_COMMAND)
       {
          byte ch = atoi(command.GetArg(1));
          UniDispatcher.SetRFChannel(ch);

          #ifdef USE_NRF_GATE
            nrfGate.SetChannel(ch);
          #endif
          
          PublishSingleton.Flags.Status = true;
          PublishSingleton = UNI_RF_CHANNEL_COMMAND; 
          PublishSingleton << PARAM_DELIMITER << REG_SUCC;
        
       }
        #if defined(USE_UNIVERSAL_MODULES) && defined(USE_UNI_REGISTRATION_LINE)
        else
        if(t == UNI_REGISTER) // зарегистрировать универсальный модуль, висящий на линии
        {
          PublishSingleton.Flags.AddModuleIDToAnswer = false;

              if(uniRegistrator.IsModulePresent())
              {
                // модуль есть на линии, регистрируем его в системе.
                // сначала вычитываем переданный скратчпад и назначаем его модулю.
                // считаем, что на вызывающей стороне разобрались, что с чем, с остальным
                // разберётся модуль регистрации.
                const char* scratchData = command.GetArg(1);
                // теперь конвертируем данные скратчпада из текстового представления в нормальное
                char buff[3] = {0};
                uint8_t len = strlen(scratchData);

                UniRawScratchpad scratch;
                byte* raw = (byte* )&scratch;

                for(uint8_t i=0;i<len;i+=2)
                {
                  buff[0] = scratchData[i];
                  buff[1] = scratchData[i+1];
                  *raw = WorkStatus::FromHex(buff);
                  raw++;
                } // for
                
                if(uniRegistrator.SetScratchpadData(&scratch))
                {
                  uniRegistrator.Register();
                              
                  PublishSingleton.Flags.Status = true;
                  PublishSingleton = REG_SUCC;
                } // if
                else
                {
                   // разные типы скратчпадов, возможно, подсоединили другой модуль
                   PublishSingleton = UNI_DIFFERENT_SCRATCHPAD;
                }
                
              } // if
              else
              {
                // модуля нет на линии
                PublishSingleton = UNI_NOT_FOUND;
              }
              
          
        } // UNI_REGISTER
        #endif // USE_UNI_REGISTRATION_LINE
       
       else if(t == ID_COMMAND)
       {
          //String newID = command.GetArg(1);
          MainController->GetSettings()->SetControllerID((uint8_t)atoi(command.GetArg(1)));
          PublishSingleton.Flags.Status = true;
          PublishSingleton = ID_COMMAND; 
          PublishSingleton << PARAM_DELIMITER << REG_SUCC;
        
       }       
       #ifdef USE_DS3231_REALTIME_CLOCK
       else if(t == SETTIME_COMMAND)
       {
         // установка даты/времени
         String rawDatetime = command.GetArg(1);
         int8_t idx = rawDatetime.indexOf(F(" "));
         String tm, dt;
         if(idx != -1)
         {
          dt = rawDatetime.substring(0,idx);
          tm = rawDatetime.substring(idx+1);

            String month,day,year;
            String hour,minute,sec;
            idx = dt.indexOf(F("."));
            if(idx != -1)
             {
              day = dt.substring(0,idx);
              dt = dt.substring(idx+1);
             }
             
            idx = dt.indexOf(F("."));
            if(idx != -1)
             {
              month = dt.substring(0,idx);
              year = dt.substring(idx+1);
             }

             idx = tm.indexOf(F(":"));
             if(idx != -1)
             {
              hour = tm.substring(0,idx);
              tm = tm.substring(idx+1);
             }

             idx = tm.indexOf(F(":"));
             if(idx != -1)
             {
              minute = tm.substring(0,idx);
              sec = tm.substring(idx+1);
             }

             // вычисляем день недели
             int yearint = year.toInt();
             int monthint = month.toInt();
             int dayint = day.toInt();
             
             int dow;
             byte mArr[12] = {6,2,2,5,0,3,5,1,4,6,2,4};
             dow = (yearint % 100);
             dow = dow*1.25;
             dow += dayint;
             dow += mArr[monthint-1];
             
             if (((yearint % 4)==0) && (monthint<3))
               dow -= 1;
               
             while (dow>7)
               dow -= 7;             

            
             RealtimeClock cl = MainController->GetClock();
             cl.setTime(sec.toInt(),minute.toInt(),hour.toInt(),dow,dayint,monthint,yearint);

             PublishSingleton.Flags.Status = true;
             PublishSingleton = REG_SUCC;
         } // if
       }
       #endif
       else
       {
         // неизвестная команда
       } // else
      } // else argsCount > 1
    } // else
    
  } // if
 
 // отвечаем на команду
 if(canPublish) // можем публиковать
  MainController->Publish(this,command);
 else
 {
  PublishSingleton.Reset();
 }
    
  return PublishSingleton.Flags.Status;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------


