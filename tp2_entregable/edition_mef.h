#ifndef EDITION_MEF_H_
#define EDITION_MEF_H_

#include <stdlib.h>
#include "lcd.h"
#include "clock.h"

typedef enum {
  EDIT_YEAR,
  EDIT_MONTH,
  EDIT_DAY,
  EDIT_HOUR,
  EDIT_MINUTE,
  EDIT_SECOND,
  EDIT_DONE,
  EDIT_CANCELED
} edition_mef_state;

void edition_mef_init(void);
void edition_mef_update(TIME);
void edit_data(uint8_t min, uint8_t max, uint8_t pressed_key, uint8_t *data);
void print_data(uint8_t x, uint8_t y, uint8_t *data);

#endif /* EDITION_MEF_H_ */
