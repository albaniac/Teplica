


/*

DS18B20 №1  oneWire_in
Черный - GND
Белый - 2
Красный - +5в

DS18B20 №1  oneWire_in
Черный - GND
Белый - 3
Красный - +5в

DS18B20 №2  oneWire_out
Черный - GND
Белый - 4
Красный - +5в

DS18B20 №3  oneWire_sun
Черный - GND
Белый - 5
Красный - +5в

Connection: BH1750
VCC-5v
GND-GND
SCL-SCL(analog pin 5)
SDA-SDA(analog pin 4)
ADD-NC or GND

*/





// Date and time functions using just software, based on millis() & timer

#include <Arduino.h>
#include <Wire.h>         // this #include still required because the RTClib depends on it
#include "RTClib.h"
#include <SoftwareSerial.h>
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <BH1750.h>
#include <EEPROM2.h>
#include <HMC5883L.h>
#include "Kalman.h"


#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
#define Serial SerialUSB
#endif

//+++++++++++++++++++++++++++ Настройка часов +++++++++++++++++++++++++++++++


RTC_Millis rtc;
uint8_t second = 0;                                    //Initialization time
uint8_t minute = 10;
uint8_t hour = 10;
uint8_t dow = 2;
uint8_t day = 15;
uint8_t month = 3;
uint16_t year = 16;


#define  SW_West   6                    // Назначение концевик Запад  
#define  SW_East   7                    // Назначение концевик Восток 
#define  SW_High   8                    // Назначение концевик Верх
#define  SW_Down   9                    // Назначение концевик Низ
#define  Rele1    10                    // Назначение Реле 1
#define  Rele2    11                    // Назначение Реле 2  
#define  motor_West  12                 // Назначение  мотор Запад
#define  motor_East  A3                 // Назначение  мотор Восток  Светодиод подсоединен к цифровому выводу 13 
#define  motor_High  A6                 // Назначение  мотор Вверх
#define  motor_Down  A7                 // Назначение  мотор Вниз 

#define  ds1   2                        // Назначение DS1820 №1  
#define  ds2   3                        // Назначение DS1820 №2  
#define  ds3   4                        // Назначение DS1820 №3  
#define  ds4   5                        // Назначение DS1820 №4  

float t1 = 0.0;                         // Результат измерения датчика температуры №1
float t2 = 0.0;                         // Результат измерения датчика температуры №2
float t3 = 0.0;                         // Результат измерения датчика температуры №3
float t4 = 0.0;                         // Результат измерения датчика температуры №4

uint8_t oneWirePins[] = { ds1, ds2, ds3, ds4 };                      //номера датчиков температуры DS18x20. Переставляя номера можно устанавливать очередность передачи в строке.
															// Сейчас первым идет внутренний датчик.
uint8_t oneWirePinsCount = sizeof(oneWirePins) / sizeof(int);

OneWire ds18x20_1(oneWirePins[0]);
OneWire ds18x20_2(oneWirePins[1]);
OneWire ds18x20_3(oneWirePins[2]);
OneWire ds18x20_4(oneWirePins[3]);
DallasTemperature sensor1(&ds18x20_1);
DallasTemperature sensor2(&ds18x20_2);
DallasTemperature sensor3(&ds18x20_3);
DallasTemperature sensor4(&ds18x20_4);


int setTmp  = 45; // переменная для заданного значения температуры

				 //Объявим переменную для хранения состояния реле
boolean relay1Status = LOW;
boolean relay2Status = LOW;
int delta_motor = 10;

//Объявим переменные для задания задержки
long previousMillis1 = 0;
long interval1 = 3000; // интервал опроса датчиков температуры


BH1750 lightMeter;
unsigned int lux = 0;


//+++++++++++++++++++++++++ Расчет положения солнца ++++++++++++++++++++++++++


//////////////////////////////////////////////////  
//PUT YOUR LATITUDE, LONGITUDE, AND TIME ZONE HERE
float latitude = 55.59;
float longitude = 37.12;
float timezone = 3;
//////////////////////////////////////////////////  

//If you live in the southern hemisphere, it would probably be easier
//for you if you make north as the direction where the azimuth equals
//0 degrees. To do so, switch the 0 below with 180.  
float northOrSouth = 180;

