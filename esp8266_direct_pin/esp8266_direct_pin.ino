//#include "nodeMCU_def.h"
#define myPin D1
#define myPinBit (1<<myPin)
#define bittime 103

void setup() {
  pinMode(myPin,OUTPUT);
  digitalWrite(myPin, HIGH);
}

void loop() {
  os_intr_lock();
  WRITE_PERI_REG( 0x60000308, myPinBit );
  delayMicroseconds(bittime);
  WRITE_PERI_REG( 0x60000304, myPinBit );
  delayMicroseconds(bittime);
  WRITE_PERI_REG( 0x60000308, myPinBit );
  delayMicroseconds(bittime);
  WRITE_PERI_REG( 0x60000304, myPinBit );
  delayMicroseconds(bittime);
  WRITE_PERI_REG( 0x60000308, myPinBit );
  delayMicroseconds(bittime);
  WRITE_PERI_REG( 0x60000304, myPinBit );
  delayMicroseconds(bittime);
  WRITE_PERI_REG( 0x60000308, myPinBit );
  delayMicroseconds(bittime);
  WRITE_PERI_REG( 0x60000304, myPinBit );
  delayMicroseconds(bittime);
  WRITE_PERI_REG( 0x60000308, myPinBit );
  WRITE_PERI_REG( 0x60000304, myPinBit );
  os_intr_unlock();
  delayMicroseconds(5500);
}
