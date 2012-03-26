#include "configuration.h"
#include "p33Fxxxx.h"

// disables the JTAG to prevent conflicts with TRISA
_FICD(JTAGEN_OFF);

// disables the watchdog
_FWDT(FWDTEN_OFF);

// The dsPIC will be clocked by the primary oscillator with a 10MHz crystal.
// We want to use the PLL to obtain Fosc = 80MHz ( <=> 40MIPS ).
// Problem : with a 10MHz crystal, PLL constraints are not met with the
// default parameter.
// Solution : 
//	- boot using the internal FRC oscillator as clock source,
//	- set the right PLL parameters to obtain Fosc = 80MHz, without violating
//	  the PLL constraints,
//	- switch the clock source to the PLL
//	- wait for the end of the clock switch
//

// Select internal FRC oscillator as clock source
_FOSCSEL(FNOSC_FRC);

// enables clock switching and configure the primary oscillator for a 10MHz crystal
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_XT);

void PllInit( void )
{
	// Configures PLL prescaler, PLL postscaler, PLL divisor
	// to obtain Fosc = 80MHz with the 10MHz xt oscillator
	// Fosc = Fin*M/(N1+N2), where :
	// 		N1 = PLLPRE + 2
	// 		N2 = 2 x (PLLPOST + 1)
	//		M = PLLDIV + 2
	PLLFBD = 30;						// M = 32
	CLKDIVbits.PLLPOST = 0;				// N1 = 2 
	CLKDIVbits.PLLPRE = 0;				// N2 = 2 
	// Initiate Clock Switch to Primary Oscillator with PLL 
	__builtin_write_OSCCONH( 3 );
	__builtin_write_OSCCONL( 1 );
	// Wait for Clock switch to occur
	while (OSCCONbits.COSC != 0b011);
}



// Configures the LEDs IO pins as digital outputs (modified by Duy)
void LedsInit(void)
{	
	//Turn LEDs off
	LED1 = 0;
	LED2 = 0;
	LED3 = 0;
	LED4 = 0;
	
	//Configure LEDs as outputs
	LED1_TRIS = 0;
	LED2_TRIS = 0;
	LED3_TRIS = 0;
	LED4_TRIS = 0;
	
	//Configure LEDS as digital pins
	LED1_MODE = 1;
	LED2_MODE = 1;
	LED3_MODE = 1;
	LED4_MODE = 1;
}

// Configures and starts timer2 to create a 10ms time base (modified by Duy)
void Timer2Init(void)
{
	T2CONbits.TON = 0;			// Stop any 16-bit Timer2 operation
	T2CONbits.TCS = 0;			// internal instruction cycle clock
	T2CONbits.TGATE = 0;		// Timer2 Gated Time Accumulation off
	T2CONbits.TCKPS = 3;		// Prescaler 1:256
	TMR2 = 0;					// Clear Timer
	PR2 = 15625 ;				// Load period value : 100ms = 15625 * 256 * 25ns (40MIPS)
	T2CONbits.TON = 1;			// Start Timer
}
