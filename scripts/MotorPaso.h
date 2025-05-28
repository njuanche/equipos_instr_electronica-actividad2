#ifndef MOTORPASO_H
#define MOTORPASO_H

#include <Arduino.h>
#include <AccelStepper.h>

// Pines de motor
const int STEP_PIN_FRIO  = 3;
const int DIR_PIN_FRIO   = 4;
const int STEP_PIN_CALOR = 5;
const int DIR_PIN_CALOR  = 6;

// LED indicador (opcional)
const int LED_AZUL = 10;
const int LED_ROJO = 9;

// Motor: 1.8° con 1/16 microstepping = 3200 pasos por vuelta
const float stepsPerRev = 3200.0;

// Declaración de los motores (definidos en .cpp)
extern AccelStepper stepperFrio;
extern AccelStepper stepperCalor;

// Inicialización
void setupMotor();

// Movimiento a ángulo deseado
void moveToAngleFrio(float angle);
void moveToAngleCalor(float angle);

// PWM → ángulo
void setPWMFrio(int pwmFrio);
void setPWMCalor(int pwmCalor);

// Ejecutar movimientos
void actualizarValvulas();

#endif
