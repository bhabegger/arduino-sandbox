#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Native USB only
  }

  Serial.println("Serial echo");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(115200);
}

void loop() {
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
}
