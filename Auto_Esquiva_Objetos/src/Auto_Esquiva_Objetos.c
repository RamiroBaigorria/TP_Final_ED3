#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_systick.h"

#include <cr_section_macros.h>

/* -----------------------------------------------------------------------
 * 						Definicion de variables
 * --------------------------------------------------------------------- */

#define TRNASFERSIZE 7

#define DIST_MIN 10 	/* Distancia mínima de frenado en [cm] */

/* Pines motores */
#define PUERTO_MOTOR PORT_0
#define MOT_IZQ PIN_4
#define MOT_DER PIN_5

#define MOTOR_MASK ((1 << 4) | (1 << 5))

volatile uint16_t adcBuffer[TRNASFERSIZE]; 	/* Buffer destino del DMA */

volatile uint8_t  buffer_ok = 0;	/* Flag que indica que el DMA completó las 7 transferencias */

volatile uint8_t  estaGirando = 0; 	/* 0 = avanzando, 1 = girando */

volatile uint32_t giro_2seg = 0; 	/* Contador [ms] para el giro (usamos SysTick)*/

void cfgPINS(void);
void cfgADC(void);
void cfgTIMER0(void);
void cfgTIMER1_PWM(void);
void cfgDMA(void);
void cfgDAC(void);
void cfgSysTick(void);

void avanceAuto(void);
void stopAuto(void);
void giroAuto(void);

static uint32_t promedioDistancia(void);

/* -----------------------------------------------------------------------
 * 								  Main
 * --------------------------------------------------------------------- */
int main(void){
	cfgPINS();
    cfgADC();
    cfgTIMER0();
    cfgTIMER1_PWM();
    cfgDMA();
    cfgDAC();
    cfgSysTick();

    avanceAuto();

    while (1){
		/* estado de fin de giro */
		if (estaGirando && giro_2seg == 0){
			estaGirando = 0; 	/* terminaron los 2seg => volver a avanzar */
			stopAuto();

			for (volatile uint32_t i = 0; i < 500000; i++); 	/* Pequeña pausa para que los motores frenen (∼20 ms) */

			avanceAuto();
			GPDMA_ChannelStop(GPDMA_CH_0);
			buffer_ok = 0;
			cfgDMA();
		}

		/* testear distancia cuando el buffer de muestras este completo */
		if (buffer_ok && !estaGirando){
			uint32_t distCm = promedioDistancia();

			if (distCm <= DIST_MIN){ 	/* objeto muy cerca */
				stopAuto();

				for (volatile uint32_t i = 0; i < 500000; i++); 	/* Pequeña pausa para que los motores frenen (∼20 ms) */

				estaGirando = 1;
				giro_2seg = 2000; /*2000ms -> 2seg */

				giroAuto();
				/* El DMA se reinicia al terminar el giro */
			}
			else{
				/*reinicia DMA para la próxima ráfaga de muestras */
				GPDMA_ChannelStop(GPDMA_CH_0);
				buffer_ok = 0;
				cfgDMA();
			}
		}
    }
    return 0;
}

/* -----------------------------------------------------------------------
 * 							Configuraciones
 * --------------------------------------------------------------------- */
void cfgPINS(void){
    PINSEL_CFG_T cfgP4;
    cfgP4.port 		= PUERTO_MOTOR;
    cfgP4.pin  		= MOT_IZQ;
    cfgP4.func      = PINSEL_FUNC_00;
    cfgP4.mode      = PINSEL_PULLUP;
    cfgP4.openDrain = DISABLE;

    PINSEL_ConfigPin(&cfgP4);

    PINSEL_CFG_T cfgP5;
    cfgP5.port 		= PUERTO_MOTOR;
    cfgP5.pin  		= MOT_DER;
    cfgP5.func      = PINSEL_FUNC_00;
    cfgP5.mode      = PINSEL_PULLUP;
    cfgP5.openDrain = DISABLE;

    PINSEL_ConfigPin(&cfgP5);

    GPIO_SetDir(PUERTO_MOTOR, MOTOR_MASK, GPIO_OUTPUT);
    GPIO_ClearPins(PUERTO_MOTOR, MOTOR_MASK);

    /*configuro un led de control en caso de que ocurra un error de transferencia del gpdma*/
    PINSEL_CFG_T cfgP22;
	cfgP22.port = PORT_0;
	cfgP22.pin = PIN_22;
	cfgP22.func = PINSEL_FUNC_00;
	cfgP22.mode = PINSEL_PULLDOWN;
	cfgP22.openDrain = DISABLE;

	PINSEL_ConfigPin(&cfgP22);

	GPIO_SetDir(PORT_0, 0x400000, GPIO_OUTPUT);
	GPIO_ClearPins(PORT_0, 0x400000);
}

