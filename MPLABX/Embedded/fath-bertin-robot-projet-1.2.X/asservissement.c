#include "asservissement.h"
#include "Toolbox.h"





/*
float getFloatFromBytes(const unsigned char *p, int index)
{
    float f;
    unsigned char *f_ptr = (unsigned char*)&f;

    for (int i = 0; i < 4; i++)
        f_ptr[i] = p[index + i];

    return f;
}
 */

unsigned char* toto1[20] = {}; 
int d = 0;
    

    getFloatFromBytes(toto1, d);


void SetupPidAsservissement(volatile PidCorrector* PidCorr, double Kp, double Ki, double Kd, double proportionelleMax, double integralMax, double deriveeMax){
    
    PidCorr->Kp = Kp;
    PidCorr->erreurProportionelleMax = proportionelleMax; //On limite la correction due au Kp
    PidCorr->Ki = Ki;
    PidCorr->erreurIntegraleMax = integralMax; //On limite la correction due au Ki
    PidCorr->Kd = Kd;
    PidCorr->erreurDeriveeMax = deriveeMax;
}

