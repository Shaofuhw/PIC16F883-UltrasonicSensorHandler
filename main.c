/*
 * File:   main.c
 * Author: Fu
 *
 * Created on 29 de febrero de 2016, 12:22
 */

#include <xc.h>
#include "Funciones.h"

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
//Del 0 al 4 se guardan las 5 últimas medidas, en 5 se guarda la media de las cuatro anteriores.
int t4[6], t5[6], t6[6], t7[6] = {0};
char rbon = 0;

char distancias[8] = {0};

//Programa
void main(void) {

    Inicializaciones();                     //Se configuran y inicializan los puertos, timer, interrupciones y LCD
    /*La velocidad de la transmisión depende de la dirección asiganada*/
    I2C_Slave_Init(0x30);
    
    while(1)
    {
        /*Es necesario que las lecturas de los sensores sean de manera secuencial, ya que si no puede haber
         errores debido a la solapación de las señales de ECHO.
         Los trigger se envian de manera secuencial, con un margen de 40ms entre cada pulso, ya que según el datasheet,
         la longitud máxima del pulso es de cerca de 38ms para cuando no detecta ningún obstáculo.
         Después de cada lectura, se calcula el valor de dicho pulso, y una vez se han leído los 4 sensores, se sacan por el LCD
         */
        /*Se utiliza un array para cada sensor, de esta manera se guardan los ultimos valores de cada uno, y se hace un filtro
        para evitar errores en la medida de los posible.
        Se descartan medidas que son muy diferentes a las anteriores, y se realiza la media*/
        Trigger();
        CalcularDistancia(t4, t5, t6, t7, distancias);
        //PrintDistancias(t4, t5, t6, t7);
        ShiftArrays(t4, t5, t6, t7);
        ResetEcho();                      //Se pone el pin de echo como salida, y se pone a 0, a veces se queda atascado
    }
    return;
}

//Vector de Servicio de Interrupción
void interrupt Interrupcion()
{
    if(RBIF == 1)                           //Comprobar flag del puerto B
    {
        IntPortb(t4, t5, t6, t7, &rbon);
    }
    /*Este PIC es un esclavo en al comunicación I2C.
     Cuando la controladora de vuelo solicite un dato nuevo, le enviará a través del I2C los datos requeridos
     que en este caso son los 8 bytes de distancia, en los que cada dos, son la distancia de cada sensor*/
    else if(SSPIF == 1){
        IntI2C(distancias);
    }
}