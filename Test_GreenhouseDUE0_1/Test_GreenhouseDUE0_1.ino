/*

Пин	Наименование	Применение
0	RX0          	Порт программирования  Programming port USB
1	TX0	            Порт программирования  Programming port USB
2	D2	            PWM2  Utouch тачскрин   D_IRQ
3	D3	            PWM3  Utouch тачскрин   D_OUT
4	D4	            PWM4  Utouch тачскрин   D_DIN
5	D5	            PWM5  Utouch тачскрин   D_CS
6	D6	            PWM6  Utouch тачскрин   D_CLK
7	D7	            PWM7  ST7920 BLA(19)
8	D8	            PWM8 SIM800C NETLIGHT (attachInterrupt (8) DUE)
9	D9	            PWM9 SIM800C STATUS (attachInterrupt (9) DUE)
10	D10	            PWM10 WL5100
11	D11	            PWM11  Счетчик воды №1
12	D12            	PWM12  Счетчик воды №2
13	D13	            PWM13  LED13
14	TX3	            Nextion   / RS485
15	RX3	            Nextion   / RS485
16	TX1	            ESP8266 RX
17	RX1         	ESP8266 TX
18	TX2	            SIM800C RX
19	RX2	            SIM800C TX
20	SDA	            SDA0_3
21	SCL         	SCL0_3
22	D22	            Display UTFT  DB8    (DB10)
23	D23          	Display UTFT  DB9    (DB11)
24	D24          	Display UTFT  DB10  (DB12)
25	D25	            Display UTFT  DB11  (DB13)
26	D26         	Display UTFT  DB12  (DB14)
27	D27         	Display UTFT  DB13  (DB15)
28	D28	            Display UTFT  DB14  (DB16)
29	D29	            Display UTFT  DB15  (DB17)
30	D30	            Display UTFT  DB7
31	D31	            Display UTFT  DB6
32	D32	            Display UTFT  DB5
33	D33	            Display UTFT  DB4
34	D34	            Display UTFT  DB3
35	D35           	Display UTFT  DB2
36	D36         	Display UTFT  DB1
37	D37	            Display UTFT  DB0
38	D38	            Display UTFT  RS
39	D39           	Display UTFT  WR
40	D40	            Display UTFT  CS
41	D41	            Display UTFT  RST
42	D42         	74HC595  ST_CP
43	D43	            74HC595  DS
44	D44         	74HC595  SH_CP
45	D45	            74HC595  OE
46	D46	            MCP23S17 CS
47	D47	            nRF24L01+  CE
48	D48          	nRF24L01+  CSN
49	D49         	Регистратор модулей
50	D50	            DHT11/DHT22
51	D51	            DHT11/DHT22
52	D52	            SD CS
53	D53 / CANTX1/IO	ST7920 RS
54	A0 / D54	    DS1820  Т снаружи
55	A1 / D55	    DS1821  Т внутри N1
56	A2 / D56	    DS1822  Т внутри N2
57	A3 / D57	    Аналоговые датчики влажности
58	A4 / D58	    Аналоговые датчики влажности
59	A5 / D59	    Питание ESP8266
60	A6 / D60	    Питание nRF24L01+
61	A7 / D61	    Питание W5100,  Mini W5100
62	A8 / D62	    Encoder Vin+  (1)
63	A9 / D63	    Encoder Vin-  (3)
64	A10 / D64	    Encoder SW2  (5)
65	A11 / D65	    ПитаниеSIM800C  Power  ON/OFF SIM800C
66	D66/DAC0	    Сброс сторожевого таймера
67	D67/DAC1	    PWRKEY   SIM800C
68	D68 / CANRX0	RX/TX RS485
69	D69 / CANTX0	Mini W5100 (4)
70	D70/SDA1	    Свободен  / вариант 2     I2C
71	D71/SCL1	    Свободен  / вариант 2     I2C
72	D72/ RX	        Управление светодиодом RX (не использовать)
73	D73/TX	        Управление светодиодом TX (не использовать)
74	D74 / MISO	    MISO
75	D75 / MOSI	    ST7920 R/W
76	D76 / SPCK (SCK)	ST7920 E

USB Native	        Native port USB


адр1	MCP23S17	Модуль управления досветкой  (адр1)
адр2	MCP23S17	Модуль резерв (адр2)
адр3	MCP23S17	Датчик окно отрыто (адр3)
адр4	MCP23S17	Датчик окно закрыто  (адр4)
адр5	MCP23S17	Управление поливом 14 каналов  (адр5)
адр6	MCP23S17	Модуль расширения портов (адр6)
адр6	MCP23S17	A0 - USE_READY_DIODE
адр6	MCP23S17	A1 - USE_WINDOWS_MANUAL_MODE_DIODE
адр6	MCP23S17	A2 - USE_WATERING_MANUAL_MODE_DIODE
адр6	MCP23S17	A3 - USE_LIGHT_MANUAL_MODE_DIODE
адр6	MCP23S17	A4 - USE_GSM_BLUE_DIODE
адр6	MCP23S17	A5 - USE_GSM_GREEN_DIODE
адр6	MCP23S17	A6 - USE_GSM_RED_DIODE
адр6	MCP23S17	A7 - SOUNDER (пищалка)



*/

