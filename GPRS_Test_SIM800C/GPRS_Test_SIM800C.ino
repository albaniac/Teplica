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

#define LED13    13                                    // ��������� �����������

// ��������� ��� ����������� �������
//#define SIM800_BAUD 115200
//#define SIM800_POWER_PIN 65
//#define SIM800_RESET_PIN 67

// ��������� ��� ����������� OBDII

#define SIM800_BAUD      115200
#define SIM800_POWER_PIN 27
#define SIM800_RESET_PIN 47
#define NETLIGHT_PIN     37                              // ��������� NETLIGHT
#define STATUS_PIN       57                              // ��������� STATUS
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

unsigned long previousMillis = 0;                      //  
unsigned long interval = 10;                           // �������� �������� ������ 5 �����
bool time_set = false;                                 // ����������� �������� �������� ���
bool ssl_set = true;                                   // ������� ���������� 
unsigned long time_ping = 380;                         // �������� �������� ping 6 �����. 
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
{                                                       // 
	if (state_device == 0)                              // 
	{
		setColor(COLOR_RED);
	}
	if (state_device == 1)                             // �������� ������� ��������� - ����� �������
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

	if (state_device == 2)                             // �������� ����� ���������, ����� ������� �������� - �����������
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

		if (state_device == 3)                            // �������� ������� ��������� - ����������� ���������, ��������� ����������� � ���������
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
	// ������������ �������� ��������� ������ SIM800C (�������������� ������ NETLIGHT �� ����������� ����������)
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
			Timer6.stop();                                                   // ��������� ������ ���������� ��������� �����������
			//gprs.reboot(gprs.errors);                                      // ��� �� ����� �� ��� � ������������ �� �������
		}
	}
	else if(metering_NETLIGHT > 855 && metering_NETLIGHT < 870)
	{
		state_device = 1;                                                     // 1 �� ��������������� � ����, �����
		count_blink1++;
		if (count_blink1 > 250)                                               // � ���� �� ��������������� - ������������
		{
			state_device = 0;
			Timer6.stop();                                                    // ��������� ������ ���������� ��������� �����������
			//gprs.reboot(gprs.errors);                                       // ��� �� ����� �� ��� � ������������ �� �������
		}
	}
	else if(metering_NETLIGHT > 350 && metering_NETLIGHT < 370)
	{
		state_device = 3;                                                     // 3 - GPRS ����� �����������

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
		//	check_SMS();                                   // ��������� ������ ����� ��� 
			Serial.print("Attempt to transfer data .."); Serial.println(count_send);
		}
		delay(2000);                                       // ��������� 5 ������
	}

}





