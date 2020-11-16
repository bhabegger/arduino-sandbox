#include<Servo.h>
#include<LiquidCrystal.h>

Servo servo;

// Sonar Pins
const int trigPin = 12;
const int echoPin = 13;

// Wheel Pins
const int wheelLeftForward   =  9;
const int wheelLeftBackward  =  8;
const int wheelRightForward  = 10;
const int wheelRightBackward = 11;

// Head servo
const int headServoPin = 4;
const int frontAngle = 90;


boolean movingForward = false;
void setup() {
  servo.attach(headServoPin);
  
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);  // Sets the echoPin as an Input
  Serial.begin(9600);       // Starts the serial communication
  
  Serial.write("Head check");
  acknowledge();
  delay(1000);
  
  Serial.write("Motor check");
  rotate(360);
  movingForward = false;
  delay(1000);

  Serial.write("Ready");
}

void loop() {  
   int distance = checkSonar();
   // Serial.print("Distance : ");
   // Serial.println(distance);
   if(distance < 15) {
       halt();
       
       servo.write(frontAngle - 90);
       delay(500);
       servo.write(frontAngle + 90);
       delay(500);
       servo.write(frontAngle);

       delay(2000);
       backward();
       delay(1000);
       halt();
       
       int bestAngle = scanSonar();
       Serial.print("Best angle : ");
       Serial.print(bestAngle);
       Serial.println();
       servo.write(bestAngle);
       delay(2000);
       servo.write(frontAngle + bestAngle);
       delay(1000);
       
       if(bestAngle > -30 && bestAngle < 30) {
         Serial.print("U turn");
         rotate(180);
       } else {
         Serial.print("Rotate: ");
         Serial.println(bestAngle);
         servo.write(bestAngle);
         rotate(bestAngle);
         servo.write(frontAngle);
       }
       halt();
       Serial.println("");
   }
   forward();
}


void acknowledge() {
   servo.write(calibratedAngle(-90));
   delay(500);
      
   servo.write(calibratedAngle(90));
   delay(500);
   
   servo.write(calibratedAngle(0));  
   delay(100);
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

int calibratedAngle(int angle) {
 return (frontAngle + angle + 360) % 360;
}

void backward() {
  delay(100);
  digitalWrite(wheelLeftForward, LOW);
  digitalWrite(wheelLeftBackward, HIGH);
  digitalWrite(wheelRightForward, LOW);
  digitalWrite(wheelRightBackward, HIGH);
}

int scanSonar() {
  int angleStep = 10;
  int bestDistance = 0;
  int bestAngle = 0;
  for(int angle=-90;angle<=90; angle+=angleStep) {
    servo.write(calibratedAngle(angle));
    delay(200);
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
  servo.write(frontAngle);
  return bestAngle;
}

int checkSonar() {     
     int distance;
     long duration;

     delay(100);
     
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