#include <SPI.h>
#include <SD.h>
#include "RF24.h"
#include <MCP23S17.h>
#include <DueTimer.h>
#include "SIM800.h"


const int ledPin13 = 13;
const int power_nRF24 = 60;
const uint8_t chipSelect_MCP = 46;
const int timer_pin = 66;
bool timer_set = false;
// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// MKRZero SD: SDCARD_SS_PIN
const int chipSelect_SD = 52;








#define ESP8266 Serial1
String SSID = "WiFi-Repeater";
String PASSWORD = "panasonic";

int Power_ESP8266 = 59;
boolean FAIL_8266 = false;



void clearESP8266SerialBuffer(String title = "")
{
	Serial.println("= clearESP8266SerialBuffer(" + title + ") =");
	while (ESP8266.available() > 0) {
		char a = ESP8266.read();
		Serial.write(a);
	}
	Serial.println("==============================");
}






/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 0;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(47, 48);
/**********************************************************/

byte addresses[][6] = { "1Node","2Node" };

// Used to control whether this node is sending or receiving
bool role = 0;


MCP23S17 Bank1(&SPI, chipSelect_MCP, 1);
MCP23S17 Bank2(&SPI, chipSelect_MCP, 2);
MCP23S17 Bank3(&SPI, chipSelect_MCP, 3);
MCP23S17 Bank4(&SPI, chipSelect_MCP, 4);
MCP23S17 Bank5(&SPI, chipSelect_MCP, 5);
MCP23S17 Bank6(&SPI, chipSelect_MCP, 6);


/*
Shift Register Example
for 74HC595 shift register

Created 22 May 2009
Created 23 Mar 2010
by Tom Igoe

*/

//Пин подключен к ST_CP входу 74HC595
int latchPin = 42;
//Пин подключен к SH_CP входу 74HC595
int clockPin = 44;
//Пин подключен к DS входу 74HC595
int dataPin = 43;
int OE_Pin = 45;
char inputString[2];






#define SIM800CSerial Serial2
#define con Serial

#define PWR_SIM800       65                              // Включение питания модуля SIM800
//#define PWR_SIM800       27                              // Включение питания модуля SIM800
#define SIM800_RESET_PIN 67                              // Сброс модуля SIM800
//#define SIM800_RESET_PIN 47                              // Сброс модуля SIM800



#define LED13           13                               // Индикация светодиодом
#define NETLIGHT         8                               // Индикация NETLIGHT
#define STATUS           9                               // Индикация STATUS
//#define analog_dev1      A5                             // Аналоговый вход 1
//#define digital_inDev2   12                             // Цифровой вход 12
//#define digital_outDev3  A4                             // Цифровой выход управления внешним устройством


#define APN "connect"
#define con Serial
static const char* url = "http://arduinodev.com/datetime.php";

CGPRS_SIM800 gprs;
uint32_t count = 0;
uint32_t errors = 0;

