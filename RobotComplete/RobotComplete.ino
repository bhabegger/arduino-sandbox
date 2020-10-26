#include<Servo.h>
#include<LiquidCrystal.h>

Servo servo;

LiquidCrystal lcd(2,1,4,5,6,7);

// Sonar Pins
const int trigPin = 12;
const int echoPin = 13;

// Wheel Pins
const int wheelLeftForward   =  9;
const int wheelLeftBackward  =  8;
const int wheelRightForward  = 10;
const int wheelRightBackward = 11;

// Head servo
const int headServoPin = 3;

boolean movingForward = false;
void setup() {
  servo.attach(headServoPin);
  
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600); // Starts the serial communication
  
  servo.write(90);
  delay(100);
  
  rotate(360);
  movingForward = false;
  
  lcd.begin(16,2);
  lcd.clear();
  lcd.print("Hello");
}

void loop() {  
   int distance = checkSonar();
   // Serial.print("Distance : ");
   // Serial.println(distance);
   if(distance < 15) {
       halt();
       
       servo.write(0);
       delay(500);
       servo.write(180);
       delay(500);
       servo.write(90);

       delay(2000);
       backward();
       halt();
       
       int bestAngle = scanSonar();
       Serial.print("Best angle : ");
       Serial.print(bestAngle);
       Serial.println();
       servo.write(bestAngle);
       delay(2000);
       servo.write(90);
       delay(1000);
       
       if(bestAngle > 60 && bestAngle < 120) {
         Serial.print("U turn");
         rotate(180);
       } else {
         Serial.print("Rotate: ");
         Serial.println(bestAngle);
         servo.write(bestAngle);
         rotate(bestAngle - 90);
         servo.write(90);
       }
       halt();
       Serial.println("");
   }
   forward();
}

void rotate(int deg) {
  delay(100);
  double calcDelay = 3.5 * deg;
  if(deg > 0) {
    digitalWrite(wheelLeftForward, HIGH);
    digitalWrite(wheelLeftBackward, LOW);
    digitalWrite(wheelRightForward, LOW);
    digitalWrite(wheelRightBackward, HIGH);    
  } else {
    digitalWrite(wheelLeftForward, LOW);
    digitalWrite(wheelLeftBackward, HIGH);
    digitalWrite(wheelRightForward, HIGH);
    digitalWrite(wheelRightBackward, LOW);    
    calcDelay = -1 * calcDelay;
  }
  Serial.print("Delay for angle ");
  Serial.print(deg);
  Serial.print(" : ");
  Serial.println(calcDelay);
  delay(calcDelay);
  halt();
}

void halt() {
  delay(100);
  movingForward = false;
  digitalWrite(wheelLeftForward, LOW);
  digitalWrite(wheelLeftBackward, LOW);
  digitalWrite(wheelRightForward, LOW);
  digitalWrite(wheelRightBackward, LOW);
}

void forward() {
  delay(100);
  if(!movingForward) {
    Serial.println("forward");
    movingForward = true;
    digitalWrite(wheelLeftForward, HIGH);
    digitalWrite(wheelLeftBackward, LOW);
    digitalWrite(wheelRightForward, HIGH);
    digitalWrite(wheelRightBackward, LOW);
  }
}

void backward() {
  delay(100);
  digitalWrite(wheelLeftForward, LOW);
  digitalWrite(wheelLeftBackward, HIGH);
  digitalWrite(wheelRightForward, LOW);
  digitalWrite(wheelRightBackward, HIGH);
}

int scanSonar() {
  int angleStep = 5;
  int bestDistance = 0;
  int bestAngle = 90;
  for(int angle=0;angle<=180; angle+=angleStep) {
    servo.write(angle);
    int curDistance = checkSonar();
    Serial.print("Distance at angle ");
    Serial.print(angle);
    Serial.print(" : ");
    Serial.print(curDistance);
    Serial.print(" (best: ");
    Serial.print(bestDistance);
    Serial.println(")");
    if(curDistance > bestDistance) {
      bestAngle = angle;
      bestDistance = curDistance; 
    }
  }
  servo.write(90);
  return bestAngle;
}

int checkSonar() {     
     int distance;
     long duration;

     delay(15);
     
     // Clears the trigPin
     digitalWrite(trigPin, LOW);
     delayMicroseconds(2);
     
     // Sets the trigPin on HIGH state for 10 micro seconds
     digitalWrite(trigPin, HIGH);
     delayMicroseconds(10);
     digitalWrite(trigPin, LOW);
  
     // Reads the echoPin, returns the sound wave travel time in microseconds
     duration = pulseIn(echoPin, HIGH);
     // Calculating the distance
     distance= duration*0.034/2;
     if(distance>700) {
       distance = -1; // Ignore big values
     }
     return distance;
}

