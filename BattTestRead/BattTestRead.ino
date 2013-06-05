#include <Wire.h>

#define AT24C256_ADDR 0x50

void setup() {
  Serial.begin(9600);
  while(!Serial){}
  Wire.begin();
}

boolean done = false;
void loop(){
  if(!done) {
    done = true;
    readAll(); 
  }
}

void readAll() {
  byte buffer[64];

  Wire.beginTransmission(AT24C256_ADDR);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.endTransmission();

  for (unsigned short page = 0; page < 3; page++) {
    Wire.beginTransmission(AT24C256_ADDR);
    Wire.requestFrom(AT24C256_ADDR, sizeof(buffer));
    for(int index = 0; index < sizeof(buffer); index++) {
      buffer[index] = Wire.read();
    }
    Wire.endTransmission();
    
    for(int index = 0; index < sizeof(buffer); index++) {
      if (index % 8 == 0) {
        Serial.println();
        Serial.print(page * sizeof(buffer) + index, HEX);
        Serial.print(": ");
      }
      Serial.print(buffer[index], HEX);
      Serial.print(" ");
    }
  }
}

