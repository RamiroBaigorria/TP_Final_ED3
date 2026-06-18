# 🚗 Auto Autónomo Evasor de Obstáculos
> **Asignatura:** Electrónica Digital III - Universidad Nacional de Córdoba (FCEFyN)  
> **Integrantes:** * Baigorria, Ramiro Javier  
>                  * Cravero, Lorenzo
>                  * Gonzalez, Tobias
> **Profesor:** Blasco

---

## 🚀 Descripción General del Proyecto
    
Este proyecto consiste en el diseño y desarrollo de un **vehículo autónomo capaz de evadir obstáculos en tiempo real**. El sistema utiliza un enfoque de hardware de alto rendimiento basado en la arquitectura ARM Cortex-M3, procesando de manera asíncrona las lecturas de un sensor de distancia analógico para determinar si la trayectoria del coche es segura mediante algoritmos de control de movimiento directo sobre motores de corriente continua. 

Nuestro sistema aprovecha las capacidades avanzadas de microcontrolador. Utiliza temporizadores de hardware para sincronizar el muestreo, automatiza la transferencia de datos mediante accesos directos a memoria (DMA) y controla la potencia del motor por PWM, minimizando drásticamente la carga de procesamiento de la CPU.

---

### 🎯 Alcances del Proyecto
* **El sistema es capaz de:**
  * Realizar un muestreo automatizado y no bloqueante de un sensor de distancia analógico de 0 a 4095 (12 bits) mediante el periférico ***ADC***, sincronizado por hardware con el ***Timer0***, para que cada 10 ms tome muestras.
  * Transferir los resultados de las conversiones directamente a la memoria RAM utilizando el controlador de ***DMA (GPDMA)***, eliminando la sobrecarga por interrupciones repetitivas en el procesador.
  * Generar señales ***PWM*** mediante hardware con el ***Timer1***, para seleccionar de forma independiente la velocidad y el sentido de giro de los motores de tracción a través de un puente H.
  * Ejecutar una lógica de "evasión" en tiempo real: si la medición supera el umbral crítico (`Lectura >= LIMITE_OBSTACULO`), el vehículo frena y gira; de lo contrario (`Lectura < LIMITE_OBSTACULO`), mantiene su marcha hacia adelante.
  * Seleccionar la velocidad con la cual el auto se desplazará, mediante la transmision en vivo de una terminal externa (PC) a la LPC por medio del puerto serie ***UART***.
  * Conectividad inalámbrica (módulos Bluetooth) para control remoto o monitorización a distancia.

---

## 📐 Arquitectura del Sistema: Hardware y Software

### 🔌 Hardware & Interconexión
El núcleo del hardware está constituido por el microcontrolador de desarrollo (LPC1769), interactuando de forma directa con las etapas de adquisición (SENSOR analógico de PROXIMIDAD) y la etapa de potencia (controlador de MOTORES puente H).

## Hardware Utilizado
- 🔸 Sensor: GP2Y0A21YK0F
- 🔸 Motor: Motor DC (x2)
- 🔸 Microcontrolador: LPC1769
- 🔸 Driver del motor: DRV8833
- 🔸 Modulo bluetooth: HC05  

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

* **Diagrama de Flujo:** El Timer0 dispara el ADC; el ADC avisa al DMA cuando finaliza la conversión; el DMA guarda el dato en memoria; el dato se analiza; la CPU lee la variable de forma asíncrona y recalcula el ciclo de trabajo del PWM en el Timer1 para los motores.
  `![Esquemático Completo](Diseño/Diagrama de Flujo.png)`

---

## 📂 Estructura del Repositorio
El repositorio cuenta con una organización limpia y estandarizada. Los archivos temporales generados localmente por la compilación de MCUXpresso e índices Git locales se encuentran excluidos mediante el archivo `.gitignore` para preservar la limpieza del repositorio remoto.

```text
Auto_Esquiva_Objetos/
├── src/                          # Código fuente del proyecto en MCUXpresso IDE
|   ├── Auto_Esquiva_Objetos.c    # Lazo principal y lógica de decisión del auto autónomo
│   ├── inc/                      # Archivos de cabecera (.h) para la configuración modular
│   │   ├── cfg_ADC.h             # Abstracción y registros del convertidor analógico digital
│   │   ├── cfg_DAC.h             # Configuración del convertidor digital analógico
│   │   ├── cfg_DMA.h             # Configuración de canales y ráfagas del controlador GPDMA
│   │   ├── cfg_NVIC.h            # Configuración del controlador de interrupciones vectorizadas
│   │   ├── cfg_TIM0.h            # Base de tiempo de disparo para el muestreo automatizado
│   │   ├── cfg_TIM1.h            # Configuración del temporizador para salidas PWM de motores
│   │   └── cfg_UART.h            # Parámetros del transceptor serie para telemetría
│   └── src/                      # Archivos de implementación (.c)
│       ├── cfg_ADC.c
│       ├── cfg_DAC.c
│       ├── cfg_DMA.c
│       ├── cfg_NVIC.c
│       ├── cfg_TIM0.c
│       ├── cfg_TIM1.c
│       ├── cfg_UART.c 
├── hardware/                     # Archivos de diseño general del proyecto (Diagrama de flujo)
├── docs/                      
└── README.md                     # Este archivo de presentación y documentación del proyecto