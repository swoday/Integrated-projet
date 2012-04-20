#include "QEI.h"

/* Variables globales */
const float diametreRoues = 70; //(mm)
const float PI = 3.14159;

float positionAbsolueGauche, positionAbsolueDroite;         //position absolue (en mm)
int angleAbsoluGauche, angleAbsoluDroite;                 //angle absolu (en degres)
int compteurRevolutionsGauche, compteurRevolutionsDroite; //nombre de revolutions completes (adimensionnel)
int flagUnderflowGauche, flagUnderflowDroite;             //pour le calcul correct de l'angle balaye

// ----- remise a zero des variables globales -----
void resetVariables()
{
	positionAbsolueGauche = 0;       
	positionAbsolueDroite = 0;
	angleAbsoluGauche = 0;       
	angleAbsoluDroite = 0;
	compteurRevolutionsGauche = 0;   
	compteurRevolutionsDroite = 0;
	flagUnderflowGauche = 0;         
	flagUnderflowDroite = 0;
}

// ----- initialisation des registres specifiques au QEI -----
void QEIInit()
{
	resetVariables();
	
	LATCbits.LATC3 = 0;  //desactivation des pattes
	LATCbits.LATC4 = 0;
	LATBbits.LATB5 = 0;
	LATBbits.LATB6 = 0;

	TRISCbits.TRISC3 = 1; //configuration en tant qu'inputs
	TRISCbits.TRISC4 = 1;
	TRISBbits.TRISB5 = 1;
	TRISBbits.TRISB6 = 1;
	
	//reconfiguration des pattes RP19/20 (moteur gauche) et RP5/6 (moteur droit) en mode QEI
	RPINR14bits.QEA1R = 19; //phases A et B moteur gauche
	RPINR14bits.QEB1R = 20;
	RPINR16bits.QEA2R = 5;  //phases A et B moteur droit
	RPINR16bits.QEB2R = 6;	
	
	QEI1CON = 0x0000; //reset "propre" du registre
	QEI2CON = 0x0000;

	QEI1CONbits.QEIM = 0b111; //configuration en mode x4 (POSxCNT reset si = MAXxCNT)
	QEI2CONbits.QEIM = 0b111;
	
	QEI1CONbits.SWPAB = 1;
	
	MAX1CNT = 360-1;     //valeur maximale du compteur de position --> 360 pins par tour en x4
	MAX2CNT = 360-1;
	POS1CNT = 0;         //valeur initiale du compteur de position
	POS2CNT = 0;
	
	IEC3bits.QEI1IE = 1; //on active l'interruption
	IEC4bits.QEI2IE = 1;
	IFS3bits.QEI1IF = 0; //on reset le flag d'interruption
	IFS4bits.QEI2IF = 0;
}

void _ISR _QEI1Interrupt(void)
{
	if(QEI1CONbits.UPDN) //overflow
	{
		compteurRevolutionsGauche++;
		flagUnderflowGauche = 0;
	}
	else
	{
		compteurRevolutionsGauche--;
		flagUnderflowGauche = 1;
	}
		
	IFS3bits.QEI1IF = 0; //on reset le flag d'interruption
}

void _ISR _QEI2Interrupt(void)
{
	if(QEI2CONbits.UPDN)
	{
		compteurRevolutionsDroite++;
		flagUnderflowDroite = 0;
	}
	else
	{
		compteurRevolutionsDroite--;
		flagUnderflowDroite = 1;
	}

	IFS4bits.QEI2IF = 0;
}

void getAngleAbsolu()
{
	int positionCorrecteGauche, positionCorrecteDroite; 
	
	//si le robot avance, pas de soucis, 0..359 0..359
	//par contre, s'il recule, ce sera 2 1 0 359..0 359..0, ce qui fausserait le calcul d'angle (car offset de +360)
	//on corrige pour avoir 2 1 0 -1 -2.. -360
	//il faudrait corriger uniquement s'il y a underflow
	
	if(flagUnderflowGauche)
		positionCorrecteGauche = POS1CNT - 360; //on retire l'offset de 360 : ex: 359 --> -1
	else
		positionCorrecteGauche = POS1CNT;
	
	if(flagUnderflowDroite)
		positionCorrecteDroite = POS2CNT - 360; //on retire l'offset de 360 : ex: 359 --> -1
	else
		positionCorrecteDroite = POS2CNT;	
	
	angleAbsoluGauche = compteurRevolutionsGauche*360 + positionCorrecteGauche; 
	angleAbsoluDroite = compteurRevolutionsDroite*360 + positionCorrecteDroite;
}

void getPositionAbsolue()
{
	getAngleAbsolu(); //maj angles
	positionAbsolueGauche = angleAbsoluGauche*PI*diametreRoues/360.0; //la reponse sera en mm
	positionAbsolueDroite = angleAbsoluDroite*PI*diametreRoues/360.0; // pi*diametre <--> 360 degres, regle de 3 pour un angle absolu quelconque
}






