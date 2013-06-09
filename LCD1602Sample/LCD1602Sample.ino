#include<Wire.h>
#include"LiquidCrystal_I2C.h"

#define LCD_TWI_ADDR 0x20

// If you're not sure how to get your LCD and backpack to work, check out these threads:
// http://forum.arduino.cc/index.php?topic=142255.0
// http://forum.arduino.cc/index.php?topic=157817.0
//
// Data sheet for the red LCD:
// http://www.geeetech.com/wiki/index.php/1602_LCD
//
// Library for the LCD
// https://bitbucket.org/fmalpartida/new-liquidcrystal/overview

// LCD connects are: VSS, VDD, VO, RS, RW, E, D0, D1, D2, D3, D4, D5, D6, D7, A (or LED+), K (or LED-)
LiquidCrystal_I2C lcd(LCD_TWI_ADDR, 4, 5, 6, 0, 1, 2, 3, 7, NEGATIVE);

void setup() {
  lcd.begin(16, 2, LCD_5x8DOTS);
}

boolean done = false;
void loop()
{
  if(!done) {
    lcd.backlight();
    lcd.clear();
    lcd.home();
    lcd.print("ABCDEFGHIJKLM");
    lcd.setCursor(0, 2);
    lcd.print("NOPQRSTUVWXYZ");
    done = true;
  }
}

