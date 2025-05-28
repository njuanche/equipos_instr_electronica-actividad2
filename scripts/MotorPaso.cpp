#include "MotorPaso.h"
#include <AccelStepper.h>

// Instancia de motores
AccelStepper stepperFrio(AccelStepper::DRIVER, STEP_PIN_FRIO, DIR_PIN_FRIO);
AccelStepper stepperCalor(AccelStepper::DRIVER, STEP_PIN_CALOR, DIR_PIN_CALOR);

// Ángulos actuales
static float angleFrio = 0.0;
static float angleCalor = 0.0;

void setupMotor() {
  // Configuración motores
  stepperFrio.setMaxSpeed(2000);
  stepperFrio.setAcceleration(800);
  stepperFrio.setCurrentPosition(0);

  stepperCalor.setMaxSpeed(2000);
  stepperCalor.setAcceleration(800);
  stepperCalor.setCurrentPosition(0);

  // Pines de LED como salida (controlados externamente)
  pinMode(LED_AZUL, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);
  digitalWrite(LED_AZUL, LOW);
  digitalWrite(LED_ROJO, LOW);

  // Inicialización de ángulos
  angleFrio = 0.0;
  angleCalor = 0.0;
}

void moveToAngleFrio(float angle) {
  angle = constrain(angle, 0, 359);
  if (angle != angleFrio) {
    long pasos = (long)((angle / 359.0) * stepsPerRev);
    stepperFrio.moveTo(pasos);
    angleFrio = angle;
  }
}

void moveToAngleCalor(float angle) {
  angle = constrain(angle, 0, 359);
  if (angle != angleCalor) {
    long pasos = (long)((angle / 359.0) * stepsPerRev);
    stepperCalor.moveTo(pasos);
    angleCalor = angle;
  }
}

void setPWMFrio(int pwmFrio) {
  pwmFrio = constrain(pwmFrio, 0, 255);
  float angle = (pwmFrio / 255.0) * 359.0;
  moveToAngleFrio(angle);
}

void setPWMCalor(int pwmCalor) {
  pwmCalor = constrain(pwmCalor, 0, 255);
  float angle = (pwmCalor / 255.0) * 359.0;
  moveToAngleCalor(angle);
}

void actualizarValvulas() {
  if (stepperFrio.distanceToGo() != 0) {
    stepperFrio.run();
  }

  if (stepperCalor.distanceToGo() != 0) {
    stepperCalor.run();
  }
}
