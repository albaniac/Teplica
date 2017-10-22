/*************************************************************************
* Test sketch for SIM800 library
* Distributed under GPL v2.0
* Written by Stanley Huang <stanleyhuangyc@gmail.com>
* For more information, please visit http://arduinodev.com
*************************************************************************/

#include "SIM800C.h"
#include <DueTimer.h>


#define APN "connect"

#define con Serial
//static const char* url = "http://arduinodev.com/datetime.php";
static const char* url = "http://207.154.222.114:8080/echo/";



CGPRS_SIM800C gprs;
uint32_t count = 0;
uint32_t errors = 0;

#define LED13    13                                    // Индикация светодиодом

// Настройки для контроллера теплицы
//#define SIM800_BAUD 115200
//#define SIM800_POWER_PIN 65
//#define SIM800_RESET_PIN 67

// Настройки для контроллера OBDII

#define SIM800_BAUD      115200
#define SIM800_POWER_PIN 27
#define SIM800_RESET_PIN 47
#define NETLIGHT_PIN     37                              // Индикация NETLIGHT
#define STATUS_PIN       57                              // Индикация STATUS
//#define COMMON_ANODE                                   // Если светодиод с общим катодом - раскомментировать
#define LED_RED      54                                  // Индикация светодиодом RED
#define LED_BLUE     55                                  // Индикация светодиодом BLUE
#define LED_GREEN    56                                  // Индикация светодиодом GREEN

#define COLOR_NONE LOW, LOW, LOW                        // Отключить все светодиоды
#define COLOR_GREEN LOW, HIGH, LOW                      // Включить зеленый светодиод
#define COLOR_BLUE LOW, LOW, HIGH                       // Включить синий светодиод
#define COLOR_RED HIGH, LOW, LOW                        // Включить красный светодиод
volatile int stateLed = LOW;                            // Состояние светодиода при прерывистой индикации на старте
volatile int state_device = 0;                          // Состояние модуля при запуске 

														// 1 - Не зарегистрирован в сети, поиск
														// 2 - Зарегистрировано в сети
														// 3 - GPRS связь установлена
volatile int metering_NETLIGHT = 0;
volatile unsigned long metering_temp = 0;
volatile int count_blink1 = 0;                          // Счетчик попыток подключиться к базовой станции
volatile int count_blink2 = 0;                          // Счетчик попыток подключиться к базовой станции
bool send_ok = false;                                   // Признак успешной передачи данных
bool count_All_reset = false;                           // Признак выполнения команды сброса счетчика ошибок.
String imei = "861445030362268";                        // Тест IMEI
String SIMCCID = "";
#define DELIM "&"

unsigned long previousMillis = 0;                      //  
unsigned long interval = 10;                           // Интервал передачи данных 5 минут
bool time_set = false;                                 // Фиксировать интервал заданный СМС
bool ssl_set = true;                                   // Признак шифрования 
unsigned long time_ping = 380;                         // Интервал проверки ping 6 минут. 
unsigned long previousPing = 0;







void firstHandler() 
{
	con.println("[-  ] First Handler!");
}

void secondHandler() 
{
	con.println("[ - ] Second Handler!");
}

void thirdHandler() 
{
	con.println("[  -] Third Handler!");
}

void setColor(bool red, bool green, bool blue)         // Включение цвета свечения трехцветного светодиода.
{
#ifdef COMMON_ANODE                                    // Если светодиод с общим катодом
	red = !red;
	green = !green;
	blue = !blue;
#endif 
	digitalWrite(LED_RED, red);
	digitalWrite(LED_GREEN, green);
	digitalWrite(LED_BLUE, blue);
}

