/*
 * File:   main.c
 * Author: Edgar S
 *
 * Created on April 17, 2023, 1:27 PM
 */
#pragma config FOSC = INTRC_CLKOUT// Oscillator Selection bits (INTOSC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF     // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF      // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF     // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF

#pragma config BOR4V = BOR40V
#pragma config WRT  =   OFF

#include <xc.h>
#include <stdint.h>
#include <pic16f887.h>
#define _XTAL_FREQ 8000000


#include <xc.h>

unsigned int ADC_voltaje1;
int loop;
char option_selected;
int equivalent;

void setup(void);
void setupADC(void);
void initUART(void);
void print(unsigned char *palabra);
void conversion(int voltaje);


void __interrupt() isr (void){
  if(PIR1bits.ADIF){
      PORTBbits.RB7=1;
      PIR1bits.ADIF=0;
      PIR1bits.TXIF=0;
  }  
}
void main(void) {
    setup();
    setupADC();
    initUART();
    
    while(1){
        print("l)Leer Potenciometro\r");
        print("2) Enviar ASCII \r");
        
        while(PIR1bits.RCIF==0){
            ;
        }
        ADCON0bits.CHS = 0b0001;
        __delay_us(100);
        ADCON0bits.GO=1;
        while(ADCON0bits.GO==1){
            ;
        }
        ADC_voltaje1= ADRESH;
        conversion(ADC_voltaje1);
        CCPR1L= ADC_voltaje1;
        __delay_us(100);
        option_selected= RCREG;
        if(option_selected=='1'){
            print("Valor del potenciometro = ");
            TXREG= CCPR1L;
            
        }else if(option_selected == '2'){
            print("Caracter Ascii a mostrar..");
            while(PIR1bits.RCIF ==0){
                ;
            }
            PORTB=RCREG;
        }else{
            print("error al elegir");
        }
    }
    
}
void setup(void){
    TRISB=0;
    PORTB=0;
    OSCCONbits.IRCF= 0b111;
    OSCCONbits.SCS=1;
    INTCONbits.GIE=1;
    PIE1bits.ADIE=1;
    INTCONbits.TMR0IE=1;
    PIR1bits.ADIF=0;
}
void setupADC(void){
    TRISAbits.TRISA1=1;
    ANSELbits.ANS1=1;
    ADCON0bits.ADCS= 0b10;
    ADCON1bits.VCFG1=0;
    ADCON1bits.VCFG0=0;
    ADCON1bits.ADFM=0;
    ADCON0bits.CHS= 0b0001;
    ADCON0bits.ADON=1;
    __delay_us(100);
        
}
void initUART(void){
    SPBRG=12;
    TXSTAbits.SYNC=0;
    RCSTAbits.SPEN=1;
    TXSTAbits.TXEN=1;
    PIR1bits.TXIF=0;
    RCSTAbits.CREN=1;
    
}
void conversion(int voltaje){
    equivalent=(unsigned short)(48+((float)(207)/(255))*(voltaje-0));
}
void print(unsigned char *palabra){
    while(*palabra !='\0'){
        while(TXIF != 1);
        TXREG= *palabra;
        *palabra++;
    }
}