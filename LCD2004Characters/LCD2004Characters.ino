#include<Wire.h>
#include<Serial.h>
#include"LiquidCrystal_I2C.h"

// This program works with the LCM1602 LCD backpack (from kbell) and 20x4 LCD panel

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
LiquidCrystal_I2C lcd(LCD_TWI_ADDR, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void setup() {
  Wire.begin();
  Serial.begin(9600);
  lcd.begin(20, 4, LCD_5x8DOTS);
  delay(500);
  lcd.backlight();
}

void loop() {
  printChars();
}

void printChars() {
  char c = 0;
  int charsPerLine = 20 / 5;
  int charsPerScreen = charsPerLine * 4;
  int screens = 256 / charsPerScreen;
  int line = 0;
  for (int screen = 0; screen < screens; screen++) {
    lcd.clear();
    lcd.home();
    for(int charPos = 0; charPos < charsPerScreen; charPos++) {
      if (((byte) c) < 0x10) {lcd.print("0");}
      lcd.print((int) c, HEX);
      lcd.print(":");
      lcd.print(c);
      lcd.print(" ");
      ++c;
      if (c % charsPerLine == 0) {
        line = (line % 4);
        lcd.setCursor(0, ++line);
      }
    }
    delay(2 * 1000);
  }
  
}