void flash_time()                                       // Программа обработчик прерывистого свечения светодиодов при старте
{                                                       // 
	if (state_device == 0)                              // 
	{
		setColor(COLOR_RED);
	}
	if (state_device == 1)                             // Мигающий красный светодиод - поиск станции
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

	if (state_device == 2)                             // Мигающий синий светодиод, поиск станции завершен - регистрация
	{
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

		if (state_device == 3)                            // Мигающий зеленый светодиод - регистрация выполнена, индикация подключения к интернету
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
}

void check_blink()
{
	// Подпрограмма контроля состояния модуля SIM800C (контролируется сигнал NETLIGHT по аппаратному прерыванию)
	unsigned long current_M = millis();
//	wdt_reset();
	metering_NETLIGHT = current_M - metering_temp;                            // Измерение длительности сигнала NETLIGHT
	metering_temp = current_M;

	if (metering_NETLIGHT > 3055 && metering_NETLIGHT < 3070)
	{
		state_device = 2;                                                     // 2 - Зарегистрировано в сети
		count_blink2++;
		if (count_blink2 > 250)
		{
			state_device = 0;
			Timer6.stop();                                                   // Выключить таймер прерывания индикации светодиодом
			//gprs.reboot(gprs.errors);                                      // Что то пошло не так с регистрацией на станции
		}
	}
	else if(metering_NETLIGHT > 855 && metering_NETLIGHT < 870)
	{
		state_device = 1;                                                     // 1 Не зарегистрирован в сети, поиск
		count_blink1++;
		if (count_blink1 > 250)                                               // В сети не зарегистрирован - перезагрузка
		{
			state_device = 0;
			Timer6.stop();                                                    // Выключить таймер прерывания индикации светодиодом
			//gprs.reboot(gprs.errors);                                       // Что то пошло не так с регистрацией на станции
		}
	}
	else if(metering_NETLIGHT > 350 && metering_NETLIGHT < 370)
	{
		state_device = 3;                                                     // 3 - GPRS связь установлена

	}
}



void send_data()
{

	byte signal = gprs.getSignalQuality();

	String toSend = "IM=" + imei + DELIM + "S=" + String(signal) + DELIM + "E=" + String(gprs.errors) ;

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
		//	check_SMS();                                   // Проверить приход новых СМС 
			Serial.print("Attempt to transfer data .."); Serial.println(count_send);
		}
		delay(2000);                                       // Подождать 5 секунд
	}

}





