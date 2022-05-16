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

static unsigned char cont_hora = 0;
static unsigned char cont_mef = -1; // Arranca en el T = 50ms (1 tick m�s que la hora)

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
	if (++cont_mef == 2) { // Cada 100 ms
		flag_mef = 1; 
		cont_mef = 0;
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
}


ISR (TIMER1_COMPA_vect)
{
	SEOS_Schedule_Tasks();
}