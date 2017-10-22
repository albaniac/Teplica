/*
Программа передачи данных по каналу GPRS
31.03.2017г.



*/

#define __SAM3X8E__


#include <SPI.h>
#include "SIM800.h"
//#include <OneWire.h> 
#include <DueTimer.h>
#include "Wire.h"


#define con Serial
#define speed_Serial 115200
#define SIM800_BAUD 19200
//
//#define PIN_TX           7                              // Подключить  к выводу 7 сигнал RX модуля GPRS
//#define PIN_RX           8                              // Подключить  к выводу 8 сигнал TX модуля GPRS
//
//SoftwareSerial SIM800CSS = SoftwareSerial(PIN_RX, PIN_TX);
//SoftwareSerial *GPRSSerial = &SIM800CSS;
#define GPRSSerial Serial2

#define PWR_On           27                              // Включение питания модуля SIM800
#define SIM800_RESET_PIN 47                              // Сброс модуля SIM800
#define LED13            13                              // Индикация светодиодом
#define NETLIGHT         37                              // Индикация NETLIGHT
#define STATUS           57                              // Индикация STATUS
//#define analog_dev1      A5                             // Аналоговый вход 1
//#define digital_inDev2   12                             // Цифровой вход 12
//#define digital_outDev3  A4                             // Цифровой выход управления внешним устройством


												
//#define COMMON_ANODE                                  // Если светодиод с общим катодом - раскомментировать
#define LED_RED      54                                 // Индикация светодиодом RED
#define LED_BLUE     55                                 // Индикация светодиодом BLUE
#define LED_GREEN    56                                 // Индикация светодиодом GREEN

#define COLOR_NONE LOW, LOW, LOW                        // Отключить все светодиоды
#define COLOR_GREEN LOW, HIGH, LOW                      // Включить зеленый светодиод
#define COLOR_BLUE LOW, LOW, HIGH                       // Включить синий светодиод
#define COLOR_RED HIGH, LOW, LOW                        // Включить красный светодиод
volatile int stateLed = LOW;                            // Состояние светодиода при прерывистой индикации на старте
volatile int state_device = 0;                          // Состояние модуля при запуске 

														// 1 - Не зарегистрирован в сети, поиск
														// 2 - Зарегистрировано в сети
														// 3 - GPRS связь установлена
volatile int metering_NETLIGHT       = 0;
volatile unsigned long metering_temp = 0;
volatile int count_blink1            = 0;               // Счетчик попыток подключиться к базовой станции
volatile int count_blink2            = 0;               // Счетчик попыток подключиться к базовой станции
bool send_ok                         = false;           // Признак успешной передачи данных
bool count_All_reset                 = false;           // Признак выполнения команды сброса счетчика ошибок.
bool temp_dev3 = false;                                 // Переменная для временного хранения состояния исполнительного устройства
String imei = "861445030362268";                        // Тест IMEI
String SIMCCID = "";

CGPRS_SIM800 gprs;
int count                     = 0;
//unsigned int errors           = 0;
#define DELIM "&"
unsigned long time_sutki = 0;                     // Переменная для суточного сброса
unsigned long time_day         = 86400;                 // Переменная секунд в сутках
unsigned long previousMillis   = 0;                     //  
unsigned long interval         = 60;                   // Интервал передачи данных 5 минут
bool time_set                  = false;                 // Фиксировать интервал заданный СМС
bool ssl_set                   = true;                  // Признак шифрования 
bool watch_dog                 = false;                 // Признак проверки сторожевого таймера
unsigned long time_ping        = 380;                   // Интервал проверки ping 6 минут. 
unsigned long previousPing     = 0;                     // Временный Интервал проверки ping

int Address_tel1            = 100;                        // Адрес в EEPROM телефона 1
int Address_ssl             = 120;                        // Адрес в EEPROM признака шифрования
//int Address_errorAll      = 160;                        // Адрес в EEPROM счетчика общих ошибок
int Address_interval        = 200;                        // Адрес в EEPROM величины интервала
int Address_SMS_center      = 220;                        // Адрес в EEPROM SMS центра
int Address_Dev3            = 260;                        // Адрес в EEPROM состояния исполнительного устройства Dev
int Address_Dev3_ind        = 264;                        // Адрес в EEPROM признак управления сполнительного устройства Dev
int Address_num_site_ping   = 268;                        // Адрес в EEPROM признак управления сполнительного устройства Dev
int Address_watchdog        = 270;                        // Адрес в EEPROM счетчик проверки Watchdog
int Address_EEPROM_off      = 280;                        // Адрес в EEPROM запрет записи в EEPROM
byte dev3                   = 0;                          // признак управления сполнительного устройства Dev3
//uint8_t oneWirePins[]       ={16, 17, 4};                 //номера датчиков температуры DS18x20. Переставляя номера можно устанавливать очередность передачи в строке.
  												    	  // Сейчас первым идет внутренний датчик.
