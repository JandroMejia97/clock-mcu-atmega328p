/*
 * keypad.c
 *
 */ 

#include <avr/io.h>
#include "keypad.h"
#define F_CPU 16000000UL
#include <util/delay.h>

const uint8_t port_d_and_mask = 0b01111111;
const uint8_t port_d_or_mask = 0b00111100;
const uint8_t port_b_and_mask = 0b11100110;

void KEYPAD_Init(void) {
	 // D5..D2 como entrada, D7 como salida. Se mantiene la configuración en el resto de pines
	 DDRD = (DDRD & port_d_and_mask) | port_d_or_mask;
	 // B4, B3 y B0 como entrada. Se mantiene la configuración en el resto de pines
	 DDRB = DDRB & port_b_and_mask;
	 KEYPAD_Setup();
}

const uint8_t keys[4][4] = {
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'},
};


uint8_t KEYPAD_Scan(uint8_t *pressed_key) 
{
	uint8_t rows_masks[] = {
		0b00010000, // Fila 1 (B4) como entrada
		0b00001000, // Fila 2 (B3) como entrada
		0b00000001, // Fila 3 (B0) como entrada
		0b10000000, // Fila 4 (D7) como entrada
	};

	uint8_t columns_masks[] = {
		0b11110111, // Columna 1 (D3) como entrada
		0b11011111, // Columna 2 (D5) como entrada
		0b11101111, // Columna 3 (D4) como entrada
		0b11111011 // Columna 4 (D2) como entrada
	}; 

	KEYPAD_Setup();	

	uint8_t i, j;
	for (i = 0; i < 4; i++) {	// se barren las columnas
		PORTD = PORTD & columns_masks[i];
		for (j = 0; j < 4; j++) {	//se barren las filas
			if (j < 3) {
				if ((~PINB) & rows_masks[j]) {
					*pressed_key = keys[j][i];
					KEYPAD_Setup();
					return 1;
				}
			} else {
				if ((~PIND) & rows_masks[j]) {
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

void KEYPAD_Setup() {
	PORTD = (PORTD & port_d_and_mask) | port_d_or_mask; // Pongo en 0 el bit D7 y 1 los bits D5..D2 
	PORTB = PORTB & port_b_and_mask; // Pongo en 0 los bits B4, B3 y B0
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
