#ifndef PROPULSION_H_
#define PROPULSION_H_

#include "p33Fxxxx.h"
#include "QEI.h"

void initTimer1();
void initRegulation();
float regulateurPI(float pastReg, float currErr, float pastErr, float kP, float kI);
void regulationVitesse(int commande);
int conversionVitesse(int moteur, int commande);

#endif