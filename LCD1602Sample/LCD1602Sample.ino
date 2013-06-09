#include<Wire.h>
#include<LCD.h>
#include<LiquidCrystal_I2C.h>

#define LCD_TWI_ADDR 0x20

void setup() {
  Wire.begin();
  LiquidCrystal_I2C.begin(20, 2, LCD.LCD_5x8DOTS);
}

void loop() {
  
  
}
