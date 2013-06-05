#include <Wire.h>

#define AT24C256_ADDR 0x50
#define LED_PIN 13    // LED pin for Pro Mini

void setup() {
  Wire.begin();
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
}

unsigned short address = 0x0000;
int count = 0;

void loop() {
  if (count < 32) {
    ++count;
    writeMillis();  

    delay(5);  
  }
  else {    
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
}

void writeMillis() {
  unsigned long data = millis();
  
  Serial.print("address = 0x");
  Serial.println(address, HEX);
  Serial.print("data = ");
  Serial.println(data, HEX);
  
  Wire.beginTransmission(AT24C256_ADDR);
  Wire.write((address & 0xFF00) >> 8);
  Wire.write(address & 0xFF);
  
  Wire.write(data >> 24 & 0xFF);
  Wire.write(data >> 16 & 0xFF);
  Wire.write(data >> 8 & 0xFF);
  Wire.write(data & 0xFF);
  Wire.endTransmission();

  address += sizeof(long);
}

