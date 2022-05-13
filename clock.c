/*
 * clock.c
 *
 */ 

#include "clock.h"

static TIME time;

void CLOCK_init(void) {
	time.hours = 23;
	time.minutes = 59;
	time.seconds = 59;
}

TIME CLOCK_getTime(void) {
	return time;
}

void CLOCK_setTime(TIME newTime) {
	//Copiamos campo a campo porque la estructura es un puntero.
	time.hours = newTime.hours;
	time.minutes= newTime.minutes;
	time.seconds = newTime.seconds;
}

void CLOCK_updateTime(void) {
	//Se suma 1 segundo y se checkea si hubo "overflow" en segundos, minutos y horas.
	time.seconds += 1;
	
	if (time.seconds == 60) {
		time.seconds = 0;
		time.minutes += 1;
	}
	if (time.minutes == 60) {
		time.minutes = 0;
		time.hours += 1;
	}
	if (time.hours == 24) {
		time.hours = 0;
	}
}
