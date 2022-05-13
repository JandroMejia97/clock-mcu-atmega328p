/*
 * MEF_GENERAL.h
 *
 */ 


#ifndef MEF_GENERAL_H_
#define MEF_GENERAL_H_

void MEF_GENERAL_Init(void);
void MEF_GENERAL_Update(void);
//void MEF_GENERAL_ResetCounter(void);

typedef enum {DEFAULT, MODIFICAR_DATO, MOD_DIA, MOD_MES, MOD_ANIO, MOD_HORA, MOD_MINUTOS, MOD_SEGUNDOS} MEF_state;


#endif /* MEF_GENERAL_H_ */