#ifndef CONTROL_TEMPERATURA_H
#define CONTROL_TEMPERATURA_H

void setupControlTemperatura();
void loopControlTemperatura();

float getTemperaturaAmbiente();
float getTemperaturaInterna();
float getTemperaturaDeseada();

int getPWMCalor();
int getPWMFrio();

#endif
