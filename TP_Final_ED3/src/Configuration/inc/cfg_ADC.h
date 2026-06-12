#ifndef CFG_ADC_H
#define CFG_ADC_H

#include "LPC17xx.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_pinsel.h"

//----Config ADC----
#define SENSOR_ADC_CHANNEL     	ADC_CHANNEL_0
#define SENSOR_ADC_FREC        	200000        //100khz

#define LIMITE_OBSTACULO		1850  	// Equivale a 20 cm medidos en el laboratorio (REALIZAR LA MEDICION REAL NOSOTROS MISMOS, ESTE ES UN VALOR COMUN pero NO SIEMPRE CIERTO)
								 		// Poner el auto frente a una pared a la distancia exacta a la que queremos que doble (Ejemplo: 20 cm).
										// Medimos con el ADC qué valor entrega el sensor a esa distancia. (Ejemplo: El ADC nos devuelve 1850)
										// Directamente definimos la constante LIMITE_OBSTACULO en base a ese número analógico

//----Inicializo funciones----
void configADC(void);


//----Lectura de la distancia----
// lectura de 0 a 4095
// Girar si Lectura >= LIMITE_OBSTACULO
// Avanzar si Lectura < LIMITE_OBSTACULO
uint16_t lecturaADC(void);

#endif
