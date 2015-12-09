

/**
 * Extremely simple program that blinks the Pro Mini's on-board LED.
 */
void setup() {
  // put your setup code here, to run once:
}

void loop() {
  digitalWrite(13, LOW);
  delay(250);
  digitalWrite(13, HIGH);
  delay(250);
}
