// Arduino code ,используется банк А на вход,а банк В - на вывод.

// pins 15,16 и 17 к GND для I2C шины адреса 0x20

#include "Wire.h"     // подключаем библиотеку I2C
byte a=0;
boolean registers[8]; // храним состояние регистров банка В


void setup()
{
  
Serial.begin(9600);
    
  
Wire.begin();                // wake up I2C bus
// настраиваем банк B на вывод
Wire.beginTransmission(0x20);// подключение к чипу
Wire.write((byte)0x01);      // выбираем банк B
Wire.write((byte)0x00);      // установка банка B на вывод,точнее устанавливаем на всех выводах значение 0
Wire.endTransmission();      // отключаемся.
//

}

void loop()
{
// чтение банки A
Wire.beginTransmission(0x20); // подключение к чипу
Wire.write(0x12);             // выбираем для работы банку A
Wire.endTransmission();       // отключаемся.
Wire.requestFrom(0x20, 1);    // отправляем один байт
a=Wire.read();                // чтение состояния портов банка А в переменную 'a'

// выводим состояние портов банка А,если на них подан минус.
if (!((a>>0)&1))  Serial.print(0);
if (!((a>>0)&2))  Serial.print(1);
if (!((a>>0)&4))  Serial.print(2);
if (!((a>>0)&8))  Serial.print(3);
if (!((a>>0)&16))  Serial.print(4);
if (!((a>>0)&32))  Serial.print(5);
if (!((a>>0)&64))  Serial.print(6);
if (!((a>>0)&128))  Serial.print(7);

//  Serial.print(a);

Serial.println("--");
 
// тест мигалка на выводе банки В порта номер 3
mcWrite(3,0); 
delay(500);

mcWrite(3,1);
delay(500);
}




// подпрограмма записи в банк B
void mcWrite(int whichPin, int whichState) {
 byte d=0;
 byte l=1;
  registers[whichPin] = whichState;

for(int i = 0; i <=  7; i++){
d=registers[i]*l+d;
l=l*2;
}
//запись в банк B
Wire.beginTransmission(0x20);  // подключение к чипу
Wire.write(0x13);              // выбираем для работы банку B
Wire.write(d);                 // запись байта
Wire.endTransmission();        // отключаемся

}

