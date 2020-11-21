#include <Wire.h>

#define I2C_SDA 15
#define I2C_SCL 13

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");
  Wire.begin(I2C_SDA,I2C_SCL);
}

byte x = 0;

void loop() {
  Serial.println("Begin Transmission");
  Wire.beginTransmission(4);
  Wire.write("x is ");
  Wire.write(x);
  Serial.print("Sent: ");
  Serial.println(x);  
  Wire.endTransmission(); 
  Serial.println("End Transmission");

  x++;
  delay(300);
}
