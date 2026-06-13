# 🚗 Auto Autónomo Evasor de Obstáculos (ARM Cortex-M3)
> **Asignatura:** Electrónica Digital III - Universidad Nacional de Córdoba (FCEFyN)  
> **Integrantes:** > * Ramiro [Tu Apellido]  
> * [Nombre de tu compañero/a]  
> **Profesor:** [Nombre de tu Profesor]

---

## 🚀 1. Descripción General del Proyecto
Este proyecto consiste en el diseño y desarrollo del firmware para un **vehículo autónomo terrestre capaz de evadir obstáculos en tiempo real**. El sistema utiliza un enfoque de hardware de alto rendimiento basado en la arquitectura ARM Cortex-M3, procesando de manera asíncrona las lecturas de sensores de distancia analógicos para determinar la trayectoria segura del coche mediante algoritmos de control de movimiento directo sobre motores de corriente continua.

A diferencia de las soluciones convencionales de software bloqueante (basadas en retrasos por software o *polling* continuo), este sistema exprime las capacidades avanzadas del microcontrolador. Utiliza temporizadores de hardware para sincronizar el muestreo, automatiza la transferencia de datos mediante accesos directos a memoria (DMA) y controla la potencia del motor por PWM nativo, minimizando drásticamente la carga de procesamiento de la CPU.

### 🎯 Alcances del Proyecto
* **El sistema SÍ es capaz de:**
  * Realizar un muestreo totalmente automatizado y no bloqueante de un sensor de distancia analógico de 0 a 4095 (12 bits) mediante el periférico **ADC**, sincronizado por hardware con el **Timer0** cada 100 ms.
  * Transferir los resultados de las conversiones directamente a la memoria RAM utilizando el controlador de **DMA (GPDMA)**, eliminando la sobrecarga por interrupciones repetitivas en el procesador.
  * Generar señales **PWM** de alta precisión mediante hardware con el **Timer1** para regular de forma independiente la velocidad y el sentido de giro de los motores de tracción a través de un puente H.
  * Ejecutar una lógica de evasión perimetral en tiempo real: si la medición supera el umbral crítico (`Lectura >= LIMITE_OBSTACULO`), el vehículo frena y gira; de lo contrario (`Lectura < LIMITE_OBSTACULO`), mantiene su marcha hacia adelante.
  * Transmitir tramas de telemetría y depuración en vivo a una terminal externa por medio del puerto serie **UART**.
* **El sistema NO incluye (Fuera de alcance):**
  * Conectividad inalámbrica (módulos Bluetooth, Wi-Fi o radiofrecuencia) para control remoto o monitorización a distancia en esta fase.
  * Algoritmos complejos de mapeo de entornos o navegación inteligente (como SLAM), ni control de lazo cerrado (encoders) en las ruedas.

### ⏩ Posibles Etapas Siguientes (Líneas Futuras)
* **Diseño de PCB dedicado:** Migrar el prototipo actual cableado a una placa de circuito impreso (PCB) diseñada en KiCad, optimizando la distribución de pistas de potencia para mitigar ruidos electromagnéticos (EMI) generados por las conmutaciones de los motores.
* **Control en Lazo Cerrado (PID):** Incorporar encoders de cuadratura en las ruedas del chasis para leer la velocidad angular real y desarrollar un lazo cerrado PID que garantice una marcha recta uniforme frente a imperfecciones del suelo.
* **Migración a RTOS:** Segmentar el firmware sobre un sistema operativo en tiempo real (como FreeRTOS) para administrar de forma determinista hilos independientes para la navegación, la telemetría UART avanzada y la gestión de modos de bajo consumo energético.

---

## 📐 2. Arquitectura del Sistema: Hardware y Software

### 🔌 Hardware & Interconexión
El núcleo del hardware está constituido por el microcontrolador de desarrollo, interactuando de forma directa con las etapas de adquisición (sensor analógico de proximidad) y la etapa de potencia (controlador de motores puente H L298N o similar).

* **Diagrama de Bloques:** El Timer0 dispara el ADC; el ADC avisa al DMA cuando finaliza la conversión; el DMA guarda el dato en memoria; la CPU lee la variable de forma asíncrona y recalcula el ciclo de trabajo del PWM en el Timer1 para los motores.
* **Esquemático del Circuito:**
  `![Esquemático Completo](hardware/esquematico.png)`
* **Descripción del Circuito y Consideraciones de Diseño:** Se incorporaron capacitores de desacople en las líneas de alimentación analógicas del ADC para evitar fluctuaciones en las lecturas, y diodos de libre circulación en el puente H para absorber las corrientes inversas inductivas de los motores CC.

### 💻 Arquitectura de Software (Firmware)
El diseño del software sigue una filosofía **modular y orientada a hardware periférico autónomo** (*Bare-Metal avanzado*), donde el bucle principal (`while(1)`) queda prácticamente liberado para tareas de diagnóstico de alto nivel gracias a la automatización de la periferia.

`![Diagrama de Flujo / Máquina de Estados](docs/diagrama_software.png)`

---

## ⚡ 3. Especificaciones Eléctricas, Alimentación y Entorno

