#include "LPC17xx.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

void cfgUART(void);

int main(void){
	cfgUART();

	uint8_t dato;

    while (1){
        /* se queda esperando hasta que enviemos un dato por terminal, y no deja de esperar hasta que se haya recibido todo el dato (BLOCKING) */
        UART_Receive(UART1, &dato, 1, BLOCKING);

        dato += 1; /* le suma 1 valor de la tabla ASCII -> si envio una a, devuelve una b, si envio un 4, devuelve un 5, ... */

        /* envia un dato a la terminal, y espera hasta que se termine de enviar todo el dato (BLOCKING) */
        UART_Send(UART1, &dato, 1, BLOCKING);
    }

    return 0;
}

void cfgUART(void){
	UART_CFG_T uartCfg;
	uartCfg.baudRate = 9600;
	uartCfg.parity = UART_PARITY_NONE;
	uartCfg.dataBits = UART_DBITS_8;
	uartCfg.stopBits = UART_STOPBIT_1;

	UART_FIFO_CFG_T fifoCfg;
	fifoCfg.resetRxBuf = ENABLE;
	fifoCfg.resetTxBuf = ENABLE;
	fifoCfg.dmaMode = DISABLE;
	fifoCfg.level = UART_FIFO_TRGLEV0;

	UART_Init(UART1,&uartCfg);
	UART_FIFOConfig(UART1,&fifoCfg);
	UART_TxEnable(UART1);

	UART_PinConfig(UART_TX1_P0_15);
	UART_PinConfig(UART_RX1_P0_16);
}






