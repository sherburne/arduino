#include <Wire.h>

#define AT24C256_ADDR 0x50
#define EEPROM_KBITS    256
#define PAGE_BYTES      64

void setup() {
  Serial.begin(9600);
  while(!Serial){}
  Wire.begin();
  
  readAll();
}

void loop(){}

void readAll() {
  byte buffer[PAGE_BYTES];

  Wire.beginTransmission(AT24C256_ADDR);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.endTransmission();
  
  unsigned short pages = 512;//(EEPROM_KBITS / 8 * 1024) / PAGE_BYTES;

  Serial.print("Reading ");
  Serial.print(pages, DEC);
  Serial.println(" pages");
  
  for (unsigned short page = 0; page < pages; page++) {
    Wire.beginTransmission(AT24C256_ADDR);
    Wire.requestFrom(AT24C256_ADDR, sizeof(buffer));
    for(int index = 0; index < sizeof(buffer); index++) {
      buffer[index] = Wire.read();
    }
    Wire.endTransmission();
    
    for(int index = 0; index < PAGE_BYTES;) {
      unsigned long time = 0;
      time = (time + buffer[index++]) << 8;
      time = (time + buffer[index++]) << 8;
      time = (time + buffer[index++]) << 8;
      time = (time + buffer[index++]);
      
      unsigned long altitude = 0;
      altitude = (altitude + buffer[index++]) << 8;
      altitude = (altitude + buffer[index++]) << 8;
      altitude = (altitude + buffer[index++]) << 8;
      altitude = (altitude + buffer[index++]);
      
      Serial.print("time: ");
      Serial.print(time, DEC);
      Serial.print(" ms, altitude: ");
      Serial.print(altitude, DEC);
      Serial.println(" ft");
    }
  }
}

