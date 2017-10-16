/*************************************************************************
* Test sketch for SIM800 library
* Distributed under GPL v2.0
* Written by Stanley Huang <stanleyhuangyc@gmail.com>
* For more information, please visit http://arduinodev.com
*************************************************************************/

#include "SIM800.h"

#define APN "connect"
#define Serial Serial
static const char* url = "http://arduinodev.com/datetime.php";

CGPRS_SIM800 gprs;
uint32_t count = 0;
uint32_t errors = 0;

void setup()
{
  Serial.begin(9600);
  while (!Serial);

  Serial.println("SIM800 TEST");

  for (;;) {
    Serial.print("Resetting...");
    while (!gprs.init()) {
      Serial.write('.');
    }
    Serial.println("OK");
    
    Serial.print("Setting up network...");
    byte ret = gprs.setup(APN);
    if (ret == 0)
      break;
    Serial.print("Error code:");
    Serial.println(ret);
    Serial.println(gprs.buffer);
  }
  Serial.println("OK");
  delay(3000);  
  
  if (gprs.getOperatorName()) {
    Serial.print("Operator:");
    Serial.println(gprs.buffer);
  }
  int ret = gprs.getSignalQuality();
  if (ret) {
     Serial.print("Signal:");
     Serial.print(ret);
     Serial.println("dB");
  }
  for (;;) {
    if (gprs.httpInit()) break;
    Serial.println(gprs.buffer);
    gprs.httpUninit();
    delay(1000);
  }
  delay(3000);
}

void loop()
{
  
  char mydata[16];
  sprintf(mydata, "t=%lu", millis());
  Serial.print("Requesting ");
  Serial.print(url);
  Serial.print('?');
  Serial.println(mydata);
  gprs.httpConnect(url, mydata);
  count++;
  while (gprs.httpIsConnected() == 0) {
    // can do something here while waiting
    Serial.write('.');
    for (byte n = 0; n < 25 && !gprs.available(); n++) {
      delay(10);
    }
  }
  if (gprs.httpState == HTTP_ERROR) {
    Serial.println("Connect error");
    errors++;
    delay(3000);
    return; 
  }
  Serial.println();
  gprs.httpRead();
  int ret;
  while ((ret = gprs.httpIsRead()) == 0) {
    // can do something here while waiting
  }
  if (gprs.httpState == HTTP_ERROR) {
    Serial.println("Read error");
    errors++;
    delay(3000);
    return; 
  }

  // now we have received payload
  Serial.print("[Payload]");
  Serial.println(gprs.buffer);

  // show position
  GSM_LOCATION loc;
  if (gprs.getLocation(&loc)) {
    Serial.print("LAT:");
    Serial.print(loc.lat, 6);
    Serial.print(" LON:");
    Serial.print(loc.lon, 6);
    Serial.print(" TIME:");
    Serial.print(loc.hour);
    Serial.print(':');
    Serial.print(loc.minute);
    Serial.print(':');
    Serial.println(loc.second);
  }
  
  // show stats  
  Serial.print("Total Requests:");
  Serial.print(count);
  if (errors) {
    Serial.print(" Errors:");
    Serial.print(errors);
  }
  Serial.println();
}

