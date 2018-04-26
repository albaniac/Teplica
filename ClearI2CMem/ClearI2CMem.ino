
#include <Wire.h> 



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
  Wire.requestFrom(deviceaddress,1);
  if (Wire.available()) rdata = Wire.read();
  return rdata;
}
void i2c_eeprom_read_buffer( int deviceaddress, unsigned int eeaddress, byte *buffer, int length )
{
  
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8)); // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress,length);
  int c = 0;
  for ( c = 0; c < length; c++ )
  if (Wire.available()) buffer[c] = Wire.read();
  
}
void i2c_eeprom_write_page( int deviceaddress, unsigned int eeaddresspage, byte* data, byte length ) 
{
  
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddresspage >> 8)); // MSB
  Wire.write((int)(eeaddresspage & 0xFF)); // LSB
  byte c;
  for ( c = 0; c < length; c++)
  Wire.write(data[c]);
  Wire.endTransmission();
  
}

void i2c_test1()
{
    Serial.print("Start clear"); //print content to serial port
  for( int n = 0; n<2048; n++)
  {
    i2c_eeprom_write_byte(0x50, n,0);
    
  }

  for(unsigned int x=0;x<2048;x++)
  {
    int  b = i2c_eeprom_read_byte(0x50, x); //access an address from the memory
    delay(10);
    Serial.print(x); //print content to serial port
    Serial.print(" - "); //print content to serial port
    Serial.println(b); //print content to serial port
  }
     Serial.print("Clear End"); //print content to serial port
}

void i2c_test()
{ 
  /*
  
  Serial.println("--------  EEPROM Test  ---------");
  char somedata[] = "this data from the eeprom i2c"; // data to write
  i2c_eeprom_write_page(0x50, 0, (byte *)somedata, sizeof(somedata)); // write to EEPROM 
  delay(100); //add a small delay
  Serial.println("Written Done");    
  delay(10);
  Serial.print("Read EERPOM:");
  byte b = i2c_eeprom_read_byte(0x50, 0); // access the first address from the memory
  char addr=0; //first address
  
  while (b!=0) 
  {
    Serial.print((char)b); //print content to serial port
    if (b!=somedata[addr]){
     e1=0;
     break;
     }      
    addr++; //increase address
    b = i2c_eeprom_read_byte(0x50, addr); //access an address from the memory
  }
   Serial.println();
   */
}


void setup() 
{
    Serial.begin(57600); 
  Wire.begin();
i2c_test1();
}

void loop() {
  // put your main code here, to run repeatedly:

}
