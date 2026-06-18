#include "cfg_DMA.h"
#include "cfg_UART.h"
#include "cfg_DAC.h"
#include "lpc17xx_dac.h"

uint32_t promedio_distancia = 0;
volatile uint32_t adc_buffer[TRANSFERSIZE];

void configDMA(){
	GPDMA_Channel_CFG_T cfgDMA;
			cfgDMA.channelNum = CHANNEL1_P2M;
			cfgDMA.transferSize = TRANSFERSIZE;                  	// Almacenar 10 muestras consecutivas
			cfgDMA.type = GPDMA_P2M;                   	// De Periférico (ADC) a Memoria (RAM)
			cfgDMA.srcMemAddr = 0;
			cfgDMA.dstMemAddr = (uint32_t) &adc_buffer;  // Destino: nuestro vector en RAM
			cfgDMA.srcConn = GPDMA_ADC;
			cfgDMA.dstConn = 0;
			cfgDMA.src.width = GPDMA_HALFWORD;
			cfgDMA.src.burst = GPDMA_BSIZE_1;
			cfgDMA.src.increment = DISABLE;            	// El ADC es un registro fijo
			cfgDMA.dst.width = GPDMA_HALFWORD;
			cfgDMA.dst.burst = GPDMA_BSIZE_1;
			cfgDMA.dst.increment = ENABLE;             	// Incrementar para llenar el vector
			cfgDMA.intTC = ENABLE;                    	// Interrumpir cuando el buffer esté lleno
			cfgDMA.linkedList = 0;

		GPDMA_Init();
		GPDMA_SetupChannel(&cfgDMA);
}

void movingAverage(){

	 uint32_t suma = 0;

	 for(int i=0; i<TRANSFERSIZE; i++) {
		 suma += adc_buffer[i];
	 }

	 promedio_distancia = suma / TRANSFERSIZE;	//Moving Average
}

void configPIN(){
	GPIO_SetDir (PORT_1, PIN_MOTOR_IZQ_A | PIN_MOTOR_IZQ_B | PIN_MOTOR_DER_A | PIN_MOTOR_DER_B, GPIO_OUTPUT);	// Configurar los 4 pines de control de motores como salida
	GPIO_ClearPins (PORT_1, PIN_MOTOR_IZQ_A | PIN_MOTOR_IZQ_B | PIN_MOTOR_DER_A | PIN_MOTOR_DER_B);				// Estado inicial seguro: Todo apagado (Auto frenado)
	GPIO_SetDir (PORT_0, PIN_LPC, GPIO_OUTPUT);																	// Led que indica error en transferencia
}

void delay_2seg(void){
    volatile int32_t i = 20000000; // 'volatile' evita que el compilador borre el bucle por optimización
    while(i > 0){
        i--;
    }
}

void DMA_IRQHandler(void){
	//static uint8_t estado_anterior = 0; // 0 = Libre, 1 = Obstáculo

	if (GPDMA_IntGetStatus(GPDMA_INTTC, CHANNEL1_P2M)) {

	        GPIO_ClearPins(PORT_0, PIN_LPC); 		// No hubo error
	        movingAverage();               			// Metodo que muestrea las conversiones del ADC
	        DAC_UpdateValue(promedio_distancia); 	// Envio promedio_distancia al DAC para verlo con un tester

	        GPDMA_ClearIntPending(GPDMA_CLR_INTTC, CHANNEL1_P2M);
	    }

	if(GPDMA_IntGetStatus(GPDMA_INTERR, CHANNEL1_P2M)){

			GPIO_SetPins(PORT_0, PIN_LPC); 			// Hubo error

			GPDMA_ClearIntPending(GPDMA_CLR_INTERR, GPDMA_CH_1);
		}
}
