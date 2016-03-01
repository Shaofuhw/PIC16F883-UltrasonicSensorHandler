/*
 * File:   main.c
 * Author: Fu
 *
 * Created on 29 de febrero de 2016, 12:22
 */

#include <xc.h>
#include<pic16f883.h>

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

//Funciones
void Inicializaciones(void);

//Variables Globales
int dist = 0;

void main(void) {
    Inicializaciones();
    while(1)
    {
        RB0 = 1;                                //Se activa el trigger
        __delay_us(10);
        RB0 = 0;                                //Se apaga el trigger tras retardo
        __delay_ms(60);                    //Retardo de 60ms entre lecturas, tiempo recomendado por fabricante
        if(dist <= 400)
        {
            RA0 = 1;
            if(dist <= 300)
            {
                RA1 = 1;
                if(dist <= 200)
                {
                    RA2 = 1;
                    if(dist <= 100)
                    {
                        RA3 = 1;
                    }
                }
            }
        }
    }
    return;
}

//Se inician todos los registros necesarios para configurar el puerto B, las interrupciones, y el timer1
void Inicializaciones(){
    OSCCON = 0b01110101;            //Se inicializa el oscilador a 8MHz, la máxima.
    
    //Puerto A
    TRISA = 0b11110000;                 //Puerto A del 0 a 3 como salida.
    PORTA = 0b00000000;
    ANSEL = 0b00000000;
    
    //Puerto B
    TRISB = 0b11110000;                 //Puerto B. Pines del 0 al 3 como salida. Resto entrada
    PORTB = 0b00000000;             //Inicializar a 0
    ANSELH = 0b00000000;           //Establecer como puertos Digitales
    
    /*Timer 1 Como se requiere contar hasta tiempo de hasta aproximadamente 40ms se coge el timer de 16bits
     * que a una frecuencia de 8MHz, es capaz de contar 32.726ms.
     * Si ponemos el prescaler a 2, podrá contar el doble. Hasta 65.536ms*/
    T1CON = 0b00010000;             //Prescaler a 1:2, apagado
    
    //Configuración de las interrupciones
    INTCON = 0b00001000;            //Interrupciones por cambio de estado puerto B y Periféricos
    IOCB = 0b11110000;
    PIE1 = 0b00000001;                  //Interrupción por overflow del timer1
    PIR1 = 0b00000000;                  //Limpia las banderas de los periféricos
    GIE = 1;                                    //Bit Interrupción Global
}

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