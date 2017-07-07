/*
ASCII table

Prints out byte values in all possible formats:
* as raw binary values
* as ASCII-encoded decimal, hex, octal, and binary values

For more on ASCII, see http://www.asciitable.com and http://en.wikipedia.org/wiki/ASCII

The circuit:  No external hardware needed.

created 2006
by Nicholas Zambetti
modified 9 Apr 2012
by Tom Igoe

This example code is in the public domain.

<http://www.zambetti.com>

*/

// constants won't change. Used here to set a pin number :
const int ledPin = 13;
const int ledPinA0 = 54;
const int ledPinA1 = 55;
const int ledPinA2 = 56;


// the number of the LED pin

 // Variables will change :
int ledState = LOW;             // ledState used to set the LED

								// Generally, you should use "unsigned long" for variables that hold time
								// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

										 // constants won't change :
const long interval = 400;           // interval at which to blink (milliseconds)


//#define Serial SerialUSB


void setup() {
	//Initialize serial and wait for port to open:
	Serial.begin(9600);
	//while (!Serial) {
	//	; // wait for serial port to connect. Needed for native USB port only
	//}
	Serial1.begin(9600);
	Serial2.begin(9600);
	Serial3.begin(9600);

	pinMode(ledPin, OUTPUT);
	pinMode(ledPinA0, OUTPUT);
	pinMode(ledPinA1, OUTPUT);
	pinMode(ledPinA2, OUTPUT);

	// prints title with ending line break
	Serial.println("ASCII Table ~ Character Map");
}

// first visible ASCIIcharacter '!' is number 33:
int thisByte = 33;
// you can also write ASCII characters in single quotes.
// for example. '!' is the same as 33, so you could also use this:
//int thisByte = '!';

void loop() {
	// prints value unaltered, i.e. the raw binary version of the
	// byte. The serial monitor interprets all bytes as
	// ASCII, so 33, the first number,  will show up as '!'
	Serial.write(thisByte);

	Serial.print(", dec: ");
	// prints value as string as an ASCII-encoded decimal (base 10).
	// Decimal is the  default format for Serial.print() and Serial.println(),
	// so no modifier is needed:
	Serial.print(thisByte);
	// But you can declare the modifier for decimal if you want to.
	//this also works if you uncomment it:

	// Serial.print(thisByte, DEC);


	Serial.print(", hex: ");
	// prints value as string in hexadecimal (base 16):
	Serial.print(thisByte, HEX);

	Serial.print(", oct: ");
	// prints value as string in octal (base 8);
	Serial.print(thisByte, OCT);

	Serial.print(", bin: ");
	// prints value as string in binary (base 2)
	// also prints ending line break:
	Serial.println(thisByte, BIN);

	// if printed last visible character '~' or 126, stop:
	if (thisByte == 126) {    // you could also use if (thisByte == '~') {
							  // This loop loops forever and does nothing
	/*	while (true) {
			continue;
		}
*/
		thisByte = 33;
	}
	// go on to the next character
	thisByte++;

	// here is where you'd put code that needs to be running all the time.

	// check to see if it's time to blink the LED; that is, if the
	// difference between the current time and last time you blinked
	// the LED is bigger than the interval at which you want to
	// blink the LED.
	unsigned long currentMillis = millis();

	if (currentMillis - previousMillis >= interval) {
		// save the last time you blinked the LED
		previousMillis = currentMillis;

		// if the LED is off turn it on and vice-versa:
		if (ledState == LOW) {
			ledState = HIGH;
		}
		else {
			ledState = LOW;
		}

		// set the LED with the ledState of the variable:
		digitalWrite(ledPin, ledState);
		digitalWrite(ledPinA0, ledState);
		delay(50);
		digitalWrite(ledPinA1, ledState);
		delay(50);
		digitalWrite(ledPinA2, ledState);
		delay(50);

	}
}