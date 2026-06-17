#include "cfg_ADC.h"
#include "cfg_DMA.h"
#include "lpc17xx_gpio.h"

extern void delay_2seg(void);

void configADC(void){
	ADC_Init(SENSOR_ADC_FREC);							//frecuenciaMaximaPosible = 200 [kHz]
	ADC_PinConfig(SENSOR_ADC_CHANNEL);
	ADC_PowerUp();
	ADC_StartCmd(ADC_START_ON_MAT01);
	ADC_EdgeStartConfig(ADC_START_ON_RISING);	//ADC empieza a convertir en flanco ascendente del Match1 del Timer0
	ADC_ChannelEnable(SENSOR_ADC_CHANNEL);
	ADC_BurstDisable();
	ADC_IntDisable(ADC_INT_CH0);				//Lo desactivamos ya que el que se va a encargar de manejar el fin de conversion es el Request del DMA
}

/*uint32_t lecturaADC(void){

	ADC_StartCmd(ADC_START_NOW);	//Start Conversion
	while(ADC_ChannelGetStatus(SENSOR_ADC_CHANNEL, ADC_DATA_DONE) != SET);	//Esperar a que la conversion termine
	return (uint16_t)ADC_ChannelGetData(SENSOR_ADC_CHANNEL);				//Carga a lecturaADC con el resultado de la conversion

}
*/

void avanzarLineaRecta(){
    // Configura Pines del Puerto 1 para que ambos motores vayan adelante
	GPIO_SetPins(PORT_1, PIN_MOTOR_IZQ_A | PIN_MOTOR_DER_A);
	GPIO_ClearPins(PORT_1, PIN_MOTOR_IZQ_B | PIN_MOTOR_DER_B);
}

void frenarMotores(){
	GPIO_ClearPins(PORT_1, PIN_MOTOR_IZQ_A | PIN_MOTOR_IZQ_B | PIN_MOTOR_DER_A | PIN_MOTOR_DER_B);
}

void esquivarObstaculo(){
    frenarMotores();
    delay_2seg(); // Pausa de seguridad antes de girar

    // Girar (Ej: Motor Izquierdo Adelante, Derecho Atrás)
    GPIO_SetPins(PORT_1, PIN_MOTOR_IZQ_A | PIN_MOTOR_DER_B);
    GPIO_ClearPins(PORT_1, PIN_MOTOR_IZQ_B | PIN_MOTOR_DER_A);

    delay_2seg(); // Girando durante 2 segundos
    frenarMotores();
    delay_2seg(); // Pausa antes de volver a evaluar el camino
}
