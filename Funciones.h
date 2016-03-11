#include <xc.h>
#include <stdio.h>

#define _XTAL_FREQ 8000000

void Inicializaciones(void);
void Trigger(void);
void CalcularDistancia(unsigned int,unsigned int,unsigned int,unsigned int, char**, char**, char**, char**);
void PrintDistancias(char [],char [],char [],char []);
void ResetEcho();