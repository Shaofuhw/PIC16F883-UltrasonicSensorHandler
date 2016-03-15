#include "Funciones.h"
#include "LCD.h"
#include "Uart.h"

//Se inician todos los registros necesarios para configurar el puerto B, las interrupciones, y el timer1
void Inicializaciones(){
    OSCCON = 0b01110101;            //Se inicializa el oscilador a 8MHz, la máxima.
    
    //Puerto B
    TRISB = 0b11110000;                 //Puerto B. Pines del 0 al 3 OUT.
    PORTB = 0b00000000;             //Inicializar a 0
    ANSELH = 0b00000000;           //Establecer como puertos Digitales
    
    /*Timer 1 Como se requiere contar hasta tiempo de hasta aproximadamente 40ms se coge el timer de 16bits
     * que a una frecuencia de 8MHz, es capaz de contar 32.726ms.
     * Si ponemos el prescaler a 2, podrá contar el doble. Hasta 65.536ms*/
    TMR1 = 0;
    T1CON = 0b00010000;             //Prescaler a 1:2, apagado
    
    //Configuración de las interrupciones
    INTCON = 0b00001000;            //Interrupciones por cambio de estado puerto B
    IOCB = 0b11110000;                  //Puerto B 0-3 provocan interrupción con cambio
    GIE = 1;                                    //Bit Interrupción Global
    
    lcd_init(0, 16, 2);                     //Inicializa el LCD
    lcd_clear();                            //Lo limpia
    lcd_on();                               //Lo enciende
}

//Activa el trigger y lo apaga tras un retardo
void Trigger(){
    TRISB = 0b00010000;
    RB0 = 1;
    __delay_us(10);
    RB0 = 0;
    
    __delay_ms(50);
    
    TRISB = 0b00100000;
    RB1 = 1;
    __delay_us(10);
    RB1 = 0;
    
    __delay_ms(50);

    TRISB = 0b01000000;
    RB2 = 1;
    __delay_us(10);
    RB2 = 0;
    
    __delay_ms(50);
    
    TRISB = 0b10000000;
    RB3 = 1;
    __delay_us(10);
    RB3= 0;
    
    __delay_ms(50);
}

void CalcularDistancia(unsigned int t0, unsigned int t1, unsigned int t2,
        unsigned int t3, char** distancia1, char** distancia2, char** distancia3, char** distancia4){
    
    unsigned int d0,d1,d2,d3;
    
    d0 = ( t0 ) / 58;                                          //Se divide entre 58 para calcular la distancia en centímetros
    d1 = ( t1 ) / 58;
    d2 = ( t2 ) / 58;
    d3 = ( t3 ) / 58;
    
    if(d0 == 0){ sprintf(*distancia1,"NOPE\0"); }             
    else if(d0 > 400){ sprintf(*distancia1,"OUT\0"); }
    else { sprintf(*distancia1,"%u   \0", d0); }                     
    
    if(d1 == 0){ sprintf(*distancia2,"NOPE\0"); }
    else if(d1 > 400){ sprintf(*distancia2,"OUT\0"); }
    else{ sprintf(*distancia2,"%u   \0", d1); }
    
    if(d2 == 0){ sprintf(*distancia3,"NOPE\0"); }
    else if(d2 > 400){ sprintf(*distancia3,"OUT\0"); }
    else{ sprintf(*distancia3,"%u   \0", d2); }
    
    if(d3 == 0){ sprintf(*distancia4,"NOPE\0"); }
    else if(d3 > 400){ sprintf(*distancia4,"OUT\0"); }
    else{ sprintf(*distancia4,"%u   \0", d3); }
}

void PrintDistancias(char dist1[], char dist2[], char dist3[], char dist4[])
{
    //Print al LCD
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
    
    //Print puerto serie
    UART_Write_Text(dist1);
    __delay_ms(5);
    UART_Write_Text(dist2);
    __delay_ms(5);
    UART_Write_Text(dist3);
    __delay_ms(5);
    UART_Write_Text(dist4);
    __delay_ms(5);
}

void ResetEcho()
{
    TRISB = 0b00000000;
    __delay_ms(5);
    PORTB = 0b00000000;
    __delay_ms(5);
}