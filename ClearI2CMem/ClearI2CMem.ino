
#include <Wire.h>
unsigned int adr_max = 4096;
int adr_eeprom = 0x50;

void i2c_eeprom_write_byte( int deviceaddress, unsigned int eeaddress, byte data )
{
  int rdata = data;
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8)); // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(rdata);
  Wire.endTransmission();
  delay(10);
}
byte i2c_eeprom_read_byte( int deviceaddress, unsigned int eeaddress ) {
  byte rdata = 0xFF;
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8)); // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress, 1);
  if (Wire.available()) rdata = Wire.read();
  return rdata;
}

void i2c_test1()
{
  Serial.print("Start clear"); //print content to serial port
  for ( unsigned int n = 0; n < adr_max; n++)
  {
    i2c_eeprom_write_byte(adr_eeprom, n, 0xFF);
    Serial.println(n);
  }
/*
  for (unsigned int x = 0; x < adr_max; x++)
  {
    int  b = i2c_eeprom_read_byte(adr_eeprom, x); //access an address from the memory
    delay(10);
    Serial.print(x); //print content to serial port
    Serial.print(" - "); //print content to serial port
    Serial.println(b); //print content to serial port
  }
  */
  Serial.print("Clear End"); //print content to serial port
}

void determine_max_adr()
{
  unsigned int adr_test = 2048;
  int  b = 0;
  byte x = 41;

  do {
    i2c_eeprom_write_byte(adr_eeprom, adr_test - 1, x);
    b = i2c_eeprom_read_byte(adr_eeprom, adr_test - 1); //access an address from the memory
    Serial.println(b);
    if (b == x)
    {
      adr_test += adr_test;
      Serial.println(adr_test);
      b = x;
      x++;
    }
    else
    {
      switch (adr_test)
      {
        case 4096:
          Serial.print("AT24C32 Max - ");
          break;
        case 8192:
          Serial.print("AT24C64 Max -  ");
          break;
        case 16384:
          Serial.println("AT24C128 Max - ");
          break;
        case 32768 :
          Serial.println("AT24C256 Max - ");
          break;
        case 65536 :
          Serial.println("AT24C512 Max - ");
          break;
        default:
          Serial.println(adr_test);

          break;
      }
    }
  } while (x < 46);

  Serial.println(adr_test);
  adr_max = adr_test;
}


void setup()
{
  Serial.begin(57600);
  Wire.begin();
 // determine_max_adr();
   i2c_test1();
}

void loop() {
  // put your main code here, to run repeatedly:

}