/////////////////////////////////////////////////////////// 
//MISC. VARIABLES
///////////////////////////////////////////////////////////  
float pi = 3.14159265;
float altitude;
float azimuth;
float delta;
float h;
/////////////////////////////////////////////////////////// 


//++++++++++++++++++++++++++++++++++ compass +++++++++++++++++++++++++++++++++++++++++


HMC5883L compass;
float headingDegrees = 0.00;
bool compass_enable = false;





void clock_read()
{
	DateTime now = rtc.now();
	second = now.second();
	minute = now.minute();
	hour = now.hour();
	day = now.day();
	month = now.month();
	year = now.year();
}



void print_data()
{
	DateTime now = rtc.now();
	Serial.print(now.year(), DEC);
	Serial.print('/');
	Serial.print(now.month(), DEC);
	Serial.print('/');
	Serial.print(now.day(), DEC);
	Serial.print(' ');
	Serial.print(now.hour(), DEC);
	Serial.print(':');
	Serial.print(now.minute(), DEC);
	Serial.print(':');
	Serial.print(now.second(), DEC);
	Serial.println();

}

void measure_light()
{
	lux = lightMeter.readLightLevel();
  
}


void sendTemps()
{
	sensor1.requestTemperatures();
	sensor2.requestTemperatures();
	sensor3.requestTemperatures();
	sensor4.requestTemperatures();
	t1 = sensor1.getTempCByIndex(0);
	t2 = sensor2.getTempCByIndex(0);
	t3 = sensor3.getTempCByIndex(0);
	t4 = sensor4.getTempCByIndex(0);
}

void read_compass()
{
	Vector norm = compass.readNormalize();

	// Calculate heading
	float heading = atan2(norm.YAxis, norm.XAxis);

	// Set declination angle on your location and fix heading
	// You can find your declination on: http://magnetic-declination.com/
	// (+) Positive or (-) for negative
	// For Bytom / Poland declination angle is 4'26E (positive)
	// Formula: (deg + (min / 60.0)) / (180 / M_PI);
	float declinationAngle = (4.0 + (26.0 / 60.0)) / (180 / M_PI);
	heading += declinationAngle;

	// Correct for heading < 0deg and heading > 360deg
	if (heading < 0)
	{
		heading += 2 * PI;
	}

	if (heading > 2 * PI)
	{
		heading -= 2 * PI;
	}

	// Convert to degrees
	headingDegrees = heading * 180 / M_PI;

	// Output
	Serial.print(" Heading = ");
	Serial.print(heading);
	Serial.print(" Degress = ");
	Serial.print(headingDegrees);                    // Применить для расчета положения установки
	Serial.println();
}


void sun_calc()
{
	float month2;
	float day2;
	float hour2;
	float minute2;
	clock_read();
	//SET TIME AND DATE HERE//////////////
	month2 = month;
	day2 = day;
	hour2 =  hour;//Use 24hr clock (ex: 1:00pm = 13:00) and don't use day3light saving time.
	minute2 = minute;
	//END SET TIME AND DATE /////////////


	//START OF THE CODE THAT CALCULATES THE POSITION OF THE SUN
	float n = daynum(month2) + day2;//NUMBER OF dayS SINCE THE START OF THE YEAR. 
	delta = .409279 * sin(2 * pi * ((284 + n) / 365.25));//SUN'S DECLINATION.
	day2 = dayToArrayNum(day2);//TAKES THE CURRENT day OF THE MONTH AND CHANGES IT TO A LOOK UP VALUE ON THE HOUR ANGLE TABLE.
	h = (FindH(day2, month2)) + longitude + (timezone * -1 * 15);//FINDS THE NOON HOUR ANGLE ON THE TABLE AND MODIFIES IT FOR THE USER'S OWN LOCATION AND TIME ZONE.
	h = ((((hour2 + minute2 / 60) - 12) * 15) + h)*pi / 180;//FURTHER MODIFIES THE NOON HOUR ANGLE OF THE CURRENT day AND TURNS IT INTO THE HOUR ANGLE FOR THE CURRENT HOUR AND MINUTE.
	altitude = (asin(sin(latitude) * sin(delta) + cos(latitude) * cos(delta) * cos(h))) * 180 / pi;//FINDS THE SUN'S ALTITUDE.
	azimuth = ((atan2((sin(h)), ((cos(h) * sin(latitude)) - tan(delta) * cos(latitude)))) + (northOrSouth*pi / 180)) * 180 / pi;//FINDS THE SUN'S AZIMUTH.

	Serial.print("month2 ");
	Serial.println(month2);
	Serial.print("day2 ");
	Serial.println(day);
	Serial.print("hour2 ");
	Serial.println(hour2);
	Serial.print("minute2 ");
	Serial.println(minute2);   //END OF THE CODE THAT CALCULATES THE POSITION OF THE SUN

	Serial.println("Altitude");
	Serial.println(altitude);
	Serial.println("Azimuth");
	Serial.println(azimuth);             // Применить для расчета положения установки

}


