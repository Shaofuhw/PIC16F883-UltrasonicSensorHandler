#include "Funciones.h"
//#include "LCD.h"
#include "Uart.h"
#include <stdio.h>

//Se inician todos los registros necesarios para configurar el puerto B, las interrupciones, y el timer1

void Inicializaciones() {
    OSCCON = 0b01110101; //Se inicializa el oscilador a 8MHz, la m�xima.

    //Puerto B
    TRISB = 0b00000000; //Puerto B. Pines del 0 al 3 OUT.
    __delay_us(5);
    ANSELH = 0b00000000; //Establecer como puertos Digitales
    PORTB = 0b00000000; //Inicializar a 0

    /*Timer 1 Como se requiere contar hasta tiempo de hasta aproximadamente 40ms se coge el timer de 16bits
     * que a una frecuencia de 8MHz, es capaz de contar 32.726ms.
     * Si ponemos el prescaler a 2, podr� contar el doble. Hasta 65.536ms*/
    TMR1 = 0;
    T1CON = 0b00010000; //Prescaler a 1:2, apagado

    //Configuraci�n de las interrupciones
    INTCON = 0b00001000; //Interrupciones por cambio de estado puerto B
    IOCB = 0b11110000; //Puerto B 7-4 provocan interrupci�n con cambio

    /*lcd_init(0, 16, 2);                     //Inicializa el LCD
    lcd_clear();                            //Lo limpia
    lcd_on();*/
}

//Activa el trigger y lo apaga tras un retardo

void Trigger() {
    GIE = 1;
    RBIE = 1;

    TRISB = 0b00010000;
    __delay_us(10);
    RB0 = 1;
    __delay_us(10);
    RB0 = 0;

    __delay_ms(50);

    TRISB = 0b00100000;
    __delay_us(10);
    RB1 = 1;
    __delay_us(10);
    RB1 = 0;

    __delay_ms(50);

    TRISB = 0b01000000;
    __delay_us(10);
    RB2 = 1;
    __delay_us(10);
    RB2 = 0;

    __delay_ms(50);

    TRISB = 0b10000000;
    __delay_us(10);
    RB3 = 1;
    __delay_us(10);
    RB3 = 0;

    __delay_ms(50);

    RBIE = 0;
    GIE = 0;
}

void CalcularDistancia(int *t4, int *t5, int *t6, int *t7, char *distancias) {
    //Se divide entre 58 para calcular la distancia en cent�metros
    t4[0] = (t4[0]) / 58;
    t5[0] = (t5[0]) / 58;
    t6[0] = (t6[0]) / 58;
    t7[0] = (t7[0]) / 58;

    MediaMedidas(t4);
    MediaMedidas(t5);
    MediaMedidas(t6);
    MediaMedidas(t7);

    sprintf(distancias, "%u,%u,%u,%u\n\0", t4[5], t5[5], t6[5], t7[5]);
}

void MediaMedidas(int *t) {
    char num = 0;
    unsigned int sum = 0;

    for (char i = 0; i < 5; i++) {
        if ((t[i] > 400) || (t[i] < 0)) {
            t[i] = 0;
        } else {
            sum += t[i];
            num += 1;
        }
    }
    t[5] = sum / num;
}

/*void PrintDistancias( int* t4,  int* t5,  int* t6,  int* t7)
{
    //Print al LCD
    char dist1[10];
    char dist2[10];
    char dist3[10];
    char dist4[10];
    
    sprintf(dist1, "%u   \0", t4[5]);
    sprintf(dist2, "%u   \0", t5[5]);
    sprintf(dist3, "%u   \0", t6[5]);
    sprintf(dist4, "%u   \0", t7[5]);
    
    lcd_goto(0, 0);                      
    lcd_puts("Distancias:");
    lcd_goto(12,0);
    lcd_puts(dist1);
    lcd_goto(0,1);
    lcd_puts(dist2);
    lcd_goto(6,1);
    lcd_puts(dist3);
    lcd_goto(12,1);
    lcd_puts(dist4);  
}*/

void ShiftArrays(int* t4, int* t5, int* t6, int* t7) {
    for (char i = 0; i < 4; i++) {
        t4[i + 1] = t4[i];
        t5[i + 1] = t5[i];
        t6[i + 1] = t6[i];
        t7[i + 1] = t7[i];
    }
}

void ResetEcho(char* rbon) {
    rbon = 0;
    TRISB = 0b00000000;
    __delay_us(10);
    PORTB = 0b00000000;
    __delay_us(10);
}

void IntPortb(int* t4, int* t5, int* t6, int* t7, char* rbon) {
    RBIE = 0; //Desactiva el bit de interrupcion puerto B
    /* Cuando detecta la subida de alguno de los pulsos, pone su respectiva se�al de "On" a 1, de esta forma,
     cuando dicha se�al baja, se puede identificar c�al ha sido, y evitar posibles problemas de lectura.
     Como el trigger se manda a la vez para todos los ultrasonidos, no se sabe cu�ndo va a volver cada respectivo echo
     por lo que se hace de esta forma para que con un s�lo timer, se puedan contar todas las distancias.
     No funcionar�a en el caso de que varios ECHO volviesen justo en el  mismo instante, ya que s�lo detectar�a  uno*/
    if ((RB4 == 1) && (*rbon == 0) && (TRISB4 == 1)) {
        TMR1 = 0;
        *rbon = 4;
        TMR1ON = 1;
    } else if ((RB4 == 0) && (*rbon == 4) && (TRISB4 == 1)) {
        t4[0] = TMR1;
        *rbon = 0;
        TMR1ON = 0;
    } else if ((RB5 == 1) && (*rbon == 0) && (TRISB5 == 1)) {
        TMR1 = 0;
        *rbon = 5;
        TMR1ON = 1;
    } else if ((RB5 == 0) && (*rbon == 5) && (TRISB5 == 1)) {
        t5[0] = TMR1;
        *rbon = 0;
        TMR1ON = 0;
    } else if ((RB6 == 1) && (*rbon == 0) && (TRISB6 == 1)) {
        TMR1 = 0;
        *rbon = 6;
        TMR1ON = 1;
    } else if ((RB6 == 0) && (*rbon == 6) && (TRISB6 == 1)) {
        t6[0] = TMR1;
        *rbon = 0;
        TMR1ON = 0;
    } else if ((RB7 == 1) && (*rbon == 0) && (TRISB7 == 1)) {
        TMR1 = 0;
        *rbon = 7;
        TMR1ON = 1;
    } else if ((RB7 == 0) && (*rbon == 7) && (TRISB7 == 1)) {
        t7[0] = TMR1;
        *rbon = 0;
        TMR1ON = 0;
    }

    RBIF = 0; //Limpia la bandera
    RBIE = 1; //Vuelve a activar la interrupcion
}

