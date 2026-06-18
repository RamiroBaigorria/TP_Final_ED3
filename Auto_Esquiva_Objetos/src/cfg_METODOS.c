#include "cfg_METODOS.h"
#include "cfg_DMA.h"
#include "cfg_TIM1.h"

volatile uint32_t promedio_distancia = 0;

void avanzarLineaRecta(){
    // Configura Pines del Puerto 1 para que ambos motores vayan adelante
	motores_activos = PIN_MOTOR_IZQ_A | PIN_MOTOR_DER_A;
	//GPIO_SetPins(PORT_1, PIN_MOTOR_IZQ_A | PIN_MOTOR_DER_A);
	//GPIO_ClearPins(PORT_1, PIN_MOTOR_IZQ_B | PIN_MOTOR_DER_B);
}

void frenarMotores(){
	motores_activos = 0;
	//GPIO_ClearPins(PORT_1, PIN_MOTOR_IZQ_A | PIN_MOTOR_IZQ_B | PIN_MOTOR_DER_A | PIN_MOTOR_DER_B);
}

void esquivarObstaculo(){

	motores_activos = 0;
	delay_2seg();

	motores_activos = PIN_MOTOR_IZQ_A | PIN_MOTOR_DER_B;
	delay_2seg();

	motores_activos = 0;
	delay_2seg();
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
