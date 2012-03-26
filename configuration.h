////////////////////////////////////////////////////////
// Configuration horloge interne et LEDs
// Auteurs : Michel Osee, modifie par Duy
////////////////////////////////////////////////////////
#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

// Contains the SFR definitions
#include "p33Fxxxx.h"

// LEDs pins assignation
#define LED1		(LATCbits.LATC1)
#define LED2		(LATCbits.LATC0)
#define LED3		(LATBbits.LATB3)
#define LED4		(LATBbits.LATB2)
#define LED1_TRIS	(TRISCbits.TRISC1)
#define LED2_TRIS	(TRISCbits.TRISC0)
#define LED3_TRIS	(TRISBbits.TRISB3)
#define LED4_TRIS	(TRISBbits.TRISB2)
#define LED1_MODE   (AD1PCFGLbits.PCFG7)
#define LED2_MODE   (AD1PCFGLbits.PCFG6)
#define LED3_MODE   (AD1PCFGLbits.PCFG5)
#define LED4_MODE   (AD1PCFGLbits.PCFG4)

// Configure PLL prescaler, PLL postscaler, PLL divisor
// to obtain Fosc = 80MHz with the 10MHz xt oscillator
void PllInit(void);

// Configures the LEDs IO pins as 1) digital 2) outputs (modified by Duy)
void LedsInit(void);

// Configures and starts timer2 to create a 10ms time base (modified by Duy)
void Timer2Init(void);

#endif
