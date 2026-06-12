#include "cfg_tim0.h"

void configTIMER0(void){
	TIM_TIMERCFG_T cfgTIM0;
	cfgTIM0.prescaleOpt	 = TIM_US;
	cfgTIM0.prescaleValue = 1; 			//PROVISORIO: cambiar si es necesario

	TIM_MATCHCFG_T cfgMATCH;
	cfgMATCH.channel	= TIM_MATCH_1;
	cfgMATCH.intEn 		= ENABLE;
	cfgMATCH.stopEn 	= DISABLE;
	cfgMATCH.resetEn 	= ENABLE;
	cfgMATCH.extOpt 	= TIM_NOTHING;
	cfgMATCH.matchValue = 10000; 		//Interrupcion cada 10ms
  //cfgMATCH.matchValue = 125000; 	no me parece correcto, con el prescaler en 1 , 125mil tics son 125[mS]


	TIM_InitTimer(LPC_TIM0, &cfgTIM0);
	TIM_ConfigMatch(LPC_TIM0, &cfgMATCH);

	//TIM_Enable(LPC_TIM0);				//¿Hace falta? Main

}
