/*************************************************************************
* SIM800 GPRS/HTTP Library
* Distributed under GPL v2.0
* Written by Stanley Huang <stanleyhuangyc@gmail.com>
* For more information, please visit http://arduinodev.com
*************************************************************************/

#include "SIM800C.h"

bool CGPRS_SIM800C::init(unsigned long baud)
{
	_baud       = baud;                                            // ���������� �������� Serial

//	int count_status = 0;                                          // ��������� ���������� ������� ��������� SIM800C , ����� ��� �� �� ��� - ������� ���������������
	int16_t timeout = 7000;
	SIM_SERIAL.begin(_baud);
    delay(1000);

	while (timeout > 0)
	{
		while (SIM_SERIAL.available()) SIM_SERIAL.read();
		if (sendCommand("AT"))
		{
			break;
		}
		delay(500);
		timeout -= 500;
	}

	if (timeout <= 0)
	{
		sendCommand("AT");
		delay(100);
		sendCommand("AT");
		delay(100);
		sendCommand("AT");
		delay(100);
	}

	if (sendCommand("AT"))
	{
		SIM_SERIAL.print("AT+IPR=");                	 // ���������� �������� ������
		SIM_SERIAL.print(_baud);
		delay(100);
		sendCommand("ATE0");                             // ��������� ��� 
		delay(100);
		sendCommand("AT+CFUN=1");                        // 1 � ���������� ����� (�� ���������). ������ �������� 1 � ������������� (�������� ������ � ���������� ������, �.�. ��������� = 1,1)
		delay(100);
		sendCommand("AT+CMGF=1");                        // ����� ��������� ��� - ������� (��� ����.)
		delay(100);
		sendCommand("AT+CLIP=1");                        // �������� ���
		delay(100);
		sendCommand("AT+CSCS=\"GSM\"");                   // ����� ��������� ������
		delay(100);
		//sendCommand("AT+CNMI=2,2");                    // ����������� ��� � ��������� ����� ����� ������ (��� ����� ��������� ����� ������ � ������)tln("AT+CSCS=\"GSM\""); 
		//delay(100);
		//sendCommandS("AT+CMGDA=\"DEL ALL\"");           // AT+CMGDA=�DEL ALL� ������� ������ ��� ���������
		//delay(100);
		//sendCommand("AT+GMR"));                         // ����� ��������
		//delay(100);
		//sendCommand("AT+CPMS=\"SM\",\"SM\",\"SM\"");    // selects the memory
		return true;
	}
    return false;
}