void cfgDAC(void){
	DAC_CONVERTER_CFG_T dacCfg;
	dacCfg.doubleBuffer = DISABLE;
	dacCfg.dmaCounter = DISABLE;
	dacCfg.dmaRequest = DISABLE;

	DAC_Init();
	DAC_ConfigDAConverterControl(&dacCfg);
	DAC_SetBias(DAC_700uA);
}

void cfgADC(void){
    ADC_Init(100000);
    ADC_PinConfig(ADC_CHANNEL_0);
    ADC_BurstDisable();
    ADC_StartCmd(ADC_START_ON_MAT01);
    ADC_ChannelEnable(ADC_CHANNEL_0);
    ADC_EdgeStartConfig(ADC_START_ON_RISING);
    ADC_IntDisable(ADC_INT_CH0);

    ADC_PowerUp();
}

void cfgTIMER0(void){
    TIM_TIMERCFG_T timerCfg;
    timerCfg.prescaleOpt   = TIM_US;
	timerCfg.prescaleValue = 1;

    TIM_MATCHCFG_T matchCfg;
	matchCfg.channel    = TIM_MATCH_1;
	matchCfg.intEn      = DISABLE;
	matchCfg.stopEn     = DISABLE;
	matchCfg.resetEn    = ENABLE;
	matchCfg.extOpt     = TIM_TOGGLE;   /* alterna MAT0.1 -> trigger ADC */
	matchCfg.matchValue = 5000;		/* dispara ADC cada 10 ms */

    TIM_InitTimer(LPC_TIM0, &timerCfg);
    TIM_ConfigMatch(LPC_TIM0, &matchCfg);
    TIM_Enable(LPC_TIM0);
}

void cfgTIMER1_PWM(void){
    TIM_TIMERCFG_T timerCfg;
	timerCfg.prescaleOpt   = TIM_US;
	timerCfg.prescaleValue = 1;

    /* Match0: fin de periodo → reset TC + interrupción (flanco subida) */
    TIM_MATCHCFG_T match0;
	match0.channel    = TIM_MATCH_0;
	match0.intEn      = ENABLE;
	match0.stopEn     = DISABLE;
	match0.resetEn    = ENABLE;
	match0.extOpt     = TIM_NOTHING;
	match0.matchValue = 20000; /* PWM: periodo 20 ms */

    /* Match1: fin del duty cycle -> interrupción (flanco bajada) */
    TIM_MATCHCFG_T match1;
	match1.channel    = TIM_MATCH_1;
	match1.intEn      = ENABLE;
	match1.stopEn     = DISABLE;
	match1.resetEn    = DISABLE;
	match1.extOpt     = TIM_NOTHING;
	match1.matchValue = 10000; /* duty 50 % -> 10 ms */


    TIM_InitTimer(LPC_TIM1, &timerCfg);
    TIM_ConfigMatch(LPC_TIM1, &match0);
    TIM_ConfigMatch(LPC_TIM1, &match1);

    NVIC_SetPriority(TIMER1_IRQn, 2);
    NVIC_EnableIRQ(TIMER1_IRQn);
    NVIC_ClearPendingIRQ(TIMER1_IRQn);

    TIM_Enable(LPC_TIM1);
}

/* DMA transfiere el resultado del ADC al buffer */
void cfgDMA(void){
    GPDMA_Init();

    GPDMA_Channel_CFG_T dmaCfg;
	dmaCfg.channelNum   = GPDMA_CH_0;
	dmaCfg.transferSize = TRNASFERSIZE;
	dmaCfg.type         = GPDMA_P2M;
	dmaCfg.srcMemAddr   = 0;
	dmaCfg.dstMemAddr   = (uint32_t)adcBuffer;
	dmaCfg.srcConn      = GPDMA_ADC;
	dmaCfg.dstConn      = 0;

	dmaCfg.src.width     = GPDMA_HALFWORD;
	dmaCfg.src.burst     = GPDMA_BSIZE_1;
	dmaCfg.src.increment = DISABLE;

	dmaCfg.dst.width     = GPDMA_HALFWORD;
	dmaCfg.dst.burst     = GPDMA_BSIZE_1;
	dmaCfg.dst.increment = ENABLE; 	/*incremento el destino*/

	dmaCfg.intTC      = ENABLE;
	dmaCfg.intErr     = ENABLE;
	dmaCfg.linkedList = 0;

    GPDMA_SetupChannel(&dmaCfg);

    NVIC_SetPriority(DMA_IRQn, 1);
    NVIC_EnableIRQ(DMA_IRQn);
    NVIC_ClearPendingIRQ(DMA_IRQn);

    GPDMA_ChannelStart(GPDMA_CH_0);
}

