/*
 * sEOS.c
 *
 */ 
#include <avr/io.h>
#include "clock.h"
#include "MEF_GENERAL.h"
#include <avr/interrupt.h>
#define F_CPU 16000000UL


volatile unsigned char flag_hora = 0;
volatile unsigned char flag_mef = 0;
volatile unsigned char flag_blink = 0;

static unsigned char cont_hora = 0; // Contador actualizar el clock
static unsigned char cont_mef = -1; // Arranca en el T = 50ms (1 tick mas que la hora para config)
static unsigned char cont_blink = 0; // Contador para el parpadeo de pantalla en el modo edición
static unsigned char showSpaces = 0; // Nos sirve para alternar entre el '  ' y los dos dígitos en el modo edición y lograr el efecto parpadeo.

void SEOS_Init() {
	TCCR1B |= (1 << WGM12);				// Modo CTC con OCR1A
	TCCR1B |= (1 << CS12);				// Prescaler = F_CPU/256
	OCR1A = 3125;						// seteo el contador en 3125
	TIMSK1 |= (1 << OCIE1A);			// habilito interrupci�n del contador OCR1A
	sei();								// habilito interrupciones globales
}


void SEOS_Schedule_Tasks(void)
{
	static uint8_t flag = 0;
	if (++cont_hora == 20) { // Cada 1 segundo	
		flag_hora = 1;
		cont_hora = 0;
	}
	if (++cont_mef == 1) { // Cada 50 ms
		flag_mef = 1; 
		cont_mef = 0;
	}
	if (++cont_blink == 10) { // Cada 500 ms
		flag_blink = 1;
		cont_blink = 0;
	}
}


void SEOS_Dispatch_Tasks (void) {
	if (flag_hora) {	// Mas prioridad
		CLOCK_updateTime();
		flag_hora = 0;
	}
	if (flag_mef) {
		MEF_Update();
		flag_mef = 0;
	}
	
	if (flag_blink) {
		// Cada medio segundo se va a alternar entre verdadero y falso de la variable showSpaces, por consecuencia, van a parpadear los dos dígitos cada 500ms.
		showSpaces = !showSpaces;
		LCD_Blink(showSpaces);
		flag_blink = 0;
	}
}


ISR (TIMER1_COMPA_vect) // Timer 1
{
	SEOS_Schedule_Tasks();
}