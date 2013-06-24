#include<Wire.h>
#include<Serial.h>
#include"LiquidCrystal_I2C.h"

// This program works with the mjkdz LCD backpack (version b)

#define LCD_TWI_ADDR 0x20

// If you're not sure how to get your LCD and backpack to work, check out these threads:
// http://forum.arduino.cc/index.php?topic=142255.0
// http://forum.arduino.cc/index.php?topic=157817.0
//
// Data sheet for the red LCD:
// http://www.geeetech.com/wiki/index.php/1602_LCD
// http://www.geeetech.com/Documents/LCD1602%20Green%20JH162A%5b1%5d.pdf
//
// Library for the LCD
// https://bitbucket.org/fmalpartida/new-liquidcrystal/overview

// LCD connects are: VSS, VDD, VO, RS, RW, E, D0, D1, D2, D3, D4, D5, D6, D7, A (or LED+), K (or LED-)
LiquidCrystal_I2C lcd(LCD_TWI_ADDR, 4, 5, 6, 0, 1, 2, 3, 7, NEGATIVE);

void setup() {
  Serial.begin(9600);
  while(!Serial){}
  lcd.begin(16, 2, LCD_5x8DOTS);
}

void loop() {
  printChars();
}

void printChars() {
  char c = 0;
  int iterations = 256 / 6;
  for (int iteration = 0; iteration <= iterations; iteration++) {
    lcd.clear();
    lcd.home();
    for(int charPos = 0; charPos < 6; charPos++) {
      if (charPos == 3) {
        lcd.setCursor(0, 2);
      }
      if (((byte) c) < 0x10) {lcd.print("0");}
      lcd.print((int) c, HEX);
      lcd.print(":");
      lcd.print(c);
      lcd.print(" ");
      ++c;
    }
    delay(3000);
  }
  
}

