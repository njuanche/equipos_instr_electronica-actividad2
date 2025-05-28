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

---

### Organización de archivos

- **MotorPaso.h / MotorPaso.cpp**  
  Control y gestión de los motores paso a paso (frío y calor). Incluye funciones para:
  - Inicializar motores y LEDs
  - Calcular ángulos desde PWM
  - Enviar órdenes de movimiento
  - Ejecutar movimientos paso a paso

- **ControlTemperatura.h / ControlTemperatura.cpp**  
  - Lectura del sensor de temperatura DHT22
  - Algoritmo de control proporcional con zona muerta
  - Cálculo de las señales de control (PWM) para los actuadores
  - Simulación de evolución térmica del sistema (modelo de planta)

- **sketch.ino**  
  - Inicialización general del sistema
  - Llamadas cíclicas al bucle de control y actualización de actuadores
  - Gestión de indicadores visuales (LEDs)

---

### Hardware y conexiones utilizadas con qué propósito

| Componente           | Pines             | Propósito                                               |
|---------------------|-------------------|----------------------------------------------------------|
| **Sensor DHT22**     | Pin 2 (SDA)        | Lectura de temperatura ambiente en ºC                    |
| **Motor paso a paso (frío)** | STEP = 3, DIR = 4 | Controla apertura de válvula de refrigeración           |
| **Motor paso a paso (calor)**| STEP = 5, DIR = 6 | Controla apertura de válvula de calefacción             |
| **LED azul**         | Pin 10            | Indica apertura de válvula de frío                      |
| **LED rojo**         | Pin 9             | Indica apertura de válvula de calor                     |
| **LCD 1602 (I2C)**   | A4 (SDA), A5 (SCL) | Visualización de temperaturas y estado (opcional)       |

- **DHT22**: Sensor digital de temperatura (y opcionalmente humedad). Se comunica por una línea de datos digital y proporciona medidas cada segundo.
- **Motores paso a paso**: Controlados mediante driver A4988 en modo microstepping (1/16). Cada motor representa una válvula termohidráulica virtual.
- **LEDs**: Simulan el estado de las válvulas para verificar visualmente cuándo están activas.
- **LCD** (si se usa): Proporciona retroalimentación en tiempo real del sistema.

---

### Lógica General del algoritmo desarrollado

1. **Lectura de temperatura ambiente**
   - Se usa un sensor DHT22 conectado al pin digital 2.
   - Lectura cada 1000 ms para obtener \( T_m \), la temperatura medida en grados Celsius.
   - Si la lectura falla (por ruido o error), se usa un fallback basado en el estado interno simulado del sistema.

2. **Control de tres posiciones con zona muerta**
   - Se define una temperatura deseada \( T_s \) y una banda de tolerancia \( \Delta \) (por ejemplo ±3 °C).
   - Se calcula la acción de control según:

     \[
     \text{Zona muerta} = \left[T_s - \Delta,\quad T_s + \Delta \right]
     \]

     - Si \( T_m < T_s - \Delta \) → **activar calor**:
       \[
       PWM_{calor} = \text{constrain}\left(k \cdot (T_s - \Delta - T_m), 0, 255 \right)
       \]

     - Si \( T_m > T_s + \Delta \) → **activar frío**:
       \[
       PWM_{frio} = \text{constrain}\left(k \cdot (T_m - T_s - \Delta), 0, 255 \right)
       \]

     - Si \( T_m \in [T_s - \Delta,\; T_s + \Delta] \) → **ninguna acción** (zona muerta):
       \[
       PWM_{frio} = PWM_{calor} = 0
       \]

3. **Conversión de PWM a ángulo de válvula**
   - La señal PWM (0-255) se traduce en un ángulo entre 0° y 359°:
     \[
     \theta = \frac{PWM}{255} \cdot 359^\circ
     \]
   - Cada motor recibe esta orden como posición angular a la que avanzar.

4. **Control de motores paso a paso**
   - Cada ángulo se convierte en pasos:
     \[
     \text{pasos} = \left( \frac{\theta}{359^\circ} \right) \cdot \text{stepsPerRev}
     \]
   - Se usan perfiles de aceleración para evitar movimientos bruscos.

5. **Visualización y señalización**
   - Los LED se encienden si \( PWM > 0 \) para su correspondiente válvula.
   - Se puede mostrar estado en pantalla (opcional con LCD I2C).

6. **Simulación del sistema térmico**
   - La temperatura interna del sistema se modela con ganancia de proceso \( K_p = 0.5 \) °C cada 500 ms.
   - Se simula la influencia del ambiente en la temperatura del sistema con una proporción de \(0.005\) sobre la diferencia de ambas.

---