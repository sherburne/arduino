#include<Wire.h>

#define AT24C256_ADDR   0x50

void setup() {
  Serial.begin(9600);
  while(!Serial){;}
  Wire.begin();
}

boolean done = false;
void loop() {
  if(!done) {
    done = true;
    test();
  }
}

void test() {
  Wire.beginTransmission(AT24C256_ADDR);
  Wire.write(0x00);
  Wire.write(0x00);

  for(int i = 0; i < 16; i++) {
    Wire.write(0xAA);
  }
  Wire.endTransmission();
  
  delay(5);
  
  Wire.beginTransmission(AT24C256_ADDR);
  Wire.write(0x00);
  Wire.write(0x10);

  for(int i = 0; i < 16; i++) {
    Wire.write(0xBB);
  }
  Wire.endTransmission();
    
  delay(5);
  
  Wire.beginTransmission(AT24C256_ADDR);
  Wire.write(0x00);
  Wire.write(0x20);

  for(int i = 0; i < 16; i++) {
    Wire.write(0xCC);
  }
  Wire.endTransmission();
  
  delay(5);
  
  Wire.beginTransmission(AT24C256_ADDR);
  Wire.write(0x00);
  Wire.write(0x30);

  for(int i = 0; i < 16; i++) {
    Wire.write(0xDD);
  }
  Wire.endTransmission();

  delay(5);
  
  Wire.beginTransmission(AT24C256_ADDR);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(AT24C256_ADDR);
  Wire.requestFrom(AT24C256_ADDR, 32);
  for(int i = 0; i < 32; i++) {
    byte b = Wire.read();
    if (i % 8 == 0) {Serial.println();}
    if (b >> 4 == 0) {Serial.print(0);}
    Serial.print(b, HEX);
  }
  Wire.endTransmission();
  
//  delay(1);
  
  Wire.beginTransmission(AT24C256_ADDR);
  Wire.write(0x00);
  Wire.write(0x20);
  Wire.endTransmission();

  Wire.beginTransmission(AT24C256_ADDR);
  Wire.requestFrom(AT24C256_ADDR, 32);
  for(int i = 0; i < 32; i++) {
    byte b = Wire.read();
    if (i % 8 == 0) {Serial.println();}
    Serial.print(b, HEX);
  }
  Wire.endTransmission();
}