//uint8_t oneWirePinsCount=sizeof(oneWirePins)/sizeof(int);
//
//OneWire ds18x20_1(oneWirePins[0]);
//OneWire ds18x20_2(oneWirePins[1]);
//OneWire ds18x20_3(oneWirePins[2]);
//DallasTemperature sensor1(&ds18x20_1);
//DallasTemperature sensor2(&ds18x20_2);
//DallasTemperature sensor3(&ds18x20_3);


//+++++++++++++++++++++++++++++ Внешняя память +++++++++++++++++++++++++++++++++++++++
int deviceaddress = 80;                      // Адрес микросхемы памяти
											 //unsigned int eeaddress = 0;                  // Адрес ячейки памяти
byte hi;                                     // Старший байт для преобразования числа
byte low;                                    // Младший байт для преобразования числа




void flash_time()                                       // Программа обработчик прерывистого свечения светодиодов при старте
{
	if (state_device == 0)
	{
		setColor(COLOR_RED);
	}
	if (state_device == 1)
	{
		stateLed = !stateLed;
		if (!stateLed)
		{
			setColor(COLOR_RED);
		}
		else
		{
			setColor(COLOR_NONE);
		}
	}

	if (state_device == 2)
	{
		stateLed = !stateLed;
		if (!stateLed)
		{
			setColor(COLOR_NONE);
		}
		else
		{
			setColor(COLOR_BLUE);
		}
	}

	if (state_device == 3)
	{
		stateLed = !stateLed;
		if (!stateLed)
		{
			setColor(COLOR_NONE);
		}
		else
		{
			setColor(COLOR_GREEN);
		}
	}
}

 void setColor(bool red, bool green, bool blue)        // Включение цвета свечения трехцветного светодиода.
 {
	  #ifdef COMMON_ANODE                              // Если светодиод с общим катодом
		red = !red;
		green = !green;
		blue = !blue;
	  #endif 
	  digitalWrite(LED_RED, red);
	  digitalWrite(LED_GREEN, green);
	  digitalWrite(LED_BLUE, blue);    
 }
 
void sendTemps()
{
	//sensor1.requestTemperatures();
	//sensor2.requestTemperatures();
	//sensor3.requestTemperatures();
	//float t1 = sensor1.getTempCByIndex(0);
	//float t2 = sensor2.getTempCByIndex(0);
	//float t3 = sensor3.getTempCByIndex(0);
	//float tsumma = t1 + t2 + t3 + 88.88;

	float t1 = 10;
	float t2 = 20;
	float t3 = 30;
	float tsumma = t1 + t2 + t3 + 88.88;


	unsigned int error_All = 0;
	int Address_errorAll = 160;                           // Адрес в EEPROM счетчика общих ошибок
	//EEPROM.get(Address_errorAll, error_All);

	//int dev1 = analogRead(analog_dev1);                   // Аналоговый вход 1
	//bool dev2 = digitalRead(digital_inDev2);              // Цифровой вход 2

	int dev1 = 40;                   // Аналоговый вход 1
	bool dev2 = 41;              // Цифровой вход 2

	//dev3 = EEPROM.read(Address_Dev3);                     // Состояние исполнительного устройства
	dev3 = 42;                     // Состояние исполнительного устройства
	String temp123;
	if (t1 != -127)
	{
		temp123 += DELIM; 
		temp123 += "D1=" + String(t1);
	}
	if (t2 != -127)
	{
		temp123 += DELIM;
		temp123 += "D2=" + String(t2);
	}
	if (t3 != -127)
	{
		temp123 += DELIM;
		temp123 += "D3=" + String(t3);
	}

	byte signal = gprs.getSignalQuality();

	String toSend = "IM=" + imei + temp123 + DELIM + "S=" + String(signal) + DELIM + "E=" + String(gprs.errors) + DELIM + "EA=" + String(error_All+ gprs.errors) + formEnd() + DELIM + "SM=" + String(tsumma) + DELIM + "In1=" + String(dev1) + DELIM + "In2=" + String(dev2) + DELIM + "Out=" + String(dev3);

	Serial.print(F("toSend.length: "));
	Serial.println(toSend.length());
	int count_send = 0;
	while (1)
	{
		if (gprs_send(toSend))
		{
			return;
		}
		else
		{
			count_send++;
			check_SMS();                                   // Проверить приход новых СМС 
			Serial.print("Attempt to transfer data .."); Serial.println(count_send);
		}
		delay(2000);                                       // Подождать 5 секунд
	}

}


String formEnd() 
{
	char buf[15] ;
//	EEPROM.get(Address_tel1, buf);
	String master_tel1(buf);
	master_tel1 = "Tel=" + master_tel1;
//	EEPROM.get(Address_SMS_center, buf);             //Получить из EEPROM СМС центр
	String SMS_center(buf);
	SMS_center = "SMSC=" + SMS_center;
	return DELIM +  master_tel1 + DELIM + SMS_center + DELIM + "SIM="+SIMCCID;

}


