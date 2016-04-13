/*
 * File:   main.c
 * Author: Fu
 *
 * Created on 29 de febrero de 2016, 12:22
 */

#include <xc.h>
#include "Funciones.h"
//#include "LCD.h"
#include "Uart.h"

#define _XTAL_FREQ 8000000

// CONFIG1
#pragma config FOSC = INTRC_CLKOUT  // Oscillator Selection bits (INTOSC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF                   // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF                 // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON                  // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF                       // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF                    // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON                // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = ON                    // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON               // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF                    // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR21V       // Brown-out Reset Selection bit (Brown-out Reset set to 2.1V)
#pragma config WRT = OFF                // Flash Program Memory Self Write Enable bits (Write protection off)

//Variables Globales

unsigned int t4 = 0;
unsigned int t5 = 0;
unsigned int t6 = 0;
unsigned int t7 = 0;
int rbon = 0;

char dist1[8];
char* distancia1 = dist1;
char dist2[8];
char* distancia2 = dist2;
char dist3[8];
char* distancia3 = dist3;
char dist4[8];
char* distancia4 = dist4;

//Programa
void main(void) {

    Inicializaciones();                     //Se configuran y inicializan los puertos, timer, interrupciones y LCD
    UART_Init(9600);
    
    while(1)
    {
        /*Es necesario que las lecturas de los sensores sean de manera secuencial, ya que si no puede haber
         errores debido a la solapación de las señales de ECHO.
         Los trigger se envian de manera secuencial, con un margen de 40ms entre cada pulso, ya que según el datasheet,
         la longitud máxima del pulso es de cerca de 38ms para cuando no detecta ningún obstáculo.
         Después de cada lectura, se calcula el valor de dicho pulso, y una vez se han leído los 4 sensores, se sacan por el LCD
         */
        Trigger();
        CalcularDistancia(t4, t5, t6, t7, &distancia1, &distancia2, &distancia3, &distancia4);
        PrintDistancias(dist1, dist2, dist3, dist4);        //Saca las distancias por el puerto serie y el LCD ~20ms
        ResetEcho();                      //Se pone el pin de echo como salida, y se pone a 0, a veces se queda atascado
    }
    return;
}

//Vector de Servicio de Interrupción
void interrupt echo()
{
    if(RBIF == 1)                           //Comprobar flag del puerto B
    {
        RBIE = 0;                           //Desactiva el bit de interrupcion puerto B
        /* Cuando detecta la subida de alguno de los pulsos, pone su respectiva señal de "On" a 1, de esta forma,
         cuando dicha señal baja, se puede identificar cúal ha sido, y evitar posibles problemas de lectura.
         Como el trigger se manda a la vez para todos los ultrasonidos, no se sabe cuándo va a volver cada respectivo echo
         por lo que se hace de esta forma para que con un sólo timer, se puedan contar todas las distancias.
         No funcionaría en el caso de que varios ECHO volviesen justo en el  mismo instante, ya que sólo detectaría  uno*/
        if ( (RB4 == 1) && (rbon == 0) )
        {
            TMR1 = 0;
            rbon = 4;
            TMR1ON = 1;
        }
        
        else if( (RB4 == 0) && (rbon == 4) )
        {
            t4 = TMR1;
            rbon = 0;
            TMR1ON = 0;
        }
        
        else if ( (RB5 == 1) && (rbon == 0) )
        {
            TMR1 = 0;
            rbon = 5;
            TMR1ON = 1;
        }
        
        else if( (RB5 == 0) && (rbon == 5) )
        {
            t5 = TMR1;
            rbon = 0;
            TMR1ON = 0;
        }
   
        else if ( (RB6 == 1) && (rbon == 0) )
        {
            TMR1 = 0;
            rbon = 6;
            TMR1ON = 1;
        }
        
        else if( (RB6 == 0) && (rbon == 6) )
        {
            t6 = TMR1;
            rbon = 0;
            TMR1ON = 0;
        }
        
        else if ( (RB7 == 1) && (rbon == 0) )
        {
            TMR1 = 0;
            rbon = 7;
            TMR1ON = 1;
        }
        
        else if( (RB7 == 0) && (rbon == 7) )
        {
            t7 = TMR1;
            rbon = 0;
            TMR1ON = 0;
        }
        
        RBIF = 0;                           //Limpia la bandera
        RBIE = 1;                           //Vuelve a activar la interrupcion
    }
    
}