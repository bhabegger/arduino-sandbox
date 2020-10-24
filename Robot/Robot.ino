
// L298N

int m1Forward = 7;
int m1Backward = 6;

void setup() {
  pinMode(m1Forward, INPUT);     
  pinMode(m1Backward, INPUT);
  digitalWrite(m1Forward, LOW);
  digitalWrite(m1Backward, LOW);

  Serial.begin(9600);
}

void loop() {
  Serial.println("Forward start");
  digitalWrite(m1Forward, HIGH);
  delay(3000);
  
  Serial.println("Forward stop");
  digitalWrite(m1Forward, LOW);
  delay(3000);
  
  Serial.println("Backard start");
  digitalWrite(m1Backward, HIGH);
  delay(3000);
  
  Serial.println("Backard stop");
  digitalWrite(m1Backward, LOW);
  delay(3000);
}

