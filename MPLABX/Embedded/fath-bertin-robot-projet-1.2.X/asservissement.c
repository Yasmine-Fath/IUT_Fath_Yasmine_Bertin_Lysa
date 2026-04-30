#include "asservissement.h"
#include "Toolbox.h"
#include "UART_Protocol.h"
#include "CB_TX1.h"
#include "Robot.h"
#include "main.h"
#include "CB_TX1.h"





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
//    unsigned char c1 = CB_RX1_Get();
//    UartDecodeMessage(c1);
//    SendMessage(&c, 1);
//
//unsigned char* toto1[20] = {}; 
//int d = 0;
    

    //getFloatFromBytes(toto1, d);


void SetupPidAsservissement(volatile PidCorrector* PidCorr, double Kp, double Ki, double Kd, double proportionelleMax, double integralMax, double deriveeMax){
    
    PidCorr->Kp = Kp;
    PidCorr->erreurProportionelleMax = proportionelleMax; //On limite la correction due au Kp
    PidCorr->Ki = Ki;
    PidCorr->erreurIntegraleMax = integralMax; //On limite la correction due au Ki
    PidCorr->Kd = Kd;
    PidCorr->erreurDeriveeMax = deriveeMax;
}

//double Correcteur(volatile PidCorrector* PidCorr, double erreur)
//{
//    PidCorr->erreur = erreur;
//    double erreurProportionnelle = LimitToInterval(...);
//    PidCorr->corrP = PidCorr->Kp * ;
//    
//    PidCorr->erreurIntegrale += ...;
//    PidCorr->erreurIntegrale = LimitToInterval(...);
//    PidCorr->corrI = ...;
//    
//    double erreurDerivee = (erreur - PidCorr->epsilon_1)*FREQ_ECH_QEI;
//    double deriveeBornee = LimitToInterval(erreurDerivee, -PidCorr->erreurDeriveeMax/PidCorr->Kd,Pi
//    PidCorr->epsilon_1 = erreur;
//    PidCorr->corrD = deriveeBornee * PidCorr->Kd;
//    
//    return PidCorr->corrP+PidCorr->corrI+PidCorr->corrD;
//}