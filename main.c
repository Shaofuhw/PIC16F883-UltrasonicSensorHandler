/*
 * File:   main.c
 * Author: Fu
 *
 * Created on 29 de febrero de 2016, 12:22
 */

#include <xc.h>
#include "Funciones.h"
#include "LCD.h"
#include <stdio.h>

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
int dist = 0;


//Programa
void main(void) {
    
    char distancia[10];
    
    Inicializaciones();                     //Se configuran y inicializan los puertos, timer y interrupciones
    //LCD
    lcd_init(0, 16, 2);                     //Inicializa el LCD
    lcd_clear();                            //Lo limpia
    lcd_on();                               //Lo enciende
    lcd_goto(0, 0);                       //Coloca el cursor en la primera posición de la primera fila
    lcd_puts("Distancia: ");
    while(1)
    {
        Trigger();                              //Activa el Trigger, y lo apaga trás un retardo
        __delay_ms(60);                    //Retardo de 60ms entre lecturas, tiempo recomendado por fabricante
        lcd_goto(0, 1);                     //Coloca el cursor en la primera posición de la segunda fila
        sprintf(distancia,"%d",dist);
        lcd_puts(distancia);
    }
    return;
}

//Vector de Servicio de Interrupción
void interrupt echo()
{
    if(RBIF == 1)                           //Comprobar flag del puerto B
    {
        RBIE = 0;                           //Desactiva el bit de interrupcion puerto B
        if(RB4 == 1)                        //Echo en alto
        {
            TMR1 = 0;
            TMR1ON = 1;                 //Activar el Timer1
        }
        else                                    //Echo en bajo
        {
            TMR1ON = 0;                 //Para el Timer1
            /*El reloj es de 8MHz, cada ciclo de reloj es de 0.5us, pero como el prescaler es de 2
             cada valor del Timer1 equivale a 1us, que según el datasheet, es la medida de tiempo que se necesita,
             al cual si se divide entre 58, nos da la distancia en centímetros*/
            dist = TMR1/58;
        }
        RBIF = 0;                           //Limpia la bandera
        RBIE = 1;                           //Vuelve a activar la interrupcion
    }
}