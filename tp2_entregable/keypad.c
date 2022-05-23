/*
 * keypad.c
 *
 */ 

#include <avr/io.h>
#include "keypad.h"
#define F_CPU 16000000UL
#include <util/delay.h>
/**
 * @brief Mapeo de las teclas del keypad a los valores de la estructura KEYPAD_KEY.
 */
static const uint8_t keys[4][4] = {
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'},
};

/**
 * @brief Mapeo de los pines del keypad a los pines del microcontrolador.
 * Se configuran B4, B3, B0 y D7 como filas.
 */
static const uint8_t pin_rows[4] = {4, 3, 0, 7};

/**
 * @brief Mapeo de los pines del keypad a los pines del microcontrolador.
 * Se configuran D3, D5, D4 y D2 como columnas
 */
static const uint8_t pin_columns[4] = {3, 5, 4, 2};

void KEYPAD_Restore() {
	for (uint8_t i = 0; i < 3; i++) {
		PORTB |= (1 << pin_rows[i]);
		PORTD |= (1 << pin_columns[i]);
	}
	PORTD |= 1 << pin_rows[3];
	PORTD |= 1 << pin_columns[3];
}

/**
 * @brief Método que inicializa el keypad.
 * Se configuran los pines conectados a las filas como entrada y las columnas como salida.
 */
void KEYPAD_Init(void) {
	for (uint8_t i = 0; i < 4; i++) {
		if (i < 3) {
			// Se configuran B4, B3, B0 como entradas
			DDRB &= ~(1 << pin_rows[i]);
			// Se habilita la resistencia de pull-up
			PORTB |= (1 << pin_rows[i]);
		}
		// Se configuran D3, D5, D4 y D2 como salidas
		DDRD |= (1 << pin_columns[i]);
		// Se habilita la resistencia de pull-up
		PORTD |= (1 << pin_columns[i]);
	}
	// Se configura D7 como entrada
	DDRD &= ~(1<<pin_rows[3]);
	// Se habilita la resistencia de pull-up
	PORTD |= (1<<pin_rows[3]);
}

/**
 * @brief Método que hace una lectura de los pines del keypad para determinar si se ha pulsado una tecla.
 * 
 * @param pressed_key - Puntero a una variable de tipo uint8_t que almacena el valor de la tecla pulsada.
 * @return uint8_t - Devuelve 1 si se ha pulsado una tecla, 0 en caso contrario.
 */
uint8_t KEYPAD_Scan(uint8_t *pressed_key) 
{
	KEYPAD_Restore();	

	uint8_t i, j;
	// Por comodidad, se hace una lectura por columnas y luego por filas
	for (i = 0; i < 4; i++) {
		// Habilitamos la columna i
		PORTD &= ~(1 << pin_columns[i]);
		// Se recorren las filas de dicha columna
		for (j = 0; j < 4; j++) {
			if (j < 3) {
				// Las primeras 3 filas están configuradas en el puerto B
				if (!(PINB & (1 << pin_rows[j]))) {
					*pressed_key = keys[j][i];
					KEYPAD_Restore();
					return 1;
				}
			} else {
				// La ultima fila está configurada en el puerto D
				if (!(PIND & (1 << pin_rows[j]))) {
					*pressed_key = keys[j][i];
					KEYPAD_Restore();
					return 1;
				}
			}
		}
		KEYPAD_Restore();
	}
	return 0;
}

/**
 * @brief Actualiza el estado del keypad. Se gestiona el efecto de rebote.
 * 
 * @param pressed_key - Puntero a una variable de tipo uint8_t donde se guardará el valor de la tecla presionada.
 * @return uint8_t - Devuelve 1 si se presionó una tecla, 0 en caso contrario.
 */
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