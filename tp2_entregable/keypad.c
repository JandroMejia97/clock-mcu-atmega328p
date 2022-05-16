/*
 * keypad.c
 *
 */ 

#include <avr/io.h>
#include "keypad.h"
#define F_CPU 16000000UL
#include <util/delay.h>

void KEYPAD_Init(void) {
	 DDRB = 0x0F;   // D7..D4 como entrada, D3..D0 como salida.
	 PORTB = 0x0F;	// Habilito pull-ups internos de las entradas
}


const uint8_t keys[4][4] = {
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'},
};


uint8_t KEYPAD_Scan(uint8_t *pressed_key) 
{
	uint8_t mask[] = {0b11111110, 0b11111101, 0b11111011, 0b11110111};
	PORTB = 0xF0;
	uint8_t i, j;
	for (i = 0; i<4; i++) {	//se barren las filas
		PORTB |= mask[i];
		for (j = 0; j<4; j++) {	//se barren las columnas
			if ((~PINB) & (1 << (j+4))) {
				*pressed_key = keys[i][j];
				PORTB |= 0xF0;
				return 1;
			}
		}
		PORTB = 0xF0;
	}
	return 0;
}

/************************************************************************/
/* Retorna 1 si una tecla fue presionada y la devuelve por parámetro    */
/* evitando detección multiple y efecto rebote.						    */
/************************************************************************/
uint8_t KEYPAD_Update(uint8_t *pressed_key)
{
	static uint8_t old_key;
	uint8_t key;
	static uint8_t last_valid_key = 0xFF;

	if (!KEYPAD_Scan(&key)) {
		old_key = 0xFF;
		last_valid_key = 0xFF;
		return 0;
	}
	if (key == old_key) {
		if (key != last_valid_key) {
			*pressed_key = key;
			last_valid_key = key;
			return 1;
		}
	}	
	old_key = key;
	return 0;
}
