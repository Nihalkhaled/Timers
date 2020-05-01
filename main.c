/* File:   main.c
 * Author: Hossam Eldeen Altokhy
 *
 * Created on April 4, 2020, 6:24 PM
 */
#define F_CPU 16000000UL
// Built-in headers
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// User-defined Headers
#include "Config.h"
#include "DIO.h"
#include "lcd.h"
#include "ADC.h"
#define BUTTON2        2
#define LED            3
#define Buzzer         3
#define portA           1
#define portB           2
#define portC           3
#define portD           4
#define Normal          0
#define PWM             1
#define CTC             2
#define FPWM            3
#define OC0_Closed      0
#define OC0_Toggle      1
#define OC0_Clear       2       
#define OC0_Set         3       
//#define stepValue      48828125?
char message[] = "Welcome";
char str1[] = "Volt = ";
char str2[] = " mV";
char cl[] = "    ";
// USE INT0 to increment the high pulse width in NON_inverting mode 
ISR(INT0_vect) {
    OCR0 += 20;
}
ISR(ADC_vect) {
    static int switcher = 0;
    char buffer[20];
    int noOfSteps = getADCdata();
    // convert steps to mV
    int data = (5 * noOfSteps) / 1.024;
    // Display
    itoa(data, buffer, 10);
    if (switcher) {
        LCD_String_xy(1, 7, cl);
        LCD_String_xy(1, 7, buffer);
        selectChannel(0);
        switcher = 0;
    } else {
        LCD_String_xy(0, 7, cl);
        LCD_String_xy(0, 7, buffer);
        selectChannel(1);
        switcher = 1;
    }
    _delay_ms(1000);
    //    Restart Conversion every ISR
    //    startConv();
}
void INT0_init() {
    MCUCR |= (1 << ISC01) | (1 << ISC00); // Rising Edge MOde
    GICR |= (1 << INT0);//ENABLE INT0
}
/*
// PWM with INTO INCREMENTTING OCR0 by 20
void Timer0_init() {
    TCCR0 |= (1 << WGM00) | (1 << COM01) | (1 << CS02) | (1 << CS00); // PWM Mode // Non_Inverting_MODE// CLKio/ 1024 
    //    TIMSK |= (1 << OCIE0); // Enable Timer Interrupt CompareMatch
    OCR0 = 0x80; // using INT0 TO INCREMENT OCR0
    PINBas(3, OUT); // OC0 pin as OUTPUT pin ,Here frequency =frequency(fast PWM)/2
}*/
/*
//fAST PWM with INTO INCREMENTTING OCR0 by 20
void Timer0_init() {
    TCCR0 |= (1 << WGM00) |(1 << WGM01) | (1 << COM01) | (1 << CS02) | (1 << CS00); // FAST PWM Mode // Non_Inverting_MODE// CLKio/ 1024 
    //    TIMSK |= (1 << OCIE0); // Enable Timer Interrupt CompareMatch
    OCR0 = 0x00; // using INT0 TO INCREMENT OCR0
    PINBas(3, OUT); // OC0 pin as OUTPUT pin
}
 * /
//fAST PWM 
void Timer0_init() {
    TCCR0 |= (1 << WGM00) |(1 << WGM01) | (1 << COM01) | (1 << CS02) | (1 << CS00); // fast Mode // Non_Inverting_MODE// CLKio/ 1024 
    //    TIMSK |= (1 << OCIE0); // Enable Timer Interrupt CompareMatch
    OCR0 = 0x80; // Compare_MATCH_VALUE
    PINBas(3, OUT); // OC0 pin as OUTPUT pin
}*/
//Testing OC0 TO normal mode  : Usage of using OC0 pin // enable another chip
/*
void Timer0_init() {
    TCCR0 |= (1 << COM00) | (1 << CS02) | (1 << CS00); // Toggle  //CLKio/ 1024 
    TIMSK |=(1<<TOIE0)//Enable timer interrupt overflow 
 *  OCR0 = 0x80;// the OC0 will toggle when OCR0=128 
    PINBas(3, OUT); // OC0 pin as OUTPUT pin, In proteus put an oscilloscope to see waveform
}
*/
//Testing OC0 TO CTC mode  : Usage of using oc0 pin // enable another chip
/*
void Timer0_init() {
    TCCR0 |= (1 << WGM01) | (1 << COM00) | (1 << CS02) | (1 << CS00); //CTC // Toggle OC0 PIN  //CLKio/ 1024 
    TIMSK |= (1 << OCIE0); // Enable Timer Interrupt Co
    OCR0 = 0x80;// the OC0 will toggle when OCR0=128 
    PINBas(3, OUT); // OC0 pin as OUTPUT pin, In proteus put an oscilloscope to see waveform
}
 */
/*
void Normal_Timer0_init() {
    TCCR0 |= (1 << COM00) ; // Toggle  //CLKio/ 1024 
    TIMSK |=(1<<TOIE0);//Enable timer interrupt overflow 
}
void CTC_Timer0_init() {
    TCCR0 |= (1 << WGM01) | (1 << COM00) ; //CTC // Toggle OC0 PIN  //CLKio/ 1024 
    TIMSK |= (1 << OCIE0); // Enable Timer Interrupt CMPARE
}
void FAST_PWM_Timer0_init() {
    TCCR0 |= (1 << WGM00) |(1 << WGM01) | (1 << COM01) ; // fast Mode // Non_Inverting_MODE// CLKio/ 1024 
}
void PWM_Timer0_init() {
    TCCR0 |= (1 << WGM00) | (1 << COM01) ; // PWM Mode // Non_Inverting_MODE// CLKio/ 1024        
}
*/
void _Timer0_init(char TimerMode, char OC0Mode, char OCR0_Value) {
     TCCR0=0x00;
     TIMSK=0x00;
    switch (TimerMode) {
        case Normal:          
            TIMSK |=(1<<TOIE0);               
            break;
        case PWM:      
           TCCR0 |= (1 << WGM00);
            break;
        case CTC:         
            TCCR0 |= (1 << WGM01);   
            TIMSK |= (1 << OCIE0);
               break;
        case FPWM:
            TCCR0 |= (1 << WGM00) |(1 << WGM01) ;
            break;
    }
    TCCR0 |= (OC0Mode<<4);
    OCR0 = OCR0_Value;
    TCCR0 |= (1 << CS02) | (1 << CS00);//CLKio/ 1024 
    PINBas(3, OUT); // OC0 pin as OUTPUT pin
}
void Timer0_stop() {
    // No Clock 
    TCCR0 &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
}
void Timer0_restart() {
    // No Clock 
    TCCR0 |= (1 << CS02) | (1 << CS00);
}
ISR(TIMER0_OVF_vect) { // the ISR is for interrupt flag comming from maxcompare (TOV))
 static int IntervalSecond = 0;
    IntervalSecond++;
    static int i = 1;
    if (IntervalSecond == 62) {
        // Every Second
        if (i) {
            setPORTC(0xFF);
            i = 0;
        } else {
            setPORTC(0x00);
            i = 1;
        }
        IntervalSecond = 0;
    }
}
ISR(TIMER0_COMP_vect) {// the ISR is for interrupt flag coming from compare 
    static int IntervalSecond = 0;
    IntervalSecond++;
    static int i = 1;
    if (IntervalSecond == 62) {
        // Every Second
        if (i) {
            setPORTC(0xFF);
            i = 0;
        } else {
            setPORTC(0x00);
            i = 1;
        }
        IntervalSecond = 0;
    }
}
int main(void) {
    PORTCas(OUT);
    PORTDas(OUT);
    _delay_ms(2000);
    //Timer0_init();
    _Timer0_init( FPWM, OC0_Clear, 0x80);
    INT0_init();
    LCD_Init();
    ADC_init(); // Sensor on ADC0
    sei(); // Enable Global Interrupt
    LCD_String_xy(0, 0, str1);
    LCD_String_xy(0, 13, str2);
    LCD_String_xy(1, 0, str1);
    LCD_String_xy(1, 13, str2);
    startConv();
    while (1) {
    }
}