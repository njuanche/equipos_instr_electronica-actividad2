#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2
#define DHTTYPE DHT22
#define LED_ROJO 9  
#define LED_AZUL 10 

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

const float tempDeseada = 25.0;
const float zonaMuerta = 3.0;

float tempReal = 0;
float tempInterna = 25.0;

unsigned long lastUpdate = 0;
unsigned long lastProcesoUpdate = 0;

const unsigned long updateInterval = 1000;  // lectura sensor + control cada 1s
const unsigned long retardoProceso = 500;   // actualización tempInterna cada 500ms
const float gananciaProceso = 0.5;          // incremento/decremento 0.5ºC cada 500ms a pwm=255

int pwmCalor = 0;
int pwmFrio = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();

  pinMode(LED_ROJO, OUTPUT);
  pinMode(LED_AZUL, OUTPUT);

  analogWrite(LED_ROJO, 0);
  analogWrite(LED_AZUL, 0);

  lcd.clear();
}

void loop() {
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

    // Algoritmo de control discontinuo de tres posiciones con zona muerta +/- 3º
    float limiteSuperior = tempDeseada + zonaMuerta;
    float limiteInferior = tempDeseada - zonaMuerta;

    if (tempReal > limiteSuperior) {
      // Temperatura fuera por arriba -> enfriar
      float error = tempReal - limiteSuperior; 
      if (error > zonaMuerta) error = zonaMuerta; 
      pwmFrio = (int)((error / zonaMuerta) * 255);
      pwmCalor = 0;
    }
    else if (tempReal < limiteInferior) {
      // Temperatura fuera por abajo -> calentar
      float error = limiteInferior - tempReal;
      if (error > zonaMuerta) error = zonaMuerta;
      pwmCalor = (int)((error / zonaMuerta) * 255);
      pwmFrio = 0;
    }
    else {
      // Zona muerta: no calentar ni enfriar
      pwmCalor = 0;
      pwmFrio = 0;
    }

    if (pwmCalor > 255) pwmCalor = 255;
    if (pwmFrio > 255) pwmFrio = 255;

    analogWrite(LED_ROJO, pwmCalor);
    analogWrite(LED_AZUL, pwmFrio);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("TempR:");
    lcd.print(tempReal,1);
    lcd.print("C");

    lcd.setCursor(0,1);
    lcd.print("Int:");
    lcd.print(tempInterna,1);
    lcd.print("C C:");
    lcd.print(pwmCalor > 0 ? "ON " : "OFF");
    lcd.print("F:");
    lcd.print(pwmFrio > 0 ? "ON" : "OFF");

    Serial.print("Temp ambiente: ");
    Serial.print(tempReal);
    Serial.print(" C | Temp interna: ");
    Serial.print(tempInterna);
    Serial.print(" C | Control Calor PWM: ");
    Serial.print(pwmCalor);
    Serial.print(" | Control Frio PWM: ");
    Serial.println(pwmFrio);
  }

  if (currentMillis - lastProcesoUpdate >= retardoProceso) {
    lastProcesoUpdate = currentMillis;
    tempInterna += (pwmCalor / 255.0) * gananciaProceso;
    tempInterna -= (pwmFrio / 255.0) * gananciaProceso;

    // Influencia del ambiente real sobre temp interna (perturbación externa)
    float influenciaAmbiente = 0.05; // 5% de acercamiento por ciclo
    tempInterna += (tempReal - tempInterna) * influenciaAmbiente;
  }
}
