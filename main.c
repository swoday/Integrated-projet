////////////////////////////////////////////////////////
// Boucle principale du programme
// --> pour l'instant uniquement gestion de la propulsion, sans regulation
// Auteur : Duy
////////////////////////////////////////////////////////

//#include "configuration.h" //inclus dans propulsion.h
#include "propulsion.h" //inclut egalement regulationPropulsion.h

/* declaration des variables globales */
extern typeConsigne CONSIGNE; //variable externe declaree dans propulsion.c
                              //valeurs possibles : STOP, FORWARD, BACKWARD, CLOCKWISE, COUNTERCLOCKWISE
int isON = 1;                 //boucle infinie --> en assignant la valeur 0, permet 
                              //d'arreter "proprement" le robot

/* boucle principale */
int main(void)
{
	// ----- initialisations -----
	PllInit();        //configuration de la PLL pour fonctionner a 80 MHz, soit 40 MIps (Fosc/2)
	LedsInit();       //configuration des LEDs en sorties numeriques
	Timer2Init();     //configuration et enclenchement du timer2 
	PropulsionInit(); //T3CONbits.TON = 1
	
	
	
	
	CONSIGNE = FORWARD;
	
	
	
	
	// ----- debut de la boucle infinie -----
	while(isON)
	{		
		while(!_T2IF); //synchronisation : effectue la consigne tant que le timer 2 n'a pas deborde (100ms)
		_T2IF = 0; //on remet le flag d'interruption du timer2 du registre IFS0 a O
		T3CONbits.TON = 0;
		T1CONbits.TON = 0;
		
		/* Ajouter ici la determination de la consigne */
		//implementee par le groupe FSK
		//parmi les consignes possibles, une doit etre de mettre isON = 0
		
		T3CONbits.TON = 1; //effectue automatiquement les instructions de propulsion des que le timer 3 est lance
		T1CONbits.TON = 1; //regulation automatique de la vitesse des que le timer 1 est lance
		
	}
	
	// ----- fin de la boucle infinie -----
	
	return (0);
}
