/*
 * File:   main.c
 * Author: Fu
 *
 * Created on 29 de febrero de 2016, 12:22
 */

#include <xc.h>
#include "Funciones.h"
#include "LCD.h"

#define _XTAL_FREQ 8000000

#ifndef CONFIG

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

#endif

//Variables Globales
int t1 = 0;
int t2 = 0;
int t3 = 0;
int t4 = 0;

int SensorOn = 0;

    char dist1[10];
    char* distancia1 = dist1;
    char dist2[10];
    char* distancia2 = dist2;
    char dist3[10];
    char* distancia3 = dist3;
    char dist4[10];
    char* distancia4 = dist4;

//Programa
void main(void) {

    Inicializaciones();                     //Se configuran y inicializan los puertos, timer, interrupciones y LCD
    
    while(1)
    {
        
        /*Es necesario que las lecturas de los sensores sean de manera secuencial, ya que si no puede haber
         errores debido a la solapación de las señales de ECHO.
         Los trigger se envian de manera secuencial, con un margen de 40ms entre cada pulso, ya que según el datasheet,
         la longitud máxima del pulso es de cerca de 38ms para cuando no detecta ningún obstáculo.
         Después de cada lectura, se calcula el valor de dicho pulso, y una vez se han leído los 4 sensores, se sacan por el LCD
         */
        
        Trigger0();                              //Activa los Trigger, y los apaga trás un retardo
        __delay_ms(40);
        //Envía el tiempo del pulso de ECHO, y actualiza el valor de "distancia" en la distancia real en cm
        CalcularDistancia(t1, &distancia1, sizeof(distancia1));           
        
        Trigger1();
        __delay_ms(40);
        CalcularDistancia(t2, &distancia2, sizeof(distancia2));
        
        Trigger2();
        __delay_ms(40);
        CalcularDistancia(t3, &distancia3, sizeof(distancia3));
        
        Trigger3();
        __delay_ms(40);
        CalcularDistancia(t4, &distancia4, sizeof(distancia4));
        
        lcd_goto(0, 1);                     //Coloca el cursor en la primera posición de la segunda fila
        lcd_puts(dist1);
        lcd_puts(dist2);
        lcd_puts(dist3);
        lcd_puts(dist4);
    }
    return;
}

//Vector de Servicio de Interrupción
void interrupt echo()
{
    if(RBIF == 1)                           //Comprobar flag del puerto B
    {
        RBIE = 0;                           //Desactiva el bit de interrupcion puerto B
        
        //Primer Sensor ECHO high
        if( RB4 == 1 )
        {
            SensorOn = 1;
            TMR1 = 0;
            TMR1ON = 1;
        }
        //Primer sensor ECHO low
        else if ( RB4 == 0 && SensorOn == 1)
        {
            SensorOn = 0;
            TMR1ON = 0;
            t1 = TMR1;
        }
        //Segundo sensor ECHO high
        else if( RB5 == 1 )
        {
            SensorOn = 2;
            TMR1 = 0;
            TMR1ON = 1;
        }   
        //Segundo sensor ECHO low
        else if( RB5 == 0 && SensorOn == 2 )
        {
            SensorOn = 0;
            TMR1ON = 0;
            t2 = TMR1;
        }
        //Tercer sensor ECHO high
        else if( RB6 == 1 )
        {
            SensorOn = 3;
            TMR1 = 0;
            TMR1ON = 1;
        }          
        //Tercer sensor ECHO low
        else if( RB6 == 0 && SensorOn == 3 )
        {
            SensorOn = 0;
            TMR1ON = 0;
            t3 = TMR1;
        }
        //Cuarto sensor ECHO high
        else if( RB7 == 1 )
        {
            SensorOn = 4;
            TMR1 = 0;
            TMR1ON = 1;
        }      
        //Cuarto sensor ECHO low
        else if( RB7 == 0 && SensorOn == 4 )
        {
            SensorOn = 0;
            TMR1ON = 0;
            t4 = TMR1;
        }
        
        RBIF = 0;                           //Limpia la bandera
        RBIE = 1;                           //Vuelve a activar la interrupcion
    }
}