bool gprs_send(String data) 
{
  con.print(F("Requesting .. Wait"));      
  setColor(COLOR_BLUE); 
  static const char* url1   = "http://trm7.xyz/r.php";
  static const char* urlssl = "https://trm7.xyz/r.php";
  
  connect_internet_HTTP();                               // Подключиться к интернету с учетом стека HTTP
  
  int count_init = 0;                                    // Счетчик количества попыток подключиться к HTTP
  for (;;)                                               // Бесконечный цикл пока не наступит, какое то состояние для выхода
  {
	  if (gprs.httpInit()) break;                        // Все нормально, модуль ответил , Прервать попытки и выйти из цикла
	  con.print(">");
	  con.println(gprs.buffer);                          // Не получилось, ("ERROR") 
	  String stringError = gprs.buffer;
	  if (stringError.indexOf(F("ERROR")) > -1)          
	  {
		  con.println(F("\nNo internet connection"));
		  delay(1000);
	  }
	  gprs.httpUninit();                                 // Не получилось, разединить и  попробовать снова 
	  delay(1000);                                       // Подождать секунду.
	  count_init++;
	  if (count_init > 5)
	  {
		  gprs.reboot(gprs.errors);                      // Вызываем срабатывание по Watchdog  
	  }
  }

  if (ssl_set == true)
  {
	  if (gprs.HTTP_ssl(true))
	  {
		  con.println(F("\nHTTP_ssl: set ON successfully!"));
	  }
	  else
	  {
		  con.println(F("\nHTTP_ssl: set ON false!"));
	  }
  }
  else
  {
	  if (gprs.HTTP_ssl(false))
	  {
		  con.println(F("\nHTTP_ssl: set OFF successfully!"));
	  }
	  else
	  {
		  con.println(F("\nHTTP_ssl: set OFF false!"));
	  }

  }
 
  if (ssl_set == true)
  {
	  con.print(urlssl);
	  con.print('?');
	  con.println(data);

	  gprs.httpConnectStr(urlssl, data);
  }
  else
  {
	  con.print(url1);
	  con.print('?');
	  con.println(data);
	  gprs.httpConnectStr(url1, data);
  }
  count++;
  send_ok = false;
	
  while (gprs.httpIsConnected() == 0)     // 0 - ожидание ответа, 1 - успешно, 
  {
	con.write('.');
	for (byte n = 0; n < 50 && !gprs.available(); n++) 
	{
	  delay(15);
	}
  }
  if (gprs.httpState == HTTP_ERROR) 
  {
	con.println(F("Connect error HTTP"));
	gprs.errors++;
		
	if (gprs.errors > 5)
	  {
			con.println(F("Errors exceeded"));
			delay(3000);
			gprs.reboot(gprs.errors);                               // вызываем reset после 10 ошибок
	  }
	delay(3000);
//	return; 
  }
  
  con.println();
  gprs.httpRead();
  int ret;
  while ((ret = gprs.httpIsRead()) == 0)  //  Ожидаем сообщение HTTP
  {
	// может сделать что-то здесь, во время ожидания
  }

  if (gprs.httpState == HTTP_ERROR)          // Ответ не пришел
  {
	  gprs.errors++;
	//errorAllmem();
	if (gprs.errors > 5)                         // вызываем reset после 10 ошибок
	  {
			con.println(F("The number of server errors exceeded 10"));
			delay(3000);                                // Время для чтения сообщения
			gprs.reboot(gprs.errors);                   // вызываем reset после 10 ошибок
	  }
	delay(3000);
//	return; 
  } 

  // Теперь мы получили сообщение от сайта.
   con.print(F("[Payload] "));                        
   con.println(gprs.buffer);
   String val = gprs.buffer;               // Получить строку данных с сервера
   send_ok = true;                         // Команда принята успешно
   int p0[8];
										   // Команда принята успешно

   if (val.indexOf("&") > -1)              // Определить адреса (позиции) команд в строке 
   {
	   p0[0] = val.indexOf("&01");         // Адрес 1 команды
	   p0[1] = val.indexOf("&02");         // Адрес 2 команды
	   p0[2] = val.indexOf("&03");         // Адрес 3 команды
	   p0[3] = val.indexOf("&04");         // Адрес 4 команды
	   p0[4] = val.indexOf("&05");         // Адрес 5 команды
	   p0[5] = val.indexOf("&06");         // Адрес 6 команды
	   p0[6] = val.indexOf("&07");         // Адрес 7 команды
	   p0[7] = val.indexOf('#');           // Адрес конца команд
   }

   for (int i=0;i<7;i++)
   {
	  String comm = val.substring(p0[i]+3, p0[i]+5);          // Выделить строку с номером команды
	  int comm1 = comm.toInt(); 
	  Serial.println(comm1);
	  comm = val.substring(p0[i] + 5, p0[i+1]);               // Выделить строку с данными
	  int len_str = comm.length();
	  comm[len_str] = '\0';
	  Serial.println(comm);

	  run_command(comm1, comm);                               // Последовательно выполнить все команды

   }

  // Показать статистику
  con.print(F("Total: "));                                   
  con.print(count);
  if (gprs.errors)                                                 // Если есть ошибки - сообщить
  {
	con.print(F(" Errors: "));                                
	con.print(gprs.errors);
  }
  con.println();
  Serial.print("Inteval: ");
  Serial.println(interval);
  gprs.httpUninit();                                          // Разорвать соединение HTTP
  setColor(COLOR_GREEN);
  return send_ok;
}


