#ifndef PWM_H
#define	PWM_H

#define MOTEUR_DROIT 0
#define MOTEUR_GAUCHE 1

#define PWMPER 24.0
#define M_TO_PERCENT 28

void InitPWM(void);
//void PWMSetSpeed(float vitesseEnPourcents, int moteur);
//void PWMSetSpeedConsigne(float vitesseEnPourcents, int moteur);
void PWMUpdateSpeed();
void PWMSetSpeedConsignePolaire(float vitesseLineaire, float vitesseAngulaire);

#endif