//#define COMMON_ANODE                                  // Если светодиод с общим катодом - раскомментировать
#define LED_RED      6                                 // Индикация светодиодом RED
#define LED_BLUE     4                                 // Индикация светодиодом BLUE
#define LED_GREEN    5                                 // Индикация светодиодом GREEN

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
volatile int count_blink1 = 0;                           // Счетчик попыток подключиться к базовой станции
volatile int count_blink2 = 0;                           // Счетчик попыток подключиться к базовой станции
bool send_ok = false;                                    // Признак успешной передачи данных
bool count_All_reset = false;                            // Признак выполнения команды сброса счетчика ошибок.
bool temp_dev3 = false;                                  // Переменная для временного хранения состояния исполнительного устройства
String imei = "861445030362268";                         // Тест IMEI
String SIMCCID = "";

//CGPRS_SIM800 gprs;
//int count = 0;
//unsigned int errors           = 0;
#define DELIM "&"

unsigned long previousMillis = 0;                        //  
unsigned long interval = 60;                             // Интервал передачи данных 5 минут
bool time_set = false;                                   // Фиксировать интервал заданный СМС
bool ssl_set = true;                                     // Признак шифрования 
unsigned long time_ping = 380;                           // Интервал проверки ping 6 минут. 
unsigned long previousPing = 0;                          // Временный Интервал проверки ping

int Address_tel1 = 100;                                  // Адрес в EEPROM телефона 1
int Address_ssl = 120;                                   // Адрес в EEPROM признака шифрования
											             //int Address_errorAll      = 160;                        // Адрес в EEPROM счетчика общих ошибок
int Address_interval = 200;                              // Адрес в EEPROM величины интервала
int Address_SMS_center = 220;                            // Адрес в EEPROM SMS центра
//int Address_Dev3 = 260;                                  // Адрес в EEPROM состояния исполнительного устройства Dev
//int Address_Dev3_ind = 264;                              // Адрес в EEPROM признак управления сполнительного устройства Dev
//int Address_num_site_ping = 268;                         // Адрес в EEPROM признак управления сполнительного устройства Dev
//int Address_watchdog = 270;                              // Адрес в EEPROM счетчик проверки Watchdog
//int Address_EEPROM_off = 280;                            // Адрес в EEPROM запрет записи в EEPROM
//byte dev3 = 0;                                           // признак управления исполнительного устройства Dev3


void setColor(bool red, bool green, bool blue)        // Включение цвета свечения трехцветного светодиода.
{
#ifdef COMMON_ANODE                              // Если светодиод с общим катодом
	red = !red;
	green = !green;
	blue = !blue;
#endif 

	Bank6.digitalWrite(LED_RED, red);
	Bank6.digitalWrite(LED_GREEN, green);
	Bank6.digitalWrite(LED_BLUE, blue);
}



void firstHandler() {
	//Serial.println("[-  ] First Handler!");
	digitalWrite(timer_pin, timer_set);
	timer_set = !timer_set;

}

void secondHandler() {
	Serial.println("[ - ] Second Handler!");
}

void thirdHandler() {
	Serial.println("[  -] Third Handler!");
}







