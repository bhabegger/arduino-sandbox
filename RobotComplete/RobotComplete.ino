// Sonar Pins
const int trigPin = 4;
const int echoPin = 5;

// Wheel Pins
const int wheelLeftForward   =  9;
const int wheelLeftBackward  =  8;
const int wheelRightForward  = 10;
const int wheelRightBackward = 11;

// defines variables
long duration;
int distance;

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600); // Starts the serial communication
}


void loop() {
   checkSonar();
   if(distance < 15) {
       halt();
       delay(2000);
       backward();
       delay(500);
       rotateLeft();
   } else {
      forward();
   }
}

void rotateLeft() {
   digitalWrite(wheelLeftForward, LOW);
   digitalWrite(wheelLeftBackward, HIGH);
   digitalWrite(wheelRightForward, HIGH);
   digitalWrite(wheelRightBackward, LOW);
   delay(1000);
   halt();
}

void halt() {
   digitalWrite(wheelLeftForward, LOW);
   digitalWrite(wheelLeftBackward, LOW);
   digitalWrite(wheelRightForward, LOW);
   digitalWrite(wheelRightBackward, LOW);
}

void forward() {
   digitalWrite(wheelLeftForward, HIGH);
   digitalWrite(wheelLeftBackward, LOW);
   digitalWrite(wheelRightForward, HIGH);
   digitalWrite(wheelRightBackward, LOW);
}

void backward() {
   digitalWrite(wheelLeftForward, LOW);
   digitalWrite(wheelLeftBackward, HIGH);
   digitalWrite(wheelRightForward, LOW);
   digitalWrite(wheelRightBackward, HIGH);
}

void checkSonar() {
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
   
   // Prints the distance on the Serial Monitor
   Serial.print("Distance: ");
   Serial.println(distance);
}

