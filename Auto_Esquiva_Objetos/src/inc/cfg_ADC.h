#ifndef CFG_ADC_H
#define CFG_ADC_H

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif
#include "lpc17xx_adc.h"
#include "lpc17xx_pinsel.h"

//----Config ADC----
#define SENSOR_ADC_CHANNEL     	ADC_CHANNEL_0
#define SENSOR_ADC_FREC        	200000        //100khz

//----Inicializo funciones----
void configADC(void);


//----Lectura de la distancia----
// lectura de 0 a 4095
// Girar si Lectura >= LIMITE_OBSTACULO
// Avanzar si Lectura < LIMITE_OBSTACULO
// uint32_t lecturaADC(void);

#endif