void setup_SD_Fat()
{


	Serial.print("\nInitializing SD card...");

	// we'll use the initialization code from the utility libraries
	// since we're just testing if the card is working!
	if (!card.init(SPI_HALF_SPEED, chipSelect_SD)) {
		Serial.println("initialization failed. Things to check:");
		Serial.println("* is a card inserted?");
		Serial.println("* is your wiring correct?");
		Serial.println("* did you change the chipSelect pin to match your shield or module?");
		return;
	}
	else {
		Serial.println("Wiring is correct and a card is present.");
	}

	// print the type of card
	Serial.print("\nCard type: ");
	switch (card.type()) {
	case SD_CARD_TYPE_SD1:
		Serial.println("SD1");
		break;
	case SD_CARD_TYPE_SD2:
		Serial.println("SD2");
		break;
	case SD_CARD_TYPE_SDHC:
		Serial.println("SDHC");
		break;
	default:
		Serial.println("Unknown");
	}

	// Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
	if (!volume.init(card)) {
		Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
		return;
	}


	// print the type and size of the first FAT-type volume
	uint32_t volumesize;
	Serial.print("\nVolume type is FAT");
	Serial.println(volume.fatType(), DEC);
	Serial.println();

	volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
	volumesize *= volume.clusterCount();       // we'll have a lot of clusters
	volumesize *= 512;                            // SD card blocks are always 512 bytes
	Serial.print("Volume size (bytes): ");
	Serial.println(volumesize);
	Serial.print("Volume size (Kbytes): ");
	volumesize /= 1024;
	Serial.println(volumesize);
	Serial.print("Volume size (Mbytes): ");
	volumesize /= 1024;
	Serial.println(volumesize);


	Serial.println("\nFiles found on the card (name, date and size in bytes): ");
	root.openRoot(volume);

	// list all files in the card with date and size
	root.ls(LS_R | LS_DATE | LS_SIZE);


}
void setup_nRF24()
{
	Serial.println(F("RF24/examples/GettingStarted"));
	Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));

	radio.begin();

	// Set the PA Level low to prevent power supply related issues since this is a
	// getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
	radio.setPALevel(RF24_PA_LOW);

	// Open a writing and reading pipe on each radio, with opposite addresses
	if (radioNumber) {
		radio.openWritingPipe(addresses[1]);
		radio.openReadingPipe(1, addresses[0]);
	}
	else {
		radio.openWritingPipe(addresses[0]);
		radio.openReadingPipe(1, addresses[1]);
	}

	// Start the radio listening for data
	radio.startListening();

}
void test_nRF24()
{

	/****************** Ping Out Role ***************************/
	if (role == 1) {

		radio.stopListening();                                    // First, stop listening so we can talk.


		Serial.println(F("Now sending"));

		unsigned long start_time = micros();                             // Take the time, and send it.  This will block until complete
		if (!radio.write(&start_time, sizeof(unsigned long))) {
			Serial.println(F("failed"));
		}

		radio.startListening();                                    // Now, continue listening

		unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
		boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not

		while (!radio.available()) {                             // While nothing is received
			if (micros() - started_waiting_at > 200000) {            // If waited longer than 200ms, indicate timeout and exit while loop
				timeout = true;
				break;
			}
		}

		if (timeout) {                                             // Describe the results
			Serial.println(F("Failed, response timed out."));
		}
		else {
			unsigned long got_time;                                 // Grab the response, compare, and send to debugging spew
			radio.read(&got_time, sizeof(unsigned long));
			unsigned long end_time = micros();

			// Spew it
			Serial.print(F("Sent "));
			Serial.print(start_time);
			Serial.print(F(", Got response "));
			Serial.print(got_time);
			Serial.print(F(", Round-trip delay "));
			Serial.print(end_time - start_time);
			Serial.println(F(" microseconds"));
		}

		// Try again 1s later
		delay(1000);
	}



	/****************** Pong Back Role ***************************/

	if (role == 0)
	{
		unsigned long got_time;

		if (radio.available()) {
			// Variable for the received timestamp
			while (radio.available()) {                                   // While there is data ready
				radio.read(&got_time, sizeof(unsigned long));             // Get the payload
			}

			radio.stopListening();                                        // First, stop listening so we can talk   
			radio.write(&got_time, sizeof(unsigned long));              // Send the final one back.      
			radio.startListening();                                       // Now, resume listening so we catch the next packets.     
			Serial.print(F("Sent response "));
			Serial.println(got_time);
		}
	}




	/****************** Change Roles via Serial Commands ***************************/

	if (Serial.available())
	{
		char c = toupper(Serial.read());
		if (c == 'T' && role == 0) {
			Serial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));
			role = 1;                  // Become the primary transmitter (ping out)

		}
		else
			if (c == 'R' && role == 1) {
				Serial.println(F("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK"));
				role = 0;                // Become the primary receiver (pong back)
				radio.startListening();

			}
	}
}
void setup_MCP()
{
	Bank1.begin();
	Bank2.begin();
	Bank3.begin();
	Bank4.begin();
	Bank5.begin();
	Bank6.begin();

	for (int i = 0; i < 16; i++)
	{
		Bank1.pinMode(i, OUTPUT);
		Bank2.pinMode(i, OUTPUT);
		Bank3.pinMode(i, OUTPUT);
		Bank4.pinMode(i, OUTPUT);
		Bank5.pinMode(i, OUTPUT);
		Bank6.pinMode(i, OUTPUT);
	}



}
void test_MCP()
{
	for (int i = 0; i < 16; i++)
	{
		Bank6.digitalWrite(i, HIGH);
		delay(300);
		Bank6.digitalWrite(i, LOW);
		delay(300);
	}
}

