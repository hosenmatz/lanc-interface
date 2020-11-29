/*
    ESP8266 Timer Example
    Hardware: NodeMCU
    Circuits4you.com
    2018
    LED Blinking using Timer
*/
#include <ESP8266WiFi.h>
#include <Ticker.h>

Ticker blinker;

#define LED 2  //On board LED

//=======================================================================
void ICACHE_RAM_ATTR onTimerISR(){
    digitalWrite(LED,!(digitalRead(LED)));  //Toggle LED Pin
    timer1_write(600000);//12us
}
//=======================================================================
//                               Setup
//=======================================================================
void setup()
{
    Serial.begin(115200);
    Serial.println("");

    pinMode(LED,OUTPUT);

    //Initialize Ticker every 0.5s
    timer1_attachInterrupt(onTimerISR);
    timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
    timer1_write(600000); //120000 us
}
//=======================================================================
//                MAIN LOOP
//=======================================================================
void loop()
{
}
//=======================================================================