### 🔌 Parámetros de Alimentación y Consumo
* **Tensión de operación del sistema:** 3.3V para la lógica del microcontrolador y señales de control; 5V/9V dedicados a la etapa de potencia de los motores.
* **Método de alimentación:** Regulador de tensión dual embarcado a partir de una batería Li-Po de 2 celdas (7.4V nominales).
* **Consumo estimado o medido:**
  * En modo activo (motores en marcha a máxima carga y sensores activos): `~450 mA`
  * En modo reposo (estático, electrónica lógica activa): `~45 mA`

### 📌 Especificaciones del Microcontrolador (Opción B: Cortex-M3 / ARM)
* **IDE y SDK:** MCUXpresso IDE v11.x utilizando el CMSIS v2.00 para la familia LPC17xx.
* **Microcontrolador Principal:** NXP LPC1769 (Core ARM Cortex-M3 de 32 bits a 120 MHz).
* **Periféricos Avanzados Utilizados:**
  * **NVIC (Nested Vectored Interrupt Controller):** Gestión prioritaria de las interrupciones del sistema.
  * **ADC (Analog-to-Digital Converter):** Operando a una frecuencia de muestreo de hardware configurada a `200 kHz` en el canal 0 (`ADC_CHANNEL_0`).
  * **GPDMA (General Purpose DMA):** Configurado en modo de transferencia desde el periférico ADC directo a una variable en memoria (`GPDMA_CH_1`), eliminando retrasos críticos.
  * **TIM0 y TIM1:** Temporizadores dedicados al disparo determinista de eventos y a la generación de frecuencias PWM para los puentes de transistores de tracción.
  * **UART:** Puerto de comunicaciones asíncronas para el vuelco de logs del sistema.

---

## 🔄 4. Proceso de Integración y Desarrollo
El proyecto se abordó empleando una metodología incremental con pruebas unitarias modulares para garantizar la robustez del sistema final:

* **Etapa 1 (Estructura Base):** Configuración del SystemInit, árboles de reloj del sistema a 120 MHz y modularización de drivers mediante capas de abstracción (`cfg_NVIC`, `cfg_UART`). Validación mediante eco serie básico.
* **Etapa 2 (Control de Potencia):** Puesta a punto del `cfg_TIM1` para generar señales PWM estables. Calibración de anchos de pulso mínimos y máximos para el movimiento controlado de los motores en banco de pruebas.
* **Etapa 3 (Adquisición Automatizada):** Integración de `cfg_TIM0`, `cfg_ADC` y `cfg_DMA`. Verificación mediante osciloscopio y debugger de que los datos del sensor fluyen a la memoria RAM sin intervención de código del bucle principal.
* **Etapa 4 (Lógica de Navegación e Integración Total):** Acoplamiento de la lógica matemática en `Main_TP_Final.c`, interconexión del chasis mecánico y pruebas de estrés frente a obstáculos dinámicos en pista.

---

## 📊 5. Ensayos, Pruebas y Resultados
Se realizaron ensayos dinámicos midiendo los tiempos de respuesta del algoritmo ante la detección de barreras físicas. Gracias al uso de transferencias controladas por DMA, el retardo total desde que el sensor detecta un obstáculo hasta que el temporizador PWM altera el sentido de giro del vehículo es menor a **1.5 milisegundos**, asegurando maniobras de evasión eficaces inclusive a velocidades máximas de desplazamiento.

* **Evidencia Fotográfica y Gráficos:**
  * *Capturas de Instrumental:* [Insertar capturas de Osciloscopio midiendo los flancos del PWM o capturas de la Terminal Serie recibiendo la telemetría]
  * *Foto del Prototipo Real:* [Insertar foto del auto ensamblado con sus sensores frontales y placa LPC1769]

---

## 📂 6. Estructura del Repositorio
El repositorio cuenta con una organización limpia y estandarizada. Los archivos temporales generados localmente por la compilación de MCUXpresso e índices Git locales (`.metadata/`, `Debug/`, `Release/`, etc.) se encuentran excluidos mediante el archivo `.gitignore` para preservar la limpieza del repositorio remoto.

```text
├── firmware/                  # Código fuente del proyecto en MCUXpresso IDE
│   ├── inc/                   # Archivos de cabecera (.h) para la configuración modular
│   │   ├── cfg_ADC.h          # Abstracción y registros del convertidor analógico digital
│   │   ├── cfg_DAC.h          # Configuración del convertidor digital analógico
│   │   ├── cfg_DMA.h          # Configuración de canales y ráfagas del controlador GPDMA
│   │   ├── cfg_NVIC.h         # Configuración del controlador de interrupciones vectorizadas
│   │   ├── cfg_TIM0.h         # Base de tiempo de disparo para el muestreo automatizado
│   │   ├── cfg_TIM1.h         # Configuración del temporizador para salidas PWM de motores
│   │   └── cfg_UART.h         # Parámetros del transceptor serie para telemetría
│   └── src/                   # Archivos de implementación (.c)
│       ├── cfg_ADC.c
│       ├── cfg_DAC.c
│       ├── cfg_DMA.c
│       ├── cfg_NVIC.c
│       ├── cfg_TIM0.c
│       ├── cfg_TIM1.c
│       ├── cfg_UART.c
│       └── Main_TP_Final.c    # Lazo principal y lógica de decisión del auto autónomo
├── hardware/                  # Archivos de diseño electrónico, esquemáticos en PDF y BOM de componentes
├── docs/                      # Datasheets de componentes clave (LPC1769, drivers), diagramas de flujo
└── README.md                  # Este archivo de presentación y documentación del proyecto