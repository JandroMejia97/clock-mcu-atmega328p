#include "MEF_GENERAL.h"

/**
 * @brief Estado de la MED
 */
static MEF_STATE state;
/**
 * @brief Referencia a la estructura de datos del reloj.
 */
static TIME time;
/**
 * @brief Coordenadas de la pantalla
 */
static uint8_t x = 10, y = 1;
/**
 * @brief Puntero al valor del dígito que se está editando.
 */
static uint8_t *value_to_edit = NULL;

/**
 * @brief Método que se llama en el estado por defecto
 * para que se muestre el tiempo actual.
 */
static void defaultAndUpdate() {
	TIME time = CLOCK_getTime();
  value_to_edit = &time.years;
	LCDGotoXY(4,0);
	LCDescribeDato(time.hours, 2);
	LCDsendChar(':');
	LCDescribeDato(time.minutes, 2);
	LCDsendChar(':');
	LCDescribeDato(time.seconds, 2);
	LCDGotoXY(4,1);
	LCDescribeDato(time.days, 2);
	LCDsendChar('/');
	LCDescribeDato(time.months, 2);
	LCDsendChar('/');
	LCDescribeDato(time.years, 2);
}

/**
 * @brief Método que inicializa la MEF.
 * 
 */
void MEF_Init() {
  state = DEFAULT;
  LCDclr();
  LCDhome();
  defaultAndUpdate();
}

/**
 * @brief Método que se llama una vez por segundo para actualizar el estado de la MEF.
 */
void MEF_Update() {
  uint8_t pressed_key = 0xFF;
  KEYPAD_Update(&pressed_key);
  // Rango de los valores a editar
  uint8_t min_value = 0, max_value = 99;
  MEF_STATE next_state = EDIT_YEAR;

  switch (state) {
    case DEFAULT:
      defaultAndUpdate();
      next_state = EDIT_YEAR;
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
      y = 1;
      next_state = EDIT_DAY;
      break;
    case EDIT_DAY:
      min_value = 1;
      max_value = max_days_for_each_month[time.months - 1];
      value_to_edit = &time.days;
      x = 4;
      y = 1;
      next_state = EDIT_SECOND;
      break;
    case EDIT_HOUR:
      min_value = 0;
      max_value = 23;
      value_to_edit = &time.hours;
      x = 4;
      y = 0;
      next_state = EDIT_DONE;
      break;
    case EDIT_MINUTE:
      max_value = 59;
      value_to_edit = &time.minutes;
      x = 7;
      y = 0;
      next_state = EDIT_HOUR;
      break;
    case EDIT_SECOND:
      max_value = 59;
      value_to_edit = &time.seconds;
      x = 10;
      y = 0;
      next_state = EDIT_MINUTE;
      break;
    case EDIT_DONE:
      CLOCK_setTime(time);
      next_state = DEFAULT;
      x = 10;
      y = 1;
      state = DEFAULT;
      break;
    case EDIT_CANCELED:
      state = DEFAULT;
      x = 10;
      y = 1;
      break;
  }
  
  switch (pressed_key) {
    case 'B':
    case 'C':
      if (state != DEFAULT) {
        edit_data(min_value, max_value, pressed_key, value_to_edit);
        print_data(x, y, value_to_edit);
      }
      break;
    case 'A':
      if (state == DEFAULT){
        defaultAndUpdate();
        time = CLOCK_getTime();
      } else {
        LCD_Blink(0);
      }
      state = next_state;
      break;
    case 'D':
      state = EDIT_CANCELED;
      break;
  }
  
  
}

/**
 * @brief Hace que la LCD parpadee cada cierto tiempo
 * 
 * @param showSpaces - Si es true, muestra espacios en vez de datos
 */
void LCD_Blink(uint8_t showSpaces) {
  if (state != DEFAULT) {
	if (showSpaces) {
	  LCDGotoXY(x, y);
	  LCDsendChar(' ');
	  LCDsendChar(' ');
	} else {
	  print_data(x, y, value_to_edit);
	}
  }
}

/**
 * @brief Imprime un dato en la LCD
 * 
 * @param min - Límite inferior del rango
 * @param max - Límite superior del rango
 * @param pressed_key - Tecla presionada
 * @param data - Puntero al dato a modificar
 */
void edit_data(uint8_t min, uint8_t max, uint8_t pressed_key, uint8_t *data) {
  switch (pressed_key) {
    case 'B':
      *data = (*data >= max) ? min : *data + 1;
      break;
    case 'C':
      *data = (*data <= min) ? max : *data - 1;
      break;
    }
}

/**
 * @brief Imprime un dato en la LCD
 * 
 * @param x - Posición en x
 * @param y - Posición en y
 * @param data - Puntero al dato a imprimir
 */
void print_data(uint8_t x, uint8_t y, uint8_t *data) {
  LCDGotoXY(x, y);
  LCDescribeDato(*data, 2);
}