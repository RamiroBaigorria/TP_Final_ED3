#ifndef CFG_UART_H_
#define CFG_UART_H_

#include "LPC17xx.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_pinsel.h"

void configUART(void);
void comunicacionUART(char *str);

//extern volatile uint32_t velocidad_duty_cycle;

#endif
