#include <Wire.h>

#define AT24C256_ADDR 0x50

void setup() {
  Serial.begin(9600);
  Wire.begin();
}

boolean done = false;

void loop() {
  if (!done) {
    done = true;
    writeBytes();
    readBytes();
  }
}

void writeBytes() {
  byte bytes[64];
  
  for(int i = 0; i < 64; i += 4) {
    bytes[i + 0] = 0xDE;
    bytes[i + 1] = 0xFC;
    bytes[i + 2] = 0xAB;
    bytes[i + 3] = i / 4;
  }
  
  for(int i = 0; i < 64; i++) {
    Serial.print(bytes[i], HEX);
  }
  
  Wire.beginTransmission(AT24C256_ADDR);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(AT24C256_ADDR);
  for (int i = 0; i < 64; i++) {
    size_t size = Wire.write(bytes[i]);
    if (size != 0) {  // 0 is expected according to the chip spec
      Serial.println(size, HEX);
    }
  }
  int rc = Wire.endTransmission();
  Serial.print("rc = "); Serial.println(rc, HEX);
}

void readBytes() {  
  Wire.beginTransmission(AT24C256_ADDR);
  Wire.write(0x00);
  Wire.write(0x08);
  Wire.endTransmission();

  long result = 0;
  Wire.beginTransmission(AT24C256_ADDR);
  Wire.requestFrom(AT24C256_ADDR, 8);
  for (int i = 0; i < 8; i++) {
    byte b = Wire.read();
//    result |= b;
//    if (i % 4 == 0) {
//      Serial.println(result, HEX);
//    }
//    result = result << 8;

    Serial.print(b, HEX);
  }
  Wire.endTransmission();

}

void scan() {
   for (uint8_t index = 0; index < 0xFF; index++) {
    Wire.beginTransmission(index);
    Wire.write(1);
    uint8_t result = Wire.endTransmission();
    if (result == 0) {
      Serial.print("no error at ");
      Serial.println(index, HEX);
    }
   }
   Serial.println("scan complete");
}

