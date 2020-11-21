#include<Servo.h>
// #include<LiquidCrystal.h>
#include<Wire.h>

// Command I2C slave connection
const int i2cAddress = 4;

// Sonar Pins
const int trigPin = 12;
const int echoPin = 13;

// Wheel Pins
const int wheelLeftForward   =  9;
const int wheelLeftBackward  =  8;
const int wheelRightForward  = 10;
const int wheelRightBackward = 11;

// Head servo
Servo servo;
const int headServoPin = 4;
const int frontAngle = 90;

// State
boolean movingForward = false;
boolean autoPilot = true;
char message[256];       // Buffer for formatted log messages

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Native USB only
  }
  Serial.println("Initializing");

  // I2C Slave setup
  sprintf(message, "Configuring I2C connection: address=%d", i2cAddress);
  Serial.println(message);
  Wire.begin(i2cAddress);
  Wire.onReceive(handleI2CEvent);

  // Sonar setup
  sprintf(message, "Configuring sonar: TriggerPin=%d EchoPin=%d", trigPin, echoPin);
  Serial.println(message);
  
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);  // Sets the echoPin as an Input
  int distance = checkSonar();
  
  sprintf(message, "Current distance: %d", distance);
  Serial.println(message);

  // Servo setup
  sprintf(message, "Configuring head servo: ServoPin=%d FrontAngle=%d", headServoPin, frontAngle);
  Serial.println(message);
  
  servo.attach(headServoPin);
  acknowledge();

  // Motors setup
  sprintf(message, "Configuring wheel motors: LeftWheelForwardPin=%d LeftWheelBackwardPin=%d RightWheelForwardPin=%d RightWheelBackwardPin=%d",wheelLeftForward, wheelLeftBackward, wheelRightForward, wheelRightBackward);
  Serial.println(message);
  
  rotate(360);
  movingForward = false;
  delay(1000);

  // We are all set
  Serial.println("Ready");
}

void loop() {  
   int distance = checkSonar();
   if(0 <distance && distance < 15) {
       sprintf(message, "Sonar check gave short distance (%d). Halting.",distance);
       Serial.println(message);
       halt();

       if(autoPilot) {
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
         sprintf(message, "Best angle: %d",bestAngle);
         Serial.println(message);
         
         servo.write(bestAngle);
         delay(2000);
         servo.write(frontAngle + bestAngle);
         delay(1000);
         
         if(bestAngle > -30 && bestAngle < 30) {
           Serial.println("Best angle too in front: Making a u-turn");
           rotate(180);
         } else {
           sprintf(message, "Rotating %d",bestAngle);
           Serial.println(message);
           
           servo.write(bestAngle);
           rotate(bestAngle);
           servo.write(frontAngle);
         }
         halt();
       }
   }
   if(autoPilot) {
     forward();
   }
}

void handleI2CEvent(int byteCount) {
   char message[byteCount + 1];
   message[byteCount]  = 0;
   for(int i=0; Wire.available(); i++) {
       message[i] = Wire.read();
   }
   processCommand(message);
}

void processCommand(char* command) {
  sprintf(message, "Recieved command: '%s'",command);
  Serial.println(message);
  if(strcmp(command,"stop") == 0) {
    Serial.println("Stopping");
    autoPilot = false;
    halt();
  } else if(strcmp(command,"left") == 0) {
    Serial.println("Rotating left");
    autoPilot = false;
    halt();
    rotate(frontAngle - 15);
  } else if(strcmp(command,"right") == 0) {
    Serial.println("Rotating right");
    autoPilot = false;
    halt();
    rotate(frontAngle + 15);
  } else if(strcmp(command,"forward") == 0) {
    Serial.println("Moving forward");
    autoPilot = false;
    forward();
  } else if(strcmp(command,"backward") == 0) {
    Serial.println("Moving backward");
    autoPilot = false;
    backward();
  } else if(strcmp(command,"auto") == 0) {
    Serial.println("Enabling autopilot");
    autoPilot = true;
  } else {
    Serial.print(">> ");
    Serial.println(command);
  }
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
