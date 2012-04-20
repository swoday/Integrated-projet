#include "regulationPropulsion.h"

/* variables globales */
//on ramene l'echelle des vitesses a une echelle lineaire variant de (vMIN) -100 .. 0 (stop) .. 100 (vMAX)
const float consigneMAX = 100.0; 
const float consigneMIN = 0.0;
const float vref = 0.14; // vitesse de reference des 2 moteurs 14cm/s (2 revolutions/s) -> 0,14 m s^-1
const float kPGauche = 8.5;
const float kIGauche = 2.0;
const float kPDroite = 8.5;
const float kIDroite = 2.0;

extern float positionAbsolueGauche;
extern float positionAbsolueDroite;

int flag;

float vitesseReelleGauche, vitesseReelleDroite; //vitesse physique reelle (en m/s)
float pastPosGauche, pastPosDroite;    //position a l'iteration precedente
float currPosGauche, currPosDroite;    //position courante

float pastErrGauche, pastErrDroite; //erreur precedente
float currErrGauche, currErrDroite; //erreur courante
float consigneReguleeGauche, consigneReguleeDroite; //consigne regulee appliquee

void initTimer1() //timer 4 ms
{
	T1CONbits.TON = 0;			// Stop any 16-bit Timer1 operation
	T1CONbits.TCS = 0;			// internal instruction cycle clock
	T1CONbits.TGATE = 0;		// Timer1 Gated Time Accumulation off
	T1CONbits.TCKPS = 1;		// Prescaler 1:8
	TMR1 = 0;					// Clear Timer
	PR1 = 20000;			    // Load period value : 4ms = 20 000 * 8 * 25ns (40MIPS)
	IFS0bits.T1IF = 0;          // On met le flag d'interruption du timer 1 a 0
	IEC0bits.T1IE = 1;          // On active l'interruption pour le timer 1
}

void initRegulation()
{
	flag = 0;
	vitesseReelleGauche = 0; vitesseReelleDroite = 0;
	pastPosGauche = 0; pastPosDroite = 0;
	currPosGauche = 0; currPosDroite = 0;
	
	pastErrGauche = 0; pastErrDroite = 0;
	currErrGauche = 0; currErrDroite = 0;
	consigneReguleeGauche = 0; consigneReguleeDroite = 0;
	QEIInit();
	initTimer1();
}

float regulateurPI(float pastReg, float currErr, float pastErr, float kP, float kI)
{
	float outputReg = pastReg + kP*(currErr - pastErr) + kI*(currErr + pastErr);
	
	//saturation
	if(outputReg > consigneMAX)
		outputReg = consigneMAX;
		
	if(outputReg < -consigneMAX)
		outputReg = -consigneMAX;

	return outputReg;
}

void regulationVitesse(int commande)
{
	float v1, v2;
	
	switch(commande)
	{
		case 0: //stop
		{
			v1 = 0;
			v2 = 0;
			break;
		}
		case 1: //forward
		{
			v1 = vref;
			v2 = vref;
			break;
		}
		case 2: //backward
		{
			v1 = -vref;
			v2 = -vref;
			break;
		}
		case 3: //clockwise
		{
			v1 = vref;
			v2 = -vref;
			break;
		}
		case 4: //counterclockwise
		{
			v1 = -vref;
			v2 = vref;
			break;
		}
		default:
		{
			v1 = 0;
			v2 = 0;
			break;
		}
	}
	
	currErrGauche = v1 - vitesseReelleGauche;
	currErrDroite = v2 - vitesseReelleDroite;
	
	consigneReguleeGauche = regulateurPI(consigneReguleeGauche, currErrGauche, pastErrGauche, kPGauche, kIGauche);
	consigneReguleeDroite = regulateurPI(consigneReguleeDroite, currErrDroite, pastErrDroite, kPDroite, kIDroite);
	 
	pastErrGauche = currErrGauche;
	pastErrDroite = pastErrDroite;
}

//reconversion en valeurs physiques pour la PWM
int conversionVitesse(int moteur, int commande)
{
	int output = 0;
	regulationVitesse(commande);
	
	if(moteur == 1) //moteur gauche
	{
		output = (int)(consigneReguleeGauche*25+7500);
	}
	else if(moteur == 2) //moteur droit
	{
		output = (int)(consigneReguleeDroite*(-25)+7500);
	}
	return output;
}

// a chaque fois que le timer deborde, on prend la position, et on en determine la vitesse, en faisant la difference avec la position precedente
void _ISR _T1Interrupt(void) //puisqu'on met a jour les vitesses tous les 10 ms via le timer 2, on va faire en sorte d'avoir une estimation de la vitesse reelle avant que le timer 2 ne deborde. pour ce faire, on utilise le timer 1, implemente de telle maniere qu'au moins 2 interruptions ont lieu, permettant le calcul de la vitesse. --> timer 1 de 4ms --> 2*4ms = 8 ms < 10 ms
{
	if(!flag) //flag = 0
	{
		getPositionAbsolue();
		pastPosGauche = positionAbsolueGauche;
		pastPosDroite = positionAbsolueDroite;
		flag = 1;
	}
	else //flag = 1
	{
		getPositionAbsolue();
		currPosGauche = positionAbsolueGauche;
		currPosDroite = positionAbsolueDroite;
		
		flag = 0;
		
		vitesseReelleGauche = (currPosGauche - pastPosGauche)/4.0; // millimetres, divisé par 4 millisecondes (ecart entre 2 mesures)
		vitesseReelleDroite = (currPosDroite - pastPosDroite)/4.0; 
		
	}
	
	IFS0bits.T1IF = 0;
}