void connect_internet_HTTP()
{
	bool setup_ok = false;
	int count_init = 0;
	do
	{
		Serial.println(F("\nInternet HTTP connect .."));
		
		byte ret = gprs.connect_GPRS();                                              // Подключение к GPRS
		if (ret == 0)
		{
			while (state_device != 3)                            // Ожидание регистрации в сети
			{
				delay(50);
			}

			con.println(F("Wait IP"));
			gprs.connect_IP_GPRS();                             // Получить IP адрес
			Serial.println(F("Internet connect OK!-"));
			setup_ok = true;
		}
		else                                                    // Модуль не подключиля к интернету
		{
			count_init++;                                        // Увеличить счетчик количества попыток
			if (count_init > 10)
			{
				gprs.reboot(gprs.errors);                       // вызываем reset после 10 ошибок
			}

			Serial.println(F("\nFailed connect internet"));
			delay(1000);
			if (state_device == 3)                               // Модуль одумался и все таки подключиля к интернету
			{
				con.println(F("Wait IP"));
				gprs.connect_IP_GPRS();                          // Получить IP адрес
				Serial.println(F("Internet connect OK!-"));
				setup_ok = true;
			}
		}
	} while (!setup_ok);             // 
}




void run_command(int command, String data)
{
	unsigned long interval1 = 0;
	byte dev3_set            = 0;
	byte dev3_data           = 0;
	byte ping_data           = 0;
	char buf[16];
	switch (command) 
	{
		case 1:
			interval1 = data.toInt();                              // Преобразовать строку данных в число 
			con.println(interval1);
			if (interval1 > 29 && interval1 < 86401)               // Ограничить интервалы от 29  секунд до 24 часов.
			{
				if (interval1 != interval)                         // Если информиция не изменилась - не писать в EEPROM
				{
					if (!time_set)                                 // Если нет команды фиксации интервала от СМС 
					{
						interval = interval1;                      // Переключить интервал передачи на сервер
						//EEPROM.put(Address_interval, interval1);   // Записать интервал EEPROM , полученный от сервера
					}
				}
			}
			con.println(interval);
			break;
		case 2:
			//EEPROM.get(Address_tel1, buf);             // Получить номер телефона из EEPROM
			Serial.println(buf);
			if (data != buf)                           // Если информиция не изменилась - не писать в EEPROM
			{
				con.println(F("no compare"));               //Serial.println("no compare");
				for (int i = 0; i<13; i++)
				{
					//EEPROM.write(i + Address_tel1, data[i]);
				}
				//EEPROM.get(Address_tel1, buf);
				Serial.println(buf);
			}
			else
			{
				con.println(F("Tel compare"));                
			}
			break;
		case 3:
			//EEPROM.get(Address_SMS_center, buf);      // Получить из EEPROM СМС центр
			if (data != buf)                          // Если информиция не изменилась - не писать в EEPROM
			{
				Serial.println(F("no compare"));
				for (int i = 0; i<13; i++)
				{
				//	EEPROM.write(i + Address_SMS_center, data[i]);
				}
			}

			break;
		case 4:
			if (data.toInt() == 1)
				{
					if (count_All_reset = false)                   // Признак выполнения команды сброса счетчика ошибок
					{
						int Address_errorAll = 160;                // Адрес в EEPROM счетчика общих ошибок
						count_All_reset == true;                   // Команда сброса выполнена. Повторный сброс возможен после перезагрузки
						//EEPROM.put(Address_errorAll, 0);           // Сбросить счетчик ошибок Предусмотреть блокировку повторной записи???
					}
				}
			break;
		case 5:
			if (data.toInt() == 1)
			{
				time_set = false;                                    // Снять фиксацию интервала заданного СМС
			}
			
			break;
		case 6:
			//dev3_set = EEPROM.read(Address_Dev3_ind);
			dev3_set = 0;
			if (dev3_set == 0)
			{
				dev3_data = data.toInt();
				//dev3_set = EEPROM.read(Address_Dev3);
				if (dev3_data != dev3_set)         // Если информиция не изменилась - не писать в EEPROM
				{
					//EEPROM.write(Address_Dev3, dev3_data);
					Serial.println(F("Device .."));
					if (dev3_data == 0)
					{
						Serial.println(F("OFF"));
					}
					else
					{
						Serial.println(F("ON"));
					}
				}
			}
			break;
		case 7:
			break;
		default:
			break;
			// выполняется, если не выбрана ни одна альтернатива
			// default необязателен
	}
}

