/*
 * keypad.c
 *
 */ 

#include <avr/io.h>
#include "keypad.h"
#define F_CPU 16000000UL
#include <util/delay.h>

static const uint8_t keys[4][4] = {
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'},
};

// B4, B3, B0 y D7 como filas
static const uint8_t pin_rows[4] = {4, 3, 0, 7};

// D3, D5, D4 y D2 como columnas
static const uint8_t pin_columns[4] = {3, 5, 4, 2};

void KEYPAD_Setup() {
	for (uint8_t i = 0; i < 3; i++) {
		PORTB |= (1 << pin_rows[i]);
		PORTD |= (1 << pin_columns[i]);
	}
	PORTD |= 1 << pin_rows[3];
	PORTD |= 1 << pin_columns[3];
}

void KEYPAD_Init(void) {
	for (uint8_t i = 0; i < 4; i++) {
		if (i < 3) {
			DDRB &= ~(1 << pin_rows[i]);
			PORTB |= (1 << pin_rows[i]);
		}
		DDRD |= (1 << pin_columns[i]);
		PORTD |= (1 << pin_columns[i]);
	}
	DDRD &= ~(1<<pin_rows[3]);
	PORTD |= (1<<pin_rows[3]);
}

uint8_t KEYPAD_Scan(uint8_t *pressed_key) 
{
	KEYPAD_Setup();	

	uint8_t i, j;
	for (i = 0; i < 4; i++) {	// se barren las columnas
		PORTD &= ~(1 << pin_columns[i]);
		for (j = 0; j < 4; j++) {	//se barren las filas
			if (j < 3) {
				if (!(PINB & (1 << pin_rows[j]))) {
					*pressed_key = keys[j][i];
					KEYPAD_Setup();
					return 1;
				}
			} else {
				if (!(PIND & (1 << pin_rows[j]))) {
					*pressed_key = keys[j][i];
					KEYPAD_Setup();
					return 1;
				}
			}
		}
		KEYPAD_Setup();
	}
	return 0;
}

/************************************************************************/
/* Retorna 1 si una tecla fue presionada y la devuelve por par�metro    */
/* evitando detecci�n multiple y efecto rebote.						    */
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