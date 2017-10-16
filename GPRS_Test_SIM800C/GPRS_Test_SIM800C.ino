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

#define LED13    13                                    // ��������� �����������

// ��������� ��� ����������� �������
//#define SIM800_BAUD 115200
//#define SIM800_POWER_PIN 65
//#define SIM800_RESET_PIN 67

// ��������� ��� ����������� OBDII

#define SIM800_BAUD 115200
#define SIM800_POWER_PIN 27
#define SIM800_RESET_PIN 47
#define NETLIGHT         37                              // ��������� NETLIGHT
#define STATUS           57                              // ��������� STATUS
//#define COMMON_ANODE                                   // ���� ��������� � ����� ������� - �����������������
#define LED_RED      54                                  // ��������� ����������� RED
#define LED_BLUE     55                                  // ��������� ����������� BLUE
#define LED_GREEN    56                                  // ��������� ����������� GREEN




#define COLOR_NONE LOW, LOW, LOW                        // ��������� ��� ����������
#define COLOR_GREEN LOW, HIGH, LOW                      // �������� ������� ���������
#define COLOR_BLUE LOW, LOW, HIGH                       // �������� ����� ���������
#define COLOR_RED HIGH, LOW, LOW                        // �������� ������� ���������
volatile int stateLed = LOW;                            // ��������� ���������� ��� ����������� ��������� �� ������
volatile int state_device = 0;                          // ��������� ������ ��� ������� 

														// 1 - �� ��������������� � ����, �����
														// 2 - ���������������� � ����
														// 3 - GPRS ����� �����������
volatile int metering_NETLIGHT = 0;
volatile unsigned long metering_temp = 0;
volatile int count_blink1 = 0;                          // ������� ������� ������������ � ������� �������
volatile int count_blink2 = 0;                          // ������� ������� ������������ � ������� �������
bool send_ok = false;                                   // ������� �������� �������� ������
bool count_All_reset = false;                           // ������� ���������� ������� ������ �������� ������.
String imei = "861445030362268";                        // ���� IMEI
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

void setColor(bool red, bool green, bool blue)         // ��������� ����� �������� ������������ ����������.
{
#ifdef COMMON_ANODE                                    // ���� ��������� � ����� �������
	red = !red;
	green = !green;
	blue = !blue;
#endif 
	digitalWrite(LED_RED, red);
	digitalWrite(LED_GREEN, green);
	digitalWrite(LED_BLUE, blue);
}
void flash_time()                                       // ��������� ���������� ������������ �������� ����������� ��� ������
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
void check_blink()
{
	// ������������ �������� ��������� ������ SIM800C (������ NETLIGHT)
	unsigned long current_M = millis();
//	wdt_reset();
	metering_NETLIGHT = current_M - metering_temp;                            // ��������� ������������ ������� NETLIGHT
	metering_temp = current_M;

	if (metering_NETLIGHT > 3055 && metering_NETLIGHT < 3070)
	{
		state_device = 2;                                                     // 2 - ���������������� � ����
		count_blink2++;
		if (count_blink2 > 250)
		{
			state_device = 0;
			Timer6.stop();                                                 // ��������� ������ ����������
			//gprs.reboot(gprs.errors);                                                    // ��� �� ����� �� ��� � ������������ �� �������
		}
	}
	else if (metering_NETLIGHT > 855 && metering_NETLIGHT < 870)
	{
		state_device = 1;                                                     // 1 �� ��������������� � ����, �����
		count_blink1++;
		if (count_blink1 > 250)
		{
			state_device = 0;
			Timer6.stop();                                                 // ��������� ������ ����������
			//gprs.reboot(gprs.errors);                                                    // ��� �� ����� �� ��� � ������������ �� �������
		}
	}
	else if (metering_NETLIGHT > 350 && metering_NETLIGHT < 370)
	{
		state_device = 3;                                                     // 3 - GPRS ����� �����������

	}
	//con.println(state_device);
}







