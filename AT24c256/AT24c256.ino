#include <Wire.h>

#define AT24C256_ADDR   0x50
#define LED_PIN         13    // LED pin for Pro Mini
#define EEPROM_KBITS    25
#define PAGE_BYTES      64

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(LED_PIN, OUTPUT);
}

boolean done = false;

void loop() {
  prompt();
}

String readInput() {
  while(Serial.available() == 0);
  String result = "";
  while(Serial.available() > 0) {
    byte b = Serial.read();
    if (b == '\n') {break;}
    else {result += (char) b;}
  }
  return result;
}

void prompt() {
  printMenu();
  String input = readInput();
  int choice = input.toInt();
  switch(choice) {
    case 1:
      promptReadPage();
    break;
    
    case 2:
      readAll();
    break;
    
    case 3:
      clearEeprom();
    break;
    
    case 4:
      testEeprom();
    break;
    
    default:
      Serial.print("Unknown command ");
      Serial.println(choice, DEC);
  } 
  Serial.flush();
}

void printMenu() {
  Serial.println();
  Serial.println("Atmel EEPROM App");
  Serial.println("1) read page");
  Serial.println("2) read all pages");
  Serial.println("3) erase eeprom");
  Serial.println("4) test eeprom"); 
}

void promptReadPage() {
  Serial.println();
  Serial.println("Pick a page (1 to 512):");
  String buffer = readInput();
  Serial.println(buffer);
  unsigned short page = buffer.toInt();
  if (page > 0) {
    readPage(page - 1);
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

void readPage(unsigned short page) {
  page = page * PAGE_BYTES;  
  Wire.beginTransmission(AT24C256_ADDR);
  Wire.write(page >> 8);
  Wire.write(page & 0x00FF);
  Wire.endTransmission();

  long result = 0;
  Wire.beginTransmission(AT24C256_ADDR);
  Wire.requestFrom(AT24C256_ADDR, 64);
  for (int i = 0; i < 64; i++) {
    if (i % 8 == 0) {
      Serial.println();
      Serial.print((i + page), HEX);
      Serial.print(":");
    }
    byte b = Wire.read();
    Serial.print(b, HEX);
    Serial.print(" ");
  }
  Wire.endTransmission();
  
  Serial.println();
}

void readAll() {
  unsigned short pages = EEPROM_KBITS / 8 * 1024 / PAGE_BYTES;
  for(unsigned short page = 0; page < pages; page++) {
     readPage(page);
  } 
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

void testEeprom() {
  Serial.println("testing..."); 
}

/**
 * Erase the contents of the EEPROM, writing FF into
 * every byte. Work with 16 bytes at a time to avoid
 * overflowing the buffer in Wire.h.
 */
void clearEeprom() {
  Serial.println("clearing...");
  int eepromKbits = 256;
  int blockBytes = 16;
  int kbytes = eepromKbits / 8 * 1024;
  int blocks = kbytes / blockBytes;
  
  for (int block = 0; block < blocks; block++) {
    unsigned short address = block * blockBytes;
    Wire.beginTransmission(AT24C256_ADDR);
    Wire.write(address >> 8);
    Wire.write(address & 0x00FF);
    for (int index = 0; index < blockBytes; index++) {
      Wire.write(0xFF);
    }
    Wire.endTransmission();
    Serial.print(".");
  }
  Serial.println();
  Serial.println("...done");
}