void setup_ESP8266()
{
	Serial.println("--- Start ---");

	do {
		Serial.println("--- Reset ---");
		//  ESP8266.println("AT+RST");

		ESP8266.println("AT+CWMODE=1");
		delay(5000);
		if (ESP8266.find("OK"))
		{
			Serial.println("Module is ready");
			ESP8266.println("AT+CWMODE=1");
			delay(3000);

			if (cwJoinAP())
			{
				Serial.println("CWJAP Success");
				FAIL_8266 = false;
				// Тут должно быть достаточно времени что бы подключиться к роутеру
				delay(5000);

				//Получение и вывод IP адреса
				sendESP8266Cmdln("AT+CIFSR", 1000);
				//Выбрать режима множественного подключения
				sendESP8266Cmdln("AT+CIPMUX=1", 1000);
				//Установка сервера на порт 80
				sendESP8266Cmdln("AT+CIPSERVER=1,80", 1000);

				Serial.println("Server setup finish");
			}
			else {
				Serial.println("CWJAP Fail");
				delay(500);
				FAIL_8266 = true;
			}
		}
		else {
			Serial.println("Module have no response.");
			delay(500);
			FAIL_8266 = true;
		}
	} while (FAIL_8266);



}

void sendHTTPResponse(int id, String content)
{
	String response;
	response = "HTTP/1.1 200 OK\r\n";
	response += "Content-Type: text/html; charset=UTF-8\r\n";
	response += "Content-Length: ";
	response += content.length();
	response += "\r\n";
	response += "Connection: close\r\n\r\n";
	response += content;

	String cmd = "AT+CIPSEND=";
	cmd += id;
	cmd += ",";
	cmd += response.length();

	Serial.println("--- AT+CIPSEND ---");
	sendESP8266Cmdln(cmd, 1000);

	Serial.println("--- data ---");
	sendESP8266Data(response, 1000);
}

boolean waitOKfromESP8266(int timeout)
{
	do {
		Serial.println("wait OK...");
		delay(1000);
		if (ESP8266.find("OK"))
		{
			return true;
		}

	} while ((timeout--)>0);
	return false;
}

boolean cwJoinAP()
{
	String cmd = "AT+CWJAP=\"" + SSID + "\",\"" + PASSWORD + "\"";
	ESP8266.println(cmd);
	return waitOKfromESP8266(10);
}

//Отправка команд на ESP8266, предположительный ответ OK, без проверок ошибок
//wait some time and display respond
void sendESP8266Cmdln(String cmd, int waitTime)
{
	ESP8266.println(cmd);
	delay(waitTime);
	clearESP8266SerialBuffer("sendESP8266Cmdln");
}

//То же что и sendESP8266Cmdln()
//Но используется ESP8266.print() вместо ESP8266.println()
void sendESP8266Data(String data, int waitTime)
{
	ESP8266.print(data);
	delay(waitTime);
	clearESP8266SerialBuffer("sendESP8266Data");
}
void test_ESP8266()
{
	if (ESP8266.available())
	{
		Serial.println("Something received");
		delay(1000);
		if (ESP8266.find("+IPD,"))
		{
			String action;

			Serial.println("+IPD, found");
			int connectionId = ESP8266.read() - 48;
			Serial.println("connectionId: " + String(connectionId));

			ESP8266.find("led=");
			char s = ESP8266.read();
			if (s == '0') {
				action = "led=0";
				digitalWrite(ledPin13, LOW);
			}
			else if (s == '1') {
				action = "led=1";
				digitalWrite(ledPin13, HIGH);
			}
			else {
				action = "led=?";
			}

			Serial.println(action);
			sendHTTPResponse(connectionId, action);

			//Закрытие TCP/UDP
			String cmdCIPCLOSE = "AT+CIPCLOSE=";
			cmdCIPCLOSE += connectionId;
			sendESP8266Cmdln(cmdCIPCLOSE, 1000);
		}
	}


}

