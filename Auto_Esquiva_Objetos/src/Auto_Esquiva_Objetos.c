#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include "cfg_adc.h"
#include "cfg_dac.h"
#include "cfg_dma.h"
#include "cfg_tim0.h"
#include "cfg_tim1.h"
#include "cfg_uart.h"
#include "cfg_nvic.h"

volatile uint32_t velocidad_duty_cycle = 0;  // Empieza en 0% (Totalmente frenado)
volatile uint8_t auto_en_marcha = 0;         // Flag de estado: 0 = Esperando comando, 1 = Corriendo
extern volatile uint32_t promedio_distancia; // Declarada en el modulo DMA
extern uint8_t detenerAuto;                  // Declarada en el modulo UART

void esquivarObstaculo(void);
void avanzarLineaRecta(void);
void frenarMotores(void);

int main(void) {

	// Inicialización de todos los periféricos del sistema
	configPIN();
	configTIMER0();
	configTIMER1();
	configADC();
	configDAC();
	configDMA();
	configNVIC();
	configUART();

	// Estado de reposo inicial: Cartel de bienvenida por Bluetooth
	comunicacionUART("===========================================\r\n");
	comunicacionUART("   SISTEMA INICIALIZADO - AUTO EVASOR        \r\n");
    comunicacionUART(" Envie 'W' por Bluetooth para iniciar marcha \r\n");
    comunicacionUART("===========================================\r\n");

    // El microcontrolador se queda durmiendo acá. Solo la interrupción de la UART puede cambiar la variable 'auto_en_marcha' a 1 cuando reciba una 'W'
    while(auto_en_marcha == 0) {
            __WFI();
        }
    //¡ORDEN DE LARGADA RECIBIDA! Habilitamos los Timers y el DMA de forma sincronizada para que el auto empiece a operar
    comunicacionUART("[AUTO FUNCIONANDO]: Iniciando motores y sensores...\r\n");

	// Habilitar físicamente los temporizadores para que empiecen a correr
    TIM_Enable(LPC_TIM0);           // Arranca el muestreo del ADC cada 100ms
    TIM_Enable(LPC_TIM1);           // Arranca el generador de PWM para las ruedas
    GPDMA_ChannelStart(GPDMA_CH_1); // Habilita el canal del DMA para escuchar al ADC

    while(1){
            if(detenerAuto == 1) {
                frenarMotores();
                while(detenerAuto == 1) {
                    __WFI(); // Quedarse esperando un cambio por UART de forma segura
                }
            }

            // CONTROL DE TRAYECTORIA (Procesamiento en primer plano)
            if (promedio_distancia > LIMITE_OBSTACULO) {
            	comunicacionUART("¡Obstaculo detectado! Girando...\r\n");
                esquivarObstaculo();
            } else {
            	comunicacionUART("Camino libre. Avanzando en linea recta.\r\n");
                avanzarLineaRecta();
            }
        }
        return 0;
    }

