/*
 * keypad.h
 *
 */ 


#ifndef KEYPAD_H_
#define KEYPAD_H_

void KEYPAD_Init(void);
uint8_t KEYPAD_Scan(uint8_t*);
uint8_t KEYPAD_Update(uint8_t*);



#endif /* KEYPAD_H_ */