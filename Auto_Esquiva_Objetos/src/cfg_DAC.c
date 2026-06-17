#include "cfg_DAC.h"

void configDAC(void){

	DAC_Init();							//El pin del DAC se configura solo con esta funcion. Bit 20 y 21 del PINSEL1, funcion 10: AOUT
	DAC_CONVERTER_CFG_T cfgDAC;
	cfgDAC.doubleBuffer = DISABLE;
	cfgDAC.dmaCounter = DISABLE;
	cfgDAC.dmaRequest = DISABLE;

	DAC_SetBias(DAC_700uA);
	DAC_ConfigDAConverterControl(&cfgDAC);

}
