////////////////////////////////////////////////////////
// Gestion du module propulsion
// 
// Auteur : Duy
////////////////////////////////////////////////////////
#ifndef PROPULSION_H_
#define PROPULSION_H_

#include "p33Fxxxx.h"
#include "regulationPropulsion.h"
#include "configuration.h"
//#include "QEI.h" //inclus par regulationPropulsion.h

/* Definitions */
#define LEFTMOTOR       (LATBbits.LATB7)
#define RIGHTMOTOR      (LATBbits.LATB8)
#define LEFTMOTOR_TRIS  (TRISBbits.TRISB7)
#define RIGHTMOTOR_TRIS (TRISBbits.TRISB8)

//consigne globale donnee au robot
typedef enum
{
	STOP,
	FORWARD,
	BACKWARD,
	CLOCKWISE,
	COUNTERCLOCKWISE
} typeConsigne;

//commande envoyee specifiquement a un moteur apres avoir recu la consigne generale
typedef enum
{
	moteurArret,
	moteurAvant,
	moteurArriere
} typeCommandeMoteur;



/* Fonctions implementees */

// ----- initialise les 2 moteurs (*** fonction a appeler dans MAIN ***)
// +++++ appelle initTimerPWM(), initPWM(), et met en marche le timer 3
void PropulsionInit();

// ----- determination de la consigne ----- (*** fonction a appeler dans MAIN ***)
// permet a l'utilisateur de specifier la consigne souhaitee
// void determinationConsigne(int typeConsigneSouhaitee);

// ----- initialisation du timer 3 (10 ms) pour la PWM, sans le lancer-----
void initTimerPWM();

// ----- initialisation du registre output compare pour la PWM -----
// pour les moteurs gauche (registre 1) et droit (registre 2)
void initPWM();

// ----- actions a realiser durant l'interruption -----
// +++++ appelle appliquerConsigne()
void _ISR _T3Interrupt(void);

// ----- application de la consigne -----
// ----- utilisee dans l'interruption, utilise commandeMoteurGauche()/commandeMoteurDroit()
void appliquerConsigne();

// ----- commande du moteur gauche ----- 
// met a jour la valeur de OC1RS pour le moteur gauche
void commandeMoteurGauche(typeCommandeMoteur COMMANDE);

// ----- commande du moteur droit -----
// met a jour la valeur de OC2RS pour le moteur droit
void commandeMoteurDroit(typeCommandeMoteur COMMANDE);

#endif



