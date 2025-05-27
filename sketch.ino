#include <Arduino.h>
#include "ControlTemperatura.h"
#include "MotorPaso.h"

void setup() {
  Serial.begin(9600);
  setupControlTemperatura();
  setupMotor(3, 4); // stepPin=3, dirPin=4
}

void loop() {
  loopControlTemperatura();
  actualizarValvula();
  delay(100);
}

