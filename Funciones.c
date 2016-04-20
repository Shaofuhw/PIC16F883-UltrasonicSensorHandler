#include "Funciones.h"
//#include "LCD.h"
#include <stdio.h>

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
    
    /*lcd_init(0, 16, 2);                     //Inicializa el LCD
    lcd_clear();                            //Lo limpia
    lcd_on();*/
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

void CalcularDistancia( int *t4,  int *t5,  int *t6,  int *t7, char *distancias){    
   //Se divide entre 58 para calcular la distancia en centímetros
    t4[0] = ( t4[0] ) / 58;                                          
    t5[0] = ( t5[0] ) / 58;
    t6[0] = ( t6[0] ) / 58;
    t7[0] = ( t7[0] ) / 58;
    
    MediaMedidas(t4);
    MediaMedidas(t5);
    MediaMedidas(t6);
    MediaMedidas(t7);
    
    /*Se preparan los datos para ser enviados por I2C.
     Como el protocolo solo permite enviar Bytes, y las distancias están almacenadas en variables
     de 16 bits, se dividen en dos bytes, los 8 bits mas significativos están en el primer byte, y los 8
     menos signifitcativos en el siguiente*/
    
    distancias[0] = (t4[5] >> 8) & 0xFF;
    distancias[1] = t4[5] & 0xFF;
    distancias[2] = (t5[5] >> 8) & 0xFF;
    distancias[3] = t5[5] & 0xFF;
    distancias[4] = (t6[5] >> 8) & 0xFF;
    distancias[5] = t6[5] & 0xFF;
    distancias[6] = (t7[5] >> 8) & 0xFF;
    distancias[7] = t7[5] & 0xFF;
}

void MediaMedidas( int *t){
    char num = 0;
    
    for(char i = 0; i<5; i++){
        if( (t[i]>400) || (t[i]<0) ){
            t[i] = 0;
        } else{
            t[5] += t[i];
            num += 1;
        }
    }
    t[5] = t[5] / num;
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

void ShiftArrays( int* t4,  int* t5,  int* t6,  int* t7)
{
    for(char i = 0; i < 4; i++){
        t4[i+1] = t4[i];
        t5[i+1] = t5[i];
        t6[i+1] = t6[i];
        t7[i+1] = t7[i];        
    }
}

void ResetEcho()
{
    TRISB = 0b00000000;
    __delay_ms(5);
    PORTB = 0b00000000;
    __delay_ms(5);
}

void I2C_Slave_Init(short address) {
    SSPSTAT = 0x80;
    SSPADD = address;
    SSPCON = 0x36;
    SSPCON2 = 0x01;
    TRISC3 = 1; //Setting as input as given in datasheet
    TRISC4 = 1; //Setting as input as given in datasheet
    GIE = 1;
    PEIE = 1;
    SSPIF = 0;
    SSPIE = 1;
}


void IntPortb( int* t4,  int* t5,  int* t6,  int* t7, char* rbon){
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
            t4[0] = TMR1;
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
            t5[0] = TMR1;
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
            t6[0] = TMR1;
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
            t7[0] = TMR1;
            *rbon = 0;
            TMR1ON = 0;
        }
        
        RBIF = 0;                           //Limpia la bandera
        RBIE = 1;                           //Vuelve a activar la interrupcion
}

void IntI2C(char *distancias) {
    short z = 0;
    char i = 0;
    SSPCONbits.CKP = 0;
    
    //Ha ocurrido un overflow o una colisión de datos. Se limpia el buffer, y se borran las banderas
    if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL)) {                  
        z = SSPBUF; // Read the previous value to clear the buffer
        SSPCONbits.SSPOV = 0; // Clear the overflow flag
        SSPCONbits.WCOL = 0; // Clear the collision bit
        SSPCONbits.CKP = 1;
    }
    /*Ha llegado una solicitud de envío de datos. Se pone el dato en el buffer y se inicia la comunicación
     Se requiere que se transmitan los 8 bytes, por lo que el maestro deberá solicitar datos en la misma dirección
     8 veces seguidas. Cada vez que se solicite un dato, le enviará el siguiente*/
    if (!SSPSTATbits.D_nA && SSPSTATbits.R_nW) {
        z = SSPBUF;
        BF = 0;
        SSPBUF = distancias[i];
        i += 1;
        if( i == 7 ){
            i = 0;
        }
        SSPCONbits.CKP = 1;
        while (SSPSTATbits.BF);
    }
    SSPIF = 0;
}

