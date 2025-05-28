#include "ControlTemperatura.h"
#include "MotorPaso.h"

void setup() {
  Serial.begin(115200);
  setupControlTemperatura();
  setupMotor();  
}

void loop() {
  loopControlTemperatura();

  // Recupera los valores de PWM desde el controlador
  int pwmFrio = getPWMFrio();
  int pwmCalor = getPWMCalor();

  // Encender LEDs según PWM
  digitalWrite(LED_AZUL, pwmFrio > 0 ? HIGH : LOW);
  digitalWrite(LED_ROJO, pwmCalor > 0 ? HIGH : LOW);

  // Movimiento de válvulas
  setPWMFrio(pwmFrio);
  setPWMCalor(pwmCalor);

  // Ejecutar los movimientos
  actualizarValvulas();
}
