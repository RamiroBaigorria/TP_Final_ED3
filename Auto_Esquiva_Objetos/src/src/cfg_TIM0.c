#include "cfg_tim0.h"

void configTIMER0(void){
	TIM_TIMERCFG_T cfgTIM0;
	cfgTIM0.prescaleOpt	 = TIM_US;
	cfgTIM0.prescaleValue = 1;			// Cada tick del timer dura 1 microsegundos

	TIM_MATCHCFG_T cfgMATCH;
	cfgMATCH.channel	= TIM_MATCH_1;
	cfgMATCH.intEn 		= DISABLE;
	cfgMATCH.stopEn 	= DISABLE;
	cfgMATCH.resetEn 	= ENABLE;
	cfgMATCH.extOpt 	= TIM_NOTHING;
	cfgMATCH.matchValue = 10000; 		// Interrupcion cada 10ms

	TIM_InitTimer(LPC_TIM0, &cfgTIM0);
	TIM_ConfigMatch(LPC_TIM0, &cfgMATCH);
	TIM_Enable(LPC_TIM0);				// ¿Hace falta?

}

/*void TIMER0_IRQHandler(void){
	if(TIM_GetIntStatus(LPC_TIM0, TIM_MR0_INT) == 1){

		TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
		lecturaADC();

	}
}
*/