void run_uprav(float azimuth, float headingDegrees)
{
	// azimuth - расчетная величина
	// headingDegrees показания компаса
	// Время восход 05.10 ()56.2 град,  закат 19.50 (282.5 град)
	//if (azimuth > 56 && azimuth < 280)
	if (headingDegrees > 56 && headingDegrees < 300)
	{
		if (headingDegrees < azimuth)                 // Установка восточнее расчетной величины
		{
			if ((azimuth - headingDegrees) > delta_motor)
			{
				if (SW_West)        // Не достигнут конец разворота
				{
					digitalWrite(motor_West, HIGH);                          //  
					Serial.println("PLUS");             // Применить для расчета положения установки
				}
			}
			else
			{
				digitalWrite(motor_West, LOW);                          //  
				Serial.println("PLUS Stop");             // Применить для расчета положения установки
			}
		}

		if (headingDegrees > azimuth)               // Установка западние расчетной величины
		{
			if ((headingDegrees - azimuth) > delta_motor)
			{
				if (SW_East)
				{
					digitalWrite(motor_East, HIGH);                          //  

					Serial.println("MINUS");             // Применить для расчета положения установки
				}
			}
			else
			{
				digitalWrite(motor_East, LOW);                          //  

				Serial.println("MINUS Stop");             // Применить для расчета положения установки

			}
		}

	}
	else
	{
		Serial.println("No SUN");             // Применить для расчета положения установки

	}

}


//------------------------------------------------------------------------------



void setup() {
	Serial.begin(9600);
	// following line sets the RTC to the date & time this sketch was compiled
	rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
	// This line sets the RTC with an explicit date & time, for example to set
	// January 21, 2014 at 3am you would call:
	// rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));


	DeviceAddress deviceAddress;
	sensor1.setOneWire(&ds18x20_1);
	sensor2.setOneWire(&ds18x20_2);
	sensor3.setOneWire(&ds18x20_3);
	sensor4.setOneWire(&ds18x20_4);
	sensor1.begin();
	sensor2.begin();
	sensor3.begin();
	sensor4.begin();
	if (sensor1.getAddress(deviceAddress, 0)) sensor1.setResolution(deviceAddress, 12);
	if (sensor2.getAddress(deviceAddress, 0)) sensor2.setResolution(deviceAddress, 12);
	if (sensor3.getAddress(deviceAddress, 0)) sensor3.setResolution(deviceAddress, 12);
	if (sensor4.getAddress(deviceAddress, 0)) sensor4.setResolution(deviceAddress, 12);


	pinMode(SW_West, INPUT);                                 // Назначение  
	pinMode(SW_East, INPUT);                                 // Назначение  
	pinMode(SW_High, INPUT);                                 // Назначение  
	pinMode(SW_Down, INPUT);                                 // Назначение  

	digitalWrite(SW_West, HIGH);                             //  
	digitalWrite(SW_East, HIGH);                             //  
	digitalWrite(SW_High, HIGH);                             //  
	digitalWrite(SW_Down, HIGH);                             //  

	pinMode(Rele1, OUTPUT);                                  // Назначение  
	pinMode(Rele2, OUTPUT);                                  // Назначение  
	pinMode(motor_West, OUTPUT);                             // Назначение  
	pinMode(motor_East, OUTPUT);                             // устанавливаем режим работы вывода, как "выход"
	pinMode(motor_High, OUTPUT);                             // Назначение  
	pinMode(motor_Down, OUTPUT);                             // Назначение  

	digitalWrite(Rele1, LOW);                                //  
	digitalWrite(Rele2, LOW);                                //  
	digitalWrite(motor_West, HIGH);                          //  
	digitalWrite(motor_East, HIGH);                          //  
	digitalWrite(SW_High, HIGH);                             //  
	digitalWrite(motor_Down, HIGH);                          //



	EEPROM_write_byte(0, setTmp);							 // Сохранить в постоянной памяти заданную температуру
	setTmp = EEPROM_read_byte(0);                            // Считаем из постоянной памяти заданную температуру

	latitude = latitude * pi / 180;                          // Расчет положения солнца
															 // Initialize Initialize HMC5883L
	Serial.println("Initialize HMC5883L");


	if (!compass.begin())
	{
		Serial.println("Could not find a valid HMC5883L sensor, check wiring!");
		compass_enable = false;
	}
	else
	{
		// Set measurement range
		compass.setRange(HMC5883L_RANGE_1_3GA);

		// Set measurement mode
		compass.setMeasurementMode(HMC5883L_CONTINOUS);

		// Set data rate
		compass.setDataRate(HMC5883L_DATARATE_30HZ);

		// Set number of samples averaged
		compass.setSamples(HMC5883L_SAMPLES_8);

		// Set calibration offset. See HMC5883L_calibration.ino
		compass.setOffset(0, 0);

		//read_compass();
		compass_enable = true;
	}

	Serial.println(" ");
	Serial.println(F("System initialization OK!."));        // Информация о завершении настройки
}

