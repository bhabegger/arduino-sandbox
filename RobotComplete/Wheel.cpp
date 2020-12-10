#include "Arduino.h"
#include "Wheel.h"

Wheel::Wheel(int forwardPin, int backwardPin, int speedPin, int calibration) {
    this->forwardPin = forwardPin;
    this->backwardPin = backwardPin;
    this->speedPin = speedPin;
    this->calibration = calibration;
}

void Wheel::forward(int speed) {
    digitalWrite(this->forwardPin, HIGH);
    digitalWrite(this->backwardPin, LOW);
    analogWrite(this->speedPin, speed * calibration / 100);
}

void Wheel::backward(int speed) {
    digitalWrite(this->forwardPin, LOW);
    digitalWrite(this->backwardPin, HIGH);
    analogWrite(this->speedPin, speed * calibration / 100);
}

void Wheel::stop() {
    digitalWrite(this->forwardPin, LOW);
    digitalWrite(this->backwardPin, LOW);
    analogWrite(this->speedPin, 0);
}

void Wheel::check() {
    for(int speed= 0; speed<255; speed += 75) {
        forward(speed);
        delay(600);
    }

    for(int speed= 0; speed<255; speed += 75) {
        backward(speed);
        delay(600);
    }
    stop();
}