bool gprs_send(String data)
{
	con.print(F("Requesting .. Wait"));
	setColor(COLOR_BLUE);
	static const char* url1 = "http://trm7.xyz/r.php";
	static const char* urlssl = "https://trm7.xyz/r.php";

	connect_internet_HTTP();                               // ������������ � ��������� � ������ ����� HTTP

	int count_init = 0;                                    // ������� ���������� ������� ������������ � HTTP
	for (;;)                                               // ����������� ���� ���� �� ��������, ����� �� ��������� ��� ������
	{
		if (gprs.httpInit()) break;                        // ��� ���������, ������ ������� , �������� ������� � ����� �� �����
		con.print(">");
		con.println(gprs.buffer);                          // �� ����������, ("ERROR") 
		String stringError = gprs.buffer;
		if (stringError.indexOf(F("ERROR")) > -1)
		{
			con.println(F("\nNo internet connection"));
			delay(1000);
		}
		gprs.httpUninit();                                 // �� ����������, ���������� �  ����������� ����� 
		delay(1000);                                       // ��������� �������.
		count_init++;
		if (count_init > 5)
		{
			gprs.reboot(gprs.errors);                      // �������� ������������ �� Watchdog  
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

	while (gprs.httpIsConnected() == 0)     // 0 - �������� ������, 1 - �������, 
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
			gprs.reboot(gprs.errors);                               // �������� reset ����� 10 ������
		}
		delay(3000);
//		return;
	}

	con.println();
	gprs.httpRead();
	int ret;
	while ((ret = gprs.httpIsRead()) == 0)  //  ������� ��������� HTTP
	{
		// ����� ������� ���-�� �����, �� ����� ��������
	}

	if (gprs.httpState == HTTP_ERROR)          // ����� �� ������
	{
		gprs.errors++;
		//errorAllmem();
		if (gprs.errors > 5)                         // �������� reset ����� 10 ������
		{
			con.println(F("The number of server errors exceeded 10"));
			delay(3000);                                // ����� ��� ������ ���������
			gprs.reboot(gprs.errors);                   // �������� reset ����� 10 ������
		}
		delay(3000);
		//return;
	}

	// ������ �� �������� ��������� �� �����.
	con.print(F("[Payload] "));
	con.println(gprs.buffer);
	String val = gprs.buffer;               // �������� ������ ������ � �������
	send_ok = true;                         // ������� ������� �������
	int p0[8];
	// ������� ������� �������

	if (val.indexOf("&") > -1)              // ���������� ������ (�������) ������ � ������ 
	{
		p0[0] = val.indexOf("&01");         // ����� 1 �������
		p0[1] = val.indexOf("&02");         // ����� 2 �������
		p0[2] = val.indexOf("&03");         // ����� 3 �������
		p0[3] = val.indexOf("&04");         // ����� 4 �������
		p0[4] = val.indexOf("&05");         // ����� 5 �������
		p0[5] = val.indexOf("&06");         // ����� 6 �������
		p0[6] = val.indexOf("&07");         // ����� 7 �������
		p0[7] = val.indexOf('#');           // ����� ����� ������
	}

	for (int i = 0; i<7; i++)
	{
		String comm = val.substring(p0[i] + 3, p0[i] + 5);          // �������� ������ � ������� �������
		int comm1 = comm.toInt();
		Serial.println(comm1);
		comm = val.substring(p0[i] + 5, p0[i + 1]);               // �������� ������ � �������
		int len_str = comm.length();
		comm[len_str] = '\0';
		Serial.println(comm);

	//	run_command(comm1, comm);                               // ��������������� ��������� ��� �������

	}

	// �������� ����������
	con.print(F("Total: "));
	con.print(count);
	if (gprs.errors)                                                 // ���� ���� ������ - ��������
	{
		con.print(F(" Errors: "));
		con.print(gprs.errors);
	}
	//con.println();
	//Serial.print("Inteval: ");
	//Serial.println(interval);
	gprs.httpUninit();                                          // ��������� ���������� HTTP
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

		byte ret = gprs.connect_GPRS();                                              // ����������� � GPRS
		if (ret == 0)
		{
			while (state_device != 3)                            // �������� ����������� � ����
			{
				delay(50);
			}

			con.println(F("Wait IP"));
			gprs.connect_IP_GPRS();                             // �������� IP �����
			Serial.println(F("Internet connect OK!-"));
			setup_ok = true;
		}
		else                                                    // ������ �� ���������� � ���������
		{
			count_init++;                                        // ��������� ������� ���������� �������
			if (count_init > 10)
			{
				gprs.reboot(gprs.errors);                       // �������� reset ����� 10 ������
			}

			Serial.println(F("\nFailed connect internet"));
			delay(1000);
			if (state_device == 3)                               // ������ �������� � ��� ���� ���������� � ���������
			{
				con.println(F("Wait IP"));
				gprs.connect_IP_GPRS();                          // �������� IP �����
				Serial.println(F("Internet connect OK!-"));
				setup_ok = true;
			}
		}
	} while (!setup_ok);             // 
}









