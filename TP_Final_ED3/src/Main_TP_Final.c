#include "cfg_adc.h";
#include "cfg_dac.h";
#include "cfg_dma.h";
#include "cfg_tim0.h";
#include "cfg_tim1.h";
#include "cfg_uart.h";


int main(void) {

    // Mandamos un mensaje de bienvenida apenas se enciende el auto
    comunicacionUART("--- SISTEMA INICIALIZADO - AUTO EVASOR OK ---\r\n");

    while(1){

    }
}
