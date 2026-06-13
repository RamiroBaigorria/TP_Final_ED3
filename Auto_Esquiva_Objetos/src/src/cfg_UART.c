#include "cfg_uart.h"

extern volatile uint32_t velocidad_duty_cycle;
extern volatile uint8_t auto_en_marcha;

void configUART(void)
{

	PINSEL_CFG_T cfgPIN02;// Configuracion del pin P0.2 como TXD0
		cfgPIN02.port 		= PORT_0;
		cfgPIN02.mode 		= PINSEL_TRISTATE;
		cfgPIN02.openDrain	= DISABLE;
		cfgPIN02.pin 		= PIN_2;			// Configurar P0.2 como TXD0
		cfgPIN02.func 		= PINSEL_FUNC_01;	// Función 01 selecciona UART0 TX


	PINSEL_CFG_T cfgPIN03;// Configuracion del pin P0.3 como RXD0
		cfgPIN03.port 		= PORT_0;
		cfgPIN03.mode 		= PINSEL_TRISTATE;
		cfgPIN03.openDrain	= DISABLE;
		cfgPIN03.pin 		= PIN_3;			// Configurar P0.3 como RXD0
		cfgPIN03.func 		= PINSEL_FUNC_01;	// Función 01 selecciona UART0 TX

	PINSEL_ConfigPin(&cfgPIN02);				//UART_PinConfig(UART_TX0_P0_2);
	PINSEL_ConfigPin(&cfgPIN03);				//UART_PinConfig(UART_RX0_P0_3);

    UART_CFG_T cfgUART;
    	cfgUART.baudRate = 9600;				// Si cambio este valor, lo tengo que cambiar en el programa que use en la PC para el uart (hercules, Putty o Termite)
		cfgUART.parity   = UART_PARITY_NONE;	// Sin bit de paridad
    	cfgUART.dataBits = UART_DBITS_8;		// 8 bits de datos
		cfgUART.stopBits = UART_STOPBIT_1;		// 1 bit de parada

    UART_FIFO_CFG_T cfgFIFO; // Configuración de las Colas de Espera
    	cfgFIFO.resetRxBuf = ENABLE;			// Al inicializar, limpia y vacía cualquier residuo de datos viejo que haya quedado flotando en las colas de transmisión y recepción.
    	cfgFIFO.resetTxBuf = ENABLE;			// Al inicializar, limpia y vacía cualquier residuo de datos viejo que haya quedado flotando en las colas de transmisión y recepción.
		cfgFIFO.dmaMode    = DISABLE;
		cfgFIFO.level      = UART_FIFO_TRGLEV0;	// Define que la UART de un aviso inmediato apenas ingrese 1 solo byte al buffer de recepción

    UART_Init(UART0, &cfgUART);
    UART_FIFOConfig(UART0, &cfgFIFO);

    UART_TxEnable(UART0);							// Enciende físicamente el pin de transmisión (TX). A partir de esta línea, el microcontrolador ya es capaz de escupir datos hacia afuera
    UART_IntConfig(UART0, UART_INT_RBR, ENABLE);	// Habilita la interrupción por recepción de datos (RBR)

}

void comunicacionUART(char *str)
{
    while(*str)
    {
        UART_Send(UART0, (uint8_t *)str, 1, BLOCKING);
        str++;
    }
}

void UART0_IRQHandler(void) {

    // Verificar si la interrupción fue legítimamente por recepción de dato (RBR)
    if (UART_GetIntId(UART0) & UART_IIR_INTSTAT_PEND) {

    	// Leer el byte recibido limpia la bandera de interrupción automáticamente
    	uint8_t datoRecibido = UART_ReceiveByte(UART0);

    	// CASO ESPECIAL: Si el auto está apagado y mandan la 'W', damos la orden de largada
		if (datoRecibido == 'W' && auto_en_marcha == 0) {
			velocidad_duty_cycle = 50; // Seteamos velocidad inicial al 50%
			auto_en_marcha = 1;        // Despierta al main del bucle seguro
		}
					// CONTROL EN MOVIMIENTO: Solo si el auto ya está corriendo procesamos los comandos
		else if (auto_en_marcha == 1) {


			switch (datoRecibido) {
				case 'W': // SUBIR VELOCIDAD
					if (velocidad_duty_cycle < 90) {
						velocidad_duty_cycle += 10;
					}
					break;

				case 'S': // BAJAR VELOCIDAD
					if (velocidad_duty_cycle > 10) {
						velocidad_duty_cycle -= 10;
					}
					break;

				case 'E': // PARADA DE EMERGENCIA
					velocidad_duty_cycle = 0;
					break;

				default:
					break;
			}

			// Actualizamos dinámicamente el Match 1 del Timer 1 con el nuevo Duty Cycle
			TIM_UpdateMatchValue(LPC_TIM1, 1, velocidad_duty_cycle);
		}
	}
}
