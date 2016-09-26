

/*
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




#include <modbus.h>
#include <modbusDevice.h>
#include <modbusRegBank.h>
#include <modbusSlave.h>
#include <Wire.h>
#include <BH1750.h>
#include <OneWire.h>
#include <MsTimer2.h> 
#include "RTClib.h"

#define  ds1   2                        // Назначение DS1820 №1  
#define  ds2   3                        // Назначение DS1820 №2  
#define  ds3   4                        // Назначение DS1820 №3  
#define  ds4   5                        // Назначение DS1820 №4  

#define  SW_West   6                    // Назначение концевик Запад  
#define  SW_East   7                    // Назначение концевик Восток 
#define  SW_High   8                    // Назначение концевик Верх
#define  SW_Down   9                    // Назначение концевик Низ
#define  Rele1    10                    // Назначение Реле 1
#define  Rele2    11                    // Назначение Реле 2  
#define  motor_West  12                 // Назначение  
#define  motor_East  A3                 // Светодиод подсоединен к цифровому выводу 13 
#define  motor_High  A6                 // Назначение  
#define  motor_Down  A7                 // Назначение  

//++++++++++++ GY61 ++++++++++++++++++++++
//Analog read pins
const int xPin = A0;
const int yPin = A1;
const int zPin = A2;

//The minimum and maximum values that came from
//the accelerometer while standing still
//You very well may need to change these
// Минимальные и максимальные значения, которые пришли из
// Акселерометр, стоя по-прежнему
// Вы очень хорошо, возможно, потребуется изменить эти
int minVal = 265;  
int maxVal = 402;

//to hold the caculated values
double x_GY61;
double y_GY61;
double z_GY61;
//--------------------------------------------------

RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

OneWire  ds_tube(ds1);                  // on pin 10 (a 4.7K resistor is necessary)
OneWire  ds_in(ds2);                    // on pin 10 (a 4.7K resistor is necessary)
OneWire  ds_out(ds3);                   // on pin 10 (a 4.7K resistor is necessary)
OneWire  ds_sun(ds4);                   // on pin 10 (a 4.7K resistor is necessary)

float ds_ds1;
float ds_ds2;
float ds_ds3;
float ds_ds4;

//+++++++++++++++++++ MODBUS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

modbusDevice regBank;
modbusSlave slave;

int ledState = LOW;             // ledState used to set the LED
unsigned int lux = 0;

BH1750 lightMeter;

void flash_time()                                              // Программа обработчик прерывания 
{ 
	//digitalWrite(ledPin, HIGH);   // включаем светодиод
	slave.run();
	//digitalWrite(ledPin, LOW);   // включаем светодиод
}
void measure_temp_in()
{
  byte i;
  byte present = 0;
  byte type_s = 0;
  byte data[12];
  byte addr[8];
  float celsius = 0;
 // regBank.set(40001,0); 

  if ( !ds_in.search(addr)) 
  {
    ds_in.reset_search();
    delay(250);
    return;
  }

  ds_in.reset();
  ds_in.select(addr);
  ds_in.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  present = ds_in.reset();
  ds_in.select(addr);    
  ds_in.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++)
  {           // we need 9 bytes
    data[i] = ds_in.read();
  }
  int16_t raw = 0;
  raw = (data[1] << 8) | data[0];
  if (type_s) 
  {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) 
	{
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  }
  else 
  {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
  }
  celsius = (float)raw / 16.0*10;

  regBank.set(40001,(int)celsius); 

}
void measure_temp_out()
{
  byte i;
  byte present = 0;
  byte type_s = 0;
  byte data[12];
  byte addr[8];
  float celsius = 0;
 // regBank.set(40002,0); 

  if ( !ds_out.search(addr)) 
  {
    ds_out.reset_search();
    delay(250);
    return;
  }

  ds_out.reset();
  ds_out.select(addr);
  ds_out.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  present = ds_out.reset();
  ds_out.select(addr);    
  ds_out.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++)
  {           // we need 9 bytes
    data[i] = ds_out.read();
  }
  int16_t raw = 0;
  raw = (data[1] << 8) | data[0];
  if (type_s) 
  {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) 
	{
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  }
  else 
  {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
  }
  celsius = (float)raw / 16.0*10;

  regBank.set(40002,(int)celsius); 

}
void measure_temp_sun()
{
  byte i;
  byte present = 0;
  byte type_s = 0;
  byte data[12];
  byte addr[8];
  float celsius = 0;
 // regBank.set(40003,0); 
  
  if ( !ds_sun.search(addr)) 
  {
    ds_sun.reset_search();
    delay(250);
    return;
  }

  ds_sun.reset();
  ds_sun.select(addr);
  ds_sun.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  present = ds_sun.reset();
  ds_sun.select(addr);    
  ds_sun.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++)
  {           // we need 9 bytes
    data[i] = ds_sun.read();
  }
  int16_t raw = 0;
  raw = (data[1] << 8) | data[0];
  if (type_s) 
  {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) 
	{
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  }
  else 
  {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
  }
  celsius = (float)raw / 16.0*10;

  regBank.set(40003,(int)celsius); 

}
void measure_temp_tube()
{
  byte i;
  byte present = 0;
  byte type_s = 0;
  byte data[12];
  byte addr[8];
  float celsius = 0;
 // regBank.set(40004,0); 
  
  if ( !ds_tube.search(addr)) 
  {
    ds_tube.reset_search();
    delay(250);
    return;
  }

  ds_tube.reset();
  ds_tube.select(addr);
  ds_tube.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  present = ds_tube.reset();
  ds_tube.select(addr);    
  ds_tube.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++)
  {           // we need 9 bytes
    data[i] = ds_tube.read();
  }
  int16_t raw = 0;
  raw = (data[1] << 8) | data[0];
  if (type_s) 
  {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) 
	{
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  }
  else 
  {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
  }
  celsius = (float)raw / 16.0*10;

  regBank.set(40004,(int)celsius); 

}

void measure_light()
{
 	lux = lightMeter.readLightLevel();
//	regBank.set(40005, lux);                           //   
}

void measure_GY61()
{
  //read the analog values from the accelerometer
  int xRead = analogRead(xPin);
  int yRead = analogRead(yPin);
  int zRead = analogRead(zPin);

  //convert read values to degrees -90 to 90 - Needed for atan2
  int xAng = map(xRead, minVal, maxVal, -90, 90);
  int yAng = map(yRead, minVal, maxVal, -90, 90);
  int zAng = map(zRead, minVal, maxVal, -90, 90);

  //Caculate 360deg values like so: atan2(-yAng, -zAng)
  //atan2 outputs the value of -π to π (radians)
  //We are then converting the radians to degrees
// Вычислить значения 360deg следующим образом: atan2 (-yAng, -zAng) 
// atan2 выводит значение -я до п (радиан) 
// Мы тогда преобразования радианы в градусы
  
  x_GY61 = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI);
  y_GY61 = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);
  z_GY61 = RAD_TO_DEG * (atan2(-yAng, -xAng) + PI);

  //Output the caculations
  Serial.print("x: ");
  Serial.print(x_GY61);
  Serial.print(" | y: ");
  Serial.print(y_GY61);
  Serial.print(" | z: ");
  Serial.println(z_GY61);
}

void measure_time()
{
   DateTime now = rtc.now();
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
  /*  Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");*/
	 // calculate a date which is 7 days and 30 seconds into the future
	// Вычислить дату, которая составляет 7 дней и 30 секунд в будущее
	 //DateTime future (now - TimeSpan(0,0,1,10));
  //  
  //  Serial.print(" now + 7d + 30s: ");
  //  Serial.print(future.year(), DEC);
  //  Serial.print('/');
  //  Serial.print(future.month(), DEC);
  //  Serial.print('/');
  //  Serial.print(future.day(), DEC);
  //  Serial.print(' ');
  //  Serial.print(future.hour(), DEC);
  //  Serial.print(':');
  //  Serial.print(future.minute(), DEC);
  //  Serial.print(':');
  //  Serial.print(future.second(), DEC);
  //  Serial.println();
    
    Serial.println();

}
 
void setup(void)
{
    Serial.begin(9600);
	//setup_regModbus();
	//slave._device = &regBank;  
	//slave.setSerial(0,9600);                              // Подключение к протоколу MODBUS компьютера Serial
	lightMeter.begin();

	 if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
	if (!rtc.isrunning()) 
	{
		Serial.println("RTC is NOT running!");
		// following line sets the RTC to the date & time this sketch was compiled
		rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
	
	}
	//rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

	pinMode(SW_West, INPUT);                                 // Назначение  
	pinMode(SW_East, INPUT);                                 // Назначение  
	pinMode(SW_High, INPUT);                                // Назначение  
	pinMode(SW_Down, INPUT);                                // Назначение  
	
	digitalWrite(SW_West, HIGH);                              //  
	digitalWrite(SW_East, HIGH);                              //  
	digitalWrite(SW_High, HIGH);                              //  
	digitalWrite(SW_Down, HIGH);                              //  

	pinMode(Rele1, OUTPUT);                                // Назначение  
	pinMode(Rele2, OUTPUT);                                // Назначение  
	pinMode(motor_West, OUTPUT);                                // Назначение  
	pinMode(motor_East, OUTPUT);                              // устанавливаем режим работы вывода, как "выход"
	pinMode(motor_High, OUTPUT);                                // Назначение  
	pinMode(motor_Down, OUTPUT);                                // Назначение  


	digitalWrite(Rele1, LOW);                            //  
	digitalWrite(Rele2, LOW);                            //  
	digitalWrite(motor_West,HIGH);                            //  
	digitalWrite(motor_East,HIGH);                            //  
	digitalWrite(SW_High,HIGH);                            //  
	digitalWrite(motor_Down,HIGH);    

	//MsTimer2::set(500, flash_time);                               // 300ms период таймера прерывани
	//MsTimer2::start();                                           // Включить таймер прерывания
}

void loop(void)
{
	measure_light();
//	delay(100);
	measure_temp_in();
//	delay(100);
	measure_temp_out();
//	delay(100);
	measure_temp_sun();
//	delay(100);
	measure_temp_tube();
//	delay(100);
	 measure_time();
	// measure_GY61();
	//set_port();
//	delay(1000);

 //  // if the LED is off turn it on and vice-versa:
 //   if (ledState == LOW)
	//{
 //     ledState = HIGH;
 //   } else {
 //     ledState = LOW;
 //   }

 //   // set the LED with the ledState of the variable:
 //   digitalWrite(Pin13, ledState);
}