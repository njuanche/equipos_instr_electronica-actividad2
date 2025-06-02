#include "ControlIluminacion.h"
#include <LiquidCrystal_I2C.h>

extern LiquidCrystal_I2C lcd;


const int maxLEDs = 8;
const float luxObjetivo = 60000.0; // 80% de iluminación (en lux)

void setupControlIluminacion() {
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LDR_PIN, INPUT);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Iluminacion...");
}

float medirLux() {
  int analogValue = analogRead(LDR_PIN);
  float voltage = analogValue / 1024.0 * 5.0;
  if (voltage >= 5.0) voltage = 4.99; // para evitar division por 0
  float resistance = 2000.0 * voltage / (1.0 - voltage / 5.0);
  float lux = pow(50e3 * pow(10, 0.7) / resistance, 1.0 / 0.7);
  return lux;
}

void loopControlIluminacion() {
  float lux = medirLux();

  float iluminacionAmbientePercent = (lux / luxObjetivo) * 100.0;
  iluminacionAmbientePercent = constrain(iluminacionAmbientePercent, 0, 100);

  // Calcular cuánto falta para llegar al 80%
  float iluminacionFaltante = 80.0 - iluminacionAmbientePercent;
  if (iluminacionFaltante < 0) iluminacionFaltante = 0;

  // Mapeo para LEDs
  int numLEDs = (int)round((iluminacionFaltante / 80.0) * maxLEDs);
  numLEDs = constrain(numLEDs, 0, maxLEDs);

  // Crear el byte para los LEDs
  uint8_t leds = 0;
  for (int i = 0; i < numLEDs; i++) {
    leds |= (1 << i);
  }

  // Enviar datos al registro de desplazamiento
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, leds);
  digitalWrite(LATCH_PIN, HIGH);
}

void mostrarDatosIluminacion() {
  float lux = medirLux();
  float iluminacionAmbientePercent = (lux / luxObjetivo) * 100.0;
  iluminacionAmbientePercent = constrain(iluminacionAmbientePercent, 0, 100);

  lcd.setCursor(0, 0);
  lcd.print("Amb: ");
  lcd.print(iluminacionAmbientePercent, 0);
  lcd.print("%   ");

  // Cuántos LEDs están encendidos
  float iluminacionFaltante = 80.0 - iluminacionAmbientePercent;
  if (iluminacionFaltante < 0) iluminacionFaltante = 0;
  float iluminacionSistemaPercent = (iluminacionFaltante / 80.0) * 100.0;

  lcd.setCursor(0, 1);
  lcd.print("Sis: ");
  lcd.print(iluminacionSistemaPercent, 0);
  lcd.print("%   ");
}