void loop() {


	//Модуль опроса датчиков и получения сведений о температуре
	//Вызывается 1 раз в секунду
	unsigned long currentMillis1 = millis();
	if (currentMillis1 - previousMillis1 > interval1) {
		previousMillis1 = currentMillis1;

		//Запуск процедуры измерения температуры
		print_data();
		sun_calc();
		sendTemps();
		Serial.println("Temperature: ");

		Serial.println(t1);
		Serial.println(t2);
		Serial.println(t3);
		Serial.println(t4);


		//Проверка условия включения/выключения мотора
		if (t1<setTmp&&relay1Status == LOW) { relay1Status = HIGH; digitalWrite(Rele1, LOW); }
		if (t1>setTmp&&relay1Status == HIGH) { relay1Status = LOW; digitalWrite(Rele1, HIGH); }

		if (t2<setTmp&&relay2Status == LOW) { relay2Status = HIGH; digitalWrite(Rele2, LOW); }
		if (t2>setTmp&&relay2Status == HIGH) { relay2Status = LOW; digitalWrite(Rele2, HIGH); }

		Serial.print("relay1Status: "); Serial.println(relay1Status);
		Serial.print("relay1Status: "); Serial.println(relay2Status);

		if(compass_enable)
		{
			read_compass();
			run_uprav(azimuth, headingDegrees);
		}

	}


	//Serial.print(" seconds since 1970: ");
	//Serial.println(now.unixtime());

	//// calculate a date which is 7 days and 30 seconds into the future
	//DateTime future(now.unixtime() + 7 * 86400L + 30);

	//Serial.print(" now + 7d + 30s: ");
	//Serial.print(future.year(), DEC);
	//Serial.print('/');
	//Serial.print(future.month(), DEC);
	//Serial.print('/');
	//Serial.print(future.day(), DEC);
	//Serial.print(' ');
	//Serial.print(future.hour(), DEC);
	//Serial.print(':');
	//Serial.print(future.minute(), DEC);
	//Serial.print(':');
	//Serial.print(future.second(), DEC);
	//Serial.println();

	//Serial.println();
	//delay(3000);
}


//THIS CODE TURNS THE MONTH INTO THE NUMBER OF day3S SINCE JANUARY 1ST.
//ITS ONLY PURPOSE IS FOR CALCULATING DELTA (DECLINATION), AND IS NOT USED IN THE HOUR ANGLE TABLE OR ANYWHERE ELSE.
float daynum(float month) {
	float day3;
	if (month == 1) { day3 = 0; }
	if (month == 2) { day3 = 31; }
	if (month == 3) { day3 = 59; }
	if (month == 4) { day3 = 90; }
	if (month == 5) { day3 = 120; }
	if (month == 6) { day3 = 151; }
	if (month == 7) { day3 = 181; }
	if (month == 8) { day3 = 212; }
	if (month == 9) { day3 = 243; }
	if (month == 10) { day3 = 273; }
	if (month == 11) { day3 = 304; }
	if (month == 12) { day3 = 334; }
	return day3;
}