void setup_GPRS()
{
	bool setup_ok = false;
	int count_init = 0;                            // ��������� ���������� ������� ��������� SIM800C , ����� ��� �� �� ��� - ������� ���������������
	Timer6.start(300000); 

	do
	{
		con.println(F("Initializing....(May take 5-10 seconds)"));

		digitalWrite(SIM800_POWER_PIN, HIGH);                        // Pin ��������� SIM800C
		delay(100);
		digitalWrite(SIM800_RESET_PIN, HIGH);                        // Pin ��������� SIM800C � �������� ���������
		delay(1000);
		while (digitalRead(STATUS_PIN) != LOW)                       // ��������� ���������� ������� ������ SIM800C 
		{
			delay(100);                                              //    
		}

#ifdef DEBUG
		DEBUG.println(F("\nPower SIM800 Off"));
#endif

		delay(1000);
		digitalWrite(SIM800_POWER_PIN, LOW);                                   // ������ ������� �� SIM800C    
		delay(1000);
		digitalWrite(SIM800_RESET_PIN, LOW);                                   // ������� ������� SIM800C
		delay(100);
		digitalWrite(SIM800_RESET_PIN, HIGH);                                  // Pin ��������� SIM800C � �������� ���������
		delay(1000);

		while (digitalRead(STATUS_PIN) == LOW)                       // ��������� ������ "STATUS" ������ SIM800C. ������� ������ ���� ��������. 
		{
			count_init++;                                           // �������� ������� ������� ���������
			if (count_init > 100)                                   // ���� ������ 100 �������. �������� ��������� ������ ����������������
			{
				//gprs.reboot(gprs.errors);                           // 100 �������. ��� �� ����� �� ��� ��������� �����������  ���� ������ �� ���������
			}
			delay(100);                                               // ��������� SIM800C ������ ���������.
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

		
		con.print(F("\nSetting up mobile network..."));
		while (state_device != 2)                                // �������� ����������� � ����
		{
			con.print(F("."));
			delay(1000);
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
					gprs.connect_GPRS();
					setup_ok = true; 
					 
				} while (!setup_ok);
			}
		}
	} while (count_init > 30 || setup_ok == false);    // 30 ������� ������������������ � ����


	

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
	digitalWrite(LED13, LOW);                             // ��������� ���������� �������� ���������
	pinMode(SIM800_POWER_PIN, OUTPUT);                    // Pin ���������� �������� SIM800C
	pinMode(SIM800_RESET_PIN, OUTPUT);                    // Pin ��������� SIM800C
	pinMode(NETLIGHT_PIN, INPUT);                         // ��������� NETLIGHT (���������� ���������� ����������)
	pinMode(STATUS_PIN, INPUT);                           // ��������� STATUS (�������� ��������� ������� SIM800)

	digitalWrite(SIM800_POWER_PIN, HIGH);                                   // Pin ��������� SIM800C
	delay(100);
	digitalWrite(SIM800_RESET_PIN, HIGH);                                   // Pin ��������� SIM800C � �������� ���������
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
	
	attachInterrupt(NETLIGHT_PIN, check_blink, RISING);                     // �������� ����������. ��������� ��������� ������(������ NETLIGHT) 
	//Timer3.attachInterrupt(firstHandler).start(500000); // Every 500ms
	//Timer4.attachInterrupt(secondHandler).setFrequency(1).start();
	//Timer5.attachInterrupt(thirdHandler).setFrequency(10);
	Timer6.attachInterrupt(flash_time);                                     // ��������� ������� ������ SIM800

	setup_GPRS();
	int count_init = 0;                                        // ������� ���������� ������� ������������ � HTTP
	con.println(F("OK"));

	if (gprs.deleteSMS(0))
	{
		con.println(F("All SMS delete"));                    // 
	}
	//send_data();
	//setColor(COLOR_NONE);                                      // 
	setColor(COLOR_GREEN);
	//time = millis();                                            // ����� ������� �����
	con.println(F("\nSIM800 setup end"));
	
}

void loop()
{
	if (digitalRead(STATUS_PIN) == LOW)
	{
		gprs.reboot(gprs.errors);                                     // ��� �� ����� �� ��� ��������� �����������  ���� ������ �� ���������
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


