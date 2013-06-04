#include <Wire.h>

#define AT24C256_ADDR 0x50
#define LED_PIN 13    // LED pin for Pro Mini

void setup() {
  Wire.begin();
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
}

unsigned short address = 0x0000;
boolean initDone = false;
unsigned int count = 0;

void loop() {
  if (!initDone) {
    initDone = true;
    writeLong(address, millis());
  }
  
  if (count < 10) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  
    address += sizeof(long);
    writeLong(address, millis());
    
    count++;
  }
}

void writeLong(unsigned short address, unsigned long data) {
  Serial.print("address = 0x");
  Serial.println(address, HEX);
  Serial.print("data = ");
  Serial.println(data, HEX);
  
  byte msb = (address & 0xFF00) >> 8;
  byte lsb = address & 0x00FF;
  
  byte b1 = (data & 0xFF000000) >> 24;
  byte b2 = (data & 0x00FF0000) >> 16;
  byte b3 = (data & 0x0000FF00) >> 8;
  byte b4 = (data & 0x000000FF) >> 0;

  Serial.print("msb/lsb = ");
  Serial.print(msb, HEX);
  Serial.println(lsb, HEX);
  
  Serial.print("value = 0x");
  Serial.print(b1, HEX);
  Serial.print(b2, HEX);
  Serial.print(b3, HEX);
  Serial.print(b4, HEX);
  Serial.println();
  
  Wire.beginTransmission(AT24C256_ADDR);
  Wire.write(msb);
  Wire.write(lsb);

  Wire.write(b1);
  Wire.write(b2);
  Wire.write(b3);
  Wire.write(b4);
  Wire.endTransmission();
}