byte CGPRS_SIM800C::setup(const char* apn)
{
  bool success = false;
  for (byte n = 0; n < 30; n++) {
    if (sendCommand("AT+CREG?", 2000)) {
        char *p = strstr(buffer, "0,");
        if (p) {
          char mode = *(p + 2);
#if DEBUG
		  DEBUG.print("Mode:");
		  DEBUG.println(mode);
#endif
          if (mode == '1' || mode == '5') {
            success = true;
            break;
          }
        }
    }
    delay(1000);
  }
  		
  if (!success)
    return 1;
  
  if (!sendCommand("AT+CGATT?"))
    return 2;
    
  if (!sendCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\""))
    return 3;
  
  SIM_SERIAL.print("AT+SAPBR=3,1,\"APN\",\"");
  SIM_SERIAL.print(apn);
  SIM_SERIAL.println('\"');
  if (!sendCommand(0))
    return 4;
  
  sendCommand("AT+SAPBR=1,1", 10000);
  sendCommand("AT+SAPBR=2,1", 10000);

  sendCommand("AT+CMGF=1");    // sets the SMS mode to text
  sendCommand("AT+CPMS=\"SM\",\"SM\",\"SM\""); // selects the memory

  if (!success)
    return 5;

  return 0;
}
bool CGPRS_SIM800C::getOperatorName()
{
 // display operator name

	if (sendCommand("AT+COPS?", "OK\r", "ERROR\r") == 1)
	{
		char *p = strstr(buffer, ",\"");
		if (p)
		{
			p += 2;
			char *s = strchr(p, '\"');
			if (s) *s = 0;
			strcpy(buffer, p);

			String OperatorName = buffer;
			if (OperatorName.indexOf(F("MTS")) > -1)
			{
				apn = F("internet.mts.ru");
				user = F("mts");
				pwd = F("mts");
				cont = F("internet.mts.ru");
				Serial.println(F("MTS"));
				operator_Num = 0;                                  // ���������� ����� ��������� ���
			}
			else if (OperatorName.indexOf(F("Bee")) > -1)
			{
				apn = F("internet.beeline.ru");
				user = F("beeline");
				pwd = F("beeline");
				cont = F("internet.beeline.ru");
				Serial.println(F("Beeline"));
				operator_Num = 1;                                  // ���������� ����� ��������� Beeline
			}
			else if (OperatorName.indexOf(F("Mega")) > -1)
			{
				apn = F("internet");
				user = "";
				pwd = "";
				cont = F("internet");
				Serial.println(F("MEGAFON"));
				operator_Num = 2;                                  // ���������� ����� ��������� Megafon
			}
			Serial.println(apn);
			return true;
		}
	}


  //if (sendCommand("AT+COPS?", "OK\r", "ERROR\r") == 1) {
  //    char *p = strstr(buffer, ",\"");
  //    if (p) {
  //        p += 2;
  //        char *s = strchr(p, '\"');
  //        if (s) *s = 0;
  //        strcpy(buffer, p);
  //        return true;
  //    }
  //}
  return false;
}
bool CGPRS_SIM800C::checkSMS()
{
  if (sendCommand("AT+CMGR=1", "+CMGR:", "ERROR") == 1) {
    // reads the data of the SMS
    sendCommand(0, 100, "\r\n");
    if (sendCommand(0)) {
      // remove the SMS
      sendCommand("AT+CMGD=1");
      return true;
    }
  }
  return false; 
}
int CGPRS_SIM800C::getSignalQuality()
{
  sendCommand("AT+CSQ");
  char *p = strstr(buffer, "CSQ: ");
  if (p) {
    int n = atoi(p + 2);
    if (n == 99 || n == -1) return 0;
    return n * 2 - 114;
  } else {
   return 0; 
  }
}
bool CGPRS_SIM800C::getLocation(GSM_LOCATION* loc)
{
  if (sendCommand("AT+CIPGSMLOC=1,1", 10000)) do {
    char *p;
    if (!(p = strchr(buffer, ':'))) break;
    if (!(p = strchr(p, ','))) break;
    loc->lon = atof(++p);
    if (!(p = strchr(p, ','))) break;
    loc->lat = atof(++p);
    if (!(p = strchr(p, ','))) break;
    loc->year = atoi(++p) - 2000;
    if (!(p = strchr(p, '/'))) break;
    loc->month = atoi(++p);
    if (!(p = strchr(p, '/'))) break;
    loc->day = atoi(++p);
    if (!(p = strchr(p, ','))) break;
    loc->hour = atoi(++p);
    if (!(p = strchr(p, ':'))) break;
    loc->minute = atoi(++p);
    if (!(p = strchr(p, ':'))) break;
    loc->second = atoi(++p);
    return true;
  } while(0);
  return false;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++


byte CGPRS_SIM800C::connect_GPRS()
{
	for (byte n = 0; n < 10; n++)
	{
		if (!sendCommand("AT+SAPBR=3,1","Contype","GPRS")) return 1;

		SIM_SERIAL.print(F("AT+SAPBR=3,1,\"APN\",\""));
		SIM_SERIAL.print(apn);
		SIM_SERIAL.println('\"');
		if (!sendCommand("", "OK\r", "ERROR\r"))   return 2;

		SIM_SERIAL.print(F("AT+SAPBR=3,1,\"USER\",\""));
		SIM_SERIAL.print(user);
		SIM_SERIAL.println('\"');
		if (!sendCommand("", "OK\r", "ERROR\r"))    return 2;

		SIM_SERIAL.print(F("AT+SAPBR=3,1,\"PWD\",\""));
		SIM_SERIAL.print(pwd);
		SIM_SERIAL.println('\"');
		if (!sendCommand("", "OK\r", "ERROR\r"))    return 2;

		SIM_SERIAL.print(F("AT+CGDCONT=1,\"IP\",\""));
		SIM_SERIAL.print(cont);
		SIM_SERIAL.println('\"');
		if (!sendCommand("", "OK\r", "ERROR\r"))    return 2;

		Serial.print(F("Connect GPRS..")); Serial.println(n + 1);
		sendCommand("AT+SAPBR=1,1", "OK\r", "ERROR\r", 150000);  return 0;                 // ��������� GPRS �����  !!!! �������� ������� !!!
	}
	return 3;                                                                           // ���������� �����������
}
bool CGPRS_SIM800C::connect_IP_GPRS()
{
	for (byte n = 0; n < 30; n++)
	{
		if (sendCommand("AT+SAPBR=2,1", "OK\r", "ERROR\r", 15000))  return true;         // �������� IP �����
		delay(500);
	}
	return false;
}
uint8_t CGPRS_SIM800C::getNetworkStatus()
{
	for (byte n = 0; n < 30; n++)
	{
		if (sendCommand("AT+CREG?", "OK\r", "ERROR\r"))                       // ��� ����������� ����
		{
			// ������ ��������:
			// 0 � ��� ���� ����������� ����
			// 1 � ���� ��� ����������� ����
			// 2 � ���� ��� ����������� ���� + ��� ���������
			// ������ ��������:
			// 0 � �� ���������������, ������ ���� ���
			// 1 � ���������������, �������� ����
			// 2 � �� ���������������, ��� ����� ����� ����
			// 3 � ����������� ���������
			// 4 � ����������
			// 5 � �������

			char *p = strstr(buffer, "0,");                        // �������� ������ ��������
			if (p)
			{
				char mode = *(p + 2);
#if DEBUG
				DEBUG.print("Mode:");
				DEBUG.println(mode);
#endif
				return mode;
			}
		}
		delay(1000);
	}
}
void CGPRS_SIM800C::cleanStr(String & str)
{
	str.replace("OK", "");
	str.replace("\"", "");
	str.replace("\n", "");
	str.replace("\r", "");
	str.trim();
}


bool CGPRS_SIM800C::getIMEI()
{
	if (sendCommand("AT+GSN", "OK\r", "ERROR\r") == 1)               // (sendCommand("AT+GSN", "OK\r", "ERROR\r") == 1) 
	{
		char *p = strstr(buffer, "\r");                       //������� strstr() ���������� ��������� �� ������ ��������� � ������, 
														      //�� ������� ��������� str1, ������, ��������� str2 (�������� ����������� ������� ������).
														      //���� ���������� �� ����������, ������������ NULL.
		if (p)
		{
			p += 2;
			char *s = strchr(p, '\r');                       // ������� strchr() ���������� ��������� �� ������ ��������� ������� ch � ������, 
													         //�� ������� ��������� str. ���� ������ ch �� ������,
													         //������������ NULL. 
			if (s) *s = 0;   strcpy(buffer, p);
			return true;
		}
	}
	return false;
}
bool CGPRS_SIM800C::getSIMCCID()
{
	if (sendCommand("AT+CCID", "OK\r", "ERROR\r") == 1)             // (sendCommand("AT+CCID", "OK\r", "ERROR\r") == 1)
	{
		char *p = strstr(buffer, "\r");              //������� strstr() ���������� ��������� �� ������ ��������� � ������, 
													 //���� ���������� �� ����������, ������������ NULL.
		if (p)
		{
			p += 2;
			char *s = strchr(p, '\r');          // ������� strchr() ���������� ��������� �� ������ ��������� ������� ch � ������, 
												//�� ������� ��������� str. ���� ������ ch �� ������,
												//������������ NULL. 
			if (s) *s = 0;   strcpy(buffer, p);
			int i = 0;

			for (i = 0; i<120; i++)
			{
				if (buffer[i] == 'f')  break;
				if (buffer[i] >= '0'&&buffer[i] <= '9') // ���� ����� ������� - �� ������� ��
				{
					buffer1[i] = buffer[i];
					//Serial.print(buffer1[i]);
				}
			}
			return true;
		}
	}
	return false;
}
byte CGPRS_SIM800C::ping_connect_internet()
{
	int count_connect = 0;                                                // ������� ���������� ������� �������� ����������� Network registration (�������� ���������)
	for (;;)                                                              // ����������� ���� ���� �� ��������, ����� �� ��������� ��� ������
	{
	//	if (sendCommandS(F("AT+CREG?")) == 1) break;                      // ��� ���������, � ���� ��������� ���������������� , �������� ������� � ����� �� �����
		if (sendCommand("AT+CREG?", "OK\r", "ERROR\r") == 1) break;      // ��� ���������, � ���� ��������� ���������������� , �������� ������� � ����� �� �����
		Serial.print(">");
		Serial.println(buffer);                                           // �� ����������, ("ERROR") 
		String stringError = buffer;
		if (stringError.indexOf(F("ERROR")) > -1)
		{
			Serial.println(F("\nNo GPRS connection"));
			delay(1000);
		}
		delay(1000);                                                     // ��������� �������.
		count_connect++;
		if (count_connect > 60)
		{
			reboot(errors); //break;                                   //�������� reset ��� ���������� ������� � �������� ��������� � ������� 60 ������
		}
	}
	delay(1000);
	count_connect = 0;                                                   // ������� ���������� ������� �������� ����������� Attach from GPRS service
	for (;;)                                                             // ����������� ���� ���� �� ��������, ����� �� ��������� ��� ������
	{
		if (sendCommand("AT+CGATT?", "OK\r", "ERROR\r") == 1) break;     // ��� ���������, ������ ��������� � GPRS service , �������� ������� � ����� �� �����
		Serial.print(F(">"));
		Serial.println(buffer);                                          // �� ����������, ("ERROR") 
		String stringError = buffer;
		if (stringError.indexOf(F("ERROR")) > -1)
		{
			Serial.println(F("\nNo GPRS connection"));
			delay(1000);
		}
		delay(1000);                                                     // ��������� �������.
		count_connect++;
		if (count_connect > 60)
		{
			reboot(errors);                                              //�������� reset ��� ���������� ������� �  GPRS service � ������� 60 ������
		}
	}

	//++++++++++++++++ �������� ��������, ������������ � ��������� �� ��������� TCP ��� �������� ping ++++++++++++

	switch (operator_Num)                                                   // ���������� ������� ����������� � ����������� �� ���������
	{
	case 0:
		sendCommand("AT+CSTT=\"internet.mts.ru\"", "OK\r", "ERROR\r");       //��������� ����� ������� MTS. ��� ��������� ������ ����� �������� ������. ��� ��������� ������ ��� ������ ��� �������.
		break;
	case 1:
		sendCommand("AT+CSTT=\"internet.beeline.ru\"", "OK\r", "ERROR\r");   //��������� ����� �������  beeline
		break;
	case 2:
		sendCommand("AT+CSTT=\"internet\"", "OK\r", "ERROR\r");              //��������� ����� ������� Megafon
		break;
	}
	delay(1000);
	sendCommand("AT+CIICR", "OK\r", "ERROR\r",10000);                             // ��������� �������� Bring Up Wireless Connection with GPRS  
	return 0;
}
bool CGPRS_SIM800C::ping(const char* url)
{
	sendCommand("AT+CIFSR", "OK\r", "ERROR\r");                             //�������� ��������� IP-�����
	delay(1000);
	SIM_SERIAL.print(F("AT+CIPPING=\""));                                  // ��������� ������� ping
	SIM_SERIAL.print(url);
	SIM_SERIAL.println('\"');
	delay(5000);

	//++++++++++++++++++++++++++ ������� ����� ����� �� ping  ++++++++++++++++++++++++++++++++++++++++   

	if (sendCommand("", "OK\r", "ERROR\r",10000) == 1)            // ����� �� ping ������� 
	{
		SIM_SERIAL.print(F("AT+CIPSHUT"));                           // ������� ����������
		return true;
	}
	SIM_SERIAL.print(F("AT+CIPSHUT"));                             // ������, ��� �� �� ��� �����. �� ������ ������ ��������� ����������
	return false;
}
void CGPRS_SIM800C::reboot(int count_error)
{
//	pinMode(_PWR_On, OUTPUT);
	int error_All;
	//EEPROM.get(Address_errorAll, error_All);                 // �������� ���������� ����� ������
	//EEPROM_off = EEPROM.read(Address_EEPROM_off);            // ������� ���������� �������� ����� ������ 
	//if (EEPROM_off != false)
	//{
	//	EEPROM.put(Address_errorAll, error_All + count_error); // ��������� ���������� ����� ������
	//}

	sendCommand("AT+HTTPTERM", "OK\r", "ERROR\r");                          // ������� HTTP ����������
	sendCommand("AT+CIPSHUT", "OK\r", "ERROR\r");                            // Deactivate GPRS PDP Context


	//wdt_disable();
	//wdt_enable(WDTO_2S);                                     // ������������ �� ����������� �������
	//	wdt_enable(WDTO_15MS);                                   // ������������ �� ����������� 
	//digitalWrite(_PWR_On, HIGH);                               // O�������� ������� ������ GPRS
	Serial.println("Wait reboot 60 sec");
	for (int i = 0; i < 60; i++)
	{
		delay(1000);
	//	wdt_reset();
		Serial.print(".");
	}
	//wdt_enable(WDTO_15MS);                                   // ������������ �� ����������� �������
	//while (1) {}
}
/*
bool CGPRS_SIM800C::checkSMS()
{
	if (sendCommand("AT+CMGR=1", "OK\r", "ERROR\r") == 1)       //  ���������� ������� "AT+CMGR=1", ����� ��������� ��������� +CMGR:
	{
		while (SIM_SERIAL.available())                          //���� ������ �� GSM ������
		{
			ch = SIM_SERIAL.read();
			val += char(ch);                                    //��������� ������� ������ � ���������� val
			delay(10);
		}
		return true;
	}
	return false;
}
*/
void CGPRS_SIM800C::send_sms(String text, String phone)  //��������� �������� ���
{
	Serial.println(F("SMS send started"));
	SIM_SERIAL.print("AT+CMGS=\"");                                // ��������� SMS
	delay(100);
	SIM_SERIAL.print(phone);                                // ��������� SMS
	delay(100);
	SIM_SERIAL.print("\"\r\n");                                // ��������� SMS
	delay(1000);
	SIM_SERIAL.print(text);
	delay(100);
	SIM_SERIAL.println((char)26);
	delay(100);
	Serial.println(phone);
	Serial.println(F("SMS send complete"));
	delay(2000);
}
bool CGPRS_SIM800C::deleteSMS(int n_sms)
{
	if (n_sms > 0)
	{
		if (sendCommand("AT+CMGD=1", "OK\r", "ERROR\r") == 1)                      //  remove the SMS
			return true;
	}
	else
	{
		if (sendCommand("AT+CMGDA=\"DEL ALL\"", "OK\r", "ERROR\r") == 1)          // remove all SMS
			return true;
	}
	return false;
}
boolean CGPRS_SIM800C::HTTP_ssl(boolean onoff)
{
	if (onoff)
	{
		if (sendCommand("AT+HTTPSSL=1", "OK\r", "ERROR\r",4000) == 1) return true;
	}
	else
	{
		if (sendCommand("AT+HTTPSSL=0", "OK\r", "ERROR\r",4000) == 1) return true;
	}
	return false;
}


//---------------------------------------------------------

void CGPRS_SIM800C::httpUninit()
{
  sendCommand("AT+HTTPTERM");
}
bool CGPRS_SIM800C::httpInit()
{
  if  (!sendCommand("AT+HTTPINIT", 10000) || !sendCommand("AT+HTTPPARA=\"CID\",1", 5000)) {
    httpState = HTTP_DISABLED;
    return false;
  }
  httpState = HTTP_READY;
  return true;
}
bool CGPRS_SIM800C::httpConnect(const char* url, const char* args)
{
    // Sets url
    SIM_SERIAL.print("AT+HTTPPARA=\"URL\",\"");
    SIM_SERIAL.print(url);
    if (args) {
      //  SIM_SERIAL.print('?');
        SIM_SERIAL.print(args);
    }

    SIM_SERIAL.println('\"');
    if (sendCommand(0))
    {
        // Starts GET action
        SIM_SERIAL.println("AT+HTTPACTION=0");
        httpState = HTTP_CONNECTING;
        m_bytesRecv = 0;
        m_checkTimer = millis();
    } else {
        httpState = HTTP_ERROR;
    }
    return false;
}
bool CGPRS_SIM800C::httpConnectStr(const char* url, String args)
{
	SIM_SERIAL.print(F("AT+HTTPPARA=\"URL\",\""));
	SIM_SERIAL.print(url);
	if (args)
	{
		SIM_SERIAL.print('?');
		SIM_SERIAL.print(args);
	}

	SIM_SERIAL.println('\"');
	delay(500);
	if (sendCommand("", "OK\r", "ERROR\r") == 1)
	{
		SIM_SERIAL.println(F("AT+HTTPACTION=0"));
		httpState = HTTP_CONNECTING;
		m_bytesRecv = 0;
		m_checkTimer = millis();
	}
	else
	{
		httpState = HTTP_ERROR;
	}
	return false;
}

// check if HTTP connection is established
// return 0 for in progress, 1 for success, 2 for error
byte CGPRS_SIM800C::httpIsConnected()
{
    byte ret = checkbuffer("0,200", "0,60", 10000);
    if (ret >= 2) {
        httpState = HTTP_ERROR;
        return -1;
    }
    return ret;
}
void CGPRS_SIM800C::httpRead()
{
    SIM_SERIAL.println("AT+HTTPREAD");
    httpState = HTTP_READING;
    m_bytesRecv = 0;
    m_checkTimer = millis();
}
// check if HTTP connection is established
// return 0 for in progress, -1 for error, number of http payload bytes on success
int CGPRS_SIM800C::httpIsRead()
{
    byte ret = checkbuffer("+HTTPREAD: ", "Error", 10000) == 1;
    if (ret == 1) {
        m_bytesRecv = 0;
        // read the rest data
        sendCommand(0, 100, "\r\n");
        int bytes = atoi(buffer);
        sendCommand(0);
        bytes = min(bytes, sizeof(buffer) - 1);
        buffer[bytes] = 0;
        return bytes;
    } else if (ret >= 2) {
        httpState = HTTP_ERROR;
        return -1;
    }
    return 0;
}
byte CGPRS_SIM800C::sendCommand(const char* cmd, unsigned int timeout, const char* expected)
{
  if (cmd) {
    purgeSerial();

#ifdef DEBUG
    DEBUG.print('>');
    DEBUG.println(cmd);
#endif
    SIM_SERIAL.println(cmd);
  }
  uint32_t t = millis();
  byte n = 0;
  do {
    if (SIM_SERIAL.available()) {
      char c = SIM_SERIAL.read();
      if (n >= sizeof(buffer) - 1) {
        // buffer full, discard first half
        n = sizeof(buffer) / 2 - 1;
        memcpy(buffer, buffer + sizeof(buffer) / 2, n);
      }
      buffer[n++] = c;
      buffer[n] = 0;
      if (strstr(buffer, expected ? expected : "OK\r")) {
#ifdef DEBUG
       DEBUG.print("[1]");
       DEBUG.println(buffer);
#endif
       return n;
      }
    }
  } while (millis() - t < timeout);
#ifdef DEBUG
   DEBUG.print("[0]");
   DEBUG.println(buffer);
#endif
  return 0;
}
byte CGPRS_SIM800C::sendCommand(const char* cmd, const char* expected1, const char* expected2, unsigned int timeout)
{
  if (cmd) {
    purgeSerial();
#ifdef DEBUG
    DEBUG.print('>');
    DEBUG.println(cmd);
#endif
    SIM_SERIAL.println(cmd);
  }
  uint32_t t = millis();
  byte n = 0;
  do {
    if (SIM_SERIAL.available()) {
      char c = SIM_SERIAL.read();
      if (n >= sizeof(buffer) - 1) {
        // buffer full, discard first half
        n = sizeof(buffer) / 2 - 1;
        memcpy(buffer, buffer + sizeof(buffer) / 2, n);
      }
      buffer[n++] = c;
      buffer[n] = 0;
      if (strstr(buffer, expected1)) {
#ifdef DEBUG
       DEBUG.print("[1]");
       DEBUG.println(buffer);
#endif
       return 1;
      }
      if (strstr(buffer, expected2)) {
#ifdef DEBUG
       DEBUG.print("[2]");
       DEBUG.println(buffer);
#endif
       return 2;
      }
    }
  } while (millis() - t < timeout);
#if DEBUG
   DEBUG.print("[0]");
   DEBUG.println(buffer);
#endif
  return 0;
}
byte CGPRS_SIM800C::checkbuffer(const char* expected1, const char* expected2, unsigned int timeout)
{
    while (SIM_SERIAL.available()) {
        char c = SIM_SERIAL.read();
        if (m_bytesRecv >= sizeof(buffer) - 1) {
            // buffer full, discard first half
            m_bytesRecv = sizeof(buffer) / 2 - 1;
            memcpy(buffer, buffer + sizeof(buffer) / 2, m_bytesRecv);
        }
        buffer[m_bytesRecv++] = c;
        buffer[m_bytesRecv] = 0;
        if (strstr(buffer, expected1)) {
            return 1;
        }
        if (expected2 && strstr(buffer, expected2)) {
            return 2;
        }
    }
    return (millis() - m_checkTimer < timeout) ? 0 : 3;
}
void CGPRS_SIM800C::purgeSerial()
{
  while (SIM_SERIAL.available()) SIM_SERIAL.read();
}