//int freeRam()                                   // Определить свободную память
//{
//  extern int __heap_start, *__brkval; 
//  int v; 
//  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
//}

void setTime(String val, String f_phone)
{
  if (val.indexOf(F("Timeset")) > -1)                   // 
  {
	 interval = 40;                                     // Установить интервал 40 секунд
	 time_set = true;                                   // Установить фиксацию интервала заданного СМС
	 Serial.println(interval);
	 gprs.send_sms(val, f_phone);                         //процедура отправки СМС
  } 
  else if (val.indexOf(F("Restart")) > -1) 
  {
	  Serial.print(f_phone);
	  Serial.print("..");
	  Serial.println(F("Restart"));
	  gprs.send_sms(val, f_phone);                      //процедура отправки СМС
	  delay(2000);
	  gprs.reboot(gprs.errors);                         // вызываем reset
  } 
  else if (val.indexOf(F("Timeoff")) > -1) 
  {
	 time_set = false;                                  // Снять фиксацию интервала заданного СМС
	 Serial.println(F("Timeoff"));
	 gprs.send_sms(val, f_phone);                       //процедура отправки СМС
  } 
  else if (val.indexOf(F("Sslon")) > -1)
  {
	  //EEPROM.write(Address_ssl, true);                  // Включить шифрование
	  Serial.println(F("HTTP SSL ON"));
	  gprs.send_sms("HTTP SSL ON", f_phone);            //процедура отправки СМС
	  delay(2000);
	  gprs.reboot(gprs.errors);                         // вызываем reset  
  }
  else if (val.indexOf(F("Ssloff")) > -1)
  {
	  //EEPROM.write(Address_ssl, false);                 // Отключить шифрование
	  Serial.println(F("HTTP SSL OFF"));
	  gprs.send_sms("HTTP SSL OFF", f_phone);           //процедура отправки СМС
	  delay(2000);
	  gprs.reboot(gprs.errors);                         // вызываем reset
  }
  else if (val.indexOf(F("Eon")) > -1)
  {
	  //EEPROM.write(Address_EEPROM_off, true);           // Включить 
	  Serial.println(F("EEPROM ON"));
	  gprs.send_sms("EEPROM ON", f_phone);              //процедура отправки СМС
  }
  else if (val.indexOf(F("Eoff")) > -1)
  {
	  //EEPROM.write(Address_EEPROM_off, false);          // Отключить
	  Serial.println(F("EEPROM OFF"));
	  gprs.send_sms("EEPROM OFF", f_phone);             //процедура отправки СМС
  }
  else if (val.indexOf(F("Devon")) > -1)
  {
	  //EEPROM.write(Address_Dev3, 1);                   // Включить исполнительное устройство
	  //EEPROM.write(Address_Dev3_ind, 1);
	  con.println(F("Device ON"));
	  gprs.send_sms("Device ON", f_phone);             //процедура отправки СМС
  }
  else if (val.indexOf(F("Devoff")) > -1)
  {
	  //EEPROM.write(Address_Dev3, 0);                   // Отключить исполнительное устройство
	  //EEPROM.write(Address_Dev3_ind, 0);               // 
	  con.println(F("Device OFF"));
	  gprs.send_sms("Device OFF", f_phone);            //процедура отправки СМС
  }
  else
  {
	  Serial.println(F("Unknown command"));            // Serial.println("Unknown command");
	  gprs.send_sms("Unknown command "+ val, f_phone); //процедура отправки СМС
  }
}

void check_blink()
{
	unsigned long current_M = millis();
	//wdt_reset();
	metering_NETLIGHT = current_M - metering_temp;                            // переделать для  
	metering_temp = current_M;

	if (metering_NETLIGHT > 3055 && metering_NETLIGHT < 3070)
	{
		state_device = 2;                                                     // 2 - Зарегистрировано в сети
		count_blink2++;
		if (count_blink2 > 250)
		{
			state_device = 0;
			Timer6.stop();                                                 // Выключить таймер прерывания
			gprs.reboot(gprs.errors);                                                    // Что то пошло не так с регистрацией на станции
		}
	}
	else if (metering_NETLIGHT > 855 && metering_NETLIGHT < 870)
	{
		state_device = 1;                                                     // 1 Не зарегистрирован в сети, поиск
		count_blink1++;
		if (count_blink1 > 250) 
		{
			state_device = 0;
			Timer6.stop();                                                 // Выключить таймер прерывания
			gprs.reboot(gprs.errors);                                                    // Что то пошло не так с регистрацией на станции
		}
	}
	else if (metering_NETLIGHT > 350 && metering_NETLIGHT < 370)
	{
		state_device = 3;                                                     // 3 - GPRS связь установлена
									  
	}
}


