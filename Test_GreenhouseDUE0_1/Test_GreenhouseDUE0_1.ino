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
9	D9	            PWM8 SIM800C STATUS (attachInterrupt (9) DUE)
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
65	A11 / D65	    Power  ON/OFF SIM800C
66	D66/DAC0	    Сброс сторожевого таймера
67	D67/DAC1	    PWRKEY   SIM800C
68	D68 / CANRX0	RX/TX RS485
69	D69 / CANTX1	Mini W5100 (4)
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
адр6	MCP23S17	A4 - USE_GSM_RED_DIODE
адр6	MCP23S17	A5 - USE_GSM_BLUE_DIODE
адр6	MCP23S17	A6 - USE_GSM_GREEN_DIODE
адр6	MCP23S17	A7 - SOUNDER (пищалка)



*/

#include <SPI.h>
#include <SD.h>
#include "RF24.h"
#include <MCP23S17.h>


const int ledPin13 = 13;
const int power_nRF24 = 60;
const uint8_t chipSelect_MCP = 46;

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

void setup()
{
	Serial.begin(115200);

	pinMode(ledPin13, OUTPUT);
	digitalWrite(ledPin13, HIGH);
	pinMode(power_nRF24, OUTPUT);
	digitalWrite(power_nRF24, LOW);

	setup_SD_Fat();
	setup_nRF24();
	setup_MCP();
	test_MCP();
}

void loop()
{

	//test_nRF24();
	//test_MCP();

	delay(100);
}
