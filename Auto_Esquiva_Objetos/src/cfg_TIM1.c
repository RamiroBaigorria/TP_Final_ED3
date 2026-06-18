#include "cfg_TIM1.h"
#include "cfg_DMA.h"
#include "cfg_UART.h"
#include "cfg_METODOS.h"

extern volatile uint32_t velocidad_duty_cycle;
extern uint8_t detenerAuto;
//volatile uint32_t motores_activos = 0;

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
	cfgMATCH0.matchValue 	= 100;			// Periodo total = 100 * 100us = 10ms (Va a ser el doble que el periodo del TIM0)
											// Frecuencia = 1 / Periodo ==> Frecuencia = 1 / 0,01 = 100Hz

	// MATCH 1: Define el Duty Cycle (Ancho de pulso)
	TIM_MATCHCFG_T cfgMATCH1;
	cfgMATCH1.channel 		= 1;					// USO MAT1.1 para el duty cycle
	cfgMATCH1.intEn 		= ENABLE;				// Acá se cumple el tiempo (Apagamos pines)
	cfgMATCH1.stopEn 		= DISABLE;
	cfgMATCH1.resetEn 		= DISABLE;				// Va en DISABLE ya que el RESET lo tenemos que manejar con el MATCH0
	cfgMATCH1.extOpt 		= 0;
	cfgMATCH1.matchValue 	= matchValueInicial;	// Arranca al 30% por defecto

	TIM_InitTimer(LPC_TIM1, &cfgTIM1);
	TIM_ConfigMatch(LPC_TIM1, &cfgMATCH0);
	TIM_ConfigMatch(LPC_TIM1, &cfgMATCH1);

}

	// -------------------------------------------------------------------------------------//
	// Al llegar a 100 ticks (10ms), el Timer se resetea automáticamente gracias a la		//
	// configuración cfgMATCH0.resetEn = ENABLE;, vuelve a Match 0 y todo empieza otra vez.	//
	// Esto pasa 100 veces por segundo (100 Hz), por lo que el motor no llega a frenarse,	//
	// sino que recibe un "promedio" de tensión que regula su velocidad.					//
	// -------------------------------------------------------------------------------------//

void TIMER1_IRQHandler(void){

    // Usamos una variable estática para recordar qué pines estaban encendidos antes de apagarlos
    static uint32_t motores_activos = 0;

    // ------------------- INTERRUPCIÓN MATCH 0 (Inicio del ciclo: 0%) -------------------//
    // El código lee el Puerto 1, ve qué motores estaban seleccionados para moverse y los //
    // enciende (pone en ALTO / 3.3V). Los motores empiezan a recibir energía.			  //
    // -----------------------------------------------------------------------------------//
    if(TIM_GetIntStatus(LPC_TIM1, TIM_MR0_INT) == 1){

    		// Si la velocidad es mayor a 0 y no hay orden de parada de emergencia...
            if (velocidad_duty_cycle > 0 && detenerAuto == 0) {

					// LEER: Miramos el Puerto 1 para saber qué motores quiere la lógica mantener activos
					//motores_activos = GPIO_ReadValue(PORT_1) & (PIN_MOTOR_IZQ_A | PIN_MOTOR_IZQ_B | PIN_MOTOR_DER_A | PIN_MOTOR_DER_B);

                // ENCENDER: Volvemos a poner en '1' (ALTO) esos mismos pines para arrancar el pulso PWM
                GPIO_SetPins(PORT_1, motores_activos);
            }

            // Limpiar la bandera de la alarma Match 0
            TIM_ClearIntPending(LPC_TIM1, TIM_MR0_INT);
        }

    // ---------------------------- INTERRUPCIÓN MATCH 1 (Fin del Duty Cycle) ----------------------------//
    // El Timer sigue contando y llega al valor configurado en "velocidad_duty_cycle". Se dispara la	  //
    // interrupción TIM_MR1_INT. El código apaga absolutamente todos los pines de los motores (los pone   //
    // en BAJO / 0V). El motor se queda sin energía por el resto del ciclo.								  //
    // ---------------------------------------------------------------------------------------------------//
    if(TIM_GetIntStatus(LPC_TIM1, TIM_MR1_INT) == 1){

    		// Si la velocidad es menor al 100% o el auto se debe detener, cortamos el pulso
            if (velocidad_duty_cycle <= 100 || detenerAuto == 1) {

            	// APAGAR: Ponemos en '0' (BAJO) todos los pines de los motores de golpe
                GPIO_ClearPins(PORT_1, PIN_MOTOR_IZQ_A | PIN_MOTOR_IZQ_B | PIN_MOTOR_DER_A | PIN_MOTOR_DER_B);
            }

            // Limpiar la bandera de la alarma Match 1
            TIM_ClearIntPending(LPC_TIM1, TIM_MR1_INT);
        }
}
