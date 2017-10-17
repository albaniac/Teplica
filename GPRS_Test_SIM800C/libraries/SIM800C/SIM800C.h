/*************************************************************************
* SIM800 GPRS/HTTP Library
* Distributed under GPL v2.0
* Written by Stanley Huang <stanleyhuangyc@gmail.com>
* For more information, please visit http://arduinodev.com
*************************************************************************/

#include <Arduino.h>

// change this to the pin connect with SIM800 reset pin
//#define SIM800_RESET_PIN 67

// change this to the serial UART which SIM800 is attached to
#define SIM_SERIAL Serial2

// define DEBUG to one serial UART to enable debug information output
#define DEBUG Serial

typedef enum {
    HTTP_DISABLED = 0,
    HTTP_READY,
    HTTP_CONNECTING,
    HTTP_READING,
    HTTP_ERROR,
} HTTP_STATES;

typedef struct {
  float lat;
  float lon;
  uint8_t year; /* year past 2000, e.g. 15 for 2015 */
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
} GSM_LOCATION;

class CGPRS_SIM800C {
public:
    CGPRS_SIM800C():httpState(HTTP_DISABLED) {}
    // initialize the module
    bool init(unsigned long baud, int PWR_On, int RESET_PIN);
    // setup network
    byte setup(const char* apn);
    // get network operator name
    bool getOperatorName();
    // check for incoming SMS
    bool checkSMS();
    // get signal quality level (in dB)
    int getSignalQuality();
    // get GSM location and network time
    bool getLocation(GSM_LOCATION* loc);

	//++++++++++++++++++++++++++++++++++++++++++++++

	uint8_t getNetworkStatus();
	byte connect_GPRS();
	bool connect_IP_GPRS();
	bool getIMEI();
	bool getSIMCCID();
	byte ping_connect_internet();
	bool ping(const char* url);
	void send_sms(String text, String phone);  //процедура отправки СМС
	bool deleteSMS(int n_sms);
	void cleanStr(String & str);
	//---------------------------------------------

    // initialize HTTP connection
    bool httpInit();
    // terminate HTTP connection
    void httpUninit();
    // connect to HTTP server
    bool httpConnect(const char* url, const char* args = 0);
    // check if HTTP connection is established
    // return 0 for in progress, 1 for success, 2 for error
    byte httpIsConnected();
	bool httpConnectStr(const char* url, String args = "");
	boolean HTTP_ssl(boolean onoff);
    // read data from HTTP connection
    void httpRead();
    // check if HTTP connection is established
    // return 0 for in progress, -1 for error, bytes of http payload on success
    int httpIsRead();
    // send AT command and check for expected response
    byte sendCommand(const char* cmd, unsigned int timeout = 2000, const char* expected = 0);
    // send AT command and check for two possible responses
    byte sendCommand(const char* cmd, const char* expected1, const char* expected2, unsigned int timeout = 2000);
    // toggle low-power mode
    bool sleep(bool enabled)
    {
      return sendCommand(enabled ? "AT+CFUN=0" : "AT+CFUN=1");
    }
    // check if there is available serial data
    bool available()
    {
      return SIM_SERIAL.available(); 
    }
	void reboot(int count_error);



    char buffer[256];
	char buffer1[22];
    byte httpState;
	unsigned int errors = 0;





private:
    byte checkbuffer(const char* expected1, const char* expected2 = 0, unsigned int timeout = 2000);
    void purgeSerial();
    byte m_bytesRecv;
    uint32_t m_checkTimer;
	unsigned long _baud;                    // Скорость Serial
	int _PWR_On;                            // Включение питания модуля SIM800
	int _RESET_PIN;                         // Сброс модуля SIM800
	byte operator_Num = 0;                  // Порядковый номер оператора
	//unsigned int timeout = 2000;            // 
	String apn = "";
	String user = "";
	String pwd = "";
	String cont = "";
};

