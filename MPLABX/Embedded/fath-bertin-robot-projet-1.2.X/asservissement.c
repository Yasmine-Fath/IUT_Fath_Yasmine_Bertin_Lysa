#include "asservissement.h"
#include "Toolbox.h"
#include "UART_Protocol.h"
#include "CB_TX1.h"
#include "Robot.h"
#include "main.h"
#include "CB_TX1.h"
#include "QEI.h"
#include "Utilities.h"
#include "PWM.h"




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

double Correcteur(volatile PidCorrector* PidCorr, double erreur)
{
    PidCorr->erreur = erreur;
    double erreurProportionnelle = LimitToInterval(erreur, -PidCorr->erreurProportionelleMax, PidCorr->erreurProportionelleMax);
    PidCorr->corrP = erreurProportionnelle * PidCorr->Kp;
    
    PidCorr->erreurIntegrale += erreur / FREQ_ECH_QEI;
    PidCorr->erreurIntegrale = LimitToInterval(PidCorr->erreurIntegrale,-PidCorr->erreurIntegraleMax / PidCorr->Ki,PidCorr->erreurIntegraleMax / PidCorr->Ki);
    PidCorr->corrI = PidCorr->erreurIntegrale * PidCorr->Ki;
    
    double erreurDerivee = (erreur - PidCorr->epsilon_1)*FREQ_ECH_QEI;
    double deriveeBornee = LimitToInterval(erreurDerivee, -PidCorr->erreurDeriveeMax/PidCorr->Kd,
        PidCorr->erreurDeriveeMax/PidCorr->Kd);
    PidCorr->epsilon_1 = erreur;
    PidCorr->corrD = deriveeBornee * PidCorr->Kd;
    
    return PidCorr->corrP+PidCorr->corrI+PidCorr->corrD;
}


void UpdateAsservissement()
{
   robotState.PidX.erreur = 0; //robotState.vitesseLineaireConsigne- robotState.vitesseLineaireFromOdometry;
   robotState.PidTheta.erreur = robotState.vitesseAngulaireConsigne - robotState.vitesseAngulaireFromOdometry;

    // --- Calcul des corrections via le PID ---
    robotState.CorrectionVitesseLineaire  = Correcteur(&robotState.PidX,robotState.PidX.erreur);
    robotState.CorrectionVitesseAngulaire = Correcteur(&robotState.PidTheta,robotState.PidTheta.erreur);

    // --- Application des corrections aux moteurs ---
    PWMSetSpeedConsignePolaire(robotState.CorrectionVitesseLineaire,robotState.CorrectionVitesseAngulaire);
    AffichagePID();
}


void AffichagePID(){
     unsigned char payload[24];
    //*****************correcteurs ****************
    getBytesFromFloat(payload, 0, robotState.PidX.corrP);
    getBytesFromFloat(payload, 4, robotState.PidX.corrI);
    getBytesFromFloat(payload, 8, robotState.PidX.corrD);
    
    getBytesFromFloat(payload, 12, robotState.PidTheta.corrP);
    getBytesFromFloat(payload, 16, robotState.PidTheta.corrI);
    getBytesFromFloat(payload, 20, robotState.PidTheta.corrD);
    
    UartEncodeAndSendMessage(0x0071, 24, payload );
    
    //*****************correction max ****************
    unsigned char payload1[24];
    getBytesFromFloat(payload1, 0, robotState.PidX.erreurProportionelleMax);
    getBytesFromFloat(payload1, 4, robotState.PidX.erreurIntegraleMax);
    getBytesFromFloat(payload1, 8, robotState.PidX.erreurDeriveeMax);
    
    getBytesFromFloat(payload1, 12, robotState.PidTheta.erreurProportionelleMax);
    getBytesFromFloat(payload1, 16, robotState.PidTheta.erreurIntegraleMax);
    getBytesFromFloat(payload1, 20, robotState.PidTheta.erreurDeriveeMax);
    
    UartEncodeAndSendMessage(0x0072, 24, payload );
    
    //***********************erreurs**********************
    unsigned char payload2[8];
    getBytesFromFloat(payload2, 0, robotState.PidX.erreur);
    getBytesFromFloat(payload2, 4, robotState.PidTheta.erreur);
    
    UartEncodeAndSendMessage(0x0073, 8, payload );


}