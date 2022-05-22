/*
 * keypad.c
 *
 */ 

#include <avr/io.h>
#include "keypad.h"
#define F_CPU 16000000L
#include <util/delay.h>

//pines para las filas, de A a D
const uint8_t row[4]={4,3,0,7};
//pines para las columnas, de 1 a 4
const uint8_t col[4]={3,5,4,2};
//valores ASCII de cada tecla
const char ascii[16]= {	'1','2','3','A',
	'4','5','6','B',
	'7','8','9','C',
	'0','*','#','D' };

/************************************************************************
Esta funcion configura los puertos correspondientes a las filas como
salida, y los pines correspondientes a las columnas como entrada con pull up.

Inicializa las salidas de cada fila como desactivada (1)
***********************************************************************/
void KEYPAD_Init(){
	//inicializo puertos de filas como salida, inicialmente inactivas (salida 1)
	for(int i=0;i<3;i++){
		DDRB|=1<<row[i];
		PORTB|=1<<row[i];
		
	}
	DDRD|=1<<row[3];
	PORTD|=1<<row[3];
	
	//inicializo pines de columnas como entrada con pull up
	for(int i=0;i<4;i++){
		DDRD&=~(1<<col[i]);
		PORTD|=1<<col[i];
	}
}
/****************************************************************
La funcion keypad_scan realiza polling fila por fila en el teclado
matricial, devolviendo 1 si detecta una tecla pulsada o 0 caso contrario.
Este proceso lo llamamos "escaneo".

Argumentos:

*pkey: referencia a la variable donde se almacenara la tecla que se detecte pulsada

Nota: debido al funcionamiento del teclado, solo es posible detectar una tecla
por cada llamado a la funcion. Si hay mas de una tecla presionada
se devuelve la que se encuentra primero teniendo en cuenta
el orden de escaneo (fila por fila, de izquierda a derecha)
****************************************************************/
uint8_t KEYPAD_Scan(uint8_t *pkey) {
	uint8_t i;
	uint8_t j;
	
	for(i=0;i<3;i++){
		//activo la fila i para el polling
		PORTB&=~(1<<row[i]);
		for(j=0;j<4;j++){
			//busco columna activa en bajo
			if (!( PIND & (1 << col[j]) )){
				//desactivo fila i
				PORTB|=1<<row[i];
				*pkey=ascii[i*4 + j];
				return 1;
			}	
		}
		//desactivo la fila i
		PORTB|=1<<row[i];
	}
	//activo la ultima fila
	PORTD&=~(1<<7);
	for(j=0;j<4;j++){
		//busco columna activa en bajo
		if (!( PIND & (1 << col[j]) )){
			PORTD|=1<<7;
			//el primer boton de la fila 4 es el indice 12 del arreglo
			*pkey=ascii[12 + j];
			return 1;
		}
	}
	//desactivo ultima fila
	PORTD|=(1<<7);
	return 0;
}
/************************************************************************/
/* La funcion KEYPAD_update evita el efecto rebote de manera no bloqueante, descartando
pulsaciones dobles consecutivas de la misma tecla*/
/************************************************************************/
uint8_t KEYPAD_Update(uint8_t *pkey)
{
	static uint8_t Old_key;
	uint8_t Key;
	static uint8_t Last_valid_key = 0xFF;
	
	//lee el teclado
	if (!KEYPAD_Scan(&Key))
	{
		//si no se recibe una tecla se resetean los valores y se devuelve 0
		Old_key = 0xFF;
		Last_valid_key = 0xFF;
		return 0;
	}
	
	//Si la ultima tecla leida sigue presionada. Esto permite que se estabilice la se�al del teclado.
	if (Key == Old_key)
	{
		//verifica que no sea una pulsacion consecutiva y la valida
		if (Key != Last_valid_key) {
			*pkey = Key;
			Last_valid_key = Key;
			return 1;
		}
	}
	
	//Se detecta la primera pulsacion de la tecla
	Old_key = Key;
	return 0;
}