void check_SMS()
{

	if (gprs.checkSMS())
	{
		con.print(F("SMS:"));
		con.println(gprs.val);

		if (gprs.val.indexOf("REC UNREAD") > -1)  //если обнаружена новая  СМС 
		{
			//------------- поиск кодового слова в СМС 
			byte buf[16];
			i2c_eeprom_read_buffer(deviceaddress, Address_tel1, buf,16);
			//EEPROM.get(Address_tel1, buf);                                         // Восстановить телефон хозяина 1
			//String master_tel1(buf);
			i2c_eeprom_read_buffer(deviceaddress, Address_SMS_center, buf, 16);
			//EEPROM.get(Address_SMS_center, buf);                                 // Восстановить телефон СМС центра
			//String master_SMS_center(buf);
			/*
			if (gprs.deleteSMS(1))
			{
				con.println(F("SMS delete"));                    //      con.print("SMS:");
			}

			if (gprs.val.indexOf(master_tel1) > -1)                              //если СМС от хозяина 1
			{
				con.println(F("Commanda tel1"));
				setTime(gprs.val, master_tel1);
			}
			else if (gprs.val.indexOf(master_SMS_center) > -1)                    //если СМС от хозяина 2
			{
				con.println(F("SMS centr"));
				setTime(gprs.val, master_SMS_center);
			}
			else
			{
				con.println(F("Phone ignored"));
				gprs.send_sms("Phone ignored !! " + gprs.val, master_tel1);              //процедура отправки СМС Изменить в библиотеке
			}
			*/
		}

		if (gprs.val.indexOf("REC READ") > -1)                   //если обнаружена старая  СМС 
		{
			if (gprs.deleteSMS(0))
			{
				con.println(F("SMS delete"));                    //  con.print("SMS:");
			}
		}
		gprs.val = "";
	}
}


void i2c_eeprom_write_byte(int deviceaddress, unsigned int eeaddress, byte data)
{
	int rdata = data;
	Wire.beginTransmission(deviceaddress);
	Wire.write((int)(eeaddress >> 8)); // MSB
	Wire.write((int)(eeaddress & 0xFF)); // LSB
	Wire.write(rdata);
	Wire.endTransmission();
	delay(10);
}
byte i2c_eeprom_read_byte(int deviceaddress, unsigned int eeaddress) {
	byte rdata = 0xFF;
	Wire.beginTransmission(deviceaddress);
	Wire.write((int)(eeaddress >> 8)); // MSB
	Wire.write((int)(eeaddress & 0xFF)); // LSB
	Wire.endTransmission();
	Wire.requestFrom(deviceaddress, 1);
	if (Wire.available()) rdata = Wire.read();
	return rdata;
}
void i2c_eeprom_read_buffer(int deviceaddress, unsigned int eeaddress, byte *buffer, int length)
{

	Wire.beginTransmission(deviceaddress);
	Wire.write((int)(eeaddress >> 8)); // MSB
	Wire.write((int)(eeaddress & 0xFF)); // LSB
	Wire.endTransmission();
	Wire.requestFrom(deviceaddress, length);
	int c = 0;
	for (c = 0; c < length; c++)
	if (Wire.available()) buffer[c] = Wire.read();

}
void i2c_eeprom_write_page(int deviceaddress, unsigned int eeaddresspage, byte* data, byte length)
{
	Wire.beginTransmission(deviceaddress);
	Wire.write((int)(eeaddresspage >> 8)); // MSB
	Wire.write((int)(eeaddresspage & 0xFF)); // LSB
	byte c;
	for (c = 0; c < length; c++)
		Wire.write(data[c]);
	Wire.endTransmission();

}
void i2c_test()
{

	Serial.println("--------  EEPROM Test  ---------");
	char somedata[] = "this data from the eeprom i2c"; // data to write
													   //i2c_eeprom_write_page(0x50, 0, (byte *)somedata, sizeof(somedata)); // write to EEPROM
	i2c_eeprom_write_page(deviceaddress, 0, (byte *)somedata, sizeof(somedata)); // write to EEPROM
	delay(100); //add a small delay
	Serial.println("Written Done");
	delay(10);
	Serial.print("Read EERPOM:");
	byte b = i2c_eeprom_read_byte(deviceaddress, 0); // access the first address from the memory
	char addr = 0; //first address

	while (b != 0)
	{
		Serial.print((char)b); //print content to serial port
		if (b != somedata[addr])
		{
			break;
		}
		addr++; //increase address
		b = i2c_eeprom_read_byte(0x50, addr); //access an address from the memory
	}
	Serial.println();
}


