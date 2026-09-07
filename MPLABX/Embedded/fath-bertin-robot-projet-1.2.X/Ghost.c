#include "Ghost.h"
#include "QEI.h"
#include "Robot.h"
#include "math.h"
#include "Utilities.h"
#include "main.h"
#include "UART_Protocol.h"
#include "timer.h"

volatile GSTATE ghostState;

void SetupGhostState(GSTATE* gState, float theta, float vitesse, float accel, float vitesseMax, float waypoint){
    
    gState->ThetaGhost = theta;
    gState->VTheta = vitesse; 
    gState->AccTheta = accel;
    gState->VThetamax = vitesseMax;
    gState->ThetaWayPoint = 3.14;
}

void GhostComputation(GSTATE gState){

    gState.ThetaRestant = ModuloByAngle(gState.ThetaGhost, gState.ThetaWayPoint) - gState.ThetaGhost;

    gState.ThetaArret = (gState.VTheta*gState.VTheta)/(2*gState.AccTheta);
    gState.IncrementTheta = gState.VTheta*gState.Tsampling;
    
    
    if (gState.VTheta < 0) {
       gState.ThetaArret = - gState.ThetaArret;
    
    }
    
    if (((gState.ThetaArret >= 0 && gState.ThetaRestant >=    0) || (gState.ThetaArret <= 0 && gState.ThetaRestant <= 0)) && Abs(gState.ThetaRestant)>=gState.ThetaArret) { 
        // On accélère en rampe saturée
        if (gState.ThetaRestant > 0){
            // Si la destination est devant, on accélère en positif en saturant la vitesse à V?Max
            gState.VTheta = Min(gState.VTheta + gState.AccTheta/gState.FQEI, gState.VThetamax); 
        }
        
        else if(gState.ThetaRestant < 0) {
          //Si la destination est derrière, on accélère en négatif en saturant la vitesse à ?V?Max
            gState.VTheta = Max(gState.VTheta - gState.AccTheta/gState.FQEI, - gState.VThetamax);
        
        }
    }
    
    else {
        // On freine en rampe saturée
        if (gState.VTheta > 0) {
        //Si la vitesse positive est positive, on freine en positif en saturant la vitesse à 0
        gState.VTheta = Min(gState.VTheta - gState.AccTheta/gState.FQEI, 0); 
        }
        else if (gState.VTheta < 0) {
        //Si la vitesse est négative, on freine en négatif en saturant la vitesse à 0
        gState.VTheta = Max(gState.VTheta + gState.AccTheta/gState.FQEI, 0);
                
        }
        
        if (Abs(gState.ThetaRestant) < Abs(gState.IncrementTheta)){
            gState.IncrementTheta = gState.ThetaRestant;   
        }
    }
    
    // On intègre le déplacement
    gState.ThetaGhost = gState.ThetaGhost + gState.IncrementTheta;
    
    // On gère les erreurs numériques d?arrondis
    if (gState.VTheta == 0 && Abs(gState.ThetaRestant) < 0.01){
        gState.ThetaGhost = gState.ThetaWayPoint;
    }
    
}  



// 03/09
/// AFFICHAGE DU GHOST

void AffichageGHOST(GSTATE gState){
    unsigned char payload[4];
     
    //*****************Theta Ghost ****************
    getBytesFromFloat(payload, 0, gState.ThetaGhost);
   
    UartEncodeAndSendMessage(0x0090, 4, payload );
    
}