void setup_GPRS()
{
	bool setup_ok = false;
	int count_init = 0;
	Timer6.start(300000);
	pinMode(SIM800_RESET_PIN, OUTPUT);

	digitalWrite(SIM800_RESET_PIN, HIGH);                            // ������ ����� � �������� ���������

	delay(2000);
	pinMode(SIM800_POWER_PIN, OUTPUT);
	digitalWrite(SIM800_POWER_PIN, HIGH);                              // ������ ����� � �������� ���������
	delay(3000);

	do
	{
		con.println(F("Initializing....(May take 5-10 seconds)"));

		digitalWrite(SIM800_RESET_PIN, HIGH);                          // ������ ����� � �������� ���������
		delay(1000);
		digitalWrite(LED13, LOW);                                      // ��������� ��������� �������� ������������� � �������� ���������
		digitalWrite(SIM800_POWER_PIN, HIGH);                          // �������������� ��������� ������� ������ GPRS (������������� � �������� ���������)
		delay(1000);                                                   // ������� ��������
		while (digitalRead(STATUS) != LOW)                             // ��������� ���������� ������� ������ SIM800C 
		{
			delay(100);                                                //    
		}
		con.println(F("Power SIM800 Off"));
//		delay(1000);
		digitalWrite(LED13, HIGH);                                     // ��������� ������ ��������� ������� ������ SIM800C 
		digitalWrite(SIM800_POWER_PIN, LOW);                           // ��������� ������� ������ SIM800C 
		delay(500);                                                    // ������� �������� ���������� �������� ��� ��������� �������
		//digitalWrite(SIM800_RESET_PIN, HIGH);                         // ���������� ����� ������ ����� ��������� �������
		//delay(1200);
	//	digitalWrite(SIM800_RESET_PIN, LOW);
		int count_status = 0;                                          // ��������� ���������� ������� ��������� SIM800C , ����� ��� �� �� ��� - ������� ���������������
		
		while (digitalRead(STATUS) == LOW)                             // ��������� ������ "STATUS" ������ SIM800C. ������� ������ ���� ���������. 
		{
			count_status++;                                            // �������� ������� ������� ���������
			if (count_status > 100)                                    // ���� ������ 100 �������. �������� ��������� ������ ����������������
			{
				//gprs.reboot(gprs.errors);                            // 100 �������. ��� �� ����� �� ��� ��������� �����������  ���� ������ �� ���������
			}
			delay(100);                                                // ��������� SIM800C ������ ���������.
		}
		
		con.println(F("Power SIM800 On"));

		// ������������� SIM800C
		for (;;) {
			con.print("Resetting...");
			while (!gprs.init(SIM800_BAUD, SIM800_POWER_PIN, SIM800_RESET_PIN))
			{
				con.write('.');
			}
			con.println("OK");

			con.print("Setting up network...");
	/*		byte ret = gprs.setup(APN);
			if (ret == 0)
				break;
			con.print("Error code:");
			con.println(ret);
			con.println(gprs.buffer);*/
			break;
		}
		con.println("OK");

		delay(1000);

		if (gprs.getIMEI())                                     // �������� IMEI
		{
			con.print(F("\nIMEI:"));
			//imei = gprs.buffer;                                 // ��������� �� ����� �������
			gprs.cleanStr(imei);                                // ��������� �� ����� �������
			con.println(imei);
		}




		con.print(F("\nSetting up mobile network..."));
		while (state_device != 2)                                // �������� ����������� � ����
		{
			con.print(F("."));
			delay(1000);
		}

		//GPRSSerial->begin(19200);                               // �������� ������ � ������� SIM800C
		/*		while (!gprs.begin(*GPRSSerial))                        // ��������� ������ SIM800C
		{
			con.println(F("Couldn't find module GPRS"));
			while (1);
		}

		con.println(F("OK"));
		con.print(F("\nSetting up mobile network..."));
		while (state_device != 2)                                // �������� ����������� � ����
		{
			Serial.print(F("."));
			delay(1000);
		}
		delay(1000);

		if (gprs.getIMEI())                                     // �������� IMEI
		{
			con.print(F("\nIMEI:"));
			//imei = gprs.buffer;                                 // ��������� �� ����� �������
			gprs.cleanStr(imei);                                // ��������� �� ����� �������
			con.println(imei);
		}

		if (gprs.getSIMCCID())                               // �������� ����� ��� �����
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
		if (n == '0') con.println(F("\nNot registered"));                      // 0 � �� ���������������, ������ ���� ���
		if (n == '1') con.println(F("\nRegistered (home)"));                   // 1 � ���������������, �������� ����
		if (n == '2') con.println(F("\nNot registered (searching)"));          // 2 � �� ���������������, ��� ����� ����� ����
		if (n == '3') con.println(F("\nDenied"));                              // 3 � ����������� ���������
		if (n == '4') con.println(F("\nUnknown"));                             // 4 � ����������
		if (n == '5') con.println(F("\nRegistered roaming"));                  // 5 � �������

		if (n == '1' || n == '5')                                                 // ���� �������� ���� ��� �������
		{
			if (state_device == 2)                                                // ��������� ��������� ����������� ������ � ���������
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
	} while (count_init > 30 || setup_ok == false);    // 30 ������� ������������������ � ����























/*
	con.println("SIM800C TEST");

	for (;;) {
		con.print("Resetting...");
		while (!gprs.init(SIM800_BAUD, SIM800_POWER_PIN, SIM800_RESET_PIN))
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
	*/
}




void setup()
{
	con.begin(115200);
//	while (!con);
	//pinMode(SIM800_RESET_PIN, OUTPUT);
	//digitalWrite(SIM800_RESET_PIN, HIGH);                      // ������ ����� � �������� ���������
	//delay(500);
	pinMode(LED13, OUTPUT);
	digitalWrite(LED13, LOW);                      // ������ ����� � �������� ���������
	//pinMode(SIM800_POWER_PIN, OUTPUT);
	//digitalWrite(SIM800_POWER_PIN, HIGH);            // ������ ����� � �������� ���������

	pinMode(LED_RED, OUTPUT);
	pinMode(LED_BLUE, OUTPUT);
	pinMode(LED_GREEN, OUTPUT);
	pinMode(NETLIGHT, INPUT);                      // ��������� NETLIGHT
	pinMode(STATUS, INPUT);                        // ��������� STATUS



	setColor(COLOR_RED);
	delay(500);
	setColor(COLOR_GREEN);
	delay(500);
	setColor(COLOR_BLUE);
	delay(500);
	setColor(COLOR_RED);
	delay(300);





	attachInterrupt(37, check_blink, RISING);                     // �������� ����������. ��������� ��������� ������(������ NETLIGHT) 
	//Timer3.attachInterrupt(firstHandler).start(500000); // Every 500ms
	//Timer4.attachInterrupt(secondHandler).setFrequency(1).start();
	//Timer5.attachInterrupt(thirdHandler).setFrequency(10);
	Timer6.attachInterrupt(flash_time); // Every 500ms



	setup_GPRS();

	setColor(COLOR_NONE);                                      // 
	//time = millis();                                            // ����� ������� �����
	con.println(F("\nSIM800 setup end"));

}

void loop()
{
	/*
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
	*/
}