//THIS CODE TAKES THE day3 OF THE MONTH AND DOES ONE OF THREE THINGS: ADDS A day3, SUBTRACTS A day3, OR
//DOES NOTHING. THIS IS DONE SO THAT LESS VALUES ARE REQUIRED FOR THE NOON HOUR ANGLE TABLE BELOW.
int dayToArrayNum(int day3) {
	if ((day3 == 1) || (day3 == 2) || (day3 == 3)) { day3 = 0; }
	if ((day3 == 4) || (day3 == 5) || (day3 == 6)) { day3 = 1; }
	if ((day3 == 7) || (day3 == 8) || (day3 == 9)) { day3 = 2; }
	if ((day3 == 10) || (day3 == 11) || (day3 == 12)) { day3 = 3; }
	if ((day3 == 13) || (day3 == 14) || (day3 == 15)) { day3 = 4; }
	if ((day3 == 16) || (day3 == 17) || (day3 == 18)) { day3 = 5; }
	if ((day3 == 19) || (day3 == 20) || (day3 == 21)) { day3 = 6; }
	if ((day3 == 22) || (day3 == 23) || (day3 == 24)) { day3 = 7; }
	if ((day3 == 25) || (day3 == 26) || (day3 == 27)) { day3 = 8; }
	if ((day3 == 28) || (day3 == 29) || (day3 == 30) || (day3 == 31)) { day3 = 9; }
	return day3;
}

//////////////////////////////////////////////////////////////
//HERE IS THE TABLE OF NOON HOUR ANGLE VALUES. THESE VALUES GIVE THE HOUR ANGLE, IN DEGREES, OF THE SUN AT NOON (NOT SOLAR NOON)
//WHERE LONGITUDE = 0. dayS ARE SKIPPED TO SAVE SPACE, WHICH IS WHY THERE ARE NOT 365 NUMBERS IN THIS TABLE.
float FindH(int day3, int month) {
	float h;

	if (month == 1) {
		float h_Array[10] = {
			-1.038,-1.379,-1.703,-2.007,-2.289,-2.546,-2.776,-2.978,-3.151,-3.294, };
		h = h_Array[day3];
	}

	if (month == 2) {
		float h_Array[10] = {
			-3.437,-3.508,-3.55,-3.561,-3.545,-3.501,-3.43,-3.336,-3.219,-3.081, };
		h = h_Array[day3];
	}

	if (month == 3) {
		float h_Array[10] = {
			-2.924,-2.751,-2.563,-2.363,-2.153,-1.936,-1.713,-1.487,-1.26,-1.035, };
		h = h_Array[day3];
	}

	if (month == 4) {
		float h_Array[10] = {
			-0.74,-0.527,-0.322,-0.127,0.055,0.224,0.376,0.512,0.63,0.728, };
		h = h_Array[day3];
	}

	if (month == 5) {
		float h_Array[10] = {
			0.806,0.863,0.898,0.913,0.906,0.878,0.829,0.761,0.675,0.571, };
		h = h_Array[day3];
	}

	if (month == 6) {
		float h_Array[10] = {
			0.41,0.275,0.128,-0.026,-0.186,-0.349,-0.512,-0.673,-0.829,-0.977, };
		h = h_Array[day3];
	}

	if (month == 7) {
		float h_Array[10] = {
			-1.159,-1.281,-1.387,-1.477,-1.547,-1.598,-1.628,-1.636,-1.622,-1.585, };
		h = h_Array[day3];
	}

	if (month == 8) {
		float h_Array[10] = {
			-1.525,-1.442,-1.338,-1.212,-1.065,-0.9,-0.716,-0.515,-0.299,-0.07, };
		h = h_Array[day3];
	}

	if (month == 9) {
		float h_Array[10] = {
			0.253,0.506,0.766,1.03,1.298,1.565,1.831,2.092,2.347,2.593, };
		h = h_Array[day3];
	}

	if (month == 10) {
		float h_Array[10] = {
			2.828,3.05,3.256,3.444,3.613,3.759,3.882,3.979,4.049,4.091, };
		h = h_Array[day3];
	}

	if (month == 11) {
		float h_Array[10] = {
			4.1,4.071,4.01,3.918,3.794,3.638,3.452,3.236,2.992,2.722, };
		h = h_Array[day3];
	}

	if (month == 12) {
		float h_Array[10] = {
			2.325,2.004,1.665,1.312,0.948,0.578,0.205,-0.167,-0.534,-0.893, };
		h = h_Array[day3];
	}

	return h;
}