void registerWrite(int whichPin, int whichState) 
{
	// для хранения 16 битов используем unsigned int
	unsigned int bitsToSend = 0;

	// выключаем светодиоды на время передачи битов
	digitalWrite(latchPin, LOW);

	// устанавливаем HIGH в соответствующий бит
	bitWrite(bitsToSend, whichPin, whichState);

	// разбиваем наши 16 бит на два байта
	// для записи в первый и второй регистр
	byte registerOne = highByte(bitsToSend);
	byte registerTwo = lowByte(bitsToSend);

	// "проталкиваем" байты в регистры
	shiftOut(dataPin, clockPin, MSBFIRST, registerTwo);
	shiftOut(dataPin, clockPin, MSBFIRST, registerOne);

	// "защелкиваем" регистр, чтобы биты появились на выходах регистра
	digitalWrite(latchPin, HIGH);
}
void test_74HC595()
{
	// проходим циклом по всем 16 выходам двух регистров
	for (int thisLed = 0; thisLed < 16; thisLed++) 
	{
		// записываем сигнал в регистр для очередного светодиода
		registerWrite(thisLed, HIGH);
		// если это не первый светодиод, то отключаем предыдущий
		if (thisLed > 0) {
			registerWrite(thisLed - 1, LOW);
		}
		// если это первый светодиод, то отключаем последний
		else {
			registerWrite(15, LOW);
		}
		// делаем паузу перед следующией итерацией
		delay(50);
	}
}

void setup_SIM800()
{
	//Serial.begin(9600);
	//while (!con);

	Serial.println("SIM800 TEST");
	gprs.init();
	for (;;) 
	{
		Serial.print("Resetting...");
		while (!gprs.init()) 
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

}
void test_SIM800()
{

	char mydata[16];
	//sprintf(mydata, "t=%lu", millis());

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
}

void setup()
{
	Serial.begin(115200);
	ESP8266.begin(115200);
	pinMode(ledPin13, OUTPUT);
	digitalWrite(ledPin13, HIGH);
	pinMode(power_nRF24, OUTPUT);
	digitalWrite(power_nRF24, LOW);
	pinMode(timer_pin, OUTPUT);
	digitalWrite(timer_pin, HIGH);
	pinMode(Power_ESP8266, OUTPUT);
	digitalWrite(Power_ESP8266, LOW);

	pinMode(latchPin, OUTPUT);
	pinMode(dataPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(OE_Pin, OUTPUT);
	digitalWrite(OE_Pin, LOW);

	pinMode(PWR_SIM800, OUTPUT);

	pinMode(NETLIGHT, INPUT);
	

	digitalWrite(PWR_SIM800, LOW);
	//digitalWrite(SIM800_RESET_PIN, HIGH);
	delay(2000);
	//digitalWrite(NETLIGHT, HIGH);
	//digitalWrite(STATUS, HIGH);

	pinMode(SIM800_RESET_PIN, OUTPUT);

	digitalWrite(SIM800_RESET_PIN, HIGH);
	delay(100);
	digitalWrite(SIM800_RESET_PIN, LOW);
	delay(100);
	digitalWrite(SIM800_RESET_PIN, HIGH);


	//setup_SD_Fat();
	//setup_nRF24();
	setup_MCP();
	//test_MCP();
	//setup_ESP8266();

	//Timer3.attachInterrupt(firstHandler).start(3000000); // Every 1000ms
	//Timer4.attachInterrupt(secondHandler).setFrequency(1).start();
	//Timer5.attachInterrupt(thirdHandler).setFrequency(10);
	//Timer5.start();
	setColor(COLOR_RED);
	delay(300);
	setColor(COLOR_GREEN);
	delay(300);
	setColor(COLOR_BLUE);
	delay(300);

	setup_SIM800();
	delay(1000);
	test_SIM800();
	setColor(COLOR_NONE);                                      // Включить зеленый светодиод
	//test_ESP8266();
	Serial.println("Setup Ok!");

}

void loop()
{
	//delay(1000);
	//digitalWrite(PWR_SIM800, LOW);
	//delay(1000);
	//digitalWrite(SIM800_RESET_PIN, LOW);
	//delay(100);
	//digitalWrite(SIM800_RESET_PIN, HIGH);
	//delay(200);
	//digitalWrite(SIM800_RESET_PIN, LOW);

	//delay(3000);
	//digitalWrite(PWR_SIM800, HIGH);
	//test_nRF24();
	//test_MCP();
	//test_74HC595();
	//test_SIM800();
	delay(3000);
}
