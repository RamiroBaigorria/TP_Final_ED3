#ifndef CFG_TIM1_H
#define CFG_TIM1_H

#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

#define matchValueInicial 40

void configTIMER1(void);

extern volatile uint32_t velocidad_duty_cycle;



#endif
