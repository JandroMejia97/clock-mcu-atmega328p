/*
 * MEF_GENERAL.h
 *
 */ 


#ifndef MEF_GENERAL_H_
#define MEF_GENERAL_H_
#include <stdlib.h>
#include <avr/io.h>
#include "clock.h"
#include "lcd.h"
#include "keypad.h"

typedef enum {
  DEFAULT,
  EDIT_YEAR,
  EDIT_MONTH,
  EDIT_DAY,
  EDIT_HOUR,
  EDIT_MINUTE,
  EDIT_SECOND,
  EDIT_DONE,
  EDIT_CANCELED
} MEF_STATE;

void MEF_Init(void);
void MEF_Update(void);
void LCD_Blink(void);

void edit_data(uint8_t min, uint8_t max, uint8_t pressed_key, uint8_t *data);
void print_data(uint8_t x, uint8_t y, uint8_t *data);
#endif /* MEF_GENERAL_H_ */