bool gprs_send(String data)
{
	con.print(F("Requesting .. Wait"));
	setColor(COLOR_BLUE);
	static const char* url1 = "http://trm7.xyz/r.php";
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
	/*
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
		*/
		con.print(url1);
		//con.print('?');
		con.println(data);
		gprs.httpConnectStr(url1, data);
	//}
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
//		return;
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
		//return;
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

	for (int i = 0; i<7; i++)
	{
		String comm = val.substring(p0[i] + 3, p0[i] + 5);          // Выделить строку с номером команды
		int comm1 = comm.toInt();
		Serial.println(comm1);
		comm = val.substring(p0[i] + 5, p0[i + 1]);               // Выделить строку с данными
		int len_str = comm.length();
		comm[len_str] = '\0';
		Serial.println(comm);

	//	run_command(comm1, comm);                               // Последовательно выполнить все команды

	}

	// Показать статистику
	con.print(F("Total: "));
	con.print(count);
	if (gprs.errors)                                                 // Если есть ошибки - сообщить
	{
		con.print(F(" Errors: "));
		con.print(gprs.errors);
	}
	//con.println();
	//Serial.print("Inteval: ");
	//Serial.println(interval);
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









void setup_GPRS()
{
	bool setup_ok = false;
	int count_init = 0;                            // Установим количество попыток включения SIM800C , иначе что то не так - сбросим микроконтроллер
	Timer6.start(300000); 

	do
	{
		con.println(F("Initializing....(May take 5-10 seconds)"));

		digitalWrite(SIM800_POWER_PIN, HIGH);                        // Pin включения SIM800C
		delay(100);
		digitalWrite(SIM800_RESET_PIN, HIGH);                        // Pin включения SIM800C в исходное состояние
		delay(1000);
		while (digitalRead(STATUS_PIN) != LOW)                       // Проверяем отключение питания модуля SIM800C 
		{
			delay(100);                                              //    
		}

#ifdef DEBUG
		DEBUG.println(F("\nPower SIM800 Off"));
#endif

		delay(1000);
		digitalWrite(SIM800_POWER_PIN, LOW);                                   // Подать питание на SIM800C    
		delay(1000);
		digitalWrite(SIM800_RESET_PIN, LOW);                                   // Вкючить питание SIM800C
		delay(100);
		digitalWrite(SIM800_RESET_PIN, HIGH);                                  // Pin включения SIM800C в исходное состояние
		delay(1000);

		while (digitalRead(STATUS_PIN) == LOW)                       // Проверяем сигнал "STATUS" модуля SIM800C. Питание должно быть включено. 
		{
			count_init++;                                           // Увеличим счетчик попыток включения
			if (count_init > 100)                                   // Если больше 100 попыток. Вызываем программу сброса микроконтроллера
			{
				//gprs.reboot(gprs.errors);                           // 100 попыток. Что то пошло не так программа перезапуска  если модуль не включился
			}
			delay(100);                                               // Включение SIM800C прошло нормально.
		}

#ifdef DEBUG
		DEBUG.println(F("Power SIM800 On"));
#endif

			con.print("Resetting...");
			while (!gprs.init(SIM800_BAUD))
			{
				con.write('.');
			}

		con.println("Init OK");

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

		
		con.print(F("\nSetting up mobile network..."));
		while (state_device != 2)                                // Ожидание регистрации в сети
		{
			con.print(F("."));
			delay(1000);
		}
		
		char n = gprs.getNetworkStatus();

		con.print(F("\nNetwork status "));
		con.print(n);
		con.print(F(": "));
		if (n == '0') con.println(F("\nNot registered"));                      // 0 – не зарегистрирован, поиска сети нет
		if (n == '1') con.println(F("\nRegistered (home)"));                   // 1 – зарегистрирован, домашняя сеть
		if (n == '2') con.println(F("\nNot registered (searching)"));          // 2 – не зарегистрирован, идёт поиск новой сети
		if (n == '3') con.println(F("\nDenied"));                              // 3 – регистрация отклонена
		if (n == '4') con.println(F("\nUnknown"));                             // 4 – неизвестно
		if (n == '5') con.println(F("\nRegistered roaming"));                  // 5 – роуминг

		if (n == '1' || n == '5')                                                 // Если домашняя сеть или роуминг
		{
			if (state_device == 2)                                                // Проверить аппаратно подключения модема к оператору
			{
				do
				{
					byte signal = gprs.getSignalQuality();
					con.print(F("rssi ..")); con.println(signal);
					delay(1000);
					con.println(F("GPRS connect .."));
					gprs.getOperatorName();
					gprs.connect_GPRS();
					setup_ok = true; 
					 
				} while (!setup_ok);
			}
		}
	} while (count_init > 30 || setup_ok == false);    // 30 попыток зарегистрироваться в сети


	

	for (;;)
	{
		gprs.connect_GPRS(); break;
	}

	if (gprs.getOperatorName()) 
	{
		con.print("Operator:");
		con.println(gprs.buffer);
	}
	int ret = gprs.getSignalQuality();
	if (ret) 
	{
		con.print("Signal:");
		con.print(ret);
		con.println("dB");
	}
	
	for (;;) 
	{
		if (gprs.httpInit()) break;
		con.println(gprs.buffer);
		gprs.httpUninit();
		delay(1000);
	}
	delay(3000);
}



void setup()
{
	Serial.begin(115200);
	con.begin(115200);

//	while (!con);	
	pinMode(LED13, OUTPUT);
	digitalWrite(LED13, LOW);                             // Индикация светодиода исходное состояние
	pinMode(SIM800_POWER_PIN, OUTPUT);                    // Pin управления питанием SIM800C
	pinMode(SIM800_RESET_PIN, OUTPUT);                    // Pin включения SIM800C
	pinMode(NETLIGHT_PIN, INPUT);                         // Индикация NETLIGHT (подключено аппаратное прерывание)
	pinMode(STATUS_PIN, INPUT);                           // Индикация STATUS (контроль включения питания SIM800)

	digitalWrite(SIM800_POWER_PIN, HIGH);                                   // Pin включения SIM800C
	delay(100);
	digitalWrite(SIM800_RESET_PIN, HIGH);                                   // Pin включения SIM800C в исходное состояние
	delay(100);



	pinMode(LED_RED, OUTPUT);
	pinMode(LED_BLUE, OUTPUT);
	pinMode(LED_GREEN, OUTPUT);


	setColor(COLOR_RED);
	delay(500);
	setColor(COLOR_GREEN);
	delay(500);
	setColor(COLOR_BLUE);
	delay(500);
	setColor(COLOR_RED);
	delay(300);
	
	attachInterrupt(NETLIGHT_PIN, check_blink, RISING);                     // Включить прерывания. Индикация состояния модема(сигнал NETLIGHT) 
	//Timer3.attachInterrupt(firstHandler).start(500000); // Every 500ms
	//Timer4.attachInterrupt(secondHandler).setFrequency(1).start();
	//Timer5.attachInterrupt(thirdHandler).setFrequency(10);
	Timer6.attachInterrupt(flash_time);                                     // Индикация режимов работы SIM800

	setup_GPRS();
	int count_init = 0;                                        // Счетчик количества попыток подключиться к HTTP
	con.println(F("OK"));

	if (gprs.deleteSMS(0))
	{
		con.println(F("All SMS delete"));                    // 
	}
	//send_data();
	//setColor(COLOR_NONE);                                      // 
	setColor(COLOR_GREEN);
	//time = millis();                                            // Старт отсчета суток
	con.println(F("\nSIM800 setup end"));
	
}

void loop()
{
	if (digitalRead(STATUS_PIN) == LOW)
	{
		gprs.reboot(gprs.errors);                                     // Что то пошло не так программа перезапуска  если модуль не включился
	}


	//unsigned long currentMillis = millis();
	//if ((unsigned long)(currentMillis - previousMillis) >= interval * 1000)
	//{
	//	con.print(F("Interval sec:"));
	//	con.println((currentMillis - previousMillis) / 1000);
	//	previousMillis = currentMillis;
	//	send_data();
	//	//con.print(F("\nfree memory: "));
	//	//con.println(freeRam());
	//}

	//currentMillis = millis();





	
	char mydata[16];
//	sprintf(mydata, "t=%lu", millis());
	sprintf(mydata, "Test ");
	con.print("Requesting ");
	con.print(url);
	//con.print('?');
	con.println(mydata);
	gprs.httpConnect(url, mydata);
	count++;
	while (gprs.httpIsConnected() == 0) 
	{
		// can do something here while waiting
		con.write('.');
		for (byte n = 0; n < 25 && !gprs.available(); n++) 
		{
			delay(10);
		}
	}
	if (gprs.httpState == HTTP_ERROR)
	{
		con.println("Connect error");
		errors++;
		delay(3000);
		return;
	}
	con.println();
	gprs.httpRead();
	int ret;
	while ((ret = gprs.httpIsRead()) == 0) {
		// can do something here while waiting
	}
	if (gprs.httpState == HTTP_ERROR) {
		con.println("Read error");
		errors++;
		delay(3000);
		return;
	}

	// now we have received payload
	con.print("[Payload]");
	con.println(gprs.buffer);

	// show position
	GSM_LOCATION loc;
	if (gprs.getLocation(&loc)) {
		con.print("LAT:");
		con.print(loc.lat, 6);
		con.print(" LON:");
		con.print(loc.lon, 6);
		con.print(" TIME:");
		con.print(loc.hour);
		con.print(':');
		con.print(loc.minute);
		con.print(':');
		con.println(loc.second);
	}
	// show stats  
	con.print("Total Requests:");
	con.print(count);
	if (errors) {
		con.print(" Errors:");
		con.print(errors); 
	}
	con.println();
	
}


