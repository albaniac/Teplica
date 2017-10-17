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
static const char* url = "http://arduinodev.com/datetime.php";

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

void setup_GPRS()
{
	bool setup_ok = false;
	int count_init = 0;
	//Timer6.start(300000);

	//do
	//{
		con.println(F("Initializing....(May take 5-10 seconds)"));

		// Инициализация SIM800C
		for (;;) 
		{
			con.print("Resetting...");
			while (!gprs.init(SIM800_BAUD, SIM800_POWER_PIN, SIM800_RESET_PIN, STATUS_PIN))
			{
				con.write('.');
			}
			con.println("OK");

		//	con.print("Setting up network...");

	/*		byte ret = gprs.setup(APN);
			if (ret == 0)
				break;
			con.print("Error code:");
			con.println(ret);
			con.println(gprs.buffer);*/
			break;
		}

		con.println("Init OK");

		//delay(1000);

		//if (gprs.getIMEI())                                     // Получить IMEI
		//{
		//	con.print(F("\nIMEI:"));
		//	//imei = gprs.buffer;                                 // Отключить на время отладки
		//	gprs.cleanStr(imei);                                // Отключить на время отладки
		//	con.println(imei);
		//}


		//if (gprs.getSIMCCID())                               // Получить Номер СИМ карты
		//{
		//	con.print(F("\nSIM CCID:"));
		//	SIMCCID = gprs.buffer1;
		//	gprs.cleanStr(SIMCCID);
		//	con.println(SIMCCID);
		//}

		//
		//con.print(F("\nSetting up mobile network..."));
		//while (state_device != 2)                                // Ожидание регистрации в сети
		//{
		//	con.print(F("."));
		//	delay(1000);
		//}
		//
		//char n = gprs.getNetworkStatus();

		//con.print(F("\nNetwork status "));
		//con.print(n);
		//con.print(F(": "));
		//if (n == '0') con.println(F("\nNot registered"));                      // 0 – не зарегистрирован, поиска сети нет
		//if (n == '1') con.println(F("\nRegistered (home)"));                   // 1 – зарегистрирован, домашняя сеть
		//if (n == '2') con.println(F("\nNot registered (searching)"));          // 2 – не зарегистрирован, идёт поиск новой сети
		//if (n == '3') con.println(F("\nDenied"));                              // 3 – регистрация отклонена
		//if (n == '4') con.println(F("\nUnknown"));                             // 4 – неизвестно
		//if (n == '5') con.println(F("\nRegistered roaming"));                  // 5 – роуминг

		//GPRSSerial->begin(19200);                               // Скорость обмена с модемом SIM800C
		/*		while (!gprs.begin(*GPRSSerial))                        // Настройка модуля SIM800C
		{
			con.println(F("Couldn't find module GPRS"));
			while (1);
		}

		con.println(F("OK"));
		con.print(F("\nSetting up mobile network..."));
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
					setup_ok = true;

				} while (!setup_ok);
			}
		}
		*/
	//} while (count_init > 30 || setup_ok == false);    // 30 попыток зарегистрироваться в сети























	con.println("SIM800C TEST");

	for (;;) {
		con.print("Resetting...");
		while (!gprs.init(SIM800_BAUD, SIM800_POWER_PIN, SIM800_RESET_PIN, STATUS_PIN))
		{
			con.write('.');
		}
		con.println("OK");

		con.print("Setting up network...");
		byte ret = gprs.setup(APN);
		if (ret == 0)
			break;
		con.print("Error code:");
		con.println(ret);
		con.println(gprs.buffer);
	}
	con.println("OK");
	delay(3000);

	if (gprs.getOperatorName()) {
		con.print("Operator:");
		con.println(gprs.buffer);
	}
	int ret = gprs.getSignalQuality();
	if (ret) {
		con.print("Signal:");
		con.print(ret);
		con.println("dB");
	}
	for (;;) {
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

//	while (!con);	pinMode(LED13, OUTPUT);
	digitalWrite(LED13, LOW);                          // Индикация светодиода исходное состояние
	pinMode(SIM800_RESET_PIN, OUTPUT);
	//digitalWrite(SIM800_RESET_PIN, HIGH);            // Сигнал сброс в исходное состояние
	//delay(500);
 //	pinMode(SIM800_POWER_PIN, OUTPUT);                 // Отключить питание SIM800 
	//digitalWrite(SIM800_POWER_PIN, LOW);              // Отключить питание SIM800 
	//delay(1000);
	//digitalWrite(SIM800_RESET_PIN, LOW);            // Сигнал сброс в исходное состояние
	//delay(100);
	//digitalWrite(SIM800_RESET_PIN, HIGH);            // Сигнал сброс в исходное состояние
	//delay(1000);
	


	//pinMode(LED_RED, OUTPUT);
	//pinMode(LED_BLUE, OUTPUT);
	//pinMode(LED_GREEN, OUTPUT);
	//////pinMode(NETLIGHT_PIN, INPUT);                        // Индикация NETLIGHT (подключено аппаратное прерывание)
	////pinMode(STATUS, INPUT);                            // Индикация STATUS (контроль включения питания SIM800)



	//setColor(COLOR_RED);
	//delay(500);
	//setColor(COLOR_GREEN);
	//delay(500);
	//setColor(COLOR_BLUE);
	//delay(500);
	//setColor(COLOR_RED);
	//delay(300);





	//attachInterrupt(NETLIGHT_PIN, check_blink, RISING);                     // Включить прерывания. Индикация состояния модема(сигнал NETLIGHT) 
	//Timer3.attachInterrupt(firstHandler).start(500000); // Every 500ms
	//Timer4.attachInterrupt(secondHandler).setFrequency(1).start();
	//Timer5.attachInterrupt(thirdHandler).setFrequency(10);
	//Timer6.attachInterrupt(flash_time);                          // Индикация режимов работы SIM800



	setup_GPRS();

	//setColor(COLOR_NONE);                                      // 
	setColor(COLOR_GREEN);
	//time = millis();                                            // Старт отсчета суток
	con.println(F("\nSIM800 setup end"));
	
}

void loop()
{
	
	char mydata[16];
	sprintf(mydata, "t=%lu", millis());
	con.print("Requesting ");
	con.print(url);
	con.print('?');
	con.println(mydata);
	gprs.httpConnect(url, mydata);
	count++;
	while (gprs.httpIsConnected() == 0) {
		// can do something here while waiting
		con.write('.');
		for (byte n = 0; n < 25 && !gprs.available(); n++) {
			delay(10);
		}
	}
	if (gprs.httpState == HTTP_ERROR) {
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


