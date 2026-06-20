# 🚗 Auto Autónomo Evasor de Obstáculos

> **Asignatura:** Electrónica Digital III - Universidad Nacional de Córdoba (FCEFyN)
>
> **Integrantes:**
> - Baigorria, Ramiro Javier
> - Cravero, Lorenzo
> - Gonzalez, Tobias
>
> **Profesor:** Blasco Marcos

---

## 🚀 Descripción General del Proyecto
    
Este proyecto consiste en el diseño y desarrollo de un **vehículo autónomo capaz de evadir obstáculos en tiempo real**. El sistema utiliza un enfoque de hardware de alto rendimiento basado en la arquitectura ARM Cortex-M3, procesando de manera asíncrona las lecturas de un sensor de distancia analógico para determinar si la trayectoria del coche es segura mediante algoritmos de control de movimiento directo sobre motores de corriente continua. 

Nuestro sistema aprovecha las capacidades avanzadas de microcontrolador. Utiliza temporizadores de hardware para sincronizar el muestreo, automatiza la transferencia de datos mediante accesos directos a memoria (DMA) y controla la potencia del motor por PWM, minimizando drásticamente la carga de procesamiento de la CPU.

---

### 🎯 Alcances del Proyecto
* **El sistema es capaz de:**
  * Realizar un muestreo automatizado y no bloqueante de un sensor de distancia analógico de 0 a 4095 (12 bits) mediante el periférico ***ADC***, sincronizado por hardware con el ***Timer0***, para que cada 10 ms tome muestras.
  * Transferir los resultados de las conversiones directamente a la memoria RAM utilizando el controlador de ***DMA (GPDMA)***, eliminando la sobrecarga por interrupciones repetitivas en el procesador.
  * Generar señales ***PWM*** mediante hardware con el ***Timer1***, para sincronizar de forma independiente la velocidad de los motores de tracción a través de transistores NPN. 
  * Ejecutar una lógica de "evasión" en tiempo real: si la medición supera el umbral crítico (`Lectura >= LIMITE_OBSTACULO`), el vehículo frena y gira; de lo contrario (`Lectura < LIMITE_OBSTACULO`), mantiene su marcha hacia adelante.
  * Conectividad inalámbrica (módulos Bluetooth) para control remoto o monitorización a distancia.

 * **El sistema no es capaz de:**
  * Poder sincronizar una comunicación via UART a traves de una terminal
    
---

### ⏩ Posibles Etapas Siguientes:
 * Desarrollar una comunicacion estable via UART/Bluetooth/Wifi para setear distintas especificaciones y funciones al auto (control, de velocidad, freno y giro autónomo cuando se lo requiera, etc.)
 * Mejorar el nivel de deteccion y respuesta ante obstaculos en el camino, pudiendo evadir com más precision y eliminar los limitantes o puntos débiles del proyecto actual
 * Agregar nuevas funcionalidades a la estructura del auto, como encendido y apagado de faroles, control de carga(poder cargarle cosas en una "caja o baúl" y que devuelva el peso total  y compararlo con el soportado, etc) pudienco hacerlo más vistoso y/o util según lo que se implemente

## 📐 Arquitectura del Sistema: Hardware y Software

### 🔌 Arquitectura de Hardware
El núcleo del hardware está constituido por el microcontrolador de desarrollo (LPC1769), interactuando de forma directa con las etapas de adquisición (SENSOR analógico de PROXIMIDAD) y la etapa de potencia (controlador de MOTORES).

## Hardware Utilizado
- 🔸 Sensor: GP2Y0A21YK0F
- 🔸 Motor: Motor DC (x2)
- 🔸 Microcontrolador: LPC1769
- 🔸 Transistores: BC546 (NPN)
- 🔸 Modulo bluetooth: HC05

* **Diagrama de Bloques:**   `![Diagrama de Bloques](Auto_Esquiva_Objetos/Diseño/Diagrama de Bloques.png)`

* **Esquematico del Circuito:**   `![Esquemático Completo](Auto_Esquiva_Objetos/Diseño/Esquematico.png)`

### 💻 Arquitectura de Software (Firmware)
El diseño del software sigue una filosofía **modular y configurar los periférico de forma autónoma** (solicitado por el profe), donde el bucle principal (`while()`) se encarga de realizar el diagnóstico principal utilizado para tomar decisiones respecto al entorno.

## Perifericos Utilizados
- 🔸 ADC
- 🔸 DAC
- 🔸 DMA
- 🔸 TIMER0
- 🔸 TIMER1 
- 🔸 UART
- 🔸 Interrupciones de perifericos como ADC, TIMERs, DMA y UART


| Pin   |  Utilidad       | Funcion | Periferico | Pinsel   |
|:------|:----------------|:--------|:-----------|:---------|
| P0.4  | Prender Motor 1 | GPIO    | GPIO0      | 00       |    
| P0.5  | Prender Motor 2 | GPIO    | GPIO0      | 00       |
| P0.4  | Prender Led 1   | GPIO    | GPIO0      | 00       |    
| P0.5  | Prender Led 2   | GPIO    | GPIO0      | 00       |        
| P0.23 | Conversion ADC  | AD00    | ADC        | 01       |
| P0.26 | Valor promediado| AOUT    | DAC        | 10       |
| P0.15 | Transmision UART| TXD1    | UART1      | 01       |
| P0.16 | Transmision UART| RXD1    | UART1      | 01       |

* **Diagrama de Flujo:** El Timer0 dispara el ADC; el ADC avisa al DMA cuando finaliza la conversión; el DMA guarda el dato en memoria; el dato se analiza; la CPU lee la variable de forma asíncrona y, segun la desicion tomada, habilita o corta la señal PWM del Timer1 a los motores.
  `![Diagrama de Flujo](Auto_Esquiva_Objetos/Diseño/Diagrama de Flujo.png)`

---
## 🔄 Proceso de Integración y Desarrollo (Común)

 * **ETAPA 1:** _Configuraciones_
    Inicializacion de perifericos.
    Habilitacion del ADC y TIMER0 para muestreo.
    DMA para transferencia hacia un arreglo, que calcula promedio de muestras y carga el DAC.
 * **ETAPA 2:** _Métodos_
    Evaluacion del promedio de distancia de las muestras.
    Si el limite es excedido, se cambia al estado de giro.
    Si el limite no se excede, el vehiculo sigue avanzando.
 * **ETAPA 3:** _Módulo UART_
    IDEALMENTE: Carga el valor de promedio transformado en [cm] y lo muestra por el terminal serial.
    REALIDAD: Codigo aparte: Se lee el registro de entrada de datos en el terminal, y devuelve el dato ingresado aumentado en una unidad respecto a su valor asignado en la tabla ASCII.

---

## 📂 Estructura del Repositorio
El repositorio cuenta con una organización limpia y estandarizada. Los archivos temporales generados localmente por la compilación de MCUXpresso e índices Git locales se encuentran excluidos mediante el archivo `.gitignore` para preservar la limpieza del repositorio remoto.

```text
Auto_Esquiva_Objetos/
├── diseños/
|   ├── Diagrama de Flujo.png
|   ├── Esquematico.png
|   └── Diagrama de Bloques.png
├── src/                            
|   ├── Auto_Esquiva_Objetos.c      # Código fuente del proyecto en MCUXpresso IDE
|   └── A_Trabajo_UART.c            # Código correspondiente al modulo UART 
├── docs/                      
└── README.md                       # Este archivo de presentación y documentación del proyecto
