# equipos_instr_electronica-actividad2
# Estación Meteorológica con Control de Temperatura Automático

Este proyecto consiste en una **estación meteorológica simulada** usando Arduino Uno, sensores y actuadores, implementada en Wokwi.

---

## Componentes Utilizados

- Arduino Uno
- Sensor de temperatura y humedad **DHT22**
- Pantalla **LCD I2C 16x2**
- LEDs simulando válvulas:
  - **LED rojo** → válvula de calefacción
  - **LED azul** → válvula de enfriamiento


---

## Objetivos del Proyecto

- Simular condiciones climáticas (temperatura, humedad).
- Implementar un algoritmo de control automático de temperatura.
- Visualizar información en pantalla LCD.
- Ahorrar energía activando el control solo fuera de la zona muerta.

---

## Lógica del Control de Temperatura

El control se basa en un **algoritmo de tres posiciones con zona muerta y control proporcional**:

- **Setpoint fijo:** 25 °C
- **Zona muerta:** ±3 °C
- Dentro de la zona muerta, no se actúa.
- Fuera de la zona muerta:
  - Se enfría si la temperatura real supera los 28 °C.
  - Se calienta si es menor a 22 °C.
  - El control es **proporcional** al error dentro del rango de acción (hasta 255 de PWM).

### Parámetros del modelo

- **Relación proporcional del sistema**: 0.5 °C cada 500 ms con PWM al 100%.
- **Perturbación externa**: la temperatura real (ambiental) influye en un 5% por ciclo en la interna.
- **Retardo del proceso**: 500 ms por paso de simulación del proceso térmico.

```cpp
tempInterna += (pwmCalor / 255.0) * gananciaProceso;
tempInterna -= (pwmFrio / 255.0) * gananciaProceso;
tempInterna += (tempReal - tempInterna) * influenciaAmbiente;
