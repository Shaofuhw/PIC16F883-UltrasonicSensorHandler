#include <xc.h>

#define _XTAL_FREQ 8000000

void Inicializaciones(void);
void Trigger(void);
void CalcularDistancia(unsigned int, unsigned int, unsigned int, unsigned int, char*);
void ResetEcho();
void IntPortb(unsigned int*, unsigned int*, unsigned int*, unsigned int*, char*);