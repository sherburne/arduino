#include <Wire.h>

#define AT24C256_ADDR   0x50
#define LED_PIN         13    // LED pin for Pro Mini
#define EEPROM_KBITS    256
#define PAGE_BYTES      64

void setup() {
  Serial.begin(9600);
  while(!Serial){;}
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
    int b = Serial.read();
    if (b == '\n') {break;}
    else {result += (char) b;}
    delay(100);              // This enables more than one char to be captured at a time; none of the other examples have this... ?
  }
//  Serial.print("readInput = ");
//  Serial.println(result);
  return result;
}

void printMenu() {
  Serial.println();
  Serial.println("Atmel EEPROM App");
  Serial.println("1) read page");
  Serial.println("2) read all pages");
  Serial.println("3) erase eeprom");
  Serial.println("4) test eeprom"); 
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

void promptReadPage() {
  Serial.println();
  Serial.println("Pick a page (1 to 512):");
  String buffer = readInput();
  unsigned short page = buffer.toInt();
  if (page > 0) {
    readPage(page - 1);
  }
}

/**
 * Read a 'page' of bytes, which on the AT
 * EEPROM is 64 bytes.
 */
void readPage(unsigned short page) {
  Serial.println();
  Serial.print("Page ");
  Serial.print(page + 1);
  Serial.print(", bytes ");
  page = page * PAGE_BYTES;  
  Serial.print(page, HEX);
  Serial.print(" to ");
  Serial.println(page + PAGE_BYTES - 1, HEX);
  
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
      Serial.print(": ");
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

/**
 * Erase the contents of the EEPROM, writing FF into
 * every byte. Work with 16 bytes at a time to avoid
 * overflowing the buffer in Wire.h.
 */
void clearEeprom() {
  Serial.println("clearing...");
  unsigned int blockBytes = 16;
  unsigned int kbytes = EEPROM_KBITS / 8 * 1024;
  unsigned int blocks = kbytes / blockBytes;
  
  for (int block = 0; block < blocks; block++) {
    if (block % 32 == 0) {Serial.println();}
    unsigned short address = block * blockBytes;
//    Serial.print(address, HEX);
    Wire.beginTransmission(AT24C256_ADDR);
    Wire.write(address >> 8);
    Wire.write(address & 0xFF);
    for (int index = 0; index < blockBytes; index++) {
      Wire.write(0xFF);
    }
    Wire.endTransmission();
    delay(5);                 // The Wire lib or the chip seems to get overwhelmed without a small pause...
    Serial.print(".");
  }
  Serial.println();
  Serial.println("...done");
}

/**
 * Test the EEPROM by writing an incremented
 * long value into every four bytes, then
 * read the values back and make sure they
 * match.
 */
void testEeprom() {
  Serial.println();
  Serial.print("Testing EEPROM, ");
  Serial.print((unsigned long) EEPROM_KBITS / 8 * 1024, DEC);
  Serial.println(" bytes to check");
  
  Serial.println("Writing bytes...");
  unsigned int maxCount = EEPROM_KBITS / 8 * 1024 / sizeof(long);
  for (unsigned long l = 0; l < maxCount; l++) {
    Wire.beginTransmission(AT24C256_ADDR);
    unsigned short address = (unsigned short) l * sizeof(long);
    Wire.write(address >> 8);
    Wire.write(address & 0xFF);
    Wire.write(l >> 24 & 0xFF);
    Wire.write(l >> 16 & 0xFF);
    Wire.write(l >> 8 & 0xFF);
    Wire.write(l & 0xFF);
    Wire.endTransmission();
    delay(5);
  }
  
  Serial.println("Reading bytes...");
  boolean error = false;
  for (unsigned long l = 0; l < maxCount; l++) {
    Wire.beginTransmission(AT24C256_ADDR);
    unsigned short address = (unsigned short) l * sizeof(long);
    Wire.write(address >> 8);
    Wire.write(address & 0xFF);
    Wire.endTransmission();
    
    unsigned long m = 0;
    Wire.beginTransmission(AT24C256_ADDR);
    Wire.requestFrom(AT24C256_ADDR, 4);
    m += Wire.read() << 24;
    m += Wire.read() << 16;
    m += Wire.read() << 8;
    m += Wire.read();
    Wire.endTransmission();
    if (m != l) {
      error = true;
      Serial.print("Error detected at address ");
      Serial.println(l, HEX);
      Serial.print(l, HEX);
      Serial.print(" does not equal ");
      Serial.println(m, HEX);
      break;
    }
    delay(5);
  }
  
  if (!error) {Serial.println("... done, test passed");}
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


