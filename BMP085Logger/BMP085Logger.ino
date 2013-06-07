#include <Wire.h>

#define BMP085_ADDR_WRITE 0x77
#define BMP085_ADDR_READ 0x77
#define AT24C256_ADDR 0x50

#define EEPROM_KBITS    256

#define DELTA .03

#define LED_PIN 13    // LED pin for Pro Mini

short ac1 = 0;
short ac2 = 0;
short ac3 = 0;
unsigned short ac4 = 0;
unsigned short ac5 = 0;
unsigned short ac6 = 0;
short b1 = 0;
short b2 = 0;
long b5 = 0;
short mb = 0;
short mc = 0;
short md = 0;
short oss = 0;

long lastAltitude = 0;

unsigned short memoryOffset = 0x0000;
int count = 0;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  
  pinMode(LED_PIN, OUTPUT);
  
  readCalibrationData();
}

void loop() {
  blink();
  readAltitude();
}

void readAltitude() {
  unsigned short altitude = 0;

  long t = calculateTemperature();
  long p = calculatePressure();
  
  // Convert
  t = t * 9.0 / 50.0 + 32.0;
  
  Serial.print("temp = ");
  Serial.print(t, DEC);
  Serial.println(" degF");
    
  Serial.print("pressure = ");
  Serial.print(p * 0.01, DEC);
  Serial.println(" mb");
  
  altitude = calculateAbsoluteAltitude(p);
  Serial.print("altitude = ");
  Serial.print(altitude, DEC);
  Serial.println(" m");
  
  altitude *= 3.2808; // Convert to feet
  
  Serial.print("altitude = ");
  Serial.print(altitude, DEC);
  Serial.println(" ft");

  if (altitude > lastAltitude * (1 + DELTA) || altitude < lastAltitude * (1 - DELTA)) {
    lastAltitude = altitude;
    log(millis(), t, altitude);
  }
  
}

void blink() {
  digitalWrite(LED_PIN, HIGH);
  delay(250);
  digitalWrite(LED_PIN, LOW);
  delay(250);
}

void log(unsigned long time, unsigned short temp, unsigned short altitude) {
  writeLong(memoryOffset, time);
  memoryOffset += sizeof(long);
  writeShort(memoryOffset, temp);
  memoryOffset += sizeof(short);
  writeShort(memoryOffset, altitude);
  memoryOffset += sizeof(short);
  
  if (memoryOffset > (EEPROM_KBITS / 8 * 1024)) {
    memoryOffset = 0;
  }
}

void writeLong(unsigned short address, unsigned long data) {
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
}

void writeShort(unsigned short address, unsigned short data) {
  Serial.print("address = 0x");
  Serial.println(address, HEX);
  Serial.print("data = ");
  Serial.println(data, HEX);
  
  Wire.beginTransmission(AT24C256_ADDR);
  Wire.write((address & 0xFF00) >> 8);
  Wire.write(address & 0xFF);
  
  Wire.write(data >> 8 & 0xFF);
  Wire.write(data & 0xFF);
  Wire.endTransmission();
}

uint16_t readShort(int addr) {
    uint16_t result = 0;
    Wire.beginTransmission(BMP085_ADDR_WRITE);
    Wire.write(addr);
    uint8_t rc = Wire.endTransmission();
    if (rc != 0) {
       Serial.print("readShort failed on write, rc = ");
       Serial.println(rc, DEC); 
    }
    else {    
      Wire.beginTransmission(BMP085_ADDR_READ);
      Wire.requestFrom(BMP085_ADDR_READ, 2);
      result = Wire.read();
      result <<= 8;
      result |= Wire.read();
      rc = Wire.endTransmission();
      if (rc != 0) {
         Serial.print("readShort failed on read, rc = ");
         Serial.println(rc, DEC); 
      }
    }
    return result;
}

long readRawTemperature() {
    Wire.beginTransmission(BMP085_ADDR_WRITE);
    Wire.write(0xF4);
    Wire.write(0x2E);
    Wire.endTransmission();
    
    unsigned long time = micros();
    do {
      if (micros() - time > 5000) break;
    }while (true);
    
    Wire.beginTransmission(BMP085_ADDR_WRITE);
    Wire.write(0xF6);
    Wire.endTransmission();
    
    Wire.beginTransmission(BMP085_ADDR_READ);
    Wire.requestFrom(BMP085_ADDR_READ, 2);
    long result = 0;
    result = Wire.read();
    result <<= 8;
    result |= Wire.read();
    Wire.endTransmission();
    
//    Serial.print("rawTemp = ");
//    Serial.println(result, DEC);
  
    return result;
}

long calculateTemperature() {
  long ut = readRawTemperature();
  
  long x1 = (ut - ac6) * ac5 / ((long) 1 << 15);
  long x2 = mc * ((long) 1 << 11) / (x1 + md);
  b5 = x1 + x2;
  long t = (b5 + 8) / (1 << 4);

  return t; 
}

