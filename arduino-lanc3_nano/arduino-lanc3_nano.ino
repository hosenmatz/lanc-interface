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
#define recButton 4
#define trigger 12
#define LED LED_BUILTIN
int cmdRepeatCount;
int bitDuration = 104; //Duration of one LANC bit in microseconds.

bool _play = 0;
int _plinkertime = 50;

//Start-stop video
boolean _PLAY[] = {LOW, LOW, LOW, HIGH, HIGH, LOW, LOW, LOW,   LOW, LOW, HIGH, HIGH, LOW, HIGH, LOW, LOW}; //18 34
boolean _STOP[] = {LOW, LOW, LOW, HIGH, HIGH, LOW, LOW, LOW,   LOW, LOW, HIGH, HIGH, LOW, LOW, LOW, LOW}; //18 30

void setup() {
  pinMode(lancPin, INPUT_PULLUP); //listens to the LANC line
  pinMode(cmdPin, OUTPUT); //writes to the LANC line
  pinMode(recButton, INPUT_PULLUP); //start-stop recording button
  pinMode(trigger, OUTPUT); //writes to the LANC line
  pinMode(LED, OUTPUT);
  digitalWrite(recButton, HIGH); //turn on an internal pull up resistor
  digitalWrite(cmdPin, LOW); //set LANC line to +5V
  digitalWrite(trigger, HIGH);
  digitalWrite(LED, LOW);
  delay(1000); //Wait for camera to power up completly
  bitDuration = bitDuration - 8; //Writing to the digital port takes about 8 microseconds so only 96 microseconds are left till the end of each bit

  plinker_mal();
}

void loop() {
  if (!digitalRead(recButton)) {
    if (!_play) {
      digitalWrite(trigger, HIGH);

      //"18hex" or “00011000”
      //play 34hex - 00110100
      //PLAY(); //send REC command to camera
      lancCommand(_PLAY);
      _play = 1;
      plinker_mal();
      digitalWrite(LED, HIGH);
    }
    else {
      //"18hex" or “00011000”
      //stop 30hex - 00110000<-
      //STOP();
      lancCommand(_STOP);
      _play = 0;
      digitalWrite(trigger, LOW);
      digitalWrite(LED, LOW);
    }

    delay(1000); //debounce button
    //digitalWrite(trigger, HIGH);
    //digitalWrite(LED, HIGH);
  }
}


void lancCommand(boolean lancBit[]) {

  cmdRepeatCount = 0;

  while (cmdRepeatCount < 5) {  //repeat 5 times to make sure the camera accepts the command

    while (pulseIn(lancPin, HIGH) < 5000) {
      //"pulseIn, HIGH" catches any 0V TO +5V TRANSITION and waits until the LANC line goes back to 0V
      //"pulseIn" also returns the pulse duration so we can check if the previous +5V duration was long enough (>5ms) to be the pause before a new 8 byte data packet
      //Loop till pulse duration is >5ms
    }

    //LOW after long pause means the START bit of Byte 0 is here
    delayMicroseconds(bitDuration);  //wait START bit duration

    //Write the 8 bits of byte 0
    //Note that the command bits have to be put out in reverse order with the least significant, right-most bit (bit 0) first
    for (int i = 7; i > -1; i--) {
      digitalWrite(cmdPin, lancBit[i]);  //Write bits.
      delayMicroseconds(bitDuration);
    }

    //Byte 0 is written now put LANC line back to +5V
    digitalWrite(cmdPin, LOW);
    delayMicroseconds(10); //make sure to be in the stop bit before byte 1

    while (digitalRead(lancPin)) {
      //Loop as long as the LANC line is +5V during the stop bit
    }

    //0V after the previous stop bit means the START bit of Byte 1 is here
    delayMicroseconds(bitDuration);  //wait START bit duration

    //Write the 8 bits of Byte 1
    //Note that the command bits have to be put out in reverse order with the least significant, right-most bit (bit 0) first
    for (int i = 15; i > 7; i--) {
      digitalWrite(cmdPin, lancBit[i]); //Write bits
      delayMicroseconds(bitDuration);
    }

    //Byte 1 is written now put LANC line back to +5V
    digitalWrite(cmdPin, LOW);

    cmdRepeatCount++;  //increase repeat count by 1

    /*Control bytes 0 and 1 are written, now don’t care what happens in Bytes 2 to 7
      and just wait for the next start bit after a long pause to send the first two command bytes again.*/


  }//While cmdRepeatCount < 5
}

void plinker_mal(){
  for (int i=0 ; i < 3 ; i++) {
    digitalWrite(LED, HIGH);
    delay(_plinkertime);
    digitalWrite(LED, LOW);
    delay(_plinkertime);
  }
}
