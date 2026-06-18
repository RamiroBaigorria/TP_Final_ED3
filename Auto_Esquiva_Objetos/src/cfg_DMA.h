#ifndef CFG_DMA_H
#define CFG_DMA_H

#include "LPC17xx.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

#define PIN_MOTOR_IZQ_A    (1 << 0)   // Pin P1.0
#define PIN_MOTOR_IZQ_B    (1 << 1)   // Pin P1.1
#define PIN_MOTOR_DER_A    (1 << 2)   // Pin P1.2
#define PIN_MOTOR_DER_B    (1 << 3)   // Pin P1.3

#define CHANNEL1_P2M     	GPDMA_CH_1
#define TRANSFERSIZE 		7
#define PIN_LPC				(1 << 22)

#define LIMITE_OBSTACULO	1850  	// Equivale a 20 cm medidos en el laboratorio (REALIZAR LA MEDICION REAL NOSOTROS MISMOS, ESTE ES UN VALOR COMUN pero NO SIEMPRE CIERTO)
							 		// Poner el auto frente a una pared a la distancia exacta a la que queremos que doble (Ejemplo: 20 cm).
									// Medimos con el ADC qué valor entrega el sensor a esa distancia. (Ejemplo: El ADC nos devuelve 1850)
									// Directamente definimos la constante LIMITE_OBSTACULO en base a ese número analógico

//#define memoriaADC_BUFFER 0x2007C000
//volatile uint32_t *adc_buffer[TRANSFERSIZE] = (volatile uint32_t *) memoriaADC_BUFFER;	// Buffer en RAM para almacenar las distancias

extern volatile uint32_t adc_buffer[];

void configDMA(void);

#endif
