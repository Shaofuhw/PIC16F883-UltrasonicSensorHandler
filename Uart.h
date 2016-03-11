#include <xc.h>
#define _XTAL_FREQ 8000000


char UART_Init(const long int);
char UART_TX_Empty();
char UART_Data_Ready();
char UART_Read();
void UART_Write(char );
void UART_Write_Text(char *);
