#include "cfg_dac.h"

void configDAC(void){

	DAC_Init();
	DAC_CONVERTER_CFG_T cfgDAC;
	cfgDAC.doubleBuffer = DISABLE;
	cfgDAC.dmaCounter = DISABLE;
	cfgDAC.dmaRequest = DISABLE;

	DAC_SetBias(DAC_700uA);
	DAC_ConfigDAConverterControl(&cfgDAC);
	DAC_UpdateValue(variableUsadaParaDAC);			//Aca va a ir la variable que vamos a determinar en otro lugar, que va a salir por el DAC

}
