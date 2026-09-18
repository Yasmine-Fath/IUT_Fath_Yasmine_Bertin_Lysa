#include "Ghost.h"
#include "QEI.h"
#include "Robot.h"
#include "math.h"
#include "Utilities.h"
#include "main.h"
#include "UART_Protocol.h"
#include "timer.h"

GSTATE RobotGstate;


//void SetupGhostState(GSTATE* RobotGstate, float theta, float vitesse, float accel, float vitesseMax){
    void SetupGhostState(float theta, float vitesse, float accel, float vitesseMax){

    RobotGstate.ThetaGhost = theta;
    RobotGstate.VTheta = vitesse; 
    RobotGstate.AccTheta = accel;
    RobotGstate.VThetamax = vitesseMax;
    RobotGstate.FQEI = 10;
    RobotGstate.Tsampling = 1.0f/RobotGstate.FQEI;
    RobotGstate.ThetaWayPoint = 0;
    
}

void GhostComputation(){

    RobotGstate.ThetaRestant = ModuloByAngle(RobotGstate.ThetaGhost, RobotGstate.ThetaWayPoint) - RobotGstate.ThetaGhost;

    RobotGstate.ThetaArret = (RobotGstate.VTheta*RobotGstate.VTheta)/(2*RobotGstate.AccTheta);
    RobotGstate.IncrementTheta = RobotGstate.VTheta*RobotGstate.Tsampling;
    
    
    if (RobotGstate.VTheta < 0) {
       RobotGstate.ThetaArret = - RobotGstate.ThetaArret;
    
    }
    
    if (((RobotGstate.ThetaArret >= 0 && RobotGstate.ThetaRestant >=    0) || (RobotGstate.ThetaArret <= 0 && RobotGstate.ThetaRestant <= 0)) && Abs(RobotGstate.ThetaRestant)>=RobotGstate.ThetaArret) { 
        // On accélère en rampe saturée
        if (RobotGstate.ThetaRestant > 0){
            // Si la destination est devant, on accélère en positif en saturant la vitesse à V?Max
            RobotGstate.VTheta = Min(RobotGstate.VTheta + RobotGstate.AccTheta/RobotGstate.FQEI, RobotGstate.VThetamax); 
        }
        
        else if(RobotGstate.ThetaRestant < 0) {
          //Si la destination est derrière, on accélère en négatif en saturant la vitesse à ?V?Max
            RobotGstate.VTheta = Max(RobotGstate.VTheta - RobotGstate.AccTheta/RobotGstate.FQEI, - RobotGstate.VThetamax);
        
        }
    }
    
    else {
        // On freine en rampe saturée
        if (RobotGstate.VTheta > 0) {
        //Si la vitesse positive est positive, on freine en positif en saturant la vitesse à 0
        RobotGstate.VTheta = Min(RobotGstate.VTheta - RobotGstate.AccTheta/RobotGstate.FQEI, 0); 
        }
        else if (RobotGstate.VTheta < 0) {
        //Si la vitesse est négative, on freine en négatif en saturant la vitesse à 0
        RobotGstate.VTheta = Max(RobotGstate.VTheta + RobotGstate.AccTheta/RobotGstate.FQEI, 0);                
        }
        
        if (Abs(RobotGstate.ThetaRestant) < Abs(RobotGstate.IncrementTheta))
        {
            RobotGstate.IncrementTheta = RobotGstate.ThetaRestant;   
        }
    }
    
    // On intègre le déplacement
    RobotGstate.ThetaGhost = RobotGstate.ThetaGhost + RobotGstate.IncrementTheta;

    
    
    // On gère les erreurs numériques d?arrondis
    if (RobotGstate.VTheta == 0 && Abs(RobotGstate.ThetaRestant) < 0.01){
        RobotGstate.ThetaGhost = RobotGstate.ThetaWayPoint;
        
    }
    
}  



// 03/09
/// AFFICHAGE DU GHOST

//void AffichageGHOST(GSTATE RobotGstate){
void AffichageGHOST(){
    unsigned char payload[8];
 
    //*****************Theta Ghost ****************
    getBytesFromFloat(payload, 0, RobotGstate.ThetaGhost);
    UartEncodeAndSendMessage(0x0090, 8, payload );
    
}