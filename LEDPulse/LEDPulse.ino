const int R_PIN = 9;

int r = 0;
 
void setup() {
  pinMode(R_PIN, OUTPUT);
  r = 0;  
}
 
void loop() {
  int v = abs(sin(r * (22/7))) * 255;
  analogWrite(R_PIN, v);
  r = (r + 1) % 360;
  delay(120);
}
