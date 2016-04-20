#include <xc.h>

#define _XTAL_FREQ 8000000

void Inicializaciones(void);
void Trigger(void);
void CalcularDistancia( int*,  int*,  int*,  int*, char*);
void ResetEcho();
void IntPortb( int*,  int*,  int*,  int*, char*);
void ShiftArrays( int*,  int*,  int*,  int*);
void MediaMedidas( int*);
void I2C_Slave_Init(short);
void IntI2C(char*);
//void PrintDistancias( int*,  int* ,  int* ,  int* );