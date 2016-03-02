#include "Funciones.h"
#include "LCD.h"

//Se inician todos los registros necesarios para configurar el puerto B, las interrupciones, y el timer1
void Inicializaciones(){
    OSCCON = 0b01110101;            //Se inicializa el oscilador a 8MHz, la máxima.
    
    //Puerto B
    TRISB = 0b11110000;                 //Puerto B. Pines del 0 al 3 como salida. Resto entrada
    PORTB = 0b00000000;             //Inicializar a 0
    ANSELH = 0b00000000;           //Establecer como puertos Digitales
    
    //Puerto C
    TRISC = 0b00000011;                 //Del 2 al 7 como salida. Para el LCD
    PORTC = 0b00000000;             //Inicialización
    
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
    
    lcd_init(0, 16, 2);                     //Inicializa el LCD
    lcd_clear();                            //Lo limpia
    lcd_on();                               //Lo enciende
    lcd_goto(0, 0);                       //Coloca el cursor en la primera posición de la primera fila
    lcd_puts("Distancias: ");
}

//Activa el trigger y lo apaga tras un retardo
void Trigger0(){
    RB0 = 1;                                //Se activa el trigger
    __delay_us(10);
    RB0 = 0;                                //Se apaga el trigger tras retardo
}

void Trigger1(){
    RB1 = 1;                                //Se activa el trigger
    __delay_us(10);
    RB1 = 0;                                //Se apaga el trigger tras retardo
}

void Trigger2(){
    RB2 = 1;                                //Se activa el trigger
    __delay_us(10);
    RB2 = 0;                                //Se apaga el trigger tras retardo
}

void Trigger3(){
    RB3 = 1;                                //Se activa el trigger
    __delay_us(10);
    RB3 = 0;                                //Se apaga el trigger tras retardo
}

void CalcularDistancia(int dist, char** distancia, int size){
    dist = dist / 58;                 //Calcular la distancia en cm
    sprintf(*distancia,"%d cm", dist);
    *distancia[size-1] = '\0';
}