void start_init()
{
	bool setup_ok = false;
	int count_init = 0;
	Timer6.start();                                                 // Включить таймер прерывания
	do
	{
		con.println(F("Initializing....(May take 5-10 seconds)"));

		digitalWrite(SIM800_RESET_PIN, LOW);                      // Сигнал сброс в исходное состояние
		digitalWrite(LED13, LOW);
		digitalWrite(PWR_On, HIGH);                               // Кратковременно отключаем питание модуля GPRS
		while (digitalRead(STATUS) != LOW) 
		{
			delay(100);
		}
		delay(1000);
		digitalWrite(LED13, HIGH);
		digitalWrite(PWR_On, LOW);
		delay(1000);                                              // 
		digitalWrite(SIM800_RESET_PIN, HIGH);                     // Производим сброс модема после включения питания
		delay(1200);
		digitalWrite(SIM800_RESET_PIN, LOW);
		int count_status = 0;
		while (digitalRead(STATUS) == LOW)
		{
			count_status++;
			if (count_status > 100)
			{
				gprs.reboot(gprs.errors);                                     // 100 попыток. Что то пошло не так программа перезапуска  если модуль не включился
			}
			delay(100);
		}
	
		con.println(F("Power SIM800 On"));
		while (!gprs.begin(SIM800_BAUD))                              // Настройка модуля SIM800C
		{
			Serial.println(F("Couldn't find module GPRS"));
			while (1);
		}
		con.println(F("OK"));
		Serial.print(F("\nSetting up mobile network..."));
		while (state_device != 2)                                // Ожидание регистрации в сети
		{
			Serial.print(F("."));
			delay(1000);
		}
		delay(1000);

		if (gprs.getIMEI())                                     // Получить IMEI
		{
			con.print(F("\nIMEI:"));
			//imei = gprs.buffer;                                 // Отключить на время отладки
			gprs.cleanStr(imei);                                // Отключить на время отладки
			con.println(imei);
		}

		if (gprs.getSIMCCID())                               // Получить Номер СИМ карты
		{
			con.print(F("\nSIM CCID:"));
			SIMCCID = gprs.buffer1;
			gprs.cleanStr(SIMCCID);
			con.println(SIMCCID);
		}


		char n = gprs.getNetworkStatus();
		
		Serial.print(F("\nNetwork status "));
		Serial.print(n);
		Serial.print(F(": "));
		if (n == '0') Serial.println(F("\nNot registered"));                      // 0 – не зарегистрирован, поиска сети нет
		if (n == '1') Serial.println(F("\nRegistered (home)"));                   // 1 – зарегистрирован, домашняя сеть
		if (n == '2') Serial.println(F("\nNot registered (searching)"));          // 2 – не зарегистрирован, идёт поиск новой сети
		if (n == '3') Serial.println(F("\nDenied"));                              // 3 – регистрация отклонена
		if (n == '4') Serial.println(F("\nUnknown"));                             // 4 – неизвестно
		if (n == '5') Serial.println(F("\nRegistered roaming"));                  // 5 – роуминг

		if (n == '1' || n == '5')                                                 // Если домашняя сеть или роуминг
		{
			if (state_device == 2)                                                // Проверить аппаратно подключения модема к оператору
			{
				do
				{
					byte signal = gprs.getSignalQuality();
					Serial.print(F("rssi ..")); Serial.println(signal);
					delay(1000);
					Serial.println(F("GPRS connect .."));
					gprs.getOperatorName();
					setup_ok = true;

				} while (!setup_ok);              
			}
		}
	} while (count_init > 30 || setup_ok == false);    // 30 попыток зарегистрироваться в сети
}

