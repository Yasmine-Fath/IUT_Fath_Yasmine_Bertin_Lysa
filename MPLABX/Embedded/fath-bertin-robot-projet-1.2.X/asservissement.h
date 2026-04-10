/* 
 * File:   asservissement.h
 * Author: E306-PC6
 *
 * Created on 19 janvier 2026, 15:19
 */

#ifndef ASSERVISSEMENT_H
#define	ASSERVISSEMENT_H

typedef struct _PidCorrector
{
    float Kp;
    float Ki;
    float Kd;
    float erreurProportionelleMax;
    float erreurIntegraleMax;
    float erreurDeriveeMax;
    float erreurIntegrale;
    float epsilon_1;
    float erreur;
    //For Debug only
    float corrP;
    float corrI;
    float corrD;
}PidCorrector;

void SetupPidAsservissement(volatile PidCorrector* PidCorr, double Kp, double Ki, double Kd, double proportionelleMax, double integralMax, double deriveeMax);

#endif	/* ASSERVISSEMENT_H */

