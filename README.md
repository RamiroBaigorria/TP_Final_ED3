# 🚗 Auto Autónomo Evasor de Obstáculos (ARM Cortex-M3)
> **Asignatura:** Electrónica Digital III - Universidad Nacional de Córdoba (FCEFyN)  
> **Integrantes:** > * Baigorria, Ramiro Javier  
>                    * Cravero, Lorenzo
>                    * Gonzalez, Tobias
> **Profesor:** Blasco

---

## 🚀 Descripción General del Proyecto
    Este proyecto consiste en el diseño y desarrollo de un **vehículo autónomo capaz de evadir obstáculos en tiempo real**. El sistema utiliza un enfoque de hardware de alto rendimiento basado en la arquitectura ARM Cortex-M3, procesando de manera asíncrona las lecturas de un sensor de distancia analógico para determinar si la trayectoria del coche es segura mediante algoritmos de control de movimiento directo sobre motores de corriente continua.

    A diferencia de las soluciones convencionales de software bloqueante (basadas en retrasos por software o *polling* continuo), este sistema exprime las capacidades avanzadas del microcontrolador. Utiliza temporizadores de hardware para sincronizar el muestreo, automatiza la transferencia de datos mediante accesos directos a memoria (DMA) y controla la potencia del motor por PWM nativo, minimizando drásticamente la carga de procesamiento de la CPU.

### 🎯 Alcances del Proyecto
* **El sistema es capaz de:**
  * Realizar un muestreo totalmente automatizado y no bloqueante de un sensor de distancia analógico de 0 a 4095 (12 bits) mediante el periférico ***ADC***, sincronizado por hardware con el ***Timer0*** cada 10 ms.
  * Transferir los resultados de las conversiones directamente a la memoria RAM utilizando el controlador de ***DMA (GPDMA)***, eliminando la sobrecarga por interrupciones repetitivas en el procesador.
  * Generar señales ***PWM*** de alta precisión mediante hardware con el ***Timer1*** para regular de forma independiente la velocidad y el sentido de giro de los motores de tracción a través de un puente H.
  * Ejecutar una lógica de evasión perimetral en tiempo real: si la medición supera el umbral crítico (`Lectura >= LIMITE_OBSTACULO`), el vehículo frena y gira; de lo contrario (`Lectura < LIMITE_OBSTACULO`), mantiene su marcha hacia adelante.
  * Transmitir tramas de telemetría y depuración en vivo a una terminal externa por medio del puerto serie ***UART***.
  * Conectividad inalámbrica (módulos Bluetooth) para control remoto o monitorización a distancia.

---

## 📐 Arquitectura del Sistema: Hardware y Software

### 🔌 Hardware & Interconexión
El núcleo del hardware está constituido por el microcontrolador de desarrollo, interactuando de forma directa con las etapas de adquisición (sensor analógico de proximidad) y la etapa de potencia (controlador de motores puente H).


### 💻 Arquitectura de Software (Firmware)
El diseño del software sigue una filosofía **modular y configurar los periférico de forma autónoma**, donde el bucle principal (`while(1)`) queda prácticamente liberado para tareas de diagnóstico de alto nivel gracias a la automatización de los perifericos.

* **Diagrama de Flujo:** El Timer0 dispara el ADC; el ADC avisa al DMA cuando finaliza la conversión; el DMA guarda el dato en memoria; la CPU lee la variable de forma asíncrona y recalcula el ciclo de trabajo del PWM en el Timer1 para los motores.
  `![Esquemático Completo](Diseño/Diagrama de Flujo.png)`

---

## 🔄 Proceso de Integración y Desarrollo
El proyecto se abordó empleando una metodología incremental con pruebas unitarias modulares para garantizar la robustez del sistema final:

* **Etapa 1 (Estructura Base):** Configuración del SystemInit, árboles de reloj del sistema a 100 MHz y modularización de drivers mediante capas de abstracción (`cfg_NVIC`, `cfg_UART`). Validación mediante eco serie básico.
* **Etapa 2 (Control de Potencia):** Puesta a punto del `cfg_TIM1` para generar señales PWM estables. Calibración de anchos de pulso mínimos y máximos para el movimiento controlado de los motores en banco de pruebas.
* **Etapa 3 (Adquisición Automatizada):** Integración de `cfg_TIM0`, `cfg_ADC` y `cfg_DMA`. Donde los datos del sensor fluyen a la memoria RAM sin intervención de código del bucle principal.
* **Etapa 4 (Lógica de Navegación e Integración Total):** Acoplamiento de la lógica en `Main_TP_Final.c`, interconexión del chasis mecánico y pruebas de estrés frente a obstáculos dinámicos en pista.

---

## 📊 Ensayos, Pruebas y Resultados
Se realizaron ensayos dinámicos midiendo los tiempos de respuesta del algoritmo ante la detección de barreras físicas. Gracias al uso de transferencias controladas por DMA, el retardo total desde que el sensor detecta un obstáculo hasta que el temporizador PWM altera el sentido de giro del vehículo es menor a **70 milisegundos**, asegurando maniobras de evasión eficaces inclusive a velocidades máximas de desplazamiento.

---

## 📂 Estructura del Repositorio
El repositorio cuenta con una organización limpia y estandarizada. Los archivos temporales generados localmente por la compilación de MCUXpresso e índices Git locales (`.metadata/`, `Debug/`, `Release/`, etc.) se encuentran excluidos mediante el archivo `.gitignore` para preservar la limpieza del repositorio remoto.

```text
├── firmware/                  # Código fuente del proyecto en MCUXpresso IDE
|   ├── Main_TP_Final.c    # Lazo principal y lógica de decisión del auto autónomo
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
├── hardware/                  # Archivos de diseño general del proyecto (Diagrama de flujo)
├── docs/                      
└── README.md                  # Este archivo de presentación y documentación del proyecto