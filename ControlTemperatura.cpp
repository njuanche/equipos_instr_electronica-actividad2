#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "ControlTemperatura.h"

// Constantes y defines
#define DHTPIN 2
#define DHTTYPE DHT22
#define LED_ROJO 9  
#define LED_AZUL 10 

// Objetos
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variables
const float tempDeseada = 25.0;

float tempReal = 0;
float tempInterna = 25.0;

unsigned long lastUpdate = 0;
unsigned long lastProcesoUpdate = 0;

const unsigned long updateInterval = 1000;  // 1 segundo lectura + control
const unsigned long retardoProceso = 500;   // 500 ms retardo proceso
const float gananciaProceso = 0.5;          // medio grado cada 500 ms OBLIGATORIO
const float gananciaK = 20.0;                 // ganancia proporcional K=20 (1 grado = 10% apertura)

int pwmCalor = 0;
int pwmFrio = 0;

// Limites de zona muerta +/- 3 grados alrededor de tempDeseada
const float limiteMaximo = tempDeseada + 3;
const float limiteMinimo = tempDeseada - 3;

void calcularControlPWM();
void mostrarLCD();
void imprimirSerial();
void actualizarTemperaturaInterna();

void setupControlTemperatura() {
  dht.begin();
  lcd.init();
  lcd.backlight();

  pinMode(LED_ROJO, OUTPUT);
  pinMode(LED_AZUL, OUTPUT);

  analogWrite(LED_ROJO, 0);
  analogWrite(LED_AZUL, 0);

  lcd.clear();
}

void loopControlTemperatura() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastUpdate >= updateInterval) {
    lastUpdate = currentMillis;

    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (isnan(temp) || isnan(hum)) {
      Serial.println("Error leyendo DHT22");
      return;
    }

    tempReal = temp;

    calcularControlPWM();

    analogWrite(LED_ROJO, pwmCalor);
    analogWrite(LED_AZUL, pwmFrio);

    mostrarLCD();
    imprimirSerial();
  }

  if (currentMillis - lastProcesoUpdate >= retardoProceso) {
    lastProcesoUpdate = currentMillis;

    actualizarTemperaturaInterna();
  }
}


void calcularControlPWM() {
  // Algortimo de Control discontinuo de tres posiciones con zona muerta ±3ºC

  if (tempInterna < limiteMinimo) {
    // Temperatura interna menor que limite mínimo: calentar
    pwmCalor = (int)( (tempDeseada - tempInterna) * 255.0 * gananciaK / 100.0);
    pwmCalor = constrain(pwmCalor, 0, 255);
    pwmFrio = 0;
  }
  else if (tempInterna > limiteMaximo) {
    // Temperatura interna mayor que limite máximo: enfriar
    pwmFrio = (int)( (tempInterna - tempDeseada) * 255.0 * gananciaK / 100.0);
    pwmFrio = constrain(pwmFrio, 0, 255);
    pwmCalor = 0;
  }
  else {
    // Zona muerta: ni calentar ni enfriar
    pwmCalor = 0;
    pwmFrio = 0;
  }
}

void mostrarLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TempR:");
  lcd.print(tempReal, 1);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Int:");
  lcd.print(tempInterna, 1);
  lcd.print("C C:");
  lcd.print(pwmCalor > 0 ? "ON " : "OFF");
  lcd.print("F:");
  lcd.print(pwmFrio > 0 ? "ON" : "OFF");
}

void imprimirSerial() {
  Serial.print("Temp ambiente: ");
  Serial.print(tempReal);
  Serial.print(" C | Temp interna: ");
  Serial.print(tempInterna);
  Serial.print(" C | Control Calor PWM: ");
  Serial.print(pwmCalor);
  Serial.print(" | Control Frio PWM: ");
  Serial.println(pwmFrio);
}

void actualizarTemperaturaInterna() {
  // Actualizamos temperatura interna según PWM y gananciaProceso
  tempInterna += (pwmCalor / 255.0) * gananciaProceso;
  tempInterna -= (pwmFrio / 255.0) * gananciaProceso;

  // Influencia ambiente para tender a tempReal
  float influenciaAmbiente = 0.01;
  tempInterna += (tempReal - tempInterna) * influenciaAmbiente;
}


// Funciones getter
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