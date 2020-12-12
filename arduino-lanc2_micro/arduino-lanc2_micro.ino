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


  byte 0 - 18hex
  00011000

  byte 1
  play 34hex - 00110100 -> 00101100
  oder
  stop 30hex - 00110000 -> 00001100

*/

#define cmdPin 2 //7 
#define lancPin 3 //11
#define recButton 6
#define trigger 12
#define LED 13
int cmdRepeatCount;
int bitDuration = 104; //Duration of one LANC bit in microseconds.

bool _play = 0;

void setup() {
  pinMode(lancPin, INPUT_PULLUP); //listens to the LANC line
  pinMode(cmdPin, OUTPUT); //writes to the LANC line
  pinMode(recButton, INPUT_PULLUP); //start-stop recording button
  pinMode(trigger, OUTPUT); //writes to the LANC line
  pinMode(LED, OUTPUT);
  digitalWrite(recButton, HIGH); //turn on an internal pull up resistor
  digitalWrite(cmdPin, LOW); //set LANC line to +5V
  digitalWrite(trigger, HIGH);
  digitalWrite(LED, HIGH);
  delay(5000); //Wait for camera to power up completly
  bitDuration = bitDuration - 8; //Writing to the digital port takes about 8 microseconds so only 96 microseconds are left till the end of each bit
}

void loop() {
  if (!digitalRead(recButton)) {
    if (!_play) {
      digitalWrite(LED, HIGH);
      digitalWrite(trigger, HIGH);
      PLAY(); //send REC command to camera
    }
    else {
      STOP();
      digitalWrite(LED, LOW);
      digitalWrite(trigger, LOW);
    }
    delay(1000); //debounce button
    //digitalWrite(trigger, HIGH);
    //digitalWrite(LED, HIGH);
  }
}

void PLAY() {

  cmdRepeatCount = 0;

  while (cmdRepeatCount < 5) {  //repeat 5 times to make sure the camera accepts the command

    while (pulseIn(lancPin, HIGH) < 5000) {
      //"pulseIn, HIGH" catches any 0V TO +5V TRANSITION and waits until the LANC line goes back to 0V
      //"pulseIn" also returns the pulse duration so we can check if the previous +5V duration was long enough (>5ms) to be the pause before a new 8 byte data packet
      //Loop till pulse duration is >5ms
    }

    //LOW after long pause means the START bit of Byte 0 is here
    delayMicroseconds(bitDuration);  //wait START bit duration

    //######################################  BYTE 0    ######################################
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

    while (digitalRead(lancPin)) {
      //Loop as long as the LANC line is +5V during the stop bit
    }

    //0V after the previous stop bit means the START bit of Byte 1 is here
    delayMicroseconds(bitDuration);  //wait START bit duration

    //######################################  BYTE 1    ######################################
    //Write the 8 bits of Byte 1
    //play 34hex - 00110100<-
    //Note that the command bits have to be put out in reverse order with the least significant, right-most bit (bit 0) first
    digitalWrite(cmdPin, LOW);  //Write bit 0
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, LOW);  //Write bit 1
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, HIGH);  //Write bit 2
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
    delayMicroseconds(10); //make sure to be in the stop bit before byte 1

    while (digitalRead(lancPin)) {
      //Loop as long as the LANC line is +5V during the stop bit
    }

    //0V after the previous stop bit means the START bit of Byte 1 is here
    delayMicroseconds(bitDuration);  //wait START bit duration

    //######################################  ENDE    ######################################
    cmdRepeatCount++;  //increase repeat count by 1

    /*Control bytes 0 and 1 are written, now don’t care what happens in Bytes 2 to 7
      and just wait for the next start bit after a long pause to send the first two command bytes again.*/

  }//While cmdRepeatCount < 5
  _play = 1;
}


void STOP() {

  cmdRepeatCount = 0;

  while (cmdRepeatCount < 5) {  //repeat 5 times to make sure the camera accepts the command


    while (pulseIn(lancPin, HIGH) < 5000) {
      //"pulseIn, HIGH" catches any 0V TO +5V TRANSITION and waits until the LANC line goes back to 0V
      //"pulseIn" also returns the pulse duration so we can check if the previous +5V duration was long enough (>5ms) to be the pause before a new 8 byte data packet
      //Loop till pulse duration is >5ms
    }


    //LOW after long pause means the START bit of Byte 0 is here
    delayMicroseconds(bitDuration);  //wait START bit duration

    //######################################  BYTE 0    ######################################


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

    while (digitalRead(lancPin)) {
      //Loop as long as the LANC line is +5V during the stop bit
    }

    //0V after the previous stop bit means the START bit of Byte 1 is here
    delayMicroseconds(bitDuration);  //wait START bit duration

    //######################################  BYTE 1    ######################################

    //Write the 8 bits of Byte 1
    //stop 30hex - 00110000<-
    //Note that the command bits have to be put out in reverse order with the least significant, right-most bit (bit 0) first
    digitalWrite(cmdPin, LOW);  //Write bit 0
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, LOW);  //Write bit 1
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
    delayMicroseconds(10); //make sure to be in the stop bit before byte 1

    while (digitalRead(lancPin)) {
      //Loop as long as the LANC line is +5V during the stop bit
    }

    //0V after the previous stop bit means the START bit of Byte 1 is here
    delayMicroseconds(bitDuration);  //wait START bit duration

    //######################################  ENDE    ######################################

    cmdRepeatCount++;  //increase repeat count by 1

    /*Control bytes 0 and 1 are written, now don’t care what happens in Bytes 2 to 7
      and just wait for the next start bit after a long pause to send the first two command bytes again.*/

  }//While cmdRepeatCount < 5
  _play = 0;
}
