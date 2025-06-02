#include <Arduino.h>
#include "ControlTemperatura.h"
#include <DHT.h>  // Sensor DHT22
#include <LiquidCrystal_I2C.h> 

extern LiquidCrystal_I2C lcd;

// Pines y configuración del sensor
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Variables de control de temperatura
const float tempDeseada = 25.0;
float tempReal = 0.0;
float tempInterna = 25.0; // Inicia en equilibrio

const float limiteMinimo = tempDeseada - 3.0;
const float limiteMaximo = tempDeseada + 3.0;

const float gananciaProceso = 0.5; // 0.5 ºC cada 500ms a PWM 255
const float gananciaK = 20.0;       // Ganancia proporcional

int pwmCalor = 0;
int pwmFrio = 0;

// Temporizadores
unsigned long lastSensorRead = 0;
unsigned long lastProcesoUpdate = 0;

const unsigned long sensorInterval = 1000;    // Leer cada 1 segundo
const unsigned long procesoInterval = 500;    // Actualizar temp cada 500 ms

void setupControlTemperatura() {
  dht.begin();
}

void loopControlTemperatura() {
  unsigned long currentMillis = millis();

  // 1. Lectura del sensor
  if (currentMillis - lastSensorRead >= sensorInterval) {
    lastSensorRead = currentMillis;
    tempReal = dht.readTemperature();

    if (isnan(tempReal)) {
      tempReal = tempInterna; // fallback en caso de error
    }

    // 2. Control proporcional sin histéresis
    if (tempReal > limiteMaximo) {
      pwmFrio = (int)( (tempInterna - tempDeseada) * 255.0 * gananciaK / 100.0);
      pwmFrio = constrain(pwmFrio, 0, 255);
      pwmCalor = 0;
    } else if (tempReal < limiteMinimo) {
      pwmCalor = (int)( (tempDeseada - tempInterna) * 255.0 * gananciaK / 100.0);
      pwmCalor = constrain(pwmCalor, 0, 255);
      pwmFrio = 0;
    } else {
      pwmFrio = 0;
      pwmCalor = 0;
    }
  }

  // 3. Simulación de la evolución térmica del sistema
  if (currentMillis - lastProcesoUpdate >= procesoInterval) {
    lastProcesoUpdate = currentMillis;

    // Cambios de temperatura por acción de válvulas
    float cambioCalor = gananciaProceso * (pwmCalor / 255.0);
    float cambioFrio = gananciaProceso * (pwmFrio / 255.0);

    tempInterna += cambioCalor;
    tempInterna -= cambioFrio;

    // Influencia ambiental 
    float diffAmbiente = tempReal - tempInterna;
    tempInterna += diffAmbiente * 0.005;

    // Depuración
    Serial.print("TempReal: "); Serial.print(tempReal);
    Serial.print(" | TempSist: "); Serial.print(tempInterna);
    Serial.print(" | Calor: "); Serial.print(pwmCalor);
    Serial.print(" | Frio: "); Serial.println(pwmFrio);
  }
}

// Getters
int getPWMCalor() {
  return pwmCalor;
}

int getPWMFrio() {
  return pwmFrio;
}

float getTemperaturaAmbiente() {
  return tempReal;
}

float getTemperaturaInterna() {
  return tempInterna;
}

float getTemperaturaDeseada() {
  return tempDeseada;
}

void mostrarDatosTemperatura() {
  float t = getTemperaturaAmbiente();
  float tint = getTemperaturaInterna();
  lcd.setCursor(0, 0);
  lcd.print("TAmb: ");
  lcd.print(t, 1);
  lcd.print(" C    ");

  lcd.setCursor(0, 1);
  lcd.print("TSist: ");
  lcd.print(tint, 1);
  lcd.print(" C    ");
}
