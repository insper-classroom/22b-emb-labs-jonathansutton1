#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"


#define TRIGGER_PIO      PIOD
#define TRIGGER_PIO_ID   ID_PIOD
#define TRIGGER_IDX      30
#define TRIGGER_IDX_MASK (1 << TRIGGER_IDX)

#define ECHO_PIO      PIOA
#define ECHO_PIO_ID   ID_PIOA
#define ECHO_IDX      6
#define ECHO_IDX_MASK (1 << ECHO_IDX)

#define BUT1_PIO           PIOD
#define BUT1_PIO_ID        ID_PIOD
#define BUT1_PIO_IDX       28
#define BUT1_PIO_IDX_MASK (1u << BUT1_PIO_IDX)

#define LED_PIO           PIOC
#define LED_PIO_ID        ID_PIOC
#define LED_PIO_IDX       8
#define LED_PIO_IDX_MASK  (1 << LED_PIO_IDX)


volatile char echo_flag = 0;
volatile double tempo_min = 0.000058;
volatile double freq = (double) 1/(tempo_min*2); 
volatile int tempo = 0;
volatile char but1_flag = 0;
volatile char display_flag; = 0;


void io_init(void);
static void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource);
void but1_callback(void);
void echo_callback(void);

void echo_callback(void){
	if(!echo_flag){
		RTT_init(freq, 0, 0);
		echo_flag = 1;
	}
	else{
		tempo = rtt_read_timer_value(RTT);
		echo_flag = 0;
		display_flag = 1;
	}
}

void RTT_Handler(void) {
	uint32_t ul_status;

	ul_status = rtt_get_status(RTT);

}


static void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource) {

	uint16_t pllPreScale = (int) (((float) 32768) / freqPrescale);
	
	rtt_sel_source(RTT, false);
	rtt_init(RTT, pllPreScale);
	
	if (rttIRQSource & RTT_MR_ALMIEN) {
		uint32_t ul_previous_time;
		ul_previous_time = rtt_read_timer_value(RTT);
		while (ul_previous_time == rtt_read_timer_value(RTT));
		rtt_write_alarm_time(RTT, IrqNPulses+ul_previous_time);
	}

	/* config NVIC */
	NVIC_DisableIRQ(RTT_IRQn);
	NVIC_ClearPendingIRQ(RTT_IRQn);
	NVIC_SetPriority(RTT_IRQn, 4);
	NVIC_EnableIRQ(RTT_IRQn);

	/* Enable RTT interrupt */
	if (rttIRQSource & (RTT_MR_RTTINCIEN | RTT_MR_ALMIEN))
	rtt_enable_interrupt(RTT, rttIRQSource);
	else
	rtt_disable_interrupt(RTT, RTT_MR_RTTINCIEN | RTT_MR_ALMIEN);
	
}


void io_init(void)
{
	pmc_enable_periph_clk(TRIGGER_PIO_ID);
	pio_configure(TRIGGER_PIO, PIO_OUTPUT_0,TRIGGER_IDX_MASK, PIO_DEFAULT);

	pmc_enable_periph_clk(ECHO_PIO_ID);
	pio_set_input(ECHO_PIO, ECHO_IDX_MASK, PIO_DEFAULT);
	
	pmc_enable_periph_clk(BUT1_PIO_ID);
	pio_configure(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK, PIO_PULLUP|PIO_DEBOUNCE);
	pio_set_debounce_filter(ECHO_PIO, BUT1_PIO_IDX_MASK, 60);
	

	pio_handler_set(ECHO_PIO,
	ECHO_PIO_ID,
	ECHO_IDX_MASK,
	PIO_IT_EDGE,
	echo_callback);
	
	pio_handler_set(BUT1_PIO,
	BUT1_PIO_ID,
	BUT1_PIO_IDX_MASK,
	PIO_IT_EDGE,
	but1_callback);

	pio_enable_interrupt(BUT1_PIO, BUT1_PIO_IDX_MASK);
	pio_get_interrupt_status(BUT1_PIO);
	
	pio_enable_interrupt(ECHO_PIO, ECHO_IDX_MASK);
	pio_get_interrupt_status(ECHO_PIO);
	
	NVIC_EnableIRQ(BUT1_PIO_ID);
	NVIC_SetPriority(BUT1_PIO_ID, 2);
	
	NVIC_EnableIRQ(ECHO_PIO_ID);
	NVIC_SetPriority(ECHO_PIO_ID, 1);

void but1(void){
	pio_set(TRIGGER_PIO, TRIGGER_IDX_MASK);
	delay_us(15);
	pio_clear(TRIGGER_PIO, TRIGGER_IDX_MASK);
	but1_flag = 0;
}

void display(void){
	char text[128];
	double dist = (((tempo/freq))*320/2.0)*100;
	sprintf(text, "%.1f", dist);
	gfx_mono_draw_string(text, 8,8, &sysfont);
	display_flag = 0;
}

int main (void)
{
	board_init();
	sysclk_init();
	io_init();
	delay_init();
	
	WDT->WDT_MR = WDT_MR_WDDIS;

	gfx_mono_ssd1306_init();
	/* Insert application code here, after the board has been initialized. */
	while(1) {
		if (but1_flag){
			but1();
		}
		if(display_flag){
			display()
		}
}
