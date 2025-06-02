#ifndef CONTROL_TEMPERATURA_H
#define CONTROL_TEMPERATURA_H

void setupControlTemperatura();
void loopControlTemperatura();
void mostrarDatosTemperatura();


int getPWMCalor();
int getPWMFrio();
float getTemperaturaAmbiente();
float getTemperaturaInterna();
float getTemperaturaDeseada();

#endif
