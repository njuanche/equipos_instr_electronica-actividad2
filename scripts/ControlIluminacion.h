#ifndef CONTROL_ILUMINACION_H
#define CONTROL_ILUMINACION_H

#include <Arduino.h>

// Pines
#define LDR_PIN A0
#define LATCH_PIN 13
#define CLOCK_PIN 12
#define DATA_PIN 11

// Inicialización
void setupControlIluminacion();

// Bucle de control
void loopControlIluminacion();

// Valor proporcional inverso (0-255)
uint8_t getPWMIluminacion();

void mostrarDatosIluminacion();

#endif
