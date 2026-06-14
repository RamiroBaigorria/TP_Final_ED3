#include "cfg_adc.h"

void configADC(void){
	ADC_Init(SENSOR_ADC_FREC);							//frecuenciaMaximaPosible = 200 [kHz]
	ADC_PinConfig(SENSOR_ADC_CHANNEL);
	ADC_PowerUp();
	ADC_StartCmd(ADC_START_ON_MAT01);
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

