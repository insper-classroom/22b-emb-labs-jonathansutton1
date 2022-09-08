#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

//DEFINES
#define LED_PIO      PIOC
#define LED_PIO_ID   ID_PIOC
#define LED_IDX      8
#define LED_IDX_MASK (1 << LED_IDX)

#define BUT1_PIO           PIOD
#define BUT1_PIO_ID        ID_PIOD
#define BUT1_PIO_IDX       28
#define BUT1_PIO_IDX_MASK (1u << BUT1_PIO_IDX)

#define BUT2_PIO          PIOC
#define BUT2_PIO_ID       ID_PIOC
#define BUT2_PIO_IDX      31
#define BUT2_PIO_IDX_MASK (1u << BUT2_PIO_IDX) // esse já está pronto.

#define BUT3_PIO          PIOA
#define BUT3_PIO_ID       ID_PIOA
#define BUT3_PIO_IDX      19
#define BUT3_PIO_IDX_MASK (1u << BUT3_PIO_IDX) // esse já está pronto.

volatile char but1_flag;
volatile char but2_flag;
volatile char but3_flag;

//FUNCOES
void but1_callback(void){
	if(!pio_get(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK)){  
		but1_flag = 1;
	}else{
		but1_flag = 0;
	}
}

void but2_callback(void){
	if(!pio_get(BUT2_PIO, PIO_INPUT, BUT2_PIO_IDX_MASK)){
		but2_flag = 1;
	}else{
		but2_flag = 0;
	}
}

void but3_callback(void){
	if(!pio_get(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK)){
		but3_flag = 1;
	}else{
		but3_flag = 0;
	}
}

// pisca led N vez no periodo T
void pisca_led(int n, int t){
	for (int i=0;i<n;i++){
		pio_clear(LED_PIO, LED_IDX_MASK);
		delay_ms(t);
		pio_set(LED_PIO, LED_IDX_MASK);
		delay_ms(t);
	}
}

void init(void){
	// Initialize the board clock
	sysclk_init();

	// Desativa WatchDog Timer
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	// Ativa o PIO na qual o LED foi conectado
	// para que possamos controlar o LED.
	pmc_enable_periph_clk(LED_PIO_ID);
	pmc_enable_periph_clk(BUT1_PIO_ID);
	pmc_enable_periph_clk(BUT2_PIO_ID);
	pmc_enable_periph_clk(BUT3_PIO_ID);
	
	pio_set_input(BUT1_PIO, BUT1_PIO_IDX_MASK, PIO_DEFAULT);
	pio_set_input(BUT2_PIO,BUT2_PIO_IDX_MASK,PIO_DEFAULT);
	pio_set_input(BUT3_PIO,BUT3_PIO_IDX_MASK,PIO_DEFAULT);

	pio_pull_up(BUT1_PIO, BUT1_PIO_IDX_MASK, 1);
	pio_pull_up(BUT2_PIO, BUT2_PIO_IDX_MASK, 1);
	pio_pull_up(BUT3_PIO, BUT3_PIO_IDX_MASK, 1);

	
	pio_set_output(LED_PIO, LED_IDX_MASK, 0, 0, 0);

	
	// Configura interrupção no pino referente ao botao e associa
	// função de callback caso uma interrupção for gerada
	// a função de callback é a: but1_callback()
	pio_handler_set(BUT1_PIO,
	BUT1_PIO_ID,
	BUT1_PIO_IDX_MASK,
	PIO_IT_EDGE,
	but1_callback);
	
	pio_handler_set(BUT2_PIO,
	BUT2_PIO_ID,
	BUT2_PIO_IDX_MASK,
	PIO_IT_FALL_EDGE,
	but2_callback);
	
	pio_handler_set(BUT3_PIO,
	BUT3_PIO_ID,
	BUT3_PIO_IDX_MASK,
	PIO_IT_FALL_EDGE,
	but3_callback);

	// Ativa interrupção e limpa primeira IRQ gerada na ativacao
	pio_enable_interrupt(BUT1_PIO, BUT1_PIO_IDX_MASK);
	pio_enable_interrupt(BUT2_PIO, BUT2_PIO_IDX_MASK);
	pio_enable_interrupt(BUT3_PIO, BUT3_PIO_IDX_MASK);
	
	pio_get_interrupt_status(BUT1_PIO);
	pio_get_interrupt_status(BUT2_PIO);
	pio_get_interrupt_status(BUT3_PIO);

	
	// Configura NVIC para receber interrupcoes do PIO do botao
	// com prioridade 4 (quanto mais próximo de 0 maior)
	NVIC_EnableIRQ(BUT1_PIO_ID);
	NVIC_EnableIRQ(BUT2_PIO_ID);
	NVIC_EnableIRQ(BUT3_PIO_ID);
	NVIC_SetPriority(BUT1_PIO_ID, 4); // Prioridade 4
	NVIC_SetPriority(BUT2_PIO_ID, 4); // Prioridade 4
	NVIC_SetPriority(BUT3_PIO_ID, 4); // Prioridade 4
}


int main (void){
	board_init();
	sysclk_init();
	delay_init();
	init();

  // Init OLED
	gfx_mono_ssd1306_init();
	int j = 30;
  
	//gfx_mono_draw_filled_circle(20, 16, 16, GFX_PIXEL_SET, GFX_WHOLE);
    //gfx_mono_draw_string("mundo", 50,16, &sysfont);
	char str[128];
	int freq = 200;
	sprintf(str, "%d", freq); //
	gfx_mono_draw_string(str, 0, 0, &sysfont);
 
  /* Insert application code here, after the board has been initialized. */
	while(1) {
		if(but1_flag){
			delay_ms(1000);
			if(but1_flag){    //usuario mantem o botao apertado
				freq += 100;
				sprintf(str, "%d", freq); //
				gfx_mono_draw_string(str, 0, 0, &sysfont);
				j = 30;
				while(!but2_flag && j >=0){
					pisca_led(15,freq);
					j -= 1;
				}
			}
			else{
				freq -= 100;
				sprintf(str, "%d", freq); //
				gfx_mono_draw_string(str, 0, 0, &sysfont);
				j = 30;
				while(!but2_flag && j >=0){
					pisca_led(15,freq);
					j -= 1;
				}
			}
		}
		//else if(but2_flag){
			//pio_set(LED_PIO, LED_IDX_MASK);
		//}
		else if(but3_flag){
			freq += 100;
			sprintf(str, "%d", freq); //
			gfx_mono_draw_string(str, 0, 0, &sysfont);
			j = 30;
			while(!but2_flag && j >=0){
				pisca_led(15,freq);
				j -= 1;
			}
		}
		pmc_sleep(SAM_PM_SMODE_SLEEP_WFI); //utilizar somente o modo sleep mode
	}
}
