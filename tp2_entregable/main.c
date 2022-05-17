/*
 * main.c
 *
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "keypad.h"
#include "MEF_GENERAL.h"
#include "sEOS.h"
#include "clock.h"


int main(void)
{
	// inicializar teclado
	KEYPAD_Init();
	// Inicializar LCD
	LCDinit();
	// inicializar clock en 23:59:59
	CLOCK_init();
	// inicializar maq de estados
	MEF_Init();
	// habilito timers e interrupciones
	SEOS_Init();
    while (1) 
    {
		SEOS_Dispatch_Tasks();
    }

	return 0;

}

