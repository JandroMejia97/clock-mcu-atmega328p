/*
 * clock.h
 *
 */ 

#include <avr/io.h>

#ifndef CLOCK_H_
#define CLOCK_H_

typedef struct TIME {
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
	uint8_t years;
	uint8_t months;
	uint8_t days;
} TIME;

TIME CLOCK_getTime(void);
void CLOCK_init(void);
void CLOCK_setTime(TIME);
void CLOCK_updateTime(void);



#endif /* CLOCK_H_ */