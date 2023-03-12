#define UNLOCKED_LED   13
#define LOCK_PIN        7
#define LOCK_BUTTON     2
#define OPEN_BUTTON     4

#define LA 440
#define DO 523
#define MI 659

#define BUZZER          9

// Sleep time in minutes
#define SLEEP_TIME      5

int isLocked = 0;
int isOpen = 0;

void setup() {                
  pinMode(UNLOCKED_LED, OUTPUT);
  pinMode(LOCK_PIN, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  
  pinMode(LOCK_BUTTON, INPUT);
  pinMode(LOCK_PIN, INPUT);

  digitalWrite(UNLOCKED_LED, HIGH);
  digitalWrite(UNLOCKED_LED, HIGH);

  Serial.begin(115200);
  Serial.println("Initializing");
}

void loop() {
    int shouldLock = digitalRead(LOCK_BUTTON);
    int shouldOpen = digitalRead(OPEN_BUTTON);

    /*
    Serial.print("State:");
    Serial.print(" shouldLock? "); Serial.print(shouldLock);
    Serial.print(" shouldOpen? "); Serial.print(shouldOpen);
    Serial.println("");
    */
    
    if(shouldLock == HIGH) {
      lockFor(SLEEP_TIME);
    }
    if(shouldOpen == HIGH) {
      open();
    } else {
      close();
    }
    delay(500);
}

void lockFor(int sleepTime) {
    isLocked = 1;
    digitalWrite(UNLOCKED_LED, LOW);
    Serial.print("Sleeping for ");Serial.print(SLEEP_TIME);Serial.println("m");
    delay(sleepTime * 60000);
    isLocked = 0;
    digitalWrite(UNLOCKED_LED, HIGH);
}


void open() {
    pinMode(LOCK_BUTTON, HIGH);
    if(!isOpen) {
      tone(BUZZER, LA);
      delay(500);
      tone(BUZZER, DO);
      delay(500);
      tone(BUZZER, MI);
      delay(500);
    }
    noTone(BUZZER);
    isOpen = 1;
}


void close() {
    pinMode(LOCK_BUTTON, LOW);
    if(isOpen) {
      tone(BUZZER, MI);
      delay(500);
      tone(BUZZER, DO);
      delay(500);
      tone(BUZZER, LA);
      delay(500);
    }
    noTone(BUZZER);
    isOpen = 0;
}
