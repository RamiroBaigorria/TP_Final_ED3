#include "cfg_dma.h"
#include "cfg_uart.h"

uint32_t promedio_distancia = 0;

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

}

void delay_2seg(){
	uint32_t i = 7200;
	while(i>=0){
		i--;
	}
}

void DMA_IRQHandler(void){
	static uint8_t estado_anterior = 0; // 0 = Libre, 1 = Obstáculo

	if(detenerAuto == 1){

		//-----Funcion para detener auto-----

	}else{

			if (GPDMA_IntGetStatus(GPDMA_INTTC, CHANNEL1_P2M)) {

				GPIO_SetPins(PORT_0, PIN_LPC); //no hubo error
				movingAverage();
				DAC_UpdateValue(promedio_distancia);

				// Rango límite: El objeto está muy cerca
				if (promedio_distancia > LIMITE_OBSTACULO) {

					//-----Funcion para detener auto-----
					delay_2seg;

					//-----Funcion para girar auto-----
					delay_2seg;

						// ¡TELEMETRÍA INALÁMBRICA! Si antes venía libre, avisa que detectó algo
						if (estado_anterior == 0) {
							comunicacionUART("¡Obstaculo detectado! Girando...\r\n");
							estado_anterior = 1; // Cambia el estado
						}

				} else {

					//-----Funcion para avanzar auto-----
					delay_2seg;

						// ¡TELEMETRÍA INALÁMBRICA! Si antes estaba esquivando, avisa que el camino se liberó
						if (estado_anterior == 1) {
							comunicacionUART("Camino libre. Avanzando en linea recta.\r\n");
							estado_anterior = 0; // Cambia el estado
						}

					}

				// Limpiar bandera y volver a encender el canal para la próxima ráfaga de mediciones
				GPDMA_ClearIntPending(GPDMA_CLR_INTTC, CHANNEL1_P2M);

			}
		}


	if(GPDMA_IntGetStatus(GPDMA_INTERR, CHANNEL1_P2M)){

		GPIO_SetPins(PORT_0, PIN_LPC); //hubo error

		//-----Funcion para detener auto-----
		delay_2seg;

		GPDMA_ClearIntPending(GPDMA_CLR_INTERR, GPDMA_CH_1);
	}
}