void setup()
{
	//wdt_disable(); // бесполезна¤ строка до которой не доходит выполнение при bootloop Не уверен!!
	Serial.begin(115200);
	con.begin(speed_Serial);
	con.println(F("\nSIM800 setup start"));     

	Wire.begin();

	pinMode(SIM800_RESET_PIN, OUTPUT);
	digitalWrite(SIM800_RESET_PIN, LOW);            // Сигнал сброс в исходное состояние
	pinMode(LED13, OUTPUT);
	pinMode(PWR_On, OUTPUT);

	pinMode(LED_RED,  OUTPUT);
	pinMode(LED_BLUE, OUTPUT);
	pinMode(LED_GREEN,OUTPUT);
	pinMode(NETLIGHT ,INPUT);                      // Индикация NETLIGHT
	pinMode(STATUS ,INPUT);                        // Индикация STATUS

	//pinMode(digital_inDev2, INPUT);                // Цифровой вход 12
	//digitalWrite(digital_inDev2, HIGH);            // Цифровой вход 12
	//pinMode(digital_outDev3, OUTPUT); 	           // Цифровой выход
	//digitalWrite(digital_outDev3, HIGH); 	       // Цифровой выход

	setColor(COLOR_RED);
	delay(300);
	setColor(COLOR_GREEN); 
	delay(300);
	setColor(COLOR_BLUE);
	delay(300);
	setColor(COLOR_RED);

	
	//DeviceAddress deviceAddress;
	//sensor1.setOneWire(&ds18x20_1);
	//sensor2.setOneWire(&ds18x20_2);
	//sensor3.setOneWire(&ds18x20_3);
	//sensor1.begin();
	//sensor2.begin();
	//sensor3.begin();
	//if (sensor1.getAddress(deviceAddress, 0)) sensor1.setResolution(deviceAddress, 12);
	//if (sensor2.getAddress(deviceAddress, 0)) sensor2.setResolution(deviceAddress, 12);
	//if (sensor3.getAddress(deviceAddress, 0)) sensor2.setResolution(deviceAddress, 12);
	//delay(5000);                                               // Задержка 5 сек для перезаписи загрузчика, если что то не так.
	//wdt_enable(WDTO_8S);                                       // Для тестов не рекомендуется устанавливать значение менее 8 сек.

	int setup_EEPROM = 43;                                     // Произвольное число. Программа записи начальных установок при первом включении устройства после монтажа.
	byte start_eeprom = i2c_eeprom_read_byte(deviceaddress, 0);
	if(start_eeprom != setup_EEPROM)                          // Программа записи начальных установок при первом включении устройства после монтажа.
	{
		con.println (F("Start clear EEPROM"));                 //  
		for(int i = 0; i<1023;i++)
		{
			i2c_eeprom_write_byte(deviceaddress, i, 0);
		}
		i2c_eeprom_write_byte(deviceaddress, 0, setup_EEPROM);       //Записать для предотвращения повторной записи установок
		//EEPROM.put(Address_interval, interval);                // строка начальной установки интервалов
		//EEPROM.put(Address_tel1, "+79162632701");
		////EEPROM.put(Address_tel1, "+79852517615");
		//EEPROM.put(Address_SMS_center, "4556w6072556w6");
		//EEPROM.write(Address_ssl, true);
		//con.println(F("Test Watchdog"));
		//for (int i = 0; i < 15; i++)                         // Если счетчик досчитает больше 9, значит  Watchdog не работает
		//{
		//	EEPROM.write(Address_watchdog, i);               // Если произойдет перезагрузка до 9 - Watchdog работает
		//	con.println(i);
		//	delay(1000);
		//}
		con.println (F("Clear EEPROM End"));                              
	}
	
	//byte watchdog = EEPROM.read(Address_watchdog);
	//if (watchdog > 10)  con.println(F("Watchdog off"));
	//else  con.println(F("\n** Watchdog on **"));
	attachInterrupt(NETLIGHT, check_blink, RISING);                     // Включить прерывания. Индикация состояния модема(сигнал NETLIGHT) 
	//EEPROM.get(Address_interval, interval);                      // Получить из EEPROM интервал
	interval = 60;                                               // Получить из EEPROM интервал
	//ssl_set = EEPROM.read(Address_ssl);							 // Устанивить признак шифрования
	con.print(F("Interval sec:"));
	con.println(interval);
	//con.print(F("\nfree memory: "));
	//con.println(freeRam());

	//MsTimer2::set(300, flash_time);                            // 300ms период таймера прерывани
	Timer6.attachInterrupt(flash_time).start(500000);            // Индикация режимов работы SIM800


	start_init();

	int count_init = 0;                                        // Счетчик количества попыток подключиться к HTTP
	con.println(F("OK"));
	
		if (gprs.deleteSMS(0))
		{
			con.println(F("All SMS delete"));                    // 
		}
	
    sendTemps();
	Timer6.stop();
	setColor(COLOR_GREEN);                                      // Включить зеленый светодиод
	time_sutki = millis();                                            // Старт отсчета суток
	con.println(F("\nSIM800 setup end"));
}

void loop()
{
	if (digitalRead(STATUS) == LOW)
	{
		gprs.reboot(gprs.errors);                                     // Что то пошло не так программа перезапуска  если модуль не включился
	}

 check_SMS();                   // Проверить приход новых СМС

 unsigned long currentMillis = millis();
	//if(!time_set)                                                               // 
	//{
	//	 EEPROM.get( Address_interval, interval);                               // Получить интервал из EEPROM Address_interval
	//}
	if ((unsigned long)(currentMillis - previousMillis) >= interval*1000) 
	{
		con.print(F("Interval sec:"));                                       
		con.println((currentMillis-previousMillis)/1000);
		previousMillis = currentMillis;
		sendTemps();
		//con.print(F("\nfree memory: "));                                 
		//con.println(freeRam());
	}

	currentMillis = millis();

	if ((unsigned long)(currentMillis - previousPing) >= time_ping * 1000)
	{
		con.print(F("Interval ping sec:"));
		con.println((currentMillis - previousPing) / 1000);
		setColor(COLOR_BLUE);
		previousPing = currentMillis;
		setColor(COLOR_GREEN);
	}

	//dev3 = EEPROM.read(Address_Dev3);

	if (dev3 != temp_dev3)
	{
		temp_dev3 = dev3;
		if (dev3 == 0)
		{
			con.println(F("Device OFF"));
			//digitalWrite(digital_outDev3, HIGH); 	       // Цифровой выход
		}
		else
		{
			con.println(F("Device ON"));
			//digitalWrite(digital_outDev3, LOW); 	       // Цифровой выход
		}
	}

	if(millis() - time_sutki > time_day*1000)
	{
		gprs.reboot(gprs.errors);                         // вызываем reset интервалом в сутки
	}
	delay(500);
}
