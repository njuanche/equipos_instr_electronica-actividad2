# Sistema de control y actuación en función del clima
## Descripción del sistema

xxxxxx

---

## Estructura del hardware

| Componente         | Descripción                                       | Conexiones Arduino                      |
|--------------------|-------------------------------------------------|---------------------------------------|
| Arduino Uno        | Microcontrolador principal                        | Fuente de alimentación y pines digitales|
| DHT22              | Sensor de temperatura y humedad                   | Pin digital (por ejemplo, pin 2)       |
| LCD 16x2 con I2C   | Display para visualización de datos                | Pines SDA (A4) y SCL (A5)               |
| Driver A4988       | Controlador de motor paso a paso                   | Pins STEP (3), DIR (4), MS1-MS3 (configurados a 5V), VMOT y GND  |
| Motor paso a paso  | Actuador que regula la válvula                      | Conectado al driver A4988               |
| LED RGB            | Indicador visual de estados del sistema            | Pines PWM con resistencias en R y B, común a GND |

---

## Algoritmo de control para la temperatura

El objetivo principal de este sistema es mantener la temperatura ambiente cercana a un setpoint definido mediante un control discontinuo de tres posiciones con zona muerta. El sistema regula la apertura de válvulas de frío y calor mediante motores paso a paso, controlados proporcionalmente en base a la diferencia térmica. Se evita el sobrecontrol mediante una zona muerta configurable.


### Descripción del sistema

Este proyecto implementa un sistema de control térmico automático que mantiene la temperatura ambiente cerca de un valor de consigna. Se utilizan motores paso a paso para abrir o cerrar válvulas de frío y calor, un sensor DHT22 para lectura de temperatura y un algoritmo de **control discontinuo de tres posiciones con zona muerta**.

---

### Estructura del hardware

| Componente             | Descripción                                       | Conexiones Arduino                      |
|------------------------|---------------------------------------------------|-----------------------------------------|
| Arduino Uno            | Microcontrolador principal                        | Fuente de alimentación y pines digitales |
| DHT22                  | Sensor de temperatura y humedad                   | Pin digital 2                            |
| LCD 16x2 con I2C       | Display para visualización de datos               | Pines SDA (A4) y SCL (A5)               |
| Driver A4988 (x2)      | Controlador de motores paso a paso                | Pines STEP, DIR, VMOT, GND              |
| Motores paso a paso    | Actuadores para válvulas de frío y calor          | Conectados a los drivers                |
| LED RGB Azul               | Indica activación de la válvula de frío           | Pin 10                                  |
| LED RGB Rojo               | Indica activación de la válvula de calor          | Pin 9                                   |

---

### Organización de archivos

- **MotorPaso.h / MotorPaso.cpp**  
  Control de los motores de frío y calor:
  - Inicialización de pines y aceleración
  - Conversión de PWM a ángulo
  - Movimiento del motor con aceleración
  - Control de LEDs según actividad

- **ControlTemperatura.h / ControlTemperatura.cpp**  
  Lógica de control:
  - Lectura del sensor DHT22
  - Control proporcional por temperatura
  - Aplicación de zona muerta
  - Simulación térmica interna

- **sketch.ino**  
  - Inicialización general
  - Ciclo principal de ejecución
  - Llamadas a funciones de control y actuación

---

### Hardware y conexiones utilizadas con qué propósito

| Componente                | Pines Arduino       | Propósito                                         |
|--------------------------|---------------------|--------------------------------------------------|
| Sensor DHT22             | D2                  | Lectura digital de temperatura ambiente          |
| Motor paso a paso (frío) | STEP = 3, DIR = 4   | Control de válvula de refrigeración              |
| Motor paso a paso (calor)| STEP = 5, DIR = 6   | Control de válvula de calefacción                |
| LED RGB Azul                 | D10                 | Indica actividad de la válvula de frío           |
| LED RGB Rojo                 | D9                  | Indica actividad de la válvula de calor          |
| LCD 1602 (I2C)           | A4 (SDA), A5 (SCL)  | Visualización de datos                 |

---

### Lógica general del algoritmo desarrollado

#### 1. Lectura del sensor

- Cada 1 s se lee la temperatura ambiente con el sensor DHT22.
- Si la lectura es válida, se actualiza la temperatura medida `T_m`.
- Si falla, se emplea la temperatura interna simulada del sistema.
---

#### 2. Control con zona muerta

Se define:
- $` T_s `$ emperatura de consigna
- $`\Delta `$: semiancho de zona muerta
- $'T_m \`$: temperatura medida

El comportamiento del control se describe así:

$$
\text{Zona muerta} = [T_s - \Delta,\quad T_s + \Delta]
$$

- Si $`( T_m < T_s - \Delta ) `$ → Activar calor:
  
  $$
  PWM_{calor} = \text{constrain}(k \cdot (T_s - \Delta - T_m),\ 0,\ 255)
  $$

- Si $`( T_m > T_s + \Delta ) `$ → Activar frío:
  
  $$
  PWM_{frio} = \text{constrain}(k \cdot (T_m - T_s - \Delta),\ 0,\ 255)
  $$

- Si $`( T_m \in [T_s - \Delta,\ T_s + \Delta] )`$ → No hacer nada:
  
  $$
  PWM_{calor} = PWM_{frio} = 0
  $$

Donde $` k  `$ es una constante de ganancia proporcional (ej. 20.0).

---

#### 3. Conversión de PWM a ángulo

La señal PWM (0-255) se transforma en ángulo para la válvula:

$$
\theta = \frac{PWM}{255} \cdot 359^\circ
$$

---

#### 4. Conversión de ángulo a pasos de motor

Con $` \text{stepsPerRev} = 3200 `$ pasos/rev (1.8° con 1/16 microstepping):

$$
\text{pasos} = \left( \frac{\theta}{359} \right) \cdot \text{stepsPerRev}
$$

---

#### 5. Movimiento de motores

- Cada motor se mueve usando el método `stepper.run()` de la librería **AccelStepper**.
- Se aplica aceleración para evitar movimientos bruscos.

---

#### 6. Indicadores LED

- LED RGB Azul (pin 10): se enciende si $` PWM_{frio} > 0 `$
- LED RGB Rojo (pin 9): se enciende si $` PWM_{calor} > 0 `$

---

#### 7. Simulación térmica del sistema

Para validar el sistema sin hardware físico, se simula una planta térmica:

$$
\Delta T_{calor} = 0.5 \cdot \frac{PWM_{calor}}{255}
$$

$$
\Delta T_{frio} = 0.5 \cdot \frac{PWM_{frio}}{255}
$$

Influencia del ambiente:

$$
\Delta T_{ambiente} = 0.005 \cdot (T_{amb} - T_{sistema})
$$

Actualización de la temperatura interna simulada:

$$
T_{sistema}(t+1) = T_{sistema}(t) + \Delta T_{calor} - \Delta T_{frio} + \Delta T_{ambiente}
$$

---

### Lógica de ejecución del sistema

1. Se lee la temperatura ambiente con el DHT22.
2. Si la lectura falla, se mantiene el valor anterior.
3. Se calcula la diferencia con el setpoint y se aplica el control de 3 posiciones.
4. Se generan las señales PWM para frío y calor.
5. Cada PWM se convierte en ángulo y luego en pasos de motor.
6. Se mueven los motores con aceleración.
7. Se actualizan los LEDs según los PWM.
8. Se simula el efecto térmico de las válvulas y del ambiente.

---
