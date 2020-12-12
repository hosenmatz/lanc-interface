/*
    ESP8266 Timer Example
    Hardware: NodeMCU
    Circuits4you.com
    2018
    LED Blinking using Timer
*/

#include <SoftwareSerial.h>
SoftwareSerial Serial(3, 4); // RX, TX


#define LED 2  //On board LED
#define cmdPin 5

// The code uses fast I/O write because its time critical,
// therefore setting pins are done by writing directly to the registers:

//#define cmdPinON (PORTB = B10000000)   // Set digtal pin 7 (PD7)
//#define cmdPinOFF (PORTB = B00000000)  // Reset digtal pin 7 (PD7)

#define cmdPinON (PORTB = B00000100)   // Set digtal pin 2 (PD2)
#define cmdPinOFF (PORTB = B00000000)  // Reset digtal pin 2 (PD2)


#define ledON (PORTB = B00000010)      // Set LED pin 1 (PB1)
#define ledOFF (PORTB = B00000000)     // Reset LED pin 1 (PB1)
#define lancPinREAD (PINB & B00000001) // Reads pin 0 (PB0)


#define lancPin 11

int bitDura = 104;           // Duration of one LANC bit in microseconds, orginal 104
int halfbitDura = 52;        // Half of bitDura
byte strPointer = 0;         // Index when receiving chars
char inString[5];            // A string to hold incoming data
char outString[25];          // A string to hold outgoing data
boolean strComplete = false; // Indicator to see if the string is complete
boolean lancCmd[16];         // Array for the lancCmd in bits
boolean lancMessage[64];     // Array for the complete LANC message in bits


//500=102.5-103us
//1000=204.85-205.6 -> 104 = 509
#define triggertime 104

//=======================================================================

//=======================================================================
//                               Setup
//=======================================================================
void setup()
{
  Serial.begin(57600);        // Start serial port
  Serial.println("Welcome to the Arduino LANC-RS232 interface");
  Serial.println("Send two bytes in hex form etc. 02AF and wait for reply from camera");

  pinMode(LED, OUTPUT);
  pinMode(cmdPin, OUTPUT);
}


//=======================================================================
//                MAIN LOOP
//=======================================================================
void loop() {
  ledON;
  cmdPinON;
  delayMicroseconds(triggertime);
  cmdPinOFF;
  delayMicroseconds(triggertime+2);
  ledOFF;
}
//=======================================================================


void bitarraytohexchar() {
  // The bit array lancMessage contains the whole LANC message (8 bytes) with LSB first
  // This function converts them to hex chars and stores them in outString (16 chars)

  byte temp = 0;

  for ( int i = 0 ; i < 8 ; i++) {  // Byte loop

    for ( int j = 0 ; j < 4 ; j++) { // Bit loop
      temp += (pow2(j) * lancMessage[(j + 4) + i * 8]);
    }
    outString[i * 3] = bytetohexchar(temp);
    temp = 0;

    for ( int j = 0 ; j < 4 ; j++) { // Bit loop
      temp += (pow2(j) * lancMessage[j + i * 8]);
    }
    outString[i * 3 + 1] = bytetohexchar(temp);
    outString[i * 3 + 2] = ' ';
    temp = 0;
  }

  outString[24] = '\n';

}


boolean hexchartobitarray() {
  // The hex code in char (4 chars) is located in inString
  // This function fills the lancCmd array with the bits in the order they should be sent
  // First byte 1 then byte 2 but with LSB first for both bytes

  int byte1, byte2;

  for (int i = 0 ; i < 4 ; i++ ) {
    if (!(isHexadecimalDigit(inString[i]))) {
      return 0;
    }
  }

  byte1 = (hexchartoint(inString[0]) << 4) + hexchartoint(inString[1]);
  byte2 = (hexchartoint(inString[2]) << 4) + hexchartoint(inString[3]);

  for (int i = 0 ; i < 8 ; i++ ) {
    lancCmd[i] = bitRead(byte1, i);     // Reads one bit from a number, x is number, n is position (0 is LSB)
  }
  for (int i = 0 ; i < 8 ; i++ ) {
    lancCmd[i + 8] = bitRead(byte2, i); // Reads one bit from a number, x is number, n is position (0 is LSB)
  }

  return 1;
}



void sendLanc(byte repeats) {
  // This function is time critical and optimized for Arduino Pro Mini
  // It takes ~3.2us for the arduino to set a pin state with the digitalWrite command
  // It takes ~80ns for the arduino to set pin state using the direct register method
  // delayMicroseconds(50) ~ 49us, delayMicroseconds(100) ~ 99us

  int i = 0;
  int bytenr = 0;

  while (pulseIn(lancPin, HIGH) < 5000) {
    // Sync to next LANC message
    // "pulseIn, HIGH" catches any 0V TO +5V TRANSITION and waits until the LANC line goes back to 0V
    // "pulseIn" also returns the pulse duration so we can check if the previous +5V duration was long enough (>5ms) to be the pause before a new 8 byte data packet
  }

  while (repeats) {

    i = 0;
    bytenr = 0;

    ledON;                                       // LANC message LED indicator on


    for ( ; bytenr < 8 ; bytenr++) {
      delayMicroseconds(bitDura - 7);            // LOW after long pause means the START bit of Byte 0 is here, wait START bit duration
      for ( ; i < (8 * (bytenr + 1)) ; i++) {
        if (bytenr < 2) {
          if (lancCmd[i]) {                      // During the first two bytes the adapter controls the line and puts out data
            cmdPinON;
          }
          else {
            cmdPinOFF;
          }
        }
        delayMicroseconds(halfbitDura);
        lancMessage[i] = !lancPinREAD;           // Read data line during middle of bit
        delayMicroseconds(halfbitDura);
      }
      cmdPinOFF;
      if (bytenr == 7) {
        ledOFF;
      }
      delayMicroseconds(halfbitDura);            // Make sure to be in the stop bit before waiting for next byte
      while (lancPinREAD) {                      // Loop as long as the LANC line is +5V during the stop bit or between messages
      }
    }
    repeats--;
  }
}

/*
  SerialEvent occurs whenever a new data comes in the
  hardware serial RX.  This routine is run between each
  time loop() runs, so using delay inside loop can delay
  response.  Multiple bytes of data may be available.
*/

/*
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();                               // Get the new byte
    inString[strPointer++] = inChar;                                 // Add it to the input string
    if ((inChar == '\n') || (inChar == '\r') || (strPointer > 4)) {  // If the incoming character is a newline, carriage return or 4 bytes has been received flag so the main loop can act
      strComplete = true;
      strPointer = 0;
    }
  }
}
*/

int pow2(int x) {
  switch (x) {
    case 0:
      return 1;
      break;
    case 1:
      return 2;
      break;
    case 2:
      return 4;
      break;
    case 3:
      return 8;
      break;
    case 4:
      return 16;
      break;
    case 5:
      return 32;
      break;
    case 6:
      return 64;
      break;
    case 7:
      return 128;
      break;
    default:
      return 0;
      break;
  }
}


char bytetohexchar(byte hexbyte) {
  switch (hexbyte) {
    case 15:
      return 'F';
      break;
    case 14:
      return 'E';
      break;
    case 13:
      return 'D';
      break;
    case 12:
      return 'C';
      break;
    case 11:
      return 'B';
      break;
    case 10:
      return 'A';
      break;
    default:
      return (hexbyte + 48);
      break;
  }
}


int hexchartoint(char hexchar) {
  switch (hexchar) {
    case 'F':
      return 15;
      break;
    case 'f':
      return 15;
      break;
    case 'E':
      return 14;
      break;
    case 'e':
      return 14;
      break;
    case 'D':
      return 13;
      break;
    case 'd':
      return 13;
      break;
    case 'C':
      return 12;
      break;
    case 'c':
      return 12;
      break;
    case 'B':
      return 11;
      break;
    case 'b':
      return 11;
      break;
    case 'A':
      return 10;
      break;
    case 'a':
      return 10;
      break;
    default:
      return (int) (hexchar - 48);
      break;
  }
}
