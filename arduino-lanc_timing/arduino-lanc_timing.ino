/*
  Send a Start/Sop Recording command to the LANC port of a video camera.
  Tested with a Canon XF300 camcorder
  This code requires a simple interface see http://micro.arocholl.com
  Feel free to use this code in any way you want.

  Comprehensive LANC info: www.boehmel.de/lanc.htm


  "LANC" is a registered trademark of SONY.
  CANON calls their LANC compatible port "REMOTE".

  2011, Martin Koch
  http://controlyourcamera.blogspot.com/2011/02/arduino-controlled-video-recording-over.html

  D0  16  GPIO16 blaue LED
  D1  5   GPIO5 Normaler I/O Pin

  digitalWrite(cmdPin, HIGH); 
  dauert 0.9us beim ESP8266

  digitalWrite(cmdPin, LOW);
  dauert 7.8us beim ESP8266
  
*/

#define cmdPin 5
#define ledPin 2

int cmdRepeatCount;
int bitDuration = 104; //Duration of one LANC bit in microseconds.
int highlowswitch = 1;
int lowhighswitch = 3;
int bitDurationhl = bitDuration - highlowswitch;
int bitDurationlh = bitDuration - lowhighswitch;

void setup() {

  pinMode(ledPin, OUTPUT); //listens to the LANC line
  pinMode(cmdPin, OUTPUT); //writes to the LANC line
  digitalWrite(cmdPin, LOW); //set LANC line to +5V
  digitalWrite(ledPin, LOW);
  //delay(5000); //Wait for camera to power up completly
  //bitDuration = bitDuration - 8; //Writing to the digital port takes about 8 microseconds so only 96 microseconds are left till the end of each bit

  delay(5000);
  digitalWrite(ledPin, HIGH);
}


void loop() {
  
  digitalWrite(ledPin, LOW);
  dripple();
  digitalWrite(ledPin, HIGH);
  delay(2000);
  
  /*
  digitalWrite(cmdPin, LOW);
  delayMicroseconds(bitDuration-lowlowhighswitch);
  digitalWrite(cmdPin, HIGH);
  delayMicroseconds(bitDuration-highhighlowswitch);
  */
}

void dripple() {

  cmdRepeatCount = 0;

  while (cmdRepeatCount < 4) {

    digitalWrite(cmdPin, HIGH);  //Write bit 1
    delayMicroseconds(bitDurationhl);
    digitalWrite(cmdPin, LOW);  //Write bit 2
    delayMicroseconds(bitDurationlh);
    digitalWrite(cmdPin, HIGH);  //Write bit 3
    delayMicroseconds(bitDurationhl);
    digitalWrite(cmdPin, LOW);  //Write bit 4
    delayMicroseconds(bitDurationlh);

    digitalWrite(cmdPin, HIGH);  //Write bit 5
    delayMicroseconds(bitDurationhl);
    digitalWrite(cmdPin, LOW);  //Write bit 6
    delayMicroseconds(bitDurationlh);
    digitalWrite(cmdPin, HIGH);  //Write bit 7
    delayMicroseconds(bitDurationhl);
    digitalWrite(cmdPin, LOW);  //Write bit 8
    delayMicroseconds(bitDurationlh);

    delayMicroseconds(bitDurationhl-lowhighswitch);

    cmdRepeatCount++;  //increase repeat count by 1
  }

}

void REC() {

  cmdRepeatCount = 0;

  while (cmdRepeatCount < 5) {  //repeat 5 times to make sure the camera accepts the command

    /*
      while (pulseIn(ledPin, HIGH) < 5000) {
      //"pulseIn, HIGH" catches any 0V TO +5V TRANSITION and waits until the LANC line goes back to 0V
      //"pulseIn" also returns the pulse duration so we can check if the previous +5V duration was long enough (>5ms) to be the pause before a new 8 byte data packet
      //Loop till pulse duration is >5ms
      }
    */

    //LOW after long pause means the START bit of Byte 0 is here
    delayMicroseconds(bitDuration);  //wait START bit duration

    //Write the 8 bits of byte 0
    //"18hex" or “00011000”  tells the camera that there will be a normal command to camera in the next byte
    //Note that the command bits have to be put out in reverse order with the least significant, right-most bit (bit 0) first
    digitalWrite(cmdPin, LOW);  //Write bit 0.
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, LOW);  //Write bit 1
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, LOW);  //Write bit 2
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, HIGH);  //Write bit 3
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, HIGH);  //Write bit 4
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, LOW);  //Write bit 5
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, LOW);  //Write bit 6
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, LOW);  //Write bit 7
    delayMicroseconds(bitDuration);
    //Byte 0 is written now put LANC line back to +5V
    digitalWrite(cmdPin, LOW);
    delayMicroseconds(10); //make sure to be in the stop bit before byte 1

    /*
      while (digitalRead(ledPin)) {
      //Loop as long as the LANC line is +5V during the stop bit
      }
    */

    //0V after the previous stop bit means the START bit of Byte 1 is here
    delayMicroseconds(bitDuration);  //wait START bit duration

    //Write the 8 bits of Byte 1
    //"33hex" or “00110011” sends the  Record Start/Stop command
    //Note that the command bits have to be put out in reverse order with the least significant, right-most bit (bit 0) first
    digitalWrite(cmdPin, HIGH);  //Write bit 0
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, HIGH);  //Write bit 1
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, LOW);  //Write bit 2
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, LOW);  //Write bit 3
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, HIGH);  //Write bit 4
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, HIGH);  //Write bit 5
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, LOW);  //Write bit 6
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, LOW);  //Write bit 7
    delayMicroseconds(bitDuration);
    //Byte 1 is written now put LANC line back to +5V
    digitalWrite(cmdPin, LOW);

    cmdRepeatCount++;  //increase repeat count by 1

    /*Control bytes 0 and 1 are written, now don’t care what happens in Bytes 2 to 7
      and just wait for the next start bit after a long pause to send the first two command bytes again.*/


  }//While cmdRepeatCount < 5
}
