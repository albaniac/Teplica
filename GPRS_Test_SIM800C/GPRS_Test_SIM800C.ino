/*************************************************************************
* Test sketch for SIM800 library
* Distributed under GPL v2.0
* Written by Stanley Huang <stanleyhuangyc@gmail.com>
* For more information, please visit http://arduinodev.com
*************************************************************************/

#include "SIM800C.h"

#define APN "connect"
#define Serial Serial
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

#define SIM800_BAUD 115200
#define SIM800_POWER_PIN 27
#define SIM800_RESET_PIN 47
#define NETLIGHT         37                              // Индикация NETLIGHT
#define STATUS           57                              // Индикация STATUS
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



void check_blink()
{
	unsigned long current_M = millis();
//	wdt_reset();
	metering_NETLIGHT = current_M - metering_temp;                            // переделать для  
	metering_temp = current_M;

	if (metering_NETLIGHT > 3055 && metering_NETLIGHT < 3070)
	{
		state_device = 2;                                                     // 2 - Зарегистрировано в сети
		count_blink2++;
		if (count_blink2 > 250)
		{
			state_device = 0;
			//MsTimer2::stop();                                                 // Включить таймер прерывания
			//gprs.reboot(gprs.errors);                                                    // Что то пошло не так с регистрацией на станции
		}
	}
	else if (metering_NETLIGHT > 855 && metering_NETLIGHT < 870)
	{
		state_device = 1;                                                     // 1 Не зарегистрирован в сети, поиск
		count_blink1++;
		if (count_blink1 > 250)
		{
			state_device = 0;
			//MsTimer2::stop();                                                 // Включить таймер прерывания
			//gprs.reboot(gprs.errors);                                                    // Что то пошло не так с регистрацией на станции
		}
	}
	else if (metering_NETLIGHT > 350 && metering_NETLIGHT < 370)
	{
		state_device = 3;                                                     // 3 - GPRS связь установлена

	}
	//Serial.println(state_device);
}







void setup_GPRS()
{
	bool setup_ok = false;
	int count_init = 0;

	pinMode(SIM800_RESET_PIN, OUTPUT);

	digitalWrite(SIM800_RESET_PIN, HIGH);                            // Сигнал сброс в исходное состояние

	delay(2000);
	pinMode(SIM800_POWER_PIN, OUTPUT);
	digitalWrite(SIM800_POWER_PIN, HIGH);                              // Сигнал сброс в исходное состояние
	delay(3000);

	do
	{
		Serial.println(F("Initializing....(May take 5-10 seconds)"));

		digitalWrite(SIM800_RESET_PIN, HIGH);                          // Сигнал сброс в исходное состояние
		delay(1000);
		digitalWrite(LED13, LOW);                                      // Светодиод индикации процесса устанавливаем в исходное состояние
		digitalWrite(SIM800_POWER_PIN, HIGH);                          // Кратковременно отключаем питание модуля GPRS (устанавливаем в исходное состояние)
		delay(1000);                                                   // Немного подождем
		while (digitalRead(STATUS) != LOW)                             // Проверяем отключение питания модуля SIM800C 
		{
			delay(100);                                                //    
		}
		Serial.println(F("Power SIM800 Off"));
//		delay(1000);
		digitalWrite(LED13, HIGH);                                     // Индикация старта включения питания модуля SIM800C 
		digitalWrite(SIM800_POWER_PIN, LOW);                           // Включение питания модуля SIM800C 
		delay(500);                                                    // Немного подождем переходные просессы при включении питания
		//digitalWrite(SIM800_RESET_PIN, HIGH);                         // Производим сброс модема после включения питания
		//delay(1200);
	//	digitalWrite(SIM800_RESET_PIN, LOW);
		int count_status = 0;                                          // Установим количество попыток включения SIM800C , иначе что то не так - сбросим микроконтроллер
		
		while (digitalRead(STATUS) == LOW)                             // Проверяем сигнал "STATUS" модуля SIM800C. Питание должно быть отключено. 
		{
			count_status++;                                            // Увеличим счетчик попыток включения
			if (count_status > 100)                                    // Если больше 100 попыток. Вызываем программу сброса микроконтроллера
			{
				//gprs.reboot(gprs.errors);                            // 100 попыток. Что то пошло не так программа перезапуска  если модуль не включился
			}
			delay(100);                                                // Включение SIM800C прошло нормально.
		}
		
		Serial.println(F("Power SIM800 On"));

		// Инициализация SIM800C
		for (;;) {
			Serial.print("Resetting...");
			while (!gprs.init(SIM800_BAUD, SIM800_POWER_PIN, SIM800_RESET_PIN))
			{
				Serial.write('.');
			}
			Serial.println("OK");

			Serial.print("Setting up network...");
	/*		byte ret = gprs.setup(APN);
			if (ret == 0)
				break;
			Serial.print("Error code:");
			Serial.println(ret);
			Serial.println(gprs.buffer);*/
			break;
		}
		Serial.println("OK");

		delay(1000);

		if (gprs.getIMEI())                                     // Получить IMEI
		{
			Serial.print(F("\nIMEI:"));
			//imei = gprs.buffer;                                 // Отключить на время отладки
			gprs.cleanStr(imei);                                // Отключить на время отладки
			Serial.println(imei);
		}









		Serial.print(F("\nSetting up mobile network..."));
		while (state_device != 2)                                // Ожидание регистрации в сети
		{
			Serial.print(F("."));
			delay(1000);
		}

		//GPRSSerial->begin(19200);                               // Скорость обмена с модемом SIM800C
		/*		while (!gprs.begin(*GPRSSerial))                        // Настройка модуля SIM800C
		{
			Serial.println(F("Couldn't find module GPRS"));
			while (1);
		}

		Serial.println(F("OK"));
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
		*/
	} while (count_init > 30 || setup_ok == false);    // 30 попыток зарегистрироваться в сети























/*
	Serial.println("SIM800C TEST");

	for (;;) {
		Serial.print("Resetting...");
		while (!gprs.init(SIM800_BAUD, SIM800_POWER_PIN, SIM800_RESET_PIN))
		{
			Serial.write('.');
		}
		Serial.println("OK");

		Serial.print("Setting up network...");
		byte ret = gprs.setup(APN);
		if (ret == 0)
			break;
		Serial.print("Error code:");
		Serial.println(ret);
		Serial.println(gprs.buffer);
	}
	Serial.println("OK");
	delay(3000);

	if (gprs.getOperatorName()) {
		Serial.print("Operator:");
		Serial.println(gprs.buffer);
	}
	int ret = gprs.getSignalQuality();
	if (ret) {
		Serial.print("Signal:");
		Serial.print(ret);
		Serial.println("dB");
	}
	for (;;) {
		if (gprs.httpInit()) break;
		Serial.println(gprs.buffer);
		gprs.httpUninit();
		delay(1000);
	}
	delay(3000);
	*/
}




void setup()
{
	Serial.begin(115200);
//	while (!Serial);
	//pinMode(SIM800_RESET_PIN, OUTPUT);
	//digitalWrite(SIM800_RESET_PIN, HIGH);                      // Сигнал сброс в исходное состояние
	//delay(500);
	pinMode(LED13, OUTPUT);
	digitalWrite(LED13, LOW);                      // Сигнал сброс в исходное состояние
	//pinMode(SIM800_POWER_PIN, OUTPUT);
	//digitalWrite(SIM800_POWER_PIN, HIGH);            // Сигнал сброс в исходное состояние

	pinMode(LED_RED, OUTPUT);
	pinMode(LED_BLUE, OUTPUT);
	pinMode(LED_GREEN, OUTPUT);
	pinMode(NETLIGHT, INPUT);                      // Индикация NETLIGHT
	pinMode(STATUS, INPUT);                        // Индикация STATUS



	setColor(COLOR_RED);
	delay(500);
	setColor(COLOR_GREEN);
	delay(500);
	setColor(COLOR_BLUE);
	delay(500);
	setColor(COLOR_RED);
	delay(300);





	attachInterrupt(37, check_blink, RISING);                     // Включить прерывания. Индикация состояния модема(сигнал NETLIGHT) 







	setup_GPRS();

	setColor(COLOR_NONE);                                      // 
	//time = millis();                                            // Старт отсчета суток
	Serial.println(F("\nSIM800 setup end"));

}

void loop()
{
	/*
	char mydata[16];
	sprintf(mydata, "t=%lu", millis());
	Serial.print("Requesting ");
	Serial.print(url);
	Serial.print('?');
	Serial.println(mydata);
	gprs.httpConnect(url, mydata);
	count++;
	while (gprs.httpIsConnected() == 0) {
		// can do something here while waiting
		Serial.write('.');
		for (byte n = 0; n < 25 && !gprs.available(); n++) {
			delay(10);
		}
	}
	if (gprs.httpState == HTTP_ERROR) {
		Serial.println("Connect error");
		errors++;
		delay(3000);
		return;
	}
	Serial.println();
	gprs.httpRead();
	int ret;
	while ((ret = gprs.httpIsRead()) == 0) {
		// can do something here while waiting
	}
	if (gprs.httpState == HTTP_ERROR) {
		Serial.println("Read error");
		errors++;
		delay(3000);
		return;
	}

	// now we have received payload
	Serial.print("[Payload]");
	Serial.println(gprs.buffer);

	// show position
	GSM_LOCATION loc;
	if (gprs.getLocation(&loc)) {
		Serial.print("LAT:");
		Serial.print(loc.lat, 6);
		Serial.print(" LON:");
		Serial.print(loc.lon, 6);
		Serial.print(" TIME:");
		Serial.print(loc.hour);
		Serial.print(':');
		Serial.print(loc.minute);
		Serial.print(':');
		Serial.println(loc.second);
	}

	// show stats  
	Serial.print("Total Requests:");
	Serial.print(count);
	if (errors) {
		Serial.print(" Errors:");
		Serial.print(errors);
	}
	Serial.println();
	*/
}


