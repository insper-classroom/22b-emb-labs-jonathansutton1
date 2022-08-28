/**
 * 5 semestre - Eng. da Computação - Insper
 * Rafael Corsi - rafael.corsi@insper.edu.br
 *
 * Projeto 0 para a placa SAME70-XPLD
 *
 * Objetivo :
 *  - Introduzir ASF e HAL
 *  - Configuracao de clock
 *  - Configuracao pino In/Out
 *
 * Material :
 *  - Kit: ATMEL SAME70-XPLD - ARM CORTEX M7
 */

/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include "asf.h"

/************************************************************************/
/* defines                                                              */
/************************************************************************/

#define LED_PIO           PIOC                 // periferico que controla o LED
#define LED_PIO_ID        ID_PIOC                  // ID do periférico PIOC (controla LED)
#define LED_PIO_IDX       8                    // ID do LED no PIO
#define LED_PIO_IDX_MASK  (1 << LED_PIO_IDX)   // Mascara para CONTROLARMOS o LED

#define LED1_PIO           PIOA                 // periferico que controla o LED
#define LED1_PIO_ID        ID_PIOA                  // ID do periférico PIOC (controla LED)
#define LED1_PIO_IDX       0                    // ID do LED no PIO
#define LED1_PIO_IDX_MASK  (1 << LED1_PIO_IDX)   // Mascara para CONTROLARMOS o LED

#define LED2_PIO           PIOC                 // periferico que controla o LED
#define LED2_PIO_ID        ID_PIOC                  // ID do periférico PIOC (controla LED)
#define LED2_PIO_IDX       30                    // ID do LED no PIO
#define LED2_PIO_IDX_MASK  (1 << LED2_PIO_IDX)   // Mascara para CONTROLARMOS o LED

#define LED3_PIO           PIOB                 // periferico que controla o LED
#define LED3_PIO_ID        ID_PIOB                  // ID do periférico PIOC (controla LED)
#define LED3_PIO_IDX       2                    // ID do LED no PIO
#define LED3_PIO_IDX_MASK  (1 << LED3_PIO_IDX)   // Mascara para CONTROLARMOS o LED

#define BUT_PIO          PIOA
#define BUT_PIO_ID       ID_PIOA
#define BUT_PIO_IDX      11
#define BUT_PIO_IDX_MASK (1u << BUT_PIO_IDX) // esse já está pronto.

#define BUT1_PIO          PIOD
#define BUT1_PIO_ID       ID_PIOD
#define BUT1_PIO_IDX      28
#define BUT1_PIO_IDX_MASK (1u << BUT1_PIO_IDX) // esse já está pronto.

#define BUT2_PIO          PIOC
#define BUT2_PIO_ID       ID_PIOC
#define BUT2_PIO_IDX      31
#define BUT2_PIO_IDX_MASK (1u << BUT2_PIO_IDX) // esse já está pronto.

#define BUT3_PIO          PIOA
#define BUT3_PIO_ID       ID_PIOA
#define BUT3_PIO_IDX      19
#define BUT3_PIO_IDX_MASK (1u << BUT3_PIO_IDX) // esse já está pronto.

/*  Default pin configuration (no attribute). */
#define _PIO_DEFAULT             (0u << 0)
/*  The internal pin pull-up is active. */
#define _PIO_PULLUP              (1u << 0)
/*  The internal glitch filter is active. */
#define _PIO_DEGLITCH            (1u << 1)
/*  The internal debouncing filter is active. */
#define _PIO_DEBOUNCE            (1u << 3)


/************************************************************************/
/* constants                                                            */
/************************************************************************/

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/

void init(void);

/************************************************************************/
/* interrupcoes                                                         */
/************************************************************************/

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

void _pio_set(Pio *p_pio, const uint32_t ul_mask){
	p_pio->PIO_SODR = ul_mask;
}

void _pio_clear(Pio *p_pio, const uint32_t ul_mask){
	p_pio->PIO_CODR = ul_mask;
}

void _pio_pull_up(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_pull_up_enable){
	if(ul_pull_up_enable){
		p_pio->PIO_PUER = ul_mask;
	}
	else{
		p_pio->PIO_PUDR = ul_mask;
	}
}

void _pio_set_input(Pio *p_pio, const uint32_t ul_mask,const uint32_t ul_attribute){
	_pio_pull_up(p_pio, ul_mask, ul_attribute && _PIO_PULLUP);
	if(ul_attribute && (_PIO_DEBOUNCE | _PIO_DEGLITCH)){
		p_pio -> PIO_IFER = ul_mask;
	}
	else{
		p_pio -> PIO_IFDR = ul_mask;
	}
}

void _pio_set_output(Pio *p_pio, const uint32_t ul_mask,const uint32_t ul_default_level,const uint32_t ul_multidrive_enable,const uint32_t ul_pull_up_enable){
	p_pio -> PIO_OER = ul_mask;
	p_pio -> PIO_PER = ul_mask;
	
	_pio_pull_up(p_pio,ul_mask,ul_pull_up_enable);
	
	if (ul_multidrive_enable) {
		p_pio -> PIO_MDER = ul_mask;
	}
	else{
		p_pio -> PIO_MDDR = ul_mask;
	}

	if (ul_default_level) {
		p_pio->PIO_SODR = ul_mask;
	}
	else{
		p_pio->PIO_CODR = ul_mask;
	}


}

