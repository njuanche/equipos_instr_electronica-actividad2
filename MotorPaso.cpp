#include <Arduino.h>
#include "MotorPaso.h"
#include "ControlTemperatura.h"

// Pines para motor paso a paso
static int _stepPin;
static int _dirPin;

// Estado actual del motor en pasos relativos (negativo = frío, positivo = calor)
static int posicionActual = 0;

// Máximo número de pasos para abrir la válvula en cada dirección
const int pasoMaximo = 50; // Ajusta según tu motor y mecánica

// Inicialización del motor paso a paso
void setupMotor(int stepPin, int dirPin) {
  _stepPin = stepPin;
  _dirPin = dirPin;
  pinMode(_stepPin, OUTPUT);
  pinMode(_dirPin, OUTPUT);
  posicionActual = 0;
}

// Función que mueve el motor una cantidad relativa de pasos
void moverMotor(int pasosRelativos) {
  int nuevaPosicion = constrain(posicionActual + pasosRelativos, -pasoMaximo, pasoMaximo);
  int pasosMover = abs(nuevaPosicion - posicionActual);
  int direccion = (nuevaPosicion > posicionActual) ? HIGH : LOW;

  digitalWrite(_dirPin, direccion);

  for (int i = 0; i < pasosMover; i++) {
    digitalWrite(_stepPin, HIGH);
    delayMicroseconds(800);
    digitalWrite(_stepPin, LOW);
    delayMicroseconds(800);
  }

  posicionActual = nuevaPosicion;
}

// Actualiza la posición de la válvula según las señales PWM de calor y frío
void actualizarValvula() {
  int pwmCalor = getPWMCalor();
  int pwmFrio = getPWMFrio();

  int pasosObjetivo = 0; // posición neutra

  if (pwmCalor > 0 && pwmFrio == 0) {
    float proporcion = pwmCalor / 255.0;
    pasosObjetivo = int(proporcion * pasoMaximo);  // positivo: abre válvula calor
  }
  else if (pwmFrio > 0 && pwmCalor == 0) {
    float proporcion = pwmFrio / 255.0;
    pasosObjetivo = -int(proporcion * pasoMaximo); // negativo: abre válvula frío
  }
  else {
    pasosObjetivo = 0; // zona muerta: válvula en posición neutra
  }

  if (pasosObjetivo != posicionActual) {
    int pasosRelativos = pasosObjetivo - posicionActual;
    moverMotor(pasosRelativos);
    Serial.print("Posición válvula actual: ");
    Serial.println(posicionActual);
  }
}
