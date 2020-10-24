int led1 = 11;
int led2 = 12;

int joyPinX = 0; 
int joyPinY = 1;
int valueX = 0;
int valueY = 0;


// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led1, OUTPUT);     
  pinMode(led2, OUTPUT);     

  digitalWrite(led2, LOW);
  digitalWrite(led1, LOW);
  
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  valueX = analogRead(joyPinX);
  delay(100);
  valueY = analogRead(joyPinY);


  if(valueY > 600) {
    forward();   
  } else if(valueY < 400) {
    backward();
  } else {
    halt(); 
  }
  
  Serial.print("X=");
  Serial.println(valueX);
  Serial.print("Y=");
  Serial.println(valueY);
}

void forward() {
  digitalWrite(led1, HIGH);
  digitalWrite(led2, LOW);
}

void backward() {
  digitalWrite(led1, LOW);
  digitalWrite(led2, HIGH);
}

void halt() {
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
}
