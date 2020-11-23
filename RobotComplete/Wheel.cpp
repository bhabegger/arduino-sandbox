#include "Arduino.h"
#include "Wheel.h"

Wheel::Wheel(int forwardPin, int backwardPin, int speedPin) {
    this->forwardPin = forwardPin;
    this->backwardPin = backwardPin;
    this->speedPin = speedPin;
}

void Wheel::forward(int speed) {
    digitalWrite(this->forwardPin, HIGH);
    digitalWrite(this->backwardPin, LOW);
    analogWrite(this->speedPin, speed);
}

void Wheel::backward(int speed) {
    digitalWrite(this->forwardPin, LOW);
    digitalWrite(this->backwardPin, HIGH);
    analogWrite(this->speedPin, speed);
}

void Wheel::stop() {
    digitalWrite(this->forwardPin, LOW);
    digitalWrite(this->backwardPin, LOW);
    analogWrite(this->speedPin, 0);
}

void Wheel::check() {
    for(int speed= 0; speed<255; speed += 15) {
        forward(speed);
        delay(300);
    }

    for(int speed= 0; speed<255; speed += 15) {
        backward(speed);
        delay(300);
    }
    stop();
}