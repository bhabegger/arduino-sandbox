#include <Wire.h>

void setup() {
  Wire.begin(4);
  Wire.onReceive(receiveEvent);
  Serial.begin(115200);
  Serial.println("Initializing");
}

void loop() {
  delay(50);
}

void receiveEvent(int howMany) {
  while(Wire.available() > 1) {
    char c = Wire.read(); 
    Serial.print(c); 
  }
  int x = Wire.read();
  Serial.println(x);
}