long readRawPressure() {
    Wire.beginTransmission(BMP085_ADDR_WRITE);
    Wire.write(0xF4);
    Wire.write(0x34);
    Wire.endTransmission();
    
    unsigned long time = micros();
    do {
      if (micros() - time > 5000) break;
    }while (true);
    
    Wire.beginTransmission(BMP085_ADDR_WRITE);
    Wire.write(0xF6);
    Wire.endTransmission();
    
    Wire.beginTransmission(BMP085_ADDR_READ);
    Wire.requestFrom(BMP085_ADDR_READ, 1);
    long msb = Wire.read();
    Wire.endTransmission();

    Wire.beginTransmission(BMP085_ADDR_WRITE);
    Wire.write(0xF7);
    Wire.endTransmission();

    Wire.beginTransmission(BMP085_ADDR_READ);
    Wire.requestFrom(BMP085_ADDR_READ, 1);
    long lsb = Wire.read();
    Wire.endTransmission();

    Wire.beginTransmission(BMP085_ADDR_WRITE);
    Wire.write(0xF8);
    Wire.endTransmission();

    Wire.beginTransmission(BMP085_ADDR_READ);
    Wire.requestFrom(BMP085_ADDR_READ, 1);
    long xlsb = Wire.read();
    Wire.endTransmission();

    long result = ((msb << 16) + (lsb << 8) + xlsb) >> (8 - oss);

//    Serial.print("rawPres = ");
//    Serial.println(result, DEC);
  
    return result;
}

long calculatePressure() {
  long up = readRawPressure();
  long p = 0;
  
  long b6 = b5 - 4000;
  long x1 = (b2 * (b6 * b6 / ((long) 1<<12))) / ((long) 1<<11);
  long x2 = ac2 * b6 / ((long) 1<<11);
  long x3 = x1 + x2;
  long b3 = (((ac1 * 4 + x3) << oss) + 2) / 4;
  x1 = ac3 * b6 / ((long) 1<<13);
  x2 = (b1 * (b6 * b6 / ((long) 1<<12))) / ((long) 1<<16);
  x3 = ((x1 + x2) + 2) / ((long) 1<<2);
  unsigned long b4 = ac4 * (unsigned long) (x3 + 32768) / ((long) 1<<15);
  unsigned long b7 = ((unsigned long) up - b3) * (50000 >> oss);
  if (b7 < 0x80000000) {
    p = (b7 * 2) / b4;
  }
  else {
    p = (b7 / b4) * 2;
  }
  x1 = (p / ((long) 1<<8)) * (p / ((long) 1<<8));
  x1 = (x1 * 3038) / ((long) 1<<16);
  x2 = (-7357 * p) / ((long) 1<<16);
  p = p + (x1 + x2 + 3791) / ((long) 1<<4);
  
//  Serial.print("true pressure = ");
//  Serial.print(p * 0.0002961, DEC);
//  Serial.println(" in");

  return p;
}

long calculateAbsoluteAltitude(long p) {
  double a = pow((p * 0.01) / 1013.25, 1 / 5.255);
  a = 44330 * (1 - a);
  
  return a;
}

void readCalibrationData() {       
    int baseEepromRegister = 0xAA;

    ac1 = readShort(baseEepromRegister);
    baseEepromRegister += 2;
    ac2 = readShort(baseEepromRegister);
    baseEepromRegister += 2;
    ac3 = readShort(baseEepromRegister);
    baseEepromRegister += 2;
    ac4 = readShort(baseEepromRegister);
    baseEepromRegister += 2;
    ac5 = readShort(baseEepromRegister);
    baseEepromRegister += 2;
    ac6 = readShort(baseEepromRegister);
    baseEepromRegister += 2;
    b1 = readShort(baseEepromRegister);
    baseEepromRegister += 2;
    b2 = readShort(baseEepromRegister);
    baseEepromRegister += 2;
    mb = readShort(baseEepromRegister);
    baseEepromRegister += 2;
    mc = readShort(baseEepromRegister);
    baseEepromRegister += 2;
    md = readShort(baseEepromRegister);

//    Serial.println("Calibration data:");
//    Serial.print("ac1 = ");Serial.println(ac1, DEC);
//    Serial.print("ac2 = ");Serial.println(ac2, DEC);
//    Serial.print("ac3 = ");Serial.println(ac3, DEC);
//    Serial.print("ac4 = ");Serial.println(ac4, DEC);
//    Serial.print("ac5 = ");Serial.println(ac5, DEC);
//    Serial.print("ac6 = ");Serial.println(ac6, DEC);
//    Serial.print("b1 = ");Serial.println(b1, DEC);
//    Serial.print("b2 = ");Serial.println(b2, DEC);
//    Serial.print("mb = ");Serial.println(mb, DEC);
//    Serial.print("mc = ");Serial.println(mc, DEC);
//    Serial.print("md = ");Serial.println(md, DEC);
}

