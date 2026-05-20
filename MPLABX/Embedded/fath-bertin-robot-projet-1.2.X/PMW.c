#include <xc.h>
#include "IO.h"
#include "PWM.h"
#include "Robot.h"
#include "Utilities.h"
#include "QEI.h"



double talon = 50;
int acceleration = 5;

void InitPWM(void) {
    PTCON2bits.PCLKDIV = 0b000; //Divide by 1
    PTPER = 100 * PWMPER; //ÈPriode en pourcentage

    //ÈRglage PWM moteur 1 sur hacheur 1
    IOCON1bits.PMOD = 0b11; //PWM I/O pin pair is in the True Independent Output mode
    IOCON1bits.PENL = 1;
    IOCON1bits.PENH = 1;
    FCLCON1 = 0x0003; //ÈDsactive la gestion des faults

    IOCON2bits.PMOD = 0b11; //PWM I/O pin pair is in the True Independent Output mode
    IOCON2bits.PENL = 1;
    IOCON2bits.PENH = 1;
    FCLCON2 = 0x0003; //ÈDsactive la gestion des faults

    /* Enable PWM Module */
    PTCONbits.PTEN = 1;
}

void PWMSetSpeed(float vitesseEnPourcents, int moteur)
{
    if(vitesseEnPourcents<0){
        vitesseEnPourcents=-vitesseEnPourcents;
        if( moteur==1){
            SDC2 = vitesseEnPourcents * PWMPER + talon;
            PDC2 = talon;}
        else{
            PDC1 = vitesseEnPourcents * PWMPER + talon;
            SDC1 = talon;}
    }
    else{
        if( moteur==1){
            SDC1 = vitesseEnPourcents * PWMPER + talon;
            PDC1 = talon;}
        else{
            PDC2 = vitesseEnPourcents * PWMPER + talon;
            SDC2 = talon;} 
    }
}

/*void PWMSetSpeedConsigne(float vitesseEnPourcents, int moteur) {
    if (moteur == MOTEUR_GAUCHE) 
        robotState.vitesseGaucheConsigne = vitesseEnPourcents;
    else if (moteur == MOTEUR_DROIT) 
        robotState.vitesseDroiteConsigne = vitesseEnPourcents;
        
}*/

void PWMUpdateSpeed() {
    // Cette fonction est appelee sur timer et permet de suivre des rampes d acceleration
    if (robotState.vitesseGaucheCourantePercent < robotState.vitesseGaucheConsignePercent) {
        robotState.vitesseGaucheCourantePercent = Min(
                robotState.vitesseGaucheCourantePercent + acceleration,
                robotState.vitesseGaucheConsignePercent);
    }

    if (robotState.vitesseGaucheCourantePercent > robotState.vitesseGaucheConsignePercent) {
        robotState.vitesseGaucheCourantePercent = Max(
                robotState.vitesseGaucheCourantePercent - acceleration,
                robotState.vitesseGaucheConsignePercent);
    }

    if (robotState.vitesseGaucheCourantePercent > 0) {
        PDC1 = robotState.vitesseGaucheCourantePercent * PWMPER + talon;
        SDC1 = talon;//gauche
    }
    else {
        PDC1 = talon;
        SDC1 = -robotState.vitesseGaucheCourantePercent * PWMPER + talon;
    }

    if (robotState.vitesseDroiteCourantePercent < robotState.vitesseDroiteConsignePercent) {
        robotState.vitesseDroiteCourantePercent = Min(
                robotState.vitesseDroiteCourantePercent + acceleration,
                robotState.vitesseDroiteConsignePercent);
    }

    if (robotState.vitesseDroiteCourantePercent > robotState.vitesseDroiteConsignePercent) {
        robotState.vitesseDroiteCourantePercent = Max(
                robotState.vitesseDroiteCourantePercent - acceleration,
                robotState.vitesseDroiteConsignePercent);
    }

    if (robotState.vitesseDroiteCourantePercent >= 0) {
        PDC2 = robotState.vitesseDroiteCourantePercent * PWMPER + talon;
        SDC2 = talon;//droite
    } else {
        PDC2 = talon;
        SDC2 = -robotState.vitesseDroiteCourantePercent * PWMPER + talon;
    }
}

void PWMSetSpeedConsignePolaire(float vitesseLineaire, float vitesseAngulaire) {
    robotState.vitesseDroiteConsigne = vitesseLineaire + vitesseAngulaire*DISTROUES/2;
    robotState.vitesseGaucheConsigne = vitesseLineaire - vitesseAngulaire*DISTROUES/2;
    
    robotState.vitesseDroiteConsignePercent = -M_TO_PERCENT * robotState.vitesseDroiteConsigne;
    robotState.vitesseGaucheConsignePercent = M_TO_PERCENT * robotState.vitesseGaucheConsigne;

    robotState.vitesseDroiteConsignePercent = LimitToInterval(robotState.vitesseDroiteConsignePercent , -100, 100);
    robotState.vitesseGaucheConsignePercent = LimitToInterval(robotState.vitesseGaucheConsignePercent , -100, 100);
}


