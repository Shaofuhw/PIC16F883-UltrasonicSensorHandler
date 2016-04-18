#include "Funciones.h"
//#include "LCD.h"
//#include "Uart.h"

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
    IOCB = 0b11110000;                  //Puerto B 7-4 provocan interrupción con cambio
    GIE = 1;                                    //Bit Interrupción Global
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

void CalcularDistancia(unsigned int t0, unsigned int t1, unsigned int t2, unsigned int t3, char *distancias){
    
    unsigned int d0,d1,d2,d3;
    
    d0 = ( t0 ) / 58;                                          //Se divide entre 58 para calcular la distancia en centímetros
    d1 = ( t1 ) / 58;
    d2 = ( t2 ) / 58;
    d3 = ( t3 ) / 58;
    
    //Se calculan las distancias, y se colocan en bytes
    //Como la distancia tiene un valor mayor de 256, son necesarios 2 Bytes.
    //Por lo tanto, se utilizan bytes a pares para almacenar cada distancia.
    //En el primer byte se almacena el byte más significativo, y en el segundo el menos significativo
    
    distancias[0] = (d0 >> 8) & 0xFF;
    distancias[1] = d0 & 0xFF;
    distancias[2] = (d1 >> 8) & 0xFF;
    distancias[3] = d1 & 0xFF;
    distancias[4] = (d2 >> 8) & 0xFF;
    distancias[5] = d2 & 0xFF;
    distancias[6] = (d3 >> 8) & 0xFF;
    distancias[7] = d3 & 0xFF;
}

void ResetEcho()
{
    TRISB = 0b00000000;
    __delay_ms(5);
    PORTB = 0b00000000;
    __delay_ms(5);
}

void IntPortb(unsigned int* t4, unsigned int* t5, unsigned int* t6, unsigned int* t7, char* rbon){
           RBIE = 0;                           //Desactiva el bit de interrupcion puerto B
        /* Cuando detecta la subida de alguno de los pulsos, pone su respectiva señal de "On" a 1, de esta forma,
         cuando dicha señal baja, se puede identificar cúal ha sido, y evitar posibles problemas de lectura.
         Como el trigger se manda a la vez para todos los ultrasonidos, no se sabe cuándo va a volver cada respectivo echo
         por lo que se hace de esta forma para que con un sólo timer, se puedan contar todas las distancias.
         No funcionaría en el caso de que varios ECHO volviesen justo en el  mismo instante, ya que sólo detectaría  uno*/
        if ( (RB4 == 1) && (*rbon == 0) )
        {
            TMR1 = 0;
            *rbon = 4;
            TMR1ON = 1;
        }
        
        else if( (RB4 == 0) && (*rbon == 4) )
        {
            *t4 = TMR1;
            *rbon = 0;
            TMR1ON = 0;
        }
        
        else if ( (RB5 == 1) && (*rbon == 0) )
        {
            TMR1 = 0;
            *rbon = 5;
            TMR1ON = 1;
        }
        
        else if( (RB5 == 0) && (*rbon == 5) )
        {
            *t5 = TMR1;
            *rbon = 0;
            TMR1ON = 0;
        }
   
        else if ( (RB6 == 1) && (*rbon == 0) )
        {
            TMR1 = 0;
            *rbon = 6;
            TMR1ON = 1;
        }
        
        else if( (RB6 == 0) && (*rbon == 6) )
        {
            *t6 = TMR1;
            *rbon = 0;
            TMR1ON = 0;
        }
        
        else if ( (RB7 == 1) && (*rbon == 0) )
        {
            TMR1 = 0;
            *rbon = 7;
            TMR1ON = 1;
        }
        
        else if( (RB7 == 0) && (*rbon == 7) )
        {
            *t7 = TMR1;
            *rbon = 0;
            TMR1ON = 0;
        }
        
        RBIF = 0;                           //Limpia la bandera
        RBIE = 1;                           //Vuelve a activar la interrupcion
}