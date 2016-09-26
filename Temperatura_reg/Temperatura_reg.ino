// Подключаем библиотеку для работы с шиной OneWire
// Термометр будет подключен на Pin2
#include <OneWire.h>
OneWire oneWire(2);

//Подключаем библиотеку для работы с термометром
#include <DallasTemperature.h>

//Создаем объект sensors, подключенный по OneWire
DallasTemperature sensors(&oneWire);

//Создаем переменные для работы с термометром
DeviceAddress tempDeviceAddress;  //переменная для хранения адреса датчика
float temp1=0; //переменная для текущего значения температуры
int setTmp=0; // переменная для заданного значения температуры

//Подключаем LCD-дисплей
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//Подсветка управляется через пин D10
#define BACKLIGHT_PIN 10

//Создаем переменную для хранения состояния подсветки
boolean backlightStatus = 1;

// Подключаем библиотеку для работы с ARDUINO EEPROM
//Заданная температура будет храниться по адресу 0
#include <EEPROM2.h>

//Реле подключено к пину D11
#define RELAY_PIN 11

//Объявим переменную для хранения состояния реле
boolean relayStatus1=LOW;

//Объявим переменные для задания задержки
long previousMillis1 = 0;
long interval1 = 1000; // интервал опроса датчиков температуры

//Аналоговая клавиатура подключена к пину A0
#define KEYPAD_PIN A0
//Определим значения на аналоговом входе для клавиатуры 
#define ButtonUp_LOW 90
#define ButtonUp_HIGH 100
#define ButtonDown_LOW 240
#define ButtonDown_HIGH 280
#define ButtonLeft_LOW 390
#define ButtonLeft_HIGH 450
#define ButtonRight_LOW 0
#define ButtonRight_HIGH 50
#define ButtonSelect_LOW 620
#define ButtonSelect_HIGH 650

void setup() {

//Настроим пин для управления реле
  pinMode(RELAY_PIN,OUTPUT);
  digitalWrite(RELAY_PIN,LOW);

//Считаем из постоянной памяти заданную температуру
  setTmp=EEPROM_read_byte(0);

//Инициализируем термодатчик и установим разрешающую способность 12 бит (обычно она установлена по умолчанию, так что последнюю строчку можно опустить)
  sensors.begin();
  sensors.getAddress(tempDeviceAddress, 0);
  sensors.setResolution(12);
    
//Настроим подсветку дисплея
  pinMode(BACKLIGHT_PIN, OUTPUT);
  digitalWrite(BACKLIGHT_PIN, backlightStatus);

//Выведем на дисплей стартовое сообщение на 2 секунды
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Temp. Controller");
  lcd.setCursor(0, 1);
  lcd.print("      v1.0      ");
  delay(2000);

// выведем на дисплей заданное значение температуры на 2 секунды
  lcd.setCursor(0, 1);
  lcd.print("  Set temp:     ");
  lcd.setCursor(12,1);
  lcd.print(setTmp);
  delay(2000);

//Очистим дисплей
  lcd.begin(16, 2);
}



//Определим функцию для опроса аналоговой клавиатуры
//Функция опроса клавиатуры, принимает адрес пина, к которому подключена клавиатура, и возвращает код клавиши:
// 1 - UP
// 2 - DOWN
// 3 - LEFT
// 4 - RIGHT
// 5 - SELECT

int ReadKey(int keyPin)
{
 int KeyNum=0;
 int KeyValue1=0;
 int KeyValue2=0;

//Читаем в цикле аналоговый вход, для подавления дребезга и нестабильности читаем по два раза подряд, пока значения не будут равны.
//Если значения равны 1023 – значит не была нажата ни  одна клавиша.

do 
{
KeyValue1=analogRead(keyPin);
 KeyValue2=analogRead(keyPin);
 } while (KeyValue1==KeyValue2&&KeyValue2!=1023);

//Интерпретируем полученное значение и определяем код нажатой клавиши
 if (KeyValue2<ButtonUp_HIGH&&KeyValue2>ButtonUp_LOW) {KeyNum=1;}//Up
 if (KeyValue2<ButtonDown_HIGH&&KeyValue2>ButtonDown_LOW) {KeyNum=2;}//Down
 if (KeyValue2<ButtonLeft_HIGH&&KeyValue2>ButtonLeft_LOW) {KeyNum=3;}//Left
 if (KeyValue2<ButtonRight_HIGH&&KeyValue2>ButtonRight_LOW) {KeyNum=4;}//Right
 if (KeyValue2<ButtonSelect_HIGH&&KeyValue2>ButtonSelect_LOW) {KeyNum=5;}//Select

//Возвращаем код нажатой клавиши
return KeyNum;
}

//Определим процедуру редактирования заданной температуры
//Вызывается по нажатию клавиши Select, отображает на дисплее заданную температуру и позволяет изменять ее клавишами Up и Down

void setTemperature() {

  int keyCode=0;

//выводим на дисплей заданное значение температуры  
  lcd.begin(16,2);
  lcd.setCursor(0, 0);
  lcd.print("  Setting temp  ");
  lcd.setCursor(7, 1);
  lcd.print(setTmp);

//Опрашиваем клавиатуру, если нажата клавиша Up увеличиваем значение на 1, если Down – уменьшаем на 1
//Если нажаты клавиши Select или Right – цикл опроса прерывается
//Задержки введены для борьбы с дребезгом, если клавиши срабатывают четко – можно уменьшить время задержек или вообще их убрать
do {
  keyCode=ReadKey(KEYPAD_PIN);
  if (keyCode==1){setTmp++;delay(200);lcd.setCursor(7, 1);lcd.print(setTmp);}
  if (keyCode==2){setTmp--;delay(200);lcd.setCursor(7, 1);lcd.print(setTmp);}
} while (keyCode!=5 && keyCode!=4);
  delay(200);

//По клавише Select – созраняем в EEPROM измененное значение
//По клавише Right – восстанавливаем старое значение
if (keyCode==5) {EEPROM_write_byte(0, setTmp);}
if (keyCode==4) {setTmp = EEPROM_read_byte(0);}
}

void loop() {

//Модуль опроса датчиков и получения сведений о температуре
//Вызывается 1 раз в секунду
  unsigned long currentMillis1 = millis();
if(currentMillis1 - previousMillis1 > interval1) {
    previousMillis1 = currentMillis1;  

//Запуск процедуры измерения температуры
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures();
  sensors.setWaitForConversion(true);

delay(750); // задержка для обработки информации внутри термометра, в данном случае можно не задавать

//Считывание значения температуры
  sensors.getAddress(tempDeviceAddress, 0);
  temp1=sensors.getTempC(tempDeviceAddress);

// Вывод текущего значения температуры на дисплей
  lcd.setCursor(0, 0);
  lcd.print("  Current temp  ");
  lcd.setCursor(5, 1);
  lcd.print(temp1);
//  Serial.println(temp1,4);
}

//Проверка условия включения/выключения нагревателя
if (temp1<setTmp&&relayStatus1==LOW){relayStatus1=HIGH; digitalWrite(RELAY_PIN,HIGH);}
if (temp1>setTmp&&relayStatus1==HIGH){relayStatus1=LOW; digitalWrite(RELAY_PIN,LOW);}

// Опрос клавиатуры 
int Feature = ReadKey(KEYPAD_PIN);
if (Feature==1 ) {backlightStatus=1;digitalWrite(BACKLIGHT_PIN, backlightStatus);} //Включение подсветки
if (Feature==2 ) {backlightStatus=0;digitalWrite(BACKLIGHT_PIN, backlightStatus);} //Отключение подсветки
if (Feature==5 ) {delay(200);setTemperature();} //Переход к редактированию заданной температуры
}

