#include "ControlTemperatura.h"
#include "MotorPaso.h"
#include "ControlIluminacion.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);
  setupControlTemperatura();
  setupMotor();
  setupControlIluminacion();
  lcd.init();
  lcd.backlight();
}

void loop() {
  static unsigned long lastSwitch = 0;
  static unsigned long lastDisplayUpdate = 0;
  static bool mostrarIluminacion = true;
  const unsigned long displayUpdateInterval = 500;  // actualizar cada 500 ms

  // Alternar pantalla cada 5 segundos
  if (millis() - lastSwitch > 5000) {
    lcd.clear();
    mostrarIluminacion = !mostrarIluminacion;
    lastSwitch = millis();
    
  }

  // Actualizar hardware
  loopControlTemperatura();
  loopControlIluminacion();

  int pwmFrio = getPWMFrio();
  int pwmCalor = getPWMCalor();
  digitalWrite(LED_AZUL, pwmFrio > 0 ? HIGH : LOW);
  digitalWrite(LED_ROJO, pwmCalor > 0 ? HIGH : LOW);
  setPWMFrio(pwmFrio);
  setPWMCalor(pwmCalor);
  actualizarValvulas();

  // Mostrar datos cada 500 ms, sin limpiar LCD constantemente
  if (millis() - lastDisplayUpdate > displayUpdateInterval) {
    lastDisplayUpdate = millis();
    if (mostrarIluminacion) {
      mostrarDatosIluminacion();
    } else {
      mostrarDatosTemperatura();
    }
  }
}
