/*
 * clock.c
 *
 */ 

#include "clock.h"
#include <avr/io.h>

static TIME time;

const uint8_t max_days_for_each_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void CLOCK_init(void) {
	time.hours = 23;
	time.minutes = 59;
	time.seconds = 59;
	time.days = 31;
	time.months = 12;
	time.years = 21;
}

TIME CLOCK_getTime(void) {
	return time;
}

void CLOCK_setTime(TIME newTime) {
	//Copiamos campo a campo porque la estructura es un puntero.
	time.hours = newTime.hours;
	time.minutes= newTime.minutes;
	time.seconds = newTime.seconds;
	time.days = newTime.days;
	time.months = newTime.months;
	time.years = newTime.years;
}

void CLOCK_updateTime(void) {
	//Se suma 1 segundo y se checkea si hubo "overflow" en segundos, minutos y horas, y consecuentemente se aumenta según corresponda el dia, mes y/o año.

	if ((++time.seconds) > 59) {
		time.seconds = 0;
		time.minutes++;
	}

	if (time.minutes > 59) {
		time.minutes = 0;
		time.hours++;
	}

	if (time.hours > 23) {
		time.hours = 0;
		time.days++;
	}

	if (time.days > max_days_for_each_month[time.months - 1]) { //Comparo el mes con mi vector de meses para ver si considero 31, 30 o 28 días.
		time.days = 1;
		time.months++;
	}

	if (time.months > 12) {
		time.months = 1;
		time.years++;
	}

	if (time.years > 99) {
		time.years = 0;
	}
}
