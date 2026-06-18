#ifndef CFG_METODOS_H
#define CFG_METODOS_H

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif
#include "lpc17xx_adc.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

//----Inicializon funciones----
void avanzarLineaRecta(void);
void frenarMotores(void);
void esquivarObstaculo(void);

void movingAverage(void);
void configPIN(void);
void delay_2seg(void);

extern volatile uint32_t promedio_distancia;

#endif
