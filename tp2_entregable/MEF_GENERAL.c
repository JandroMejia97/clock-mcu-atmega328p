#include "MEF_GENERAL.h"

static MEF_STATE state;
static TIME time;

void MEF_Init() {
  state = DEFAULT;
}

void MEF_Update() {
  uint8_t pressed_key;
  KEYPAD_Scan(&pressed_key);
  uint8_t min_value = 0, max_value = 99, x = 10, y = 1;
  uint8_t *value_to_edit = NULL;
  MEF_STATE next_state = state;

  switch (state) {
    case DEFAULT:
      break;
    case EDIT_YEAR:
      value_to_edit = &time.years;
      next_state = EDIT_MONTH;
      break;
    case EDIT_MONTH:
      min_value = 1;
      max_value = 12;
      value_to_edit = &time.months;
      x = 7;
      next_state = EDIT_DAY;
      break;
    case EDIT_DAY:
      min_value = 1;
      max_value = max_days_for_each_month[time.months - 1];
      value_to_edit = &time.days;
      x = 4;
      next_state = EDIT_HOUR;
      break;
    case EDIT_HOUR:
      min_value = 0;
      max_value = 23;
      value_to_edit = &time.hours;
      y = 0;
      next_state = EDIT_MINUTE;
      break;
    case EDIT_MINUTE:
      max_value = 59;
      value_to_edit = &time.minutes;
      x = 7;
      y = 0;
      next_state = EDIT_SECOND;
      break;
    case EDIT_SECOND:
      max_value = 59;
      value_to_edit = &time.seconds;
      x = 4;
      y = 0;
      next_state = EDIT_DONE;
      break;
    case EDIT_DONE:
      CLOCK_setTime(time);
      state = DEFAULT;
      break;
    case EDIT_CANCELED:
      CLOCK_init();
      break;
  }

  switch (pressed_key) {
    case 'B':
    case 'C':
      edit_data(min_value, max_value, pressed_key, value_to_edit);
      print_data(x, y, value_to_edit);
      break;
    case 'A':
      state = next_state;
      break;
    case 'D':
      state = EDIT_CANCELED;
      break;
  }
}

void edit_data(uint8_t min, uint8_t max, uint8_t pressed_key, uint8_t *data) {
  switch (pressed_key) {
    case 'B':
      *data = (*data >= max) ? min : *data + 1;
      break;
    case 'C':
      *data = (*data == min) ? max : *data - 1;
      break;
    }
}

void print_data(uint8_t x, uint8_t y, uint8_t *data) {
  char buffer[2];
  LCDGotoXY(x, y);
  LCDstring((uint8_t *)itoa(*data, buffer, 10), 2);
}