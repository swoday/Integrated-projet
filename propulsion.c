#include "propulsion.h"

/* Variables globales */
typeConsigne CONSIGNE = STOP; //consigne globale robot (arret, translation, rotation)

// ----- initialise les 2 moteurs -----
// configuration des moteurs en tant que sorties numeriques
void PropulsionInit()
{
	//desactivation des moteurs
	LEFTMOTOR = 0;
	RIGHTMOTOR = 0;
	
	//configuration des moteurs en sortie
	LEFTMOTOR_TRIS = 0;
	RIGHTMOTOR_TRIS = 0;
	
	//reconfiguration des pattes RP7/8 pour les utiliser en mode output compare (OC1 10010 OC2 10011)
	RPOR3bits.RP7R = 0b10010;
	RPOR4bits.RP8R = 0b10011;
	
	//initialisation QEI
	//QEIInit();
	
	//initialisation du timer 3, sans le lancer
	initTimerPWM();
	
	//initialisation de la PWM
	initPWM();	
	
	initRegulation();
	
	T3CONbits.TON = 1; //demarre le timer 3
}

// ----- initialisation du timer 3 (10 ms) pour la PWM, sans le lancer-----
void initTimerPWM()
{
	T3CONbits.TON = 0;			// Stop any 16-bit Timer3 operation
	T3CONbits.TCS = 0;			// internal instruction cycle clock
	T3CONbits.TGATE = 0;		// Timer3 Gated Time Accumulation off
	T3CONbits.TCKPS = 1;		// Prescaler 1:8
	TMR3 = 0;					// Clear Timer
	PR3 = 50000;			    // Load period value : 10ms = 50 000 * 8 * 25ns (40MIPS)
	IFS0bits.T3IF = 0;          // On met le flag d'interruption du timer 3 a 0
	IEC0bits.T3IE = 1;          // On active l'interruption pour le timer 3
}

// ----- initialisation du registre output compare pour la PWM -----
// pour les moteurs gauche (registre 1) et droit (registre 2)
// la configuration du rapport cyclique se fera dans une autre fonction
void initPWM()
{
	OC1CONbits.OCM = 0;     //desactivation
	OC1CONbits.OCTSEL = 1;  //on assigne le timer 3 pour gerer la PWM
	OC1CONbits.OCM = 0b110; //PWM mode, without fault detection
	
	OC2CONbits.OCM = 0;
	OC2CONbits.OCTSEL = 1;
	OC2CONbits.OCM = 0b110;
}

// ----- actions a realiser durant l'interruption -----
void _ISR _T3Interrupt(void)
{
	appliquerConsigne(); //met a jour les registres OC1RS et OC2RS
	IFS0bits.T3IF = 0; //ne pas oublier de remettre le flag d'interruption a 0
}

// ----- determination de la consigne -----
//void determinationConsigne(int typeConsigneSouhaitee)
//{
//	CONSIGNE = typeConsigneSouhaitee;
//}

// ----- application de la consigne ----- ***** pas encore implemente une facon de savoir ce qu'il reste a parcourir
void appliquerConsigne()
{
	switch(CONSIGNE)
	{
		case STOP:
		{
			LED1 = 0;
			LED2 = 0;
			LED3 = 0;
			LED4 = 0;
			commandeMoteurGauche(moteurArret);
			commandeMoteurDroit(moteurArret);
			break;
		}
		
		case FORWARD:
		{
			LED1 = 0;
			LED2 = 1;
			LED3 = 1;
			LED4 = 0;
			commandeMoteurGauche(moteurAvant);
			commandeMoteurDroit(moteurAvant);
			break;
		}
		
		case BACKWARD:
		{
			LED1 = 1;
			LED2 = 0;
			LED3 = 0;
			LED4 = 1;
			commandeMoteurGauche(moteurArriere);
			commandeMoteurDroit(moteurArriere);
			break;
		}
		
		case CLOCKWISE:
		{
			LED1 = 0;
			LED2 = 1;
			LED3 = 0;
			LED4 = 0;
			commandeMoteurGauche(moteurAvant);
			commandeMoteurDroit(moteurArriere);
			break;
		}
		
		case COUNTERCLOCKWISE:
		{
			LED1 = 0;
			LED2 = 0;
			LED3 = 1;
			LED4 = 0;
			commandeMoteurGauche(moteurArriere);
			commandeMoteurDroit(moteurAvant);
			break;
		}
		
		default: //arret des moteurs
		{
			commandeMoteurGauche(moteurArret);
			commandeMoteurDroit(moteurArret);
			break;
		}
	}
}

// ----- commande du moteur gauche ----- ***** regulation a implementer *****
// met a jour la valeur de OC1RS pour le moteur gauche
void commandeMoteurGauche(typeCommandeMoteur COMMANDE)
{
	/*
	switch(COMMANDE)
	{
		case moteurArret:
		{
			OC1RS = 7500; // 1.5 ms = 7500 * 8 * 25 ns 
			break;
		}
		
		case moteurAvant:
		{
			OC1RS = 10000; // 2 ms = 10000 * 8 * 25 ns 
			break;
		}
		
		case moteurArriere:
		{
			OC1RS = 5000; // 1 ms = 5000 * 8 * 25 ns 
			break;
		}
		
		default: //stop
		{
			OC1RS = 7500; // 1.5 ms = 7500 * 8 * 25 ns 
			break;
		}
	}	
	*/
	OC1RS = conversionVitesse(1,COMMANDE);
}

// ----- commande du moteur droit ----- ***** regulation a implementer *****
// met a jour la valeur de OC2RS pour le moteur droit
// ATTENTION, le moteur droit est inverse!
void commandeMoteurDroit(typeCommandeMoteur COMMANDE)
{
	/*
	switch(COMMANDE)
	{
		case moteurArret:
		{
			OC2RS = 7500; // 1.5 ms = 7500 * 8 * 25 ns 
			break;
		}
		
		case moteurAvant:
		{
			OC2RS = 5000; // 2 ms = 10000 * 8 * 25 ns 
			break;
		}
		
		case moteurArriere:
		{
			OC2RS = 10000; // 1 ms = 5000 * 8 * 25 ns 
			break;
		}
		
		default: //stop
		{
			OC2RS = 7500; // 1.5 ms = 7500 * 8 * 25 ns 
			break;
		}
	}
	*/
	OC2RS = conversionVitesse(2,COMMANDE);
}
