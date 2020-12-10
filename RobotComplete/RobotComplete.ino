#include<Servo.h>
#include<Wire.h>
#include "Wheel.h"

#define SPEED_SLOW 155
#define SPEED_INCR 25
#define SPEED_MAX  255

// Command I2C slave connection
const int i2cAddress = 4;

// Sonar Pins
const int trigPin = 12;
const int echoPin = 13;

// Wheel Pins
const int wheelLeftSpeed     =  6;
const int wheelLeftForward   =  5;
const int wheelLeftBackward  =  7;
Wheel leftWheel(wheelLeftForward, wheelLeftBackward, wheelLeftSpeed, 100);

const int wheelRightSpeed    =  3;
const int wheelRightForward  = 2;
const int wheelRightBackward = 4;
Wheel rightWheel(wheelRightForward, wheelRightBackward, wheelRightSpeed, 80);

// Head servo
Servo servo;
const int headServoPin = 11;
const int frontAngle = 90;

// State
/*
 * Direction
 *   - 0 is stopped
 *   - >0 is forward  with  value as speed
 *   - <0 is backward  with negative value as speed
 */
volatile int direction = 0;
/*
 * Angle: Adapt the direction to the left or the right (by adjusting wheel speed)
 *   - 0 straight
 *   - >0 to the right
 *   - <0 to the left
 */
volatile int angle = 0;

boolean autoPilot = true;
char message[128];       // Buffer for formatted log messages
char command[128];
void setup() {
  command[0] = 0;

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
  pinMode(wheelLeftSpeed, OUTPUT);
  pinMode(wheelRightSpeed, OUTPUT);

  // leftWheel.check();
  // rightWheel.check();

  rotate(360);
  leftWheel.stop();
  rightWheel.stop();
  delay(1000);

  testWheels();

  // We are all set
  Serial.println("Ready");
}


void testWheels() {
    leftWheel.stop();
    rightWheel.stop();

    testOneWheel(leftWheel);
    testOneWheel(rightWheel);
}

void testOneWheel(Wheel wheel) {
    delay(1000);
    wheel.forward(127);
    delay(1000);
    wheel.forward(255);
    delay(1000);
    wheel.stop();
    delay(1000);
    wheel.backward(127);
    delay(1000);
    wheel.backward(255);
    delay(1000);
    wheel.stop();
}

void loop() {
    int distance = checkSonar();
    if(0 <distance && distance < 25) {
        sprintf(message, "Sonar check gave short distance (%d). Halting.",distance);
        Serial.println(message);
        halt();

        if(autoPilot) {
            autoAdjustDirection();
        }
    }
    if(command[0] != 0) {
        processCommand(command);
        command[0] = 0;
    }
    if(autoPilot) {
        forward();
    }
}

void autoAdjustDirection() {
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

void handleI2CEvent(int byteCount) {
    char request[byteCount + 1];
    request[byteCount]  = 0;
    for(int i=0; Wire.available(); i++) {
        request[i] = Wire.read();
    }
    strcpy(command, request);
    sprintf(message, "Recieved command: '%s'",request);
    Serial.println(message);
}

void processCommand(char* command) {
  sprintf(message, "Processing command: '%s'",command);
  Serial.println(message);
  if(strcmp(command,"stop") == 0) {
    Serial.println("Stopping");
    autoPilot = false;
    halt();
  } else if(strcmp(command,"left") == 0) {
    Serial.println("Turning left");
    autoPilot = false;
    turn(-15);
  } else if(strcmp(command,"right") == 0) {
    Serial.println("Turning right");
    autoPilot = false;
    turn(15);
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
  halt();

  /*
   * 250 rpm -> 4 rotations/s
   * 90 deg -> 1 rotation
   * 4 rotations = 360 degrees = 1000 ms
   */
  int calcDelay = deg * 4;
  Serial.print("Delay for angle ");
  Serial.print(deg);
  Serial.print(" : ");
  Serial.println(calcDelay);

  if(deg > 0) {
    // Turn right -> move left wheel forward & right wheel backward
    leftWheel.forward(SPEED_SLOW);
    // rightWheel.backward(SPEED_SLOW);
  } else {
    // Turn left -> move right wheel forward & left wheel backward
    rightWheel.forward(SPEED_SLOW);
    // leftWheel.backward(SPEED_SLOW);
    calcDelay = -1 * calcDelay;
  }

  delay(calcDelay);
  Serial.print("done");
  halt();
}

void halt() {
    delay(100);
    direction = 0;
    angle = 0;
    adjustWheels();
}

void forward() {
    delay(100);
    direction = 255;
    angle = 0;
    adjustWheels();
}

void backward() {
    delay(100);
    direction = -155;
    angle = 0;
    adjustWheels();
}

void turn(int deg) {
    delay(100);
    angle = (angle + deg) % 90;
    if(direction == 0) {
        direction = 127;
    }
    adjustWheels();
}

void adjustWheels() {
    if(direction == 0) {
        leftWheel.stop();
        rightWheel.stop();
    } else {
        float speed = (float)direction;
        float leftAdjust = 1.0;
        float rightAdjust = 1.0;

        if(direction > 0) {
            if(angle > 0) {
                rightAdjust = ((float)90 - angle) / 90.0;
            } else if (angle < 0) {
                leftAdjust = ((float)-90 - angle) / -90.0;
            }
        } else if (direction < 0) {
            if(angle > 0) {
                leftAdjust = ((float)90 - angle) / 90.0;
            } else if (angle < 0) {
                rightAdjust = ((float)-90 - angle) / -90.0;
            }
        }
        int speedLeft = round(leftAdjust * speed);
        int speedRight = round(rightAdjust * speed);
        Serial.print("Adjusting wheels:");
        Serial.print("angle=");         Serial.print(angle);
        Serial.print("speed=");         Serial.print(speed);
        Serial.print("leftSpeed=");     Serial.print(speedLeft);
        Serial.print("rightSpeed=");    Serial.print(speedRight);
        Serial.println();

        if(direction == 0) {
            leftWheel.stop();
        } else if(speedLeft > 0) {
            leftWheel.forward(speedLeft);
        } else {
            leftWheel.backward(-1 * speedLeft);
        }
        if(speedRight == 0) {
            rightWheel.stop();
        } else if(speedRight > 0) {
            rightWheel.forward(speedRight);
        } else {
            rightWheel.backward(-1 * speedRight);
        }
    }
}

int calibratedAngle(int angle) {
 return (frontAngle + angle + 360) % 360;
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

