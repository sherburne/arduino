#include<Wire.h>
#include<Serial.h>

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Serial.println("I2C test program");
  
  delay(500);
  
  check();
}

void check() {
  for (unsigned int addr = 0; addr <= 0xFF; addr++) {
    uint8_t result = 0;
    Wire.beginTransmission((byte) addr);
    Wire.write(addr);
    uint8_t rc = Wire.endTransmission();
    if (rc == 0) {
      Serial.print(addr, HEX);
      Wire.beginTransmission((byte) addr);
      Wire.requestFrom(addr, (uint8_t) 1);
      result = Wire.read();
      rc = Wire.endTransmission();
      if (rc != 0) {
         Serial.print(" failed on read, rc = ");
         Serial.println(rc, DEC); 
      }
      else {
        Serial.print(" result ");
        Serial.println(result, DEC);
      }
    }
  }
}

void loop() {}