/* interrupción cada 1 ms para el tiempo de giro */
void cfgSysTick(void){
    SYSTICK_InternalInit(1);    /* 1 ms */
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);
}

static uint32_t pwmActivePins = 0; 	/* Pines que reciben el pulso PWM en el ciclo actual */

static uint32_t pwmZeroPins = 0; 	/* Pines que deben mantenerse en 0 fijo (IN2 del lado activo) */

void avanceAuto(void){
    /*
     * Motor A: AIN1=PWM, AIN2=0 -> avance sentido horario
     * Motor B: BIN1=PWM, BIN2=0 -> avance sentido horario
     */
    pwmActivePins = (1 << MOT_IZQ) | (1 << MOT_DER);
    pwmZeroPins   = 0;
    GPIO_ClearPins(PUERTO_MOTOR, pwmZeroPins);
}

void stopAuto(void){
    /* todos los pines en 0 */
    pwmActivePins = 0;
    pwmZeroPins   = 0;
    GPIO_ClearPins(PUERTO_MOTOR, MOTOR_MASK);
}

void giroAuto(void){
    /*
     * Giro en el lugar hacia la derecha:
     *   Motor A (izquierda): AIN1=PWM, AIN2=0 -> avance sentido horario
     *   Motor B (derecha)  : BIN1=0,   BIN2=0 -> stop
     */
    pwmActivePins = (1 << MOT_IZQ);
    pwmZeroPins   = (1 << MOT_DER);
    GPIO_ClearPins(PUERTO_MOTOR, pwmZeroPins);
}

static uint32_t promedioDistancia(void){
    uint32_t sum = 0;

    for (uint8_t i = 0; i < TRNASFERSIZE; i++){
        sum += (adcBuffer[i] >> 4) & 0xFFF; 	
    }

    uint32_t prom =  sum / TRNASFERSIZE;

    DAC_UpdateValue(prom);

    if (prom == 0){
    	return 100;   /* evita div/0 -> asumimos que esta lejos */
    }

    uint32_t dist = 24000 / prom;

    if (dist > 80){
    	dist = 80;
    }

    if (dist <  5){
    	dist =  5;
    }

    return dist;
}

/* -----------------------------------------------------------------------
 * 								Handlers
 * --------------------------------------------------------------------- */
void TIMER1_IRQHandler(void){
    if (TIM_GetIntStatus(LPC_TIM1, TIM_MR0_INT)){
        /* Flanco de subida: activar los pines de PWM del modo actual     */
    	if (pwmActivePins){
    		GPIO_SetPins(PUERTO_MOTOR, pwmActivePins);
    	}
        TIM_ClearIntPending(LPC_TIM1, TIM_MR0_INT);
    }

    if (TIM_GetIntStatus(LPC_TIM1, TIM_MR1_INT)){
        /* Flanco de bajada: desactivar todos los pines activos           */
        if (pwmActivePins){
        	GPIO_ClearPins(PUERTO_MOTOR, pwmActivePins);
        }
        TIM_ClearIntPending(LPC_TIM1, TIM_MR1_INT);
    }
}

/* canal 0 completó la transferencia de 7 muestras */
void DMA_IRQHandler(void){
    if (GPDMA_IntGetStatus(GPDMA_INTTC, GPDMA_CH_0)){
    	GPIO_ClearPins(PORT_0, 0x400000); 	// No hubo error
    	buffer_ok = 1;

        GPDMA_ClearIntPending(GPDMA_CLR_INTTC, GPDMA_CH_0);
    }

    if (GPDMA_IntGetStatus(GPDMA_INTERR, GPDMA_CH_0)){
    	GPIO_SetPins(PORT_0, 0x400000); 	// Hubo error

        GPDMA_ClearIntPending(GPDMA_CLR_INTERR, GPDMA_CH_0);
        restartDMA();
    }
}

/* incrementa el contador de ms para el tiempo de giro */
void SysTick_Handler(void){
    SYSTICK_ClearCounterFlag();
    if (estaGirando && giro_2seg > 0){
    	giro_2seg--;
    }
}




