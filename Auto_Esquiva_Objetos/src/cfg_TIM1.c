#include "cfg_tim1.h"
#include "cfg_dma.h"

volatile uint32_t velocidad_duty_cycle = 0;

void configTIMER1(){

	TIM_TIMERCFG_T cfgTIM1;
	cfgTIM1.prescaleOpt 	= TIM_US;
	cfgTIM1.prescaleValue 	= 100;			// Cada tick del timer dura 100 microsegundos

	// MATCH 0: Define el período total (Frecuencia del PWM)
	TIM_MATCHCFG_T cfgMATCH0;
	cfgMATCH0.channel 		= 0;			// USO MAT1.0 para periodo completo (util para reiniciar)
	cfgMATCH0.intEn 		= ENABLE;
	cfgMATCH0.stopEn 		= DISABLE;
	cfgMATCH0.resetEn 		= ENABLE;		// Resetea el timer para empezar de nuevo
	cfgMATCH0.extOpt 		= 0;
	cfgMATCH0.matchValue 	= 100;			// Periodo total = 100 ticks (100 * 100us = 10ms -> 100 Hz

	// MATCH 1: Define el Duty Cycle (Ancho de pulso)
	TIM_MATCHCFG_T cfgMATCH1;
	cfgMATCH1.channel 		= 1;					// USO MAT1.1 para el duty cycle
	cfgMATCH1.intEn 		= ENABLE;				// Acá se cumple el tiempo (Apagamos pines)
	cfgMATCH1.stopEn 		= DISABLE;
	cfgMATCH1.resetEn 		= DISABLE;				// Va en DISABLE ya que el RESET lo tenemos que manejar con el MATCH0
	cfgMATCH1.extOpt 		= 0;
	cfgMATCH1.matchValue 	= matchValueInicial;	// Arranca al 40% por defecto

	TIM_InitTimer(LPC_TIM1, &cfgTIM1);
	TIM_ConfigMatch(LPC_TIM1, &cfgMATCH0);
	TIM_ConfigMatch(LPC_TIM1, &cfgMATCH1);
	TIM_Enable(LPC_TIM1);							// ¿Hace falta?
}


void TIMER1_IRQHandler(void){

    // Usamos una variable estática para recordar qué pines estaban encendidos antes de apagarlos
    static uint32_t motores_activos = 0;

    // ---------------- INTERRUPCIÓN MATCH 0 (Inicio del ciclo: 0%) ----------------//
    if(TIM_GetIntStatus(LPC_TIM1, TIM_MR0_INT) == 1){

        // Si el duty cycle es 0, dejamos todo apagado y no hacemos nada
        if (velocidad_duty_cycle > 0) {

            // Leemos el estado actual del Puerto 1 para saber qué motores están en '1' debido al DMA
            motores_activos = GPIO_ReadValue(PORT_1) & (PIN_MOTOR_IZQ_A | PIN_MOTOR_IZQ_B | PIN_MOTOR_DER_A | PIN_MOTOR_DER_B);

            // Volvemos a encender esos mismos pines para arrancar el pulso en alto
            GPIO_SetPins(PORT_1, motores_activos);
        }

        TIM_ClearIntPending(LPC_TIM1, TIM_MR0_INT);
    }

    // ---------------- INTERRUPCIÓN MATCH 1 (Fin del Duty Cycle) ----------------
    if(TIM_GetIntStatus(LPC_TIM1, TIM_MR1_INT) == 1){

        // Si el duty cycle es menor al 100%, cortamos la energía apagando los pines activos
        if (velocidad_duty_cycle < 100) {
            GPIO_ClearPins(PORT_1, PIN_MOTOR_IZQ_A | PIN_MOTOR_IZQ_B | PIN_MOTOR_DER_A | PIN_MOTOR_DER_B);
        }

        TIM_ClearIntPending(LPC_TIM1, TIM_MR1_INT);
    }
}