// Função de inicialização do uC
// Função de inicialização do uC

// Função de inicialização do uC
void init(void){
	// Initialize the board clock
	sysclk_init();

	// Disativa WatchDog Timer
	WDT->WDT_MR = WDT_MR_WDDIS;
	

	// Ativa o PIO na qual o LED foi conectado
	// para que possamos controlar o LED.
	pmc_enable_periph_clk(LED_PIO_ID);
	pmc_enable_periph_clk(LED1_PIO_ID);
	pmc_enable_periph_clk(LED2_PIO_ID);
	pmc_enable_periph_clk(LED3_PIO_ID);
	pmc_enable_periph_clk(BUT1_PIO_ID);
	pmc_enable_periph_clk(BUT2_PIO_ID);
	pmc_enable_periph_clk(BUT3_PIO_ID);
	
	
	//INPUTS
    _pio_set_input(BUT_PIO, BUT_PIO_IDX_MASK, _PIO_PULLUP | _PIO_DEBOUNCE);
    _pio_set_input(BUT1_PIO, BUT1_PIO_IDX_MASK, _PIO_PULLUP | _PIO_DEBOUNCE);
    _pio_set_input(BUT2_PIO, BUT2_PIO_IDX_MASK, _PIO_PULLUP | _PIO_DEBOUNCE);
    _pio_set_input(BUT3_PIO, BUT3_PIO_IDX_MASK, _PIO_PULLUP | _PIO_DEBOUNCE);

	
	/*
	_pio_pull_up(BUT_PIO,BUT_PIO_IDX_MASK,1);
	_pio_pull_up(BUT1_PIO,BUT1_PIO_IDX_MASK,1);
	_pio_pull_up(BUT2_PIO,BUT2_PIO_IDX_MASK,1);
	_pio_pull_up(BUT3_PIO,BUT3_PIO_IDX_MASK,1);
    */



	//Inicializa PC8 como saída
	_pio_set_output(LED_PIO, LED_PIO_IDX_MASK, 0, 0, 0);
	_pio_set_output(LED1_PIO, LED1_PIO_IDX_MASK, 0, 0, 0);
	_pio_set_output(LED2_PIO, LED2_PIO_IDX_MASK, 0, 0, 0);
	_pio_set_output(LED3_PIO, LED3_PIO_IDX_MASK, 0, 0, 0);

}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

// Funcao principal chamada na inicalizacao do uC.
int main(void)
{
  init();

  // super loop
  // aplicacoes embarcadas não devem sair do while(1).
  while(1) {
	  int i;
	  int j;
	  int k;
	  int z;
	  // Verifica valor do pino que o botão está conectado
	  if(!pio_get(BUT_PIO, PIO_INPUT, BUT_PIO_IDX_MASK)) {
		  // Pisca LED
		  for (i=0; i<10; i++) {
			  _pio_clear(LED_PIO, LED_PIO_IDX_MASK);  // Limpa o pino LED_PIO_PIN
			  delay_ms(200);                         // delay
			  _pio_set(LED_PIO, LED_PIO_IDX_MASK);    // Ativa o pino LED_PIO_PIN
			  delay_ms(200);                         // delay
		  }
	  }
	  else{
		  // Ativa o pino LED_IDX (par apagar)
		  _pio_set(LED_PIO, LED_PIO_IDX_MASK);
	  }
	  if(!pio_get(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK)) {
		  // Pisca LED
		  for (j=0; j<10; j++) {
			  _pio_clear(LED1_PIO, LED1_PIO_IDX_MASK);  // Limpa o pino LED_PIO_PIN
			  delay_ms(200);                         // delay
			  _pio_set(LED1_PIO, LED1_PIO_IDX_MASK);    // Ativa o pino LED_PIO_PIN
			  delay_ms(200);                         // delay
		  }
	  }
	  else{
		  // Ativa o pino LED_IDX (par apagar)
		  _pio_set(LED1_PIO, LED_PIO_IDX_MASK);
	  }
	  if(!pio_get(BUT2_PIO, PIO_INPUT, BUT2_PIO_IDX_MASK)) {
		  // Pisca LED
		  for (k=0; k<10; k++) {
			  _pio_clear(LED2_PIO, LED2_PIO_IDX_MASK);  // Limpa o pino LED_PIO_PIN
			  delay_ms(200);                         // delay
			  _pio_set(LED2_PIO, LED2_PIO_IDX_MASK);    // Ativa o pino LED_PIO_PIN
			  delay_ms(200);                         // delay
		  }
	  }
	  else{
		  // Ativa o pino LED_IDX (par apagar)
		  _pio_set(LED2_PIO, LED2_PIO_IDX_MASK);
	  }
	  if(!pio_get(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK)) {
		  // Pisca LED
		  for (z=0; z<10; z++) {
			  _pio_clear(LED3_PIO, LED3_PIO_IDX_MASK);  // Limpa o pino LED_PIO_PIN
			  delay_ms(200);                         // delay
			  _pio_set(LED3_PIO, LED3_PIO_IDX_MASK);    // Ativa o pino LED_PIO_PIN
			  delay_ms(200);                         // delay
		  }
	  }
	  else{
		  // Ativa o pino LED_IDX (par apagar)
		  _pio_set(LED3_PIO, LED3_PIO_IDX_MASK);
	  }
  }
  // Nunca devemos chegar aqui !
  return 0;
  }