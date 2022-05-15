/*
 * MEF_GENERAL.h
 *
 */ 


#ifndef MEF_GENERAL_H_
#define MEF_GENERAL_H_
typedef enum { DEFAULT, EDITION, EDITION_ENDED, CANCELED } MEF_state;

void MEF_GENERAL_Init(void);
void MEF_GENERAL_Update(void);

#endif /* MEF_GENERAL_H_ */