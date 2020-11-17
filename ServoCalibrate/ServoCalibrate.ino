#include<Servo.h>
Servo servo;

// Head servo
const int headServoPin = 4;

void setup() {
  servo.attach(headServoPin);  
  servo.write(90);
  delay(5000);
  servo.write(0);
  delay(5000);
  servo.write(180);
  delay(5000);
  servo.write(90);
  delay(5000);
  
  Serial.begin(9600);
}

void loop() {
   for(int angle=0; angle <= 360; angle += 10) {
      Serial.print("Angle : ");
      Serial.println(angle);
      delay(1000);
     // servo.write(angle);
      delay(2000);
   }  
}

