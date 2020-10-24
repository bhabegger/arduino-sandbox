#include <Stepper.h>

// led variables
int led1 = 11;
int led2 = 12;

// joystick variables
int joyPinX = 0; 
int joyPinY = 1;
int valueX = 500;
int valueY = 500;

// step motor variables
int s1 = 4;
int s2 = 5;
int s3 = 6;
int s4 = 7;
int stepPosition = 0;

// main loop
int controlDelay = 300; // Number of loops before taking controls into account
int loopDelay    = 3;  // Delay between loops
int loopCount    = 0;  // Current loop count

void setup() {
  // led setup
  pinMode(led1, OUTPUT);     
  pinMode(led2, OUTPUT);
  digitalWrite(led2, LOW);
  digitalWrite(led1, LOW);

  // motor setup
  pinMode(s1, OUTPUT);     
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(s4, OUTPUT);
  
  // bring motor to positoon 0
  for(int i=0; i < 8; i++) {
    stepPosition = i;
    updateCoils();
    delay(300);
  }
  // serial console
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  if(loopCount % controlDelay == 0) {
    valueX = analogRead(joyPinX);
    delay(1);
    valueY = analogRead(joyPinY);
  }
  if(valueY > 600) {
    forward();   
  } else if(valueY < 400) {
    backward();
  } else {
    halt(); 
  }
  delay(loopDelay);
  loopCount = loopCount + 1 % controlDelay;
}

void forward() {
  digitalWrite(led1, HIGH);
  digitalWrite(led2, LOW);
  
  for(byte count=0; count < 8; count++) {
    delay(3);
    stepPosition = (stepPosition + 3) % 8;
    updateCoils();
  }
}

void backward() {
  digitalWrite(led1, LOW);
  digitalWrite(led2, HIGH);
 
  stepPosition = (stepPosition + 7 - 3) % 8;
  updateCoils();
}

void halt() {
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  digitalWrite(s4, LOW);
}


void updateCoils() {
  // Coil 1
  if(stepPosition == 7 || stepPosition == 0 || stepPosition == 1) {
    digitalWrite(s1, HIGH);
  } else {
    digitalWrite(s1, LOW);
  }
  
  // Coil 2
  if(stepPosition == 1 || stepPosition == 2 || stepPosition == 3) {
    digitalWrite(s2, HIGH);
  } else {
    digitalWrite(s2, LOW);
  }
  
  // Coil 3
  if(stepPosition == 3 || stepPosition == 4 || stepPosition == 5) {
    digitalWrite(s3, HIGH);
  } else {
    digitalWrite(s3, LOW);
  }
  
  // Coil 4
  if(stepPosition == 5 || stepPosition == 6 || stepPosition == 7) {
    digitalWrite(s4, HIGH);
  } else {
    digitalWrite(s4, LOW);
  }
}
