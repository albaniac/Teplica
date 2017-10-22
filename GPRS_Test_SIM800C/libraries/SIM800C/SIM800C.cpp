/*************************************************************************
* SIM800 GPRS/HTTP Library
* Distributed under GPL v2.0
* Written by Stanley Huang <stanleyhuangyc@gmail.com>
* For more information, please visit http://arduinodev.com
*************************************************************************/

#include "SIM800C.h"

bool CGPRS_SIM800C::init(unsigned long baud)
{
	_baud       = baud;                                            // Установить скорость Serial

//	int count_status = 0;                                          // Установим количество попыток включения SIM800C , иначе что то не так - сбросим микроконтроллер
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
		SIM_SERIAL.print("AT+IPR=");                	 // Установить скорость обмена
		SIM_SERIAL.print(_baud);
		delay(100);
		sendCommand("ATE0");                             // Отключить эхо 
		delay(100);
		sendCommand("AT+CFUN=1");                        // 1 – нормальный режим (по умолчанию). Второй параметр 1 – перезагрузить (доступно только в нормальном режиме, т.е. параметры = 1,1)
		delay(100);
		sendCommand("AT+CMGF=1");                        // режим кодировки СМС - обычный (для англ.)
		delay(100);
		sendCommand("AT+CLIP=1");                        // включаем АОН
		delay(100);
		sendCommand("AT+CSCS=\"GSM\"");                   // режим кодировки текста
		delay(100);
		//sendCommand("AT+CNMI=2,2");                    // отображение смс в терминале сразу после приема (без этого сообщения молча падают в память)tln("AT+CSCS=\"GSM\""); 
		//delay(100);
		//sendCommandS("AT+CMGDA=\"DEL ALL\"");           // AT+CMGDA=«DEL ALL» команда удалит все сообщения
		//delay(100);
		//sendCommand("AT+GMR"));                         // Номер прошивки
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
				operator_Num = 0;                                  // Порядковый номер оператора МТС
			}
			else if (OperatorName.indexOf(F("Bee")) > -1)
			{
				apn = F("internet.beeline.ru");
				user = F("beeline");
				pwd = F("beeline");
				cont = F("internet.beeline.ru");
				Serial.println(F("Beeline"));
				operator_Num = 1;                                  // Порядковый номер оператора Beeline
			}
			else if (OperatorName.indexOf(F("Mega")) > -1)
			{
				apn = F("internet");
				user = "";
				pwd = "";
				cont = F("internet");
				Serial.println(F("MEGAFON"));
				operator_Num = 2;                                  // Порядковый номер оператора Megafon
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
		sendCommand("AT+SAPBR=1,1", "OK\r", "ERROR\r", 150000);  return 0;                 // установка GPRS связи  !!!! Уточнить команду !!!
	}
	return 3;                                                                           // Неуспешная регистрация
}
bool CGPRS_SIM800C::connect_IP_GPRS()
{
	for (byte n = 0; n < 30; n++)
	{
		if (sendCommand("AT+SAPBR=2,1", "OK\r", "ERROR\r", 15000))  return true;         // получить IP адрес
		delay(500);
	}
	return false;
}
uint8_t CGPRS_SIM800C::getNetworkStatus()
{
	for (byte n = 0; n < 30; n++)
	{
		if (sendCommand("AT+CREG?", "OK\r", "ERROR\r"))                       // Тип регистрации сети
		{
			// Первый параметр:
			// 0 – нет кода регистрации сети
			// 1 – есть код регистрации сети
			// 2 – есть код регистрации сети + доп параметры
			// Второй параметр:
			// 0 – не зарегистрирован, поиска сети нет
			// 1 – зарегистрирован, домашняя сеть
			// 2 – не зарегистрирован, идёт поиск новой сети
			// 3 – регистрация отклонена
			// 4 – неизвестно
			// 5 – роуминг

			char *p = strstr(buffer, "0,");                        // Получить второй параметр
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
		char *p = strstr(buffer, "\r");                       //Функция strstr() возвращает указатель на первое вхождение в строку, 
														      //на которую указывает str1, строки, указанной str2 (исключая завершающий нулевой символ).
														      //Если совпадений не обнаружено, возвращается NULL.
		if (p)
		{
			p += 2;
			char *s = strchr(p, '\r');                       // Функция strchr() возвращает указатель на первое вхождение символа ch в строку, 
													         //на которую указывает str. Если символ ch не найден,
													         //возвращается NULL. 
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
		char *p = strstr(buffer, "\r");              //Функция strstr() возвращает указатель на первое вхождение в строку, 
													 //Если совпадений не обнаружено, возвращается NULL.
		if (p)
		{
			p += 2;
			char *s = strchr(p, '\r');          // Функция strchr() возвращает указатель на первое вхождение символа ch в строку, 
												//на которую указывает str. Если символ ch не найден,
												//возвращается NULL. 
			if (s) *s = 0;   strcpy(buffer, p);
			int i = 0;

			for (i = 0; i<120; i++)
			{
				if (buffer[i] == 'f')  break;
				if (buffer[i] >= '0'&&buffer[i] <= '9') // если цифру найдено - то выводим ее
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
	int count_connect = 0;                                                // Счетчик количества попыток проверки подключения Network registration (сетевому оператору)
	for (;;)                                                              // Бесконечный цикл пока не наступит, какое то состояние для выхода
	{
	//	if (sendCommandS(F("AT+CREG?")) == 1) break;                      // Все нормально, в сети оператора зарегистрированы , Прервать попытки и выйти из цикла
		if (sendCommand("AT+CREG?", "OK\r", "ERROR\r") == 1) break;      // Все нормально, в сети оператора зарегистрированы , Прервать попытки и выйти из цикла
		Serial.print(">");
		Serial.println(buffer);                                           // Не получилось, ("ERROR") 
		String stringError = buffer;
		if (stringError.indexOf(F("ERROR")) > -1)
		{
			Serial.println(F("\nNo GPRS connection"));
			delay(1000);
		}
		delay(1000);                                                     // Подождать секунду.
		count_connect++;
		if (count_connect > 60)
		{
			reboot(errors); //break;                                   //вызываем reset при отсутствии доступа к сетевому оператору в течении 60 секунд
		}
	}
	delay(1000);
	count_connect = 0;                                                   // Счетчик количества попыток проверки подключения Attach from GPRS service
	for (;;)                                                             // Бесконечный цикл пока не наступит, какое то состояние для выхода
	{
		if (sendCommand("AT+CGATT?", "OK\r", "ERROR\r") == 1) break;     // Все нормально, модуль подключен к GPRS service , Прервать попытки и выйти из цикла
		Serial.print(F(">"));
		Serial.println(buffer);                                          // Не получилось, ("ERROR") 
		String stringError = buffer;
		if (stringError.indexOf(F("ERROR")) > -1)
		{
			Serial.println(F("\nNo GPRS connection"));
			delay(1000);
		}
		delay(1000);                                                     // Подождать секунду.
		count_connect++;
		if (count_connect > 60)
		{
			reboot(errors);                                              //вызываем reset при отсутствии доступа к  GPRS service в течении 60 секунд
		}
	}

	//++++++++++++++++ Проверки пройдены, подключаемся к интернету по протоколу TCP для проверки ping ++++++++++++

	switch (operator_Num)                                                   // Определяем вариант подключения в зависимости от оператора
	{
	case 0:
		sendCommand("AT+CSTT=\"internet.mts.ru\"", "OK\r", "ERROR\r");       //Настроить точку доступа MTS. При повторных пингах будет выдавать ошибку. Это нормально потому что данные уже внесены.
		break;
	case 1:
		sendCommand("AT+CSTT=\"internet.beeline.ru\"", "OK\r", "ERROR\r");   //Настроить точку доступа  beeline
		break;
	case 2:
		sendCommand("AT+CSTT=\"internet\"", "OK\r", "ERROR\r");              //Настроить точку доступа Megafon
		break;
	}
	delay(1000);
	sendCommand("AT+CIICR", "OK\r", "ERROR\r",10000);                             // Поднимаем протокол Bring Up Wireless Connection with GPRS  
	return 0;
}
bool CGPRS_SIM800C::ping(const char* url)
{
	sendCommand("AT+CIFSR", "OK\r", "ERROR\r");                             //Получить локальный IP-адрес
	delay(1000);
	SIM_SERIAL.print(F("AT+CIPPING=\""));                                  // Отправить команду ping
	SIM_SERIAL.print(url);
	SIM_SERIAL.println('\"');
	delay(5000);

	//++++++++++++++++++++++++++ Ожидаем ответ сайта на ping  ++++++++++++++++++++++++++++++++++++++++   

	if (sendCommand("", "OK\r", "ERROR\r",10000) == 1)            // Ответ на ping получен 
	{
		SIM_SERIAL.print(F("AT+CIPSHUT"));                           // Закрыть соединение
		return true;
	}
	SIM_SERIAL.print(F("AT+CIPSHUT"));                             // Ошибка, что то не так пошло. На всякий случай закрываем соединение
	return false;
}
void CGPRS_SIM800C::reboot(int count_error)
{
//	pinMode(_PWR_On, OUTPUT);
	int error_All;
	//EEPROM.get(Address_errorAll, error_All);                 // Получить количество общих ошибок
	//EEPROM_off = EEPROM.read(Address_EEPROM_off);            // Признак обновления счетчика общих ошибок 
	//if (EEPROM_off != false)
	//{
	//	EEPROM.put(Address_errorAll, error_All + count_error); // Сохранить количество общих ошибок
	//}

	sendCommand("AT+HTTPTERM", "OK\r", "ERROR\r");                          // Закрыть HTTP соединение
	sendCommand("AT+CIPSHUT", "OK\r", "ERROR\r");                            // Deactivate GPRS PDP Context


	//wdt_disable();
	//wdt_enable(WDTO_2S);                                     // Перезагрузка по сторожевому таймеру
	//	wdt_enable(WDTO_15MS);                                   // Перезагрузка по сторожевому 
	//digitalWrite(_PWR_On, HIGH);                               // Oтключаем питание модуля GPRS
	Serial.println("Wait reboot 60 sec");
	for (int i = 0; i < 60; i++)
	{
		delay(1000);
	//	wdt_reset();
		Serial.print(".");
	}
	//wdt_enable(WDTO_15MS);                                   // Перезагрузка по сторожевому таймеру
	//while (1) {}
}
/*
bool CGPRS_SIM800C::checkSMS()
{
	if (sendCommand("AT+CMGR=1", "OK\r", "ERROR\r") == 1)       //  отправляет команду "AT+CMGR=1", поиск ответного сообщения +CMGR:
	{
		while (SIM_SERIAL.available())                          //есть данные от GSM модуля
		{
			ch = SIM_SERIAL.read();
			val += char(ch);                                    //сохраняем входную строку в переменную val
			delay(10);
		}
		return true;
	}
	return false;
}
*/
void CGPRS_SIM800C::send_sms(String text, String phone)  //процедура отправки СМС
{
	Serial.println(F("SMS send started"));
	SIM_SERIAL.print("AT+CMGS=\"");                                // Отправить SMS
	delay(100);
	SIM_SERIAL.print(phone);                                // Отправить SMS
	delay(100);
	SIM_SERIAL.print("\"\r\n");                                // Отправить